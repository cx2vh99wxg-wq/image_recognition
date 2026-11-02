#ifndef STEERING_FEEDBACK_H
#define STEERING_FEEDBACK_H
#include "steering_types.h"

/* 接收器只由一个进程独占，不写 FSPI 或车控状态。 */
steer_result_t steering_feedback_open(const char *bind_ip, const char *source_ip, unsigned port,
                                      unsigned timeout_ms);
steer_result_t steering_feedback_poll(void);
void steering_feedback_close(void);
/* 可移植报文接收接口，供确定性检查及现有接收器复用。 */
steer_result_t steering_feedback_ingest(const uint8_t *packet, unsigned size, uint64_t received_ns);
steer_result_t steering_feedback_read(steering_feedback_t *feedback);
#endif
