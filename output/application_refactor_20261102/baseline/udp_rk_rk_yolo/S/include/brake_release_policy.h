#ifndef BRAKE_RELEASE_POLICY_H
#define BRAKE_RELEASE_POLICY_H

#include <stdint.h>

/* 行人一出现就发布制动请求；连续两秒有效无人只清除软件请求。
 * FPGA 的 STOP 锁存由人工 REARM 解除，人工也可强制解除当前仍有效的报警。 */
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

/* 返回 1 表示软件请求有效。模型失败或时钟异常会重新开始无人确认期。 */
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
