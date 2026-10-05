/*
 * shm_ipc.c — 共享内存接口实现（B 主责，A 提供可编译初版供联调）
 *
 * 使用 System V 共享内存：shmget/shmat/shmdt。
 *  - shm_create：不存在则创建（IPC_CREAT|0666），已存在则直接打开；
 *  - shm_close：仅解除本进程映射（shmdt），不删除段——段的清理由
 *    启动/停止脚本负责（见 scripts/stop_all.sh），避免误删其他进程仍
 *    在使用的共享段。
 *  - 每 key 只映射一次并缓存指针，避免每帧 shmat/shmdt 的页表开销。
 *
 * 注意：本文件属于 common/，由【人员 B】最终维护；A 提供的初版按
 * common/include/shm_ipc.h 契约实现，B 可在此基础上完善/复核。
 */
#include "shm_ipc.h"
#include "driving_config.h"   /* SHM_KEY_* / SHM_*_SIZE 宏 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/ipc.h>

/* key -> 映射指针 的缓存（每 key 至多映射一次） */
#define SHM_CACHE_MAX 8

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
    /* 与 create 相同：System V 下打开/创建均可，幂等 */
    return shm_create(NULL, key, size, ptr);
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
    memcpy(r, ptr, SHM_LANE_SIZE);
    return 0;
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
    memcpy(m, ptr, SHM_CMD_SIZE);
    return 0;
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
    memcpy(r, ptr, SHM_TL_SIZE);
    return 0;
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
    memcpy(r, ptr, SHM_ZEBRA_SIZE);
    return 0;
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
    memcpy(r, ptr, SHM_LANE_MARK_SIZE);
    return 0;
}
