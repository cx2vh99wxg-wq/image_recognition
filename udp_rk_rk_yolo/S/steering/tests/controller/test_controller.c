#include "steering_feedback.h"
#include "steering_api.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
int main(void)
{
    steering_feedback_t feedback;
    assert(steering_feedback_read(&feedback) == STEER_ERR_NOT_READY && !feedback.flags);
    steering_config_t config = {-30, 30, 2, 100, 200};
    assert(steering_init(&config) == STEER_ERR_NOT_READY);
    assert(steering_set_target_deg(15) == STEER_ERR_NOT_READY);
    assert(steering_set_target_deg(NAN) == STEER_ERR_ARGUMENT);
    steering_state_t state;
    assert(steering_get_state(&state) == STEER_ERR_NOT_READY && !state.feedback_valid);
    assert(isnan(state.angle_deg) && isnan(state.target_deg));
    assert(steering_cancel() == STEER_ERR_NOT_READY);
    puts("PASS S-side boundaries: unavailable UDP feedback/controller never claims a measured angle");
    return 0;
}
