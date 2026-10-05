/*
 * shm_ipc.h — 共享内存接口（B 实现 .c，A/C 仅使用）
 *
 * 这是三人之间的共享内存契约头文件：
 *   A 通过 shm_write_lane / shm_write_pcie_img 把感知结果和图像交给 B；
 *   C 通过 shm_read_cmd 取得 B 的控制命令。
 * 具体 System V 共享内存实现由 B 负责（common/csrc/shm_ipc.c）。
 *
 * A 不实现本文件中的函数，只声明接口以保证可编译；最终拼接时链接 B 的实现。
 */
#ifndef SHM_IPC_H
#define SHM_IPC_H

#include <stddef.h>
#include "driving_types.h"

/* 打开/创建一段共享内存，返回映射指针（ptr）。成功 0，失败 -1。 */
int shm_create(const char* name, long key, size_t size, void** ptr);
int shm_open(long key, size_t size, void** ptr);
int shm_close(void* ptr, size_t size);

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

/* B→C：控制命令 */
int shm_write_cmd(const ControlCommandMsg* m);
int shm_read_cmd(ControlCommandMsg* m);

#endif /* SHM_IPC_H */
