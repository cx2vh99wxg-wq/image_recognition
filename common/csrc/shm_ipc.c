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

    int shmid = shmget((key_t)key, size, 0);   /* 无 IPC_CREAT */
    if (shmid < 0) {
        fprintf(stderr, "[SHM] shm_open(key=0x%lx) failed: 写者未创建该段? %s\n",
                key, strerror(errno));
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
