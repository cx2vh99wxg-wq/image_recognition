/*
 * shm_ipc.h — 共享内存接口（【人员 B】实现 .c，A/C 仅使用）
 *
 * 这是三人之间的共享内存契约头文件：
 *   A 通过 shm_write_lane / shm_write_pcie_img 把感知结果和图像交给 B；
 *   C 通过 shm_read_cmd 取得 B 的控制命令。
 * 具体 System V 共享内存实现由 B 负责（common/csrc/shm_ipc.c）。
 *
 * 【B 加固记录 2026-10-06】
 *  1. shm_open 语义改为"仅打开已存在段"（不带 IPC_CREAT）：读者先启动时若
 *     写者未启动会立即失败报错，不再静默"创建成功"掩盖故障。
 *  2. 新增 shm_remove(key)：显式删除共享段（供停止脚本/退出清理，等价 ipcrm）。
 *  3. 所有 shm_read_* 内部做"双读校验"（version+frame_id 读两遍一致才返回），
 *     降低 64B 结构体 memcpy 撕裂风险（无锁单写单读的尽力而为保护）。
 */
#ifndef SHM_IPC_H
#define SHM_IPC_H

#include <stddef.h>
#include "driving_types.h"
#include "driving_config.h"   /* PersonState / DisplayMode 契约（B 侧结构暂存 config） */

/* 打开/创建一段共享内存，返回映射指针（ptr）。成功 0，失败 -1。 */
int shm_create(const char* name, long key, size_t size, void** ptr);   /* 写者：不存在则创建 */
int shm_open(long key, size_t size, void** ptr);                       /* 读者：仅打开已存在段 */
int shm_close(void* ptr, size_t size);
int shm_remove(long key);                                              /* B 新增：删除共享段 */

/* A→B：弯道结果 */
int shm_write_lane(const LaneResult* r);
int shm_read_lane(LaneResult* r);

/* A→B：红绿灯灯色结果（10-05 新增） */
int shm_write_traffic_light(const TrafficLightResult* r);
int shm_read_traffic_light(TrafficLightResult* r);

/* A→B：斑马线识别结果（10-05 新增） */
int shm_write_zebra(const ZebraResult* r);
int shm_read_zebra(ZebraResult* r);

/* A→B：虚实线/压线/变道判定结果（10-05 新增） */
int shm_write_lane_mark(const LaneMarkResult* r);
int shm_read_lane_mark(LaneMarkResult* r);

/* A→B / 显示：PCIe 原始图（640x480 RGB565） */
int shm_write_pcie_img(const void* rgb565, size_t bytes);
int shm_read_pcie_img(void* rgb565, size_t bytes);

/* B→显示：UDP 接收的远端图（640x480 RGB565，S 板 shm_udp_img） */
int shm_write_udp_img(const void* rgb565, size_t bytes);
int shm_read_udp_img(void* rgb565, size_t bytes);

/* B→C：控制命令 */
int shm_write_cmd(const ControlCommandMsg* m);
int shm_read_cmd(ControlCommandMsg* m);

/* B→B/C：行人状态（0x1234567D） */
int shm_write_person(const PersonState* s);
int shm_read_person(PersonState* s);

/* B→LCD：显示模式（0x1234567C） */
int shm_write_display(const DisplayMode* m);
int shm_read_display(DisplayMode* m);

#endif /* SHM_IPC_H */
