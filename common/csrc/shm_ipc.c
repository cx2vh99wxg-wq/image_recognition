/*
 * shm_ipc.c — 共享内存接口实现（【人员 B】主责；A 提供初版，B 于 2026-10-06 复核加固）
 *
 * 使用 System V 共享内存：shmget/shmat/shmdt。
 *  - shm_create：写者用，不存在则创建（IPC_CREAT|0666），已存在则直接打开；
 *  - shm_open  ：读者用，仅打开已存在段（不带 IPC_CREAT）。若写者未启动，
 *                立即失败并打印明确错误——修复 A 初版"读者静默创建段"掩盖故障的问题。
 *  - shm_close：仅解除本进程映射（shmdt），不删除段——段的清理由停止脚本
 *               或 shm_remove 负责，避免误删其他进程仍在使用中的共享段。
 *  - 每 key 只映射一次并缓存指针，避免每帧 shmat/shmdt 的页表开销。
 *  - 双读校验：所有 shm_read_* 读两遍并比对前 8 字节（version+frame_id），
 *    不一致则重试 SHM_READ_MAX_TRIES 次，防 64B 结构体撕裂（无锁单写单读）。
 */
#include "shm_ipc.h"
#include "driving_config.h"   /* SHM_KEY_* / SHM_*_SIZE 宏 */
#include "time_util.h"        /* now_us_mono()：失败退避计时 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/ipc.h>

/* key -> 映射指针 的缓存（每 key 至多映射一次） */
#define SHM_CACHE_MAX 12

typedef struct {
    long  key;
    int   shmid;
    void *ptr;
    size_t size;
    int   active;
} shm_cache_entry_t;

static shm_cache_entry_t g_cache[SHM_CACHE_MAX];

/* ----------------------------------------------------------------------
 * 打开失败退避（B 加固 2026-10-07）
 *
 * 问题：读者要打开的"上游段"可能长期不存在。最典型的是 S 板的
 * shm_pcie_img(0x12345679)——那是 M 板 A 进程写的摄像头原图，S 板根本没有
 * PCIe 采集，段永远不存在。而 main_planning 主循环每轮都调 shm_read_pcie_img()，
 * 旧实现在这里每轮都做一次 shmget 系统调用 + 一次 fprintf(stderr)，等于把
 * CPU 和终端带宽全部烧在"重复报同一个错"上（stderr 走 SSH/串口时尤其慢，
 * 实测足以让 UDP 消费速率再掉一个量级，加重收帧恒 0）。
 *
 * 策略：每个 key 记录连续失败次数与"下次允许真正重试"的时刻。
 *   - 退避窗口内：不 syscall、不打印，直接返回 -1（成本≈一次查表）；
 *   - 窗口到期后重试一次：写者后启动仍能自动接上，不丢连通性；
 *   - 打印限流：首次失败必打，之后每 SHM_FAIL_LOG_EVERY_MS 汇总一行。
 * 语义完全不变（该失败仍失败、该成功仍成功），只砍掉重复开销。
 * -------------------------------------------------------------------- */
#define SHM_FAIL_SLOTS        8
#define SHM_FAIL_BACKOFF_MS   1000u   /* 退避窗口：期间不再 shmget */
#define SHM_FAIL_LOG_EVERY_MS 5000u   /* 退避期内每 5s 汇总打印一行 */

typedef struct {
    long     key;
    int      used;
    uint32_t fails;        /* 连续失败次数 */
    uint64_t retry_at_us;  /* 到期前不再尝试 */
    uint64_t last_log_us;  /* 上次打印时刻（限流） */
} shm_fail_slot_t;

static shm_fail_slot_t g_fail[SHM_FAIL_SLOTS];

static shm_fail_slot_t *fail_slot(long key)
{
    shm_fail_slot_t *slot = NULL;
    for (int i = 0; i < SHM_FAIL_SLOTS; i++) {
        if (g_fail[i].used && g_fail[i].key == key) return &g_fail[i];
        if (!g_fail[i].used && !slot) slot = &g_fail[i];
    }
    if (slot) {
        memset(slot, 0, sizeof(*slot));
        slot->used = 1;
        slot->key  = key;
    }
    return slot;   /* 槽位用尽时返回 NULL：退化为旧行为 */
}

