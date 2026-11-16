#ifndef STEERING_RUNTIME_H
#define STEERING_RUNTIME_H
#include "steering_api.h"
/* 仅供已初始化的统一 FSPI 所有者调用。
 * 已请求闭环时，初始化失败不得回退为旧长脉冲。 */
steer_result_t steering_runtime_init(int bench_mode);
/* Accept only the request code the FPGA has approved. Zero revokes the output gate. */
void steering_runtime_set_hmi_request(uint8_t request);
void steering_runtime_tick(void);
int steering_runtime_available(void);
int steering_runtime_can_engage(void);
void steering_runtime_wait(void);
void steering_runtime_lane(int valid, uint32_t frame, unsigned type, unsigned confidence,
                           unsigned road_angle);
/* 非阻塞读取回调只复制共享内存，不能访问 SPI
 * 或打日志。正式运行每 10ms 检查道路，不依赖
 * 100ms 策略轮次；独立台架工具不轮询道路命令。 */
typedef int (*steering_lane_reader_t)(void *context, uint32_t *frame, unsigned *type,
                                      unsigned *confidence, unsigned *road_angle,
                                      uint64_t *deadline_ns);
void steering_runtime_poll_lane(int allowed, steering_lane_reader_t reader, void *context);
void steering_runtime_cancel(void);
void steering_runtime_close(void);
#endif
