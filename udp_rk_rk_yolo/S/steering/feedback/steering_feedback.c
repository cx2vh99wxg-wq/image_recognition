#define _POSIX_C_SOURCE 200809L
#include "steering_feedback.h"
#include "feedback_tracker.h"
#include "steering_platform.h"
#include <string.h>
#include <limits.h>
#ifndef _WIN32
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#endif
static feedback_tracker_t tracker;
static int ready;
#ifndef _WIN32
static int fd = -1;
static uint32_t source_address;
#endif
void steering_feedback_close(void)
{
#ifndef _WIN32
    if (fd >= 0)
        close(fd);
    fd = -1;
#endif
    ready = 0;
    memset(&tracker, 0, sizeof(tracker));
}
steer_result_t steering_feedback_open(const char *bind_ip, const char *source_ip, unsigned port,
                                      unsigned timeout)
{
    if (!bind_ip || !source_ip || !port || port > 65535 || !timeout || timeout > 250)
        return STEER_ERR_ARGUMENT;
#ifdef _WIN32
    return STEER_ERR_NOT_READY;
#else
    if (fd >= 0)
        return STEER_ERR_BUSY;
    struct sockaddr_in a = {0};
    struct in_addr src;
    a.sin_family = AF_INET;
    a.sin_port = htons((uint16_t)port);
    if (inet_pton(AF_INET, bind_ip, &a.sin_addr) != 1 || inet_pton(AF_INET, source_ip, &src) != 1)
        return STEER_ERR_ARGUMENT;
    int s = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
    if (s < 0)
        return STEER_ERR_IO;
    /* 禁止 REUSEPORT，防止两个消费者静默分走角度包。 */
    if (bind(s, (struct sockaddr *)&a, sizeof(a)))
    {
        int e = errno;
        close(s);
        return e == EADDRINUSE ? STEER_ERR_BUSY : STEER_ERR_IO;
    }
    if (feedback_tracker_init(&tracker, timeout) != STEER_OK)
    {
        close(s);
        return STEER_ERR_ARGUMENT;
    }
    fd = s;
    source_address = src.s_addr;
    ready = 1;
    return STEER_OK;
#endif
}
steer_result_t steering_feedback_ingest(const uint8_t *packet, unsigned size, uint64_t stamp)
{
    if (!ready)
    {
        if (feedback_tracker_init(&tracker, 100) != STEER_OK)
            return STEER_ERR_ARGUMENT;
        ready = 1;
    }
    return feedback_tracker_ingest(&tracker, packet, size, stamp);
}
steer_result_t steering_feedback_poll(void)
{
#ifndef _WIN32
    if (fd >= 0)
        for (unsigned i = 0; i < 64; ++i)
        {
            uint8_t packet[128];
            struct sockaddr_in from;
            socklen_t n = sizeof(from);
            ssize_t size =
                recvfrom(fd, packet, sizeof(packet), MSG_DONTWAIT, (struct sockaddr *)&from, &n);
            if (size < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
                    break;
                return STEER_ERR_IO;
            }
            if (from.sin_family != AF_INET || from.sin_addr.s_addr != source_address)
                continue;
            (void)steering_feedback_ingest(packet, (unsigned)size, steering_now_ns());
        }
#endif
    return ready ? STEER_OK : STEER_ERR_NOT_READY;
}
steer_result_t steering_feedback_read(steering_feedback_t *out)
{
    if (!out)
        return STEER_ERR_ARGUMENT;
    memset(out, 0, sizeof(*out));
    steer_result_t rc = steering_feedback_poll();
    if (rc != STEER_OK)
        return rc;
    uint64_t now = steering_now_ns();
    feedback_status_t status = feedback_tracker_status(&tracker, now);
    if (status != FEEDBACK_RELATIVE && status != FEEDBACK_CENTERED)
        return status == FEEDBACK_NONE ? STEER_ERR_NOT_READY : STEER_ERR_STALE;
    *out = tracker.sample;
    uint64_t held = (now - tracker.received_ns + 999999) / 1000000, age = held + out->source_age_ms;
    if (age >= tracker.timeout_ms || age > UINT32_MAX)
    {
        memset(out, 0, sizeof(*out));
        return STEER_ERR_STALE;
    }
    out->source_age_ms = (uint32_t)age;
    return STEER_OK;
}
