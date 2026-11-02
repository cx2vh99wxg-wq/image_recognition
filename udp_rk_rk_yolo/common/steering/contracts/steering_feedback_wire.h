#ifndef STEERING_FEEDBACK_WIRE_H
#define STEERING_FEEDBACK_WIRE_H
#include <stddef.h>
#include "steering_types.h"

#define STEERING_FEEDBACK_WIRE_VERSION 1u
#define STEERING_FEEDBACK_WIRE_SIZE 48u

/* STF1 线格式使用大端整数；IEEE CRC32 覆盖字节
 * 0..43。解码只验证帧格式与完整性，不认证时效、零点或模拟标记。
 * 失败不改目标对象，完整契约见 contracts/README.md。 */
steer_result_t steering_feedback_encode(uint8_t *output, size_t capacity,
                                        const steering_feedback_t *feedback);
steer_result_t steering_feedback_decode(steering_feedback_t *feedback, const uint8_t *packet,
                                        size_t size);
#endif
