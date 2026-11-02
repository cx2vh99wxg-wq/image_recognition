#ifndef BRAKE_POLICY_H
#define BRAKE_POLICY_H

#include <stdint.h>

/* 检测器每帧按 CLOCK_MONOTONIC 秒更新 last_update；
 * 本策略只管理软件请求，不拥有 FPGA STOP 锁存。 */
#define BRAKE_STATUS_TIMEOUT_SECONDS 2u

static inline int brake_required(int enabled, int status_valid, int person_detected,
                                 int brake_enable, uint32_t last_update, uint32_t now)
{
    if (!enabled)
    {
        return 0;
    }

    /* 检测缺失或过期时继续维持软件请求。
     * 撤销请求不等于解除 STOP，人工 REARM
     * 管理锁存，并可显式覆盖当前仍存在的请求。 */
    if (!status_valid || last_update == 0 || last_update > now ||
        now - last_update >= BRAKE_STATUS_TIMEOUT_SECONDS)
    {
        return 1;
    }

    return person_detected != 0 || brake_enable == 0;
}

#endif
