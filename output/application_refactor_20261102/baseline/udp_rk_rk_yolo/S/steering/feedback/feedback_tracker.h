#ifndef STEERING_FEEDBACK_TRACKER_H
#define STEERING_FEEDBACK_TRACKER_H
#include "steering_feedback_wire.h"

/* Bench telemetry only. This does not authorize an actuator or bound network delay. */
typedef enum {
    FEEDBACK_NONE, FEEDBACK_STALE, FEEDBACK_INVALID, FEEDBACK_SIMULATED,
    FEEDBACK_RELATIVE, FEEDBACK_CENTERED
} feedback_status_t;
typedef struct {
    steering_feedback_t sample;
    uint64_t received_ns;
    uint32_t retired_sessions[16];
    unsigned retired_count, retired_next, timeout_ms;
    int have_sample, clock_fault;
} feedback_tracker_t;

steer_result_t feedback_tracker_init(feedback_tracker_t *tracker, unsigned timeout_ms);
steer_result_t feedback_tracker_ingest(feedback_tracker_t *tracker,
                                     const uint8_t *packet, size_t size, uint64_t now_ns);
feedback_status_t feedback_tracker_status(const feedback_tracker_t *tracker, uint64_t now_ns);
const char *feedback_status_name(feedback_status_t status);
#endif