static shm_cache_entry_t *cache_find(long key)
{
    for (int i = 0; i < SHM_CACHE_MAX; i++) {
        if (g_cache[i].active && g_cache[i].key == key)
            return &g_cache[i];
    }
    return NULL;
}

static shm_cache_entry_t *cache_alloc(void)
{
    for (int i = 0; i < SHM_CACHE_MAX; i++) {
        if (!g_cache[i].active) {
            memset(&g_cache[i], 0, sizeof(g_cache[i]));
            return &g_cache[i];
        }
    }
    return NULL;
}

int shm_create(const char *name, long key, size_t size, void **ptr)
{
    if (!ptr || size == 0) return -1;

    shm_cache_entry_t *e = cache_find(key);
    if (e) { *ptr = e->ptr; return 0; }

    int shmid = shmget((key_t)key, size, IPC_CREAT | 0666);
    if (shmid < 0) {
        fprintf(stderr, "[SHM] shmget(%s, key=0x%lx) failed: %s\n",
                name ? name : "?", key, strerror(errno));
        return -1;
    }

    void *addr = shmat(shmid, NULL, 0);
    if (addr == (void *)-1) {
        fprintf(stderr, "[SHM] shmat(%s, key=0x%lx) failed: %s\n",
                name ? name : "?", key, strerror(errno));
        return -1;
    }

    e = cache_alloc();
    if (!e) {
        shmdt(addr);
        fprintf(stderr, "[SHM] cache full for key=0x%lx\n", key);
        return -1;
    }
    e->key = key;
    e->shmid = shmid;
    e->ptr = addr;
    e->size = size;
    e->active = 1;

    *ptr = addr;
    return 0;
}

int shm_open(long key, size_t size, void **ptr)
{
    /* B 加固：读者仅打开已存在段，不创建。
     * 写者未启动时 shmget 返回 ENOENT，明确报错而非静默"创建成功"。 */
    if (!ptr || size == 0) return -1;

    shm_cache_entry_t *e = cache_find(key);
    if (e) { *ptr = e->ptr; return 0; }

    /* 失败退避：上游段可能长期不存在（例如 S 板读到 shm_pcie_img）。
     * 退避窗口内直接返回，不做 syscall、不打印，避免拖慢主循环。 */
    const uint64_t now = now_us_mono();
    shm_fail_slot_t *f = fail_slot(key);
    if (f && f->fails > 0 && now < f->retry_at_us) {
        f->fails++;
        if (now - f->last_log_us >= (uint64_t)SHM_FAIL_LOG_EVERY_MS * 1000u) {
            f->last_log_us = now;
            fprintf(stderr, "[SHM] key=0x%lx 仍不可用（已连续失败 %u 次，"
                            "每 %ums 重试一次，不再逐轮刷屏）\n",
                    key, f->fails, SHM_FAIL_BACKOFF_MS);
        }
        return -1;
    }

    int shmid = shmget((key_t)key, size, 0);   /* 无 IPC_CREAT */
    if (shmid < 0) {
        if (f) {
            f->fails++;
            f->retry_at_us = now + (uint64_t)SHM_FAIL_BACKOFF_MS * 1000u;
            /* 首次失败一定打印（保证故障可见），之后限流到每 5s 一行 */
            if (f->fails == 1 ||
                now - f->last_log_us >= (uint64_t)SHM_FAIL_LOG_EVERY_MS * 1000u) {
                f->last_log_us = now;
                fprintf(stderr, "[SHM] shm_open(key=0x%lx) failed: 写者未创建该段? %s"
                                "（后续 %ums 内静默退避重试）\n",
                        key, strerror(errno), SHM_FAIL_BACKOFF_MS);
            }
        } else {
            fprintf(stderr, "[SHM] shm_open(key=0x%lx) failed: 写者未创建该段? %s\n",
                    key, strerror(errno));
        }
        return -1;
    }

    void *addr = shmat(shmid, NULL, 0);
    if (addr == (void *)-1) {
        fprintf(stderr, "[SHM] shmat(key=0x%lx) failed: %s\n", key, strerror(errno));
        return -1;
    }

    e = cache_alloc();
    if (!e) {
        shmdt(addr);
        fprintf(stderr, "[SHM] cache full for key=0x%lx\n", key);
        return -1;
    }
    e->key = key;
    e->shmid = shmid;
    e->ptr = addr;
    e->size = size;
    e->active = 1;

    /* 曾经失败过、现在成功（写者后启动了）：报一次"已接上"并清空退避状态 */
    if (f && f->fails > 0) {
        fprintf(stderr, "[SHM] key=0x%lx 已可用（此前失败 %u 次，现已接上写者）\n",
                key, f->fails);
        memset(f, 0, sizeof(*f));
        f->used = 1;
        f->key  = key;
    }

    *ptr = addr;
    return 0;
}

