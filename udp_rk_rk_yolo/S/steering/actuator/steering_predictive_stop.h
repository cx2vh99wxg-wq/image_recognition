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
/* 角度及角速度来自同一条真实 M 遥测流和同一确认零点。
 * 延迟包含实测射频、电机及反馈尾部，
 * 不相减双主机时钟；释放立即执行，但不能消除惯性。 */
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
    /* 电机达到运行速度后，实测尾部趋于饱和；
     * 峰值另设保守上界，不能截到平均值。 */
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
