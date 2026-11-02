#ifndef STEERING_PLATFORM_H
#define STEERING_PLATFORM_H

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <sys/stat.h>
static inline FILE *steering_create_csv(const char *path)
{
    int fd = _open(path, _O_CREAT | _O_EXCL | _O_WRONLY | _O_TEXT, _S_IREAD | _S_IWRITE);
    if (fd < 0) return NULL;
    FILE *file = _fdopen(fd, "w");
    if (!file) _close(fd);
    return file;
}
static inline uint64_t steering_now_ns(void)
{
    LARGE_INTEGER value, frequency;
    if (!QueryPerformanceCounter(&value) || !QueryPerformanceFrequency(&frequency))
        return 0;
    return (uint64_t)(value.QuadPart / frequency.QuadPart) * UINT64_C(1000000000)
        + (uint64_t)(value.QuadPart % frequency.QuadPart) * UINT64_C(1000000000)
        / (uint64_t)frequency.QuadPart;
}
static inline void steering_sleep_ms(unsigned milliseconds) { Sleep(milliseconds); }
#else
#include <time.h>
#include <unistd.h>
static inline FILE *steering_create_csv(const char *path)
{
    int fd = open(path, O_CREAT | O_EXCL | O_WRONLY, 0666);
    if (fd < 0) return NULL;
    FILE *file = fdopen(fd, "w");
    if (!file) close(fd);
    return file;
}
static inline uint64_t steering_now_ns(void)
{
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value)) return 0;
    return (uint64_t)value.tv_sec * UINT64_C(1000000000) + (uint64_t)value.tv_nsec;
}
static inline void steering_sleep_ms(unsigned milliseconds)
{
    /* The caller checks an absolute deadline and cancellation after each sleep. */
    struct timespec value = {(time_t)(milliseconds / 1000),
                             (long)(milliseconds % 1000) * 1000000L};
    (void)nanosleep(&value, NULL);
}
#endif
#endif
