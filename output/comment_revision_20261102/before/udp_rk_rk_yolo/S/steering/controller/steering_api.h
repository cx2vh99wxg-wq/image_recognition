#ifndef STEERING_API_H
#define STEERING_API_H
#include "steering_types.h"
#include "steering_actuator.h"
#include "steering_response.h"

typedef enum
{
    STEERING_NOT_READY,
    STEERING_MOVING,
    STEERING_SETTLED,
    STEERING_CANCELLED,
    STEERING_FAULT,
    STEERING_SETTLING,
    STEERING_RESOLUTION_LIMIT
} steering_status_t;
typedef struct
{
    float min_angle_deg, max_angle_deg, tolerance_deg;
    unsigned feedback_timeout_ms, max_pulse_ms;
} steering_config_t;
typedef struct
{
    float target_deg, angle_deg, error_deg; /* NAN when unknown. */
    steering_status_t status;
    steer_result_t last_error;
    int feedback_valid;
} steering_state_t;

typedef struct
{
    void *context;
    steering_actuator_io_t actuator;
    steer_result_t (*read_feedback)(void *context, steering_feedback_t *feedback);
    /* Called before start AND while active. STOP/manual/stale IO must deny. */
    steer_result_t (*permission)(void *context, steer_direction_t direction);
    const steering_response_model_t *model;
    unsigned max_pulses, settle_ms, minimum_samples, priority;
} steering_controller_io_t;
steer_result_t steering_bind(steering_controller_io_t io);
/* Only the explicit operator-confirmed reference may call this. Session change
 * revokes it; VALID/ZERO_VALID bits alone never silently establish a center. */
steer_result_t steering_confirm_reference(uint32_t session, float zero_raw_deg);
steer_result_t steering_tick(void);
int steering_pulse_active(void);
uint64_t steering_pulse_deadline_ns(void);
void steering_shutdown(void);
const char *steering_status_name(steering_status_t status);

/* Stage 3 interface. Requires IO/model binding and confirmed current M session.
 * A target is absolute relative to mechanically calibrated center, left +.
 * set_target returns immediately; it must not sleep for a whole turn.
 * cancel releases contacts, never claims to have physically centered. */
steer_result_t steering_init(const steering_config_t *config);
steer_result_t steering_set_target_deg(float target_deg);
/* Explicit per-goal tolerance; the original entry point retains config's
 * tolerance. Runtime uses a wider center band without widening turn targets. */
steer_result_t steering_set_target_with_tolerance_deg(float target_deg, float tolerance_deg);
steer_result_t steering_get_state(steering_state_t *state);
steer_result_t steering_cancel(void);
#endif
