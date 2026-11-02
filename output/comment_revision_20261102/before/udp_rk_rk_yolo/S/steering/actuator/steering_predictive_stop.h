#ifndef STEERING_PREDICTIVE_STOP_H
#define STEERING_PREDICTIVE_STOP_H
#include "steering_types.h"
#include <math.h>
enum
{
    BURST_CONTINUE = 0,
    BURST_TARGET_RELEASE = 1,
    BURST_GUARD_RELEASE = 2,
    BURST_WRONG_DIRECTION = 3,
    BURST_BAD_PREDICTION = 4
};
/* All angles/rates come from the same real M stream and confirmed reference.
 * Delay includes the observed RF/motor/feedback tail; no dual-host clock
 * subtraction is used. Releasing is immediate; it cannot erase inertia. */
static inline int steering_predictive_release_capped(float angle, float rate,
                                                     steer_direction_t direction, float target,
                                                     float tolerance, unsigned coast_ms,
                                                     unsigned guard_ms, float limit,
                                                     float coast_cap, float guard_cap)
{
    if (!isfinite(angle) || !isfinite(rate) || !isfinite(target) || !isfinite(tolerance) ||
        !isfinite(limit) || (direction != STEER_LEFT && direction != STEER_RIGHT) ||
        tolerance <= 0 || tolerance > 3 || limit <= 0 || limit > 18 || fabsf(target) > limit ||
        coast_ms > 200 || guard_ms < coast_ms || guard_ms > 500 || !isfinite(coast_cap) ||
        !isfinite(guard_cap) || coast_cap < 0 || guard_cap < coast_cap || guard_cap > 18 ||
        ((coast_cap == 0) != (guard_cap == 0)))
        return BURST_BAD_PREDICTION;
    int sign = direction == STEER_LEFT ? 1 : -1;
    if (fabsf(angle) >= limit)
        return BURST_GUARD_RELEASE;
    if (rate * sign < -5)
        return BURST_WRONG_DIRECTION;
    float outward = angle * rate > 0 ? fabsf(rate) : 0;
    float guard = outward * guard_ms / 1000.f, coast = rate * coast_ms / 1000.f;
    /* The measured tail saturates as the motor reaches its running speed.
     * Keep a separate conservative peak cap; do not clip it to the mean. */
    if (guard_cap)
    {
        guard = fminf(guard, guard_cap);
        coast = copysignf(fminf(fabsf(coast), coast_cap), rate);
    }
    if (fabsf(angle) + guard + .5f >= limit)
        return BURST_GUARD_RELEASE;
    float projected = angle + coast;
    if ((rate * sign > 1 && (target - projected) * sign <= 0) ||
        (fabsf(target - angle) <= tolerance && fabsf(target - projected) <= tolerance))
        return BURST_TARGET_RELEASE;
    return BURST_CONTINUE;
}
static inline int steering_predictive_release(float angle, float rate, steer_direction_t direction,
                                              float target, float tolerance, unsigned coast_ms,
                                              unsigned guard_ms, float limit)
{
    return steering_predictive_release_capped(angle, rate, direction, target, tolerance, coast_ms,
                                              guard_ms, limit, 0, 0);
}
#endif
