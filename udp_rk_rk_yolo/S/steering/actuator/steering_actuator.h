#ifndef STEERING_ACTUATOR_H
#define STEERING_ACTUATOR_H
#include "steering_types.h"

#define STEERING_HARD_MAX_PULSE_MS 1000u
typedef struct {
    void *context;
    /* RELEASE must attempt BOTH releases even when the first write fails.
     * LEFT/RIGHT must release the opposite contact before enabling one. */
    steer_result_t (*set_direction)(void *context, steer_direction_t direction);
    uint64_t (*now_ns)(void *context);
} steering_actuator_io_t;
typedef enum { ACTUATOR_IDLE, ACTUATOR_ACTIVE, ACTUATOR_FAULT } actuator_status_t;
typedef struct {
    steering_actuator_io_t io;
    actuator_status_t status;
    steer_direction_t direction;
    steer_result_t last_error;
    unsigned max_pulse_ms, requested_ms;
    uint64_t enable_begin_ns, enable_end_ns, deadline_ns;
    uint64_t release_begin_ns, release_end_ns;
} steering_actuator_t;

steer_result_t steering_actuator_init(steering_actuator_t *actuator,
                                    steering_actuator_io_t io, unsigned max_pulse_ms);
/* Nonblocking after the initial register writes. Caller must keep ticking. */
steer_result_t steering_actuator_start(steering_actuator_t *actuator,
                                     steer_direction_t direction, unsigned duration_ms);
steer_result_t steering_actuator_tick(steering_actuator_t *actuator);
steer_result_t steering_actuator_stop(steering_actuator_t *actuator);
#endif
