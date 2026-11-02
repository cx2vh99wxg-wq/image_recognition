#ifndef STEERING_SEQUENCE_H
#define STEERING_SEQUENCE_H
#include "steering_actuator.h"
typedef enum
{
    SEQUENCE_IDLE,
    SEQUENCE_FIRST,
    SEQUENCE_GAP,
    SEQUENCE_BRAKE,
    SEQUENCE_DONE,
    SEQUENCE_FAULT
} steering_sequence_status_t;
typedef struct
{
    steer_direction_t direction;
    unsigned duration_us;
    uint64_t enable_begin_ns, enable_end_ns, release_begin_ns, release_end_ns;
} steering_pulse_record_t;
typedef struct
{
    steering_actuator_t actuator;
    steering_sequence_status_t status;
    steer_result_t error;
    steer_direction_t direction;
    unsigned first_us, brake_us, gap_us, completed;
    uint64_t gap_end_ns;
    steering_pulse_record_t records[2];
} steering_sequence_t;
steer_result_t steering_sequence_init(steering_sequence_t *s, steering_actuator_io_t io);
/* 这是显式长时单动作表征入口，原初始化仍维持 60ms 限制。 */
steer_result_t steering_sequence_init_limit(steering_sequence_t *s, steering_actuator_io_t io,
                                            unsigned max_phase_ms);
steer_result_t steering_sequence_start(steering_sequence_t *s, steer_direction_t direction,
                                       unsigned first_us, unsigned brake_us, unsigned gap_us);
steer_result_t steering_sequence_tick(steering_sequence_t *s);
steer_result_t steering_sequence_cancel(steering_sequence_t *s);
int steering_sequence_active(const steering_sequence_t *s);
#endif
