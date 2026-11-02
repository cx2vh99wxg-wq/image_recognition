#include "feedback_tracker.h"
#include <string.h>

steer_result_t feedback_tracker_init(feedback_tracker_t *t, unsigned timeout_ms)
{
    if (!t || !timeout_ms || timeout_ms > 60000) return STEER_ERR_ARGUMENT;
    memset(t, 0, sizeof(*t)); t->timeout_ms = timeout_ms; return STEER_OK;
}
steer_result_t feedback_tracker_ingest(feedback_tracker_t *t, const uint8_t *packet,
                                     size_t size, uint64_t now)
{
    if (!t || !t->timeout_ms || !now) return STEER_ERR_ARGUMENT;
    steering_feedback_t incoming;
    steer_result_t rc = steering_feedback_decode(&incoming, packet, size);
    if (rc != STEER_OK) return rc;
    if (!incoming.sample_time_ns) return STEER_ERR_ARGUMENT;
    if (t->have_sample) {
        if (now < t->received_ns) { t->clock_fault = 1; return STEER_ERR_STALE; }
        if (incoming.session_id == t->sample.session_id) {
            uint32_t advance = incoming.sequence - t->sample.sequence;
            if (!advance || advance >= UINT32_C(0x80000000)) return STEER_ERR_STALE;
            if (t->clock_fault) return STEER_ERR_STALE;
            if (incoming.sample_time_ns <= t->sample.sample_time_ns) {
                t->clock_fault = 1; return STEER_ERR_STALE;
            }
        } else {
            for (unsigned i = 0; i < t->retired_count; ++i)
                if (incoming.session_id == t->retired_sessions[i]) return STEER_ERR_STALE;
            t->retired_sessions[t->retired_next] = t->sample.session_id;
            t->retired_next = (t->retired_next + 1) % 16;
            if (t->retired_count < 16) ++t->retired_count;
            t->clock_fault = 0;
        }
    }
    t->sample = incoming; t->received_ns = now; t->have_sample = 1;
    return STEER_OK;
}
feedback_status_t feedback_tracker_status(const feedback_tracker_t *t, uint64_t now)
{
    if (!t || !t->timeout_ms || !t->have_sample) return FEEDBACK_NONE;
    if (t->clock_fault || now < t->received_ns) return FEEDBACK_STALE;
    /* M and S clocks are not subtracted. This lower bound excludes network transit. */
    uint64_t held_ns = now - t->received_ns;
    uint64_t source_ns = (uint64_t)t->sample.source_age_ms * UINT64_C(1000000);
    uint64_t limit_ns = (uint64_t)t->timeout_ms * UINT64_C(1000000);
    if (source_ns >= limit_ns || held_ns >= limit_ns - source_ns) return FEEDBACK_STALE;
    if (!(t->sample.flags & STEER_FEEDBACK_VALID)) return FEEDBACK_INVALID;
    if (t->sample.flags & STEER_FEEDBACK_SIMULATED) return FEEDBACK_SIMULATED;
    return t->sample.flags & STEER_FEEDBACK_ZERO_VALID ? FEEDBACK_CENTERED : FEEDBACK_RELATIVE;
}
const char *feedback_status_name(feedback_status_t s)
{
    switch (s) {
    case FEEDBACK_NONE: return "NO_SAMPLE";
    case FEEDBACK_STALE: return "STALE";
    case FEEDBACK_INVALID: return "INVALID";
    case FEEDBACK_SIMULATED: return "SIMULATED";
    case FEEDBACK_RELATIVE: return "REAL_RELATIVE";
    case FEEDBACK_CENTERED: return "REAL_ZERO_VALID";
    default: return "UNKNOWN";
    }
}