int shm_close(void *ptr, size_t size)
{
    (void)size;
    if (!ptr) return -1;
    for (int i = 0; i < SHM_CACHE_MAX; i++) {
        if (g_cache[i].active && g_cache[i].ptr == ptr) {
            if (shmdt(ptr) != 0) {
                fprintf(stderr, "[SHM] shmdt failed: %s\n", strerror(errno));
                return -1;
            }
            g_cache[i].active = 0;
            return 0;
        }
    }
    /* 非缓存映射：直接解除（极少见路径） */
    if (shmdt(ptr) != 0) return -1;
    return 0;
}

int shm_remove(long key)
{
    int shmid = shmget((key_t)key, 0, 0);
    if (shmid < 0) {
        if (errno == ENOENT) return 0;   /* 段不存在，视为已清理 */
        fprintf(stderr, "[SHM] shm_remove(key=0x%lx) lookup failed: %s\n",
                key, strerror(errno));
        return -1;
    }
    if (shmctl(shmid, IPC_RMID, NULL) != 0) {
        fprintf(stderr, "[SHM] shm_remove(key=0x%lx) IPC_RMID failed: %s\n",
                key, strerror(errno));
        return -1;
    }
    /* 本进程若仍映射着该段，解除缓存 */
    shm_cache_entry_t *e = cache_find(key);
    if (e) { e->active = 0; e->ptr = NULL; }
    return 0;
}

/* ---- 双读校验：契约结构体前 8 字节均为 version(u32)+frame_id(u32) ---- */
static int read_checked(const void *src, void *dst, size_t size)
{
    uint8_t buf1[64], buf2[64];
    if (!src || !dst || size == 0 || size > 64) return -1;

    for (unsigned t = 0; t < SHM_READ_MAX_TRIES; t++) {
        memcpy(buf1, src, size);
        memcpy(buf2, src, size);
        if (memcmp(buf1, buf2, 8) == 0) {   /* version+frame_id 两读一致 → 未撕裂 */
            memcpy(dst, buf1, size);
            return 0;
        }
    }
    fprintf(stderr, "[SHM] read_checked: key 数据多次撕裂，放弃本次读取\n");
    return -1;
}

/* ---- A → B：弯道结果 ---- */

int shm_write_lane(const LaneResult *r)
{
    if (!r) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_LANE, SHM_LANE_SIZE, &ptr) != 0) return -1;
    memcpy(ptr, r, SHM_LANE_SIZE);
    return 0;
}

int shm_read_lane(LaneResult *r)
{
    if (!r) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_LANE, SHM_LANE_SIZE, &ptr) != 0) return -1;
    return read_checked(ptr, r, SHM_LANE_SIZE);
}

/* ---- A → B / 显示：PCIe 原始图（640x480 RGB565） ---- */

int shm_write_pcie_img(const void *rgb565, size_t bytes)
{
    if (!rgb565 || bytes == 0) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_PCIE_IMG, SHM_IMG_SIZE, &ptr) != 0) return -1;
    size_t n = bytes < SHM_IMG_SIZE ? bytes : SHM_IMG_SIZE;
    memcpy(ptr, rgb565, n);
    return 0;
}

int shm_read_pcie_img(void *rgb565, size_t bytes)
{
    if (!rgb565 || bytes == 0) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_PCIE_IMG, SHM_IMG_SIZE, &ptr) != 0) return -1;
    size_t n = bytes < SHM_IMG_SIZE ? bytes : SHM_IMG_SIZE;
    memcpy(rgb565, ptr, n);
    return 0;
}

/* ---- B → LCD：UDP 接收的远端图（S 板 0x1234567A） ---- */

