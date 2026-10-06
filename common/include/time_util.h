/*
 * time_util.h — 单调时钟工具（【人员 B】）
 *
 * 决策年龄判断必须使用单调时钟（CLOCK_MONOTONIC），不能用 wall-clock：
 * 系统时间被 NTP/手动修改不会影响决策的"超龄"判定。
 * 非 Linux（本机构建）返回 0。
 */
#ifndef TIME_UTIL_H
#define TIME_UTIL_H

#include <stdint.h>

#if defined(__linux__)
#include <time.h>

static inline uint64_t now_us_mono(void) __attribute__((unused));
static inline uint64_t now_us_mono(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000u + (uint64_t)ts.tv_nsec / 1000u;
}
#else
static inline uint64_t now_us_mono(void) __attribute__((unused));
static inline uint64_t now_us_mono(void) { return 0; }
#endif

#endif /* TIME_UTIL_H */
