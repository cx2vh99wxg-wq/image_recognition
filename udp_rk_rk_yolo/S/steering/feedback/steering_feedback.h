#ifndef STEERING_FEEDBACK_H
#define STEERING_FEEDBACK_H
#include "steering_types.h"

/* A/integration extension point: future dedicated UDP receiver on S.
 * Current implementation has no socket and cannot supply live feedback. */
steer_result_t steering_feedback_read(steering_feedback_t *feedback);
#endif
