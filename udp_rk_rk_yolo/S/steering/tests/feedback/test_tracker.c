#include "feedback_tracker.h"
#include <assert.h>
#include <stdio.h>

static uint8_t wire[STEERING_FEEDBACK_WIRE_SIZE];
static steer_result_t deliver(feedback_tracker_t *t, steering_feedback_t *f, uint64_t now)
{
    assert(steering_feedback_encode(wire, sizeof(wire), f) == STEER_OK);
    return feedback_tracker_ingest(t, wire, sizeof(wire), now);
}
int main(void)
{
    feedback_tracker_t t;
    assert(feedback_tracker_init(&t, 0) == STEER_ERR_ARGUMENT);
    assert(feedback_tracker_init(&t, 100) == STEER_OK);
    assert(feedback_tracker_status(&t, 1) == FEEDBACK_NONE);
    steering_feedback_t f = {1, UINT32_MAX, STEER_FEEDBACK_VALID, -4321, 2000, 10, 9000000000};
    assert(deliver(&t, &f, 1000000) == STEER_OK);
    assert(feedback_tracker_status(&t, 1000000) == FEEDBACK_RELATIVE);
    assert(t.sample.angle_mdeg == -4321);
    /* Duplicate delivery never renews the local expiry time. */
    assert(deliver(&t, &f, 80000000) == STEER_ERR_STALE);
    assert(t.received_ns == 1000000);
    assert(feedback_tracker_status(&t, 91000000) == FEEDBACK_STALE);
    f.sequence = 0; f.sample_time_ns += 10000000; f.flags = 3;
    assert(deliver(&t, &f, 100000000) == STEER_OK);
    assert(feedback_tracker_status(&t, 100000000) == FEEDBACK_CENTERED);
    f.sequence = UINT32_MAX; f.sample_time_ns += 10000000;
    assert(deliver(&t, &f, 101000000) == STEER_ERR_STALE);
    wire[20] ^= 1;
    assert(feedback_tracker_ingest(&t, wire, sizeof(wire), 102000000) == STEER_ERR_ARGUMENT);
    assert(t.sample.sequence == 0 && t.received_ns == 100000000);
    assert(feedback_tracker_ingest(&t, wire, sizeof(wire)-1, 102000000) == STEER_ERR_ARGUMENT);
    f.sequence = 1; f.sample_time_ns = 1;
    assert(deliver(&t, &f, 103000000) == STEER_ERR_STALE);
    assert(feedback_tracker_status(&t, 103000000) == FEEDBACK_STALE);
    f.sequence = 2; f.sample_time_ns = 999000000000;
    assert(deliver(&t, &f, 104000000) == STEER_ERR_STALE); /* clock fault latches for session */
    f.session_id = 2; f.sequence = 50; f.sample_time_ns = 500; f.flags = 5;
    assert(deliver(&t, &f, 105000000) == STEER_OK);
    assert(feedback_tracker_status(&t, 105000000) == FEEDBACK_SIMULATED);
    f.session_id = 1; f.sequence = 100;
    assert(deliver(&t, &f, 106000000) == STEER_ERR_STALE);
    f.session_id = 2; f.sequence = 51; f.sample_time_ns = 600; f.flags = 0;
    assert(deliver(&t, &f, 107000000) == STEER_OK);
    assert(feedback_tracker_status(&t, 107000000) == FEEDBACK_INVALID);
    f.sequence++; f.sample_time_ns++; f.source_age_ms = UINT32_MAX;
    assert(deliver(&t, &f, 108000000) == STEER_OK);
    assert(feedback_tracker_status(&t, 108000000) == FEEDBACK_STALE);
    assert(feedback_tracker_status(&t, 1) == FEEDBACK_STALE);
    puts("PASS telemetry tracker: CRC/length, signed angles, wrap, duplicate expiry, ordering, clock faults, sessions, simulation, invalidation and age overflow");
    return 0;
}
