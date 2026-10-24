#ifndef BRAKE_RELEASE_POLICY_H
#define BRAKE_RELEASE_POLICY_H

#include <stdint.h>

/* 行人一出现就制动；只有连续两秒有效、无人的检测结果才能解除制动。 */
#define BRAKE_RELEASE_HOLD_MS UINT64_C(2000)

typedef struct {
    uint64_t clear_since_ms;
    int clear_pending;
} BrakeReleaseState;

static inline void brake_release_reset(BrakeReleaseState *state)
{
    if (state) {
        state->clear_since_ms = 0;
        state->clear_pending = 0;
    }
}

/* 返回 1 表示必须制动。模型失败或时钟异常会重新开始无人确认期。 */
static inline int brake_release_required(BrakeReleaseState *state,
                                         int person_detected, int detection_valid,
                                         int clock_valid, uint64_t now_ms)
{
    if (!state) {
        return 1;
    }

    if (person_detected || !detection_valid || !clock_valid) {
        brake_release_reset(state);
        return 1;
    }

    if (!state->clear_pending || now_ms < state->clear_since_ms) {
        state->clear_since_ms = now_ms;
        state->clear_pending = 1;
        return 1;
    }

    return now_ms - state->clear_since_ms < BRAKE_RELEASE_HOLD_MS;
}

#endif
