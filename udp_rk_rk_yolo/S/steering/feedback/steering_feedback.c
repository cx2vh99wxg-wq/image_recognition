#include "steering_feedback.h"
#include <string.h>
steer_result_t steering_feedback_read(steering_feedback_t *feedback)
{
    if (!feedback) return STEER_ERR_ARGUMENT;
    memset(feedback, 0, sizeof(*feedback)); /* VALID and ZERO_VALID remain clear. */
    return STEER_ERR_NOT_READY;
}
