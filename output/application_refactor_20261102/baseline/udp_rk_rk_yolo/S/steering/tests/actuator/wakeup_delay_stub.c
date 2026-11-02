/* Test only: deterministic monotonic clock + 2ms late wake. No hardware or
 * real-time scheduling is required. Real pulse widths are checked separately. */
#include <errno.h>
#include <stdint.h>
#include <time.h>

static uint64_t now_ns = UINT64_C(1000000000);

int clock_gettime(clockid_t clock, struct timespec *value)
{
    if (clock != CLOCK_MONOTONIC) { errno = EINVAL; return -1; }
    now_ns += 1000;
    value->tv_sec = (time_t)(now_ns / UINT64_C(1000000000));
    value->tv_nsec = (long)(now_ns % UINT64_C(1000000000));
    return 0;
}

int clock_nanosleep(clockid_t clock, int flags, const struct timespec *request,
                    struct timespec *remaining)
{
    (void)remaining;
    if (clock != CLOCK_MONOTONIC || flags != TIMER_ABSTIME) return EINVAL;
    now_ns = (uint64_t)request->tv_sec * UINT64_C(1000000000) +
        (uint64_t)request->tv_nsec + UINT64_C(2000000);
    return 0;
}
