#ifndef STEERING_KEEPALIVE_H
#define STEERING_KEEPALIVE_H
#include "steering_types.h"
typedef struct
{
    uint64_t period_ns, last_ns, count;
    int failed;
} steering_keepalive_t;
steer_result_t steering_keepalive_init(steering_keepalive_t *state, unsigned period_ms);
/* Heartbeat callback is forbidden from producing contact pulses. Skip it during
 * an active pulse/calibration; repeated ticks never produce catch-up bursts. */
steer_result_t steering_keepalive_tick(steering_keepalive_t *state, uint64_t now, int busy,
                                       steer_result_t (*heartbeat)(void *context), void *context);
#endif
