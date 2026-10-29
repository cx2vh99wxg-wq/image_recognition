#include "steering_api.h"
#include <math.h>

steer_result_t steering_init(const steering_config_t *config)
{
    if (!config || !isfinite(config->min_angle_deg) || !isfinite(config->max_angle_deg) ||
        config->min_angle_deg >= config->max_angle_deg ||
        !isfinite(config->tolerance_deg) || config->tolerance_deg <= 0 ||
        !config->feedback_timeout_ms || !config->max_pulse_ms)
        return STEER_ERR_ARGUMENT;
    return STEER_ERR_NOT_READY;
}
steer_result_t steering_set_target_deg(float target_deg)
{
    return isfinite(target_deg) ? STEER_ERR_NOT_READY : STEER_ERR_ARGUMENT;
}
steer_result_t steering_get_state(steering_state_t *state)
{
    if (!state) return STEER_ERR_ARGUMENT;
    *state = (steering_state_t){NAN, NAN, NAN, STEERING_NOT_READY,
                               STEER_ERR_NOT_READY, 0};
    return STEER_ERR_NOT_READY;
}
steer_result_t steering_cancel(void) { return STEER_ERR_NOT_READY; }
