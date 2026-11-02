#include "steering_keepalive.h"
#include <string.h>
steer_result_t steering_keepalive_init(steering_keepalive_t *s, unsigned ms)
{
    if (!s || ms < 100 || ms > 60000)
        return STEER_ERR_ARGUMENT;
    memset(s, 0, sizeof(*s));
    s->period_ns = (uint64_t)ms * 1000000;
    return STEER_OK;
}
steer_result_t steering_keepalive_tick(steering_keepalive_t *s, uint64_t now, int busy,
                                       steer_result_t (*heartbeat)(void *context), void *ctx)
{
    if (!s || !s->period_ns || !now || !heartbeat)
        return STEER_ERR_ARGUMENT;
    if (s->last_ns && now < s->last_ns)
    {
        s->failed = 1;
        return STEER_ERR_STALE;
    }
    if (busy || (s->last_ns && now - s->last_ns < s->period_ns))
        return STEER_OK;
    s->last_ns = now;
    steer_result_t rc = heartbeat(ctx);
    s->failed = rc != STEER_OK;
    if (rc == STEER_OK)
        ++s->count;
    return rc;
}
