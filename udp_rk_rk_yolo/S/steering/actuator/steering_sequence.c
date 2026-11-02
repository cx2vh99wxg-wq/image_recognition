#include "steering_sequence.h"
#include <string.h>
int steering_sequence_active(const steering_sequence_t *s)
{
    return s && (s->status == SEQUENCE_FIRST || s->status == SEQUENCE_GAP ||
                 s->status == SEQUENCE_BRAKE);
}
steer_result_t steering_sequence_init(steering_sequence_t *s, steering_actuator_io_t io)
{
    return steering_sequence_init_limit(s, io, 60);
}
steer_result_t steering_sequence_init_limit(steering_sequence_t *s, steering_actuator_io_t io,
                                            unsigned limit)
{
    if (!s || !limit || limit > 250)
        return STEER_ERR_ARGUMENT;
    memset(s, 0, sizeof(*s));
    return steering_actuator_init(&s->actuator, io, limit);
}
steer_result_t steering_sequence_cancel(steering_sequence_t *s)
{
    if (!s)
        return STEER_ERR_ARGUMENT;
    steer_result_t rc = steering_actuator_stop(&s->actuator);
    s->status = rc == STEER_OK ? SEQUENCE_IDLE : SEQUENCE_FAULT;
    s->error = rc;
    return rc;
}
steer_result_t steering_sequence_start(steering_sequence_t *s, steer_direction_t d, unsigned first,
                                       unsigned brake, unsigned gap)
{
    if (!s || (d != STEER_LEFT && d != STEER_RIGHT) || !first ||
        first > s->actuator.max_pulse_ms * 1000u || brake > s->actuator.max_pulse_ms * 1000u ||
        gap > 100000 || (brake && gap < 2000))
        return STEER_ERR_ARGUMENT;
    if (steering_sequence_active(s))
        return STEER_ERR_BUSY;
    s->direction = d;
    s->first_us = first;
    s->brake_us = brake;
    s->gap_us = gap;
    s->completed = 0;
    memset(s->records, 0, sizeof(s->records));
    s->error = steering_actuator_start_us(&s->actuator, d, first);
    s->status = s->error == STEER_OK ? SEQUENCE_FIRST : SEQUENCE_FAULT;
    return s->error;
}
steer_result_t steering_sequence_tick(steering_sequence_t *s)
{
    if (!s)
        return STEER_ERR_ARGUMENT;
    if (s->status == SEQUENCE_FAULT)
        return s->error;
    if (!steering_sequence_active(s))
        return STEER_OK;
    if (s->status == SEQUENCE_GAP)
    {
        if (s->actuator.io.now_ns(s->actuator.io.context) < s->gap_end_ns)
            return STEER_OK;
        s->error = steering_actuator_start_us(
            &s->actuator, s->direction == STEER_LEFT ? STEER_RIGHT : STEER_LEFT, s->brake_us);
        s->status = s->error == STEER_OK ? SEQUENCE_BRAKE : SEQUENCE_FAULT;
        return s->error;
    }
    steer_direction_t direction = s->actuator.direction;
    steer_result_t rc = steering_actuator_tick(&s->actuator);
    if (rc != STEER_OK)
    {
        s->status = SEQUENCE_FAULT;
        return s->error = rc;
    }
    if (s->actuator.status == ACTUATOR_ACTIVE)
        return STEER_OK;
    steering_actuator_t *a = &s->actuator;
    s->records[s->completed++] =
        (steering_pulse_record_t){direction,        a->requested_us,     a->enable_begin_ns,
                                  a->enable_end_ns, a->release_begin_ns, a->release_end_ns};
    uint64_t elapsed = a->release_end_ns - a->enable_end_ns;
    if (elapsed + 50000 < (uint64_t)a->requested_us * 1000 ||
        elapsed > (uint64_t)a->requested_us * 1000 + 2000000)
    {
        s->status = SEQUENCE_FAULT;
        return s->error = STEER_ERR_IO;
    }
    if (s->status == SEQUENCE_FIRST && s->brake_us)
    {
        s->gap_end_ns = a->release_end_ns + (uint64_t)s->gap_us * 1000;
        s->status = SEQUENCE_GAP;
    }
    else
        s->status = SEQUENCE_DONE;
    return STEER_OK;
}
