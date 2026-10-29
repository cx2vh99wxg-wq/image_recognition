#ifndef STEERING_ANGLE_H
#define STEERING_ANGLE_H
#include "steering_types.h"
typedef struct {
    float axis[3], bias_dps[3];
    double angle_deg;
    float previous_rate_dps;
    uint64_t previous_time_ns;
    uint32_t session_id, previous_sequence;
    unsigned max_gap_ms;
    int have_sample, faulted;
} steering_angle_t;

/* Bench-only relative integrator. It is NOT a chassis-compensated AHRS and
 * never asserts ZERO_VALID. Caller must separately establish physical center. */
steer_result_t steering_angle_init(steering_angle_t *angle, const float axis[3],
                                  const float bias_dps[3], unsigned max_gap_ms);
/* Rejects duplicates, reversal, clock/session discontinuity and long gaps.
 * A discontinuity latches a fault; explicit re-init is required. */
steer_result_t steering_angle_update(steering_angle_t *angle,
                                    const steering_imu_sample_t *sample,
                                    double *relative_deg, float *rate_dps);
#endif
