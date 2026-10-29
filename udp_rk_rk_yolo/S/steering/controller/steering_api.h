#ifndef STEERING_API_H
#define STEERING_API_H
#include "steering_types.h"

typedef enum { STEERING_NOT_READY, STEERING_MOVING, STEERING_SETTLED,
               STEERING_CANCELLED, STEERING_FAULT } steering_status_t;
typedef struct {
    float min_angle_deg, max_angle_deg, tolerance_deg;
    unsigned feedback_timeout_ms, max_pulse_ms;
} steering_config_t;
typedef struct {
    float target_deg, angle_deg, error_deg; /* NAN when unknown. */
    steering_status_t status;
    steer_result_t last_error;
    int feedback_valid;
} steering_state_t;

/* Stage 3 interface. The supplied scaffold intentionally returns NOT_READY.
 * A target is absolute relative to mechanically calibrated center, left +.
 * set_target returns immediately; it must not sleep for a whole turn.
 * cancel releases contacts, never claims to have physically centered. */
steer_result_t steering_init(const steering_config_t *config);
steer_result_t steering_set_target_deg(float target_deg);
steer_result_t steering_get_state(steering_state_t *state);
steer_result_t steering_cancel(void);
#endif