int shm_write_udp_img(const void *rgb565, size_t bytes)
{
    if (!rgb565 || bytes == 0) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_UDP_IMG, SHM_IMG_SIZE, &ptr) != 0) return -1;
    size_t n = bytes < SHM_IMG_SIZE ? bytes : SHM_IMG_SIZE;
    memcpy(ptr, rgb565, n);
    return 0;
}

int shm_read_udp_img(void *rgb565, size_t bytes)
{
    if (!rgb565 || bytes == 0) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_UDP_IMG, SHM_IMG_SIZE, &ptr) != 0) return -1;
    size_t n = bytes < SHM_IMG_SIZE ? bytes : SHM_IMG_SIZE;
    memcpy(rgb565, ptr, n);
    return 0;
}

/* ---- B → C：控制命令 ---- */

int shm_write_cmd(const ControlCommandMsg *m)
{
    if (!m) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_CMD, SHM_CMD_SIZE, &ptr) != 0) return -1;
    memcpy(ptr, m, SHM_CMD_SIZE);
    return 0;
}

int shm_read_cmd(ControlCommandMsg *m)
{
    if (!m) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_CMD, SHM_CMD_SIZE, &ptr) != 0) return -1;
    return read_checked(ptr, m, SHM_CMD_SIZE);
}

/* ---- B → B/C：行人状态 ---- */

int shm_write_person(const PersonState *s)
{
    if (!s) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_PERSON, SHM_PERSON_SIZE, &ptr) != 0) return -1;
    memcpy(ptr, s, SHM_PERSON_SIZE);
    return 0;
}

int shm_read_person(PersonState *s)
{
    if (!s) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_PERSON, SHM_PERSON_SIZE, &ptr) != 0) return -1;
    return read_checked(ptr, s, SHM_PERSON_SIZE);
}

/* ---- B → LCD：显示模式 ---- */

int shm_write_display(const DisplayMode *m)
{
    if (!m) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_DISPLAY, SHM_DISPLAY_SIZE, &ptr) != 0) return -1;
    memcpy(ptr, m, SHM_DISPLAY_SIZE);
    return 0;
}

int shm_read_display(DisplayMode *m)
{
    if (!m) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_DISPLAY, SHM_DISPLAY_SIZE, &ptr) != 0) return -1;
    return read_checked(ptr, m, SHM_DISPLAY_SIZE);
}

/* ---- A → B：红绿灯灯色结果（10-05 新增） ---- */

int shm_write_traffic_light(const TrafficLightResult *r)
{
    if (!r) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_TRAFFIC_LIGHT, SHM_TL_SIZE, &ptr) != 0) return -1;
    memcpy(ptr, r, SHM_TL_SIZE);
    return 0;
}

int shm_read_traffic_light(TrafficLightResult *r)
{
    if (!r) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_TRAFFIC_LIGHT, SHM_TL_SIZE, &ptr) != 0) return -1;
    return read_checked(ptr, r, SHM_TL_SIZE);
}

/* ---- A → B：斑马线识别结果（10-05 新增） ---- */

int shm_write_zebra(const ZebraResult *r)
{
    if (!r) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_ZEBRA, SHM_ZEBRA_SIZE, &ptr) != 0) return -1;
    memcpy(ptr, r, SHM_ZEBRA_SIZE);
    return 0;
}

int shm_read_zebra(ZebraResult *r)
{
    if (!r) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_ZEBRA, SHM_ZEBRA_SIZE, &ptr) != 0) return -1;
    return read_checked(ptr, r, SHM_ZEBRA_SIZE);
}

/* ---- A → B：虚实线/压线/变道判定结果（10-05 新增） ---- */

int shm_write_lane_mark(const LaneMarkResult *r)
{
    if (!r) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_LANE_MARK, SHM_LANE_MARK_SIZE, &ptr) != 0) return -1;
    memcpy(ptr, r, SHM_LANE_MARK_SIZE);
    return 0;
}

int shm_read_lane_mark(LaneMarkResult *r)
{
    if (!r) return -1;
    void *ptr = NULL;
    if (shm_open(SHM_KEY_LANE_MARK, SHM_LANE_MARK_SIZE, &ptr) != 0) return -1;
    return read_checked(ptr, r, SHM_LANE_MARK_SIZE);
}
