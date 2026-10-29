#include "steering_actuator.h"
#include <string.h>

steer_result_t steering_actuator_init(steering_actuator_t *a,
                                    steering_actuator_io_t io, unsigned limit)
{
    if (!a || !io.set_direction || !io.now_ns || !limit ||
        limit > STEERING_HARD_MAX_PULSE_MS) return STEER_ERR_ARGUMENT;
    memset(a, 0, sizeof(*a)); a->io = io; a->max_pulse_ms = limit;
    return STEER_OK;
}
steer_result_t steering_actuator_stop(steering_actuator_t *a)
{
    if (!a || !a->io.set_direction || !a->io.now_ns) return STEER_ERR_ARGUMENT;
    a->release_begin_ns = a->io.now_ns(a->io.context);
    steer_result_t rc = a->io.set_direction(a->io.context, STEER_RELEASE);
    a->release_end_ns = a->io.now_ns(a->io.context);
    a->direction = STEER_RELEASE;
    a->status = rc == STEER_OK ? ACTUATOR_IDLE : ACTUATOR_FAULT;
    a->last_error = rc;
    return rc;
}
steer_result_t steering_actuator_start(steering_actuator_t *a,
                                     steer_direction_t direction, unsigned duration)
{
    if (!a || !a->io.set_direction || !a->io.now_ns ||
        (direction != STEER_LEFT && direction != STEER_RIGHT) ||
        !duration || duration > a->max_pulse_ms) return STEER_ERR_ARGUMENT;
    if (a->status == ACTUATOR_ACTIVE) return STEER_ERR_BUSY;
    if (a->status == ACTUATOR_FAULT) return STEER_ERR_IO;
    if (steering_actuator_stop(a) != STEER_OK) return a->last_error;
    a->requested_ms = duration; a->release_begin_ns = a->release_end_ns = 0;
    a->enable_begin_ns = a->io.now_ns(a->io.context);
    if (!a->enable_begin_ns) {
        a->status = ACTUATOR_FAULT; a->last_error = STEER_ERR_IO;
        return a->last_error;
    }
    steer_result_t rc = a->io.set_direction(a->io.context, direction);
    a->enable_end_ns = a->io.now_ns(a->io.context);
    uint64_t length = (uint64_t)duration * UINT64_C(1000000);
    if (rc != STEER_OK || a->enable_end_ns < a->enable_begin_ns ||
        a->enable_end_ns > UINT64_MAX - length) {
        (void)steering_actuator_stop(a);
        a->status = ACTUATOR_FAULT; a->last_error = STEER_ERR_IO;
        return a->last_error;
    }
    a->deadline_ns = a->enable_end_ns + length;
    a->direction = direction; a->status = ACTUATOR_ACTIVE; a->last_error = STEER_OK;
    return STEER_OK;
}
steer_result_t steering_actuator_tick(steering_actuator_t *a)
{
    if (!a || !a->io.now_ns) return STEER_ERR_ARGUMENT;
    if (a->status == ACTUATOR_FAULT) return a->last_error;
    if (a->status != ACTUATOR_ACTIVE) return STEER_OK;
    uint64_t now = a->io.now_ns(a->io.context);
    if (now < a->enable_end_ns) {
        (void)steering_actuator_stop(a);
        a->status = ACTUATOR_FAULT; a->last_error = STEER_ERR_IO;
        return a->last_error;
    }
    return now >= a->deadline_ns ? steering_actuator_stop(a) : STEER_OK;
}
