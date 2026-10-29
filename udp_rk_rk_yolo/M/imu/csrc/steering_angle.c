#include "steering_angle.h"
#include <math.h>
#include <string.h>

steer_result_t steering_angle_init(steering_angle_t *angle, const float axis[3],
                                  const float bias[3], unsigned max_gap_ms)
{
    if (!angle || !axis || !bias || !max_gap_ms) return STEER_ERR_ARGUMENT;
    double norm = 0;
    for (unsigned i = 0; i < 3; ++i) {
        if (!isfinite(axis[i]) || !isfinite(bias[i])) return STEER_ERR_ARGUMENT;
        norm += (double)axis[i] * axis[i];
    }
    if (norm < 1e-12 || !isfinite(norm)) return STEER_ERR_ARGUMENT;
    memset(angle, 0, sizeof(*angle)); angle->max_gap_ms = max_gap_ms;
    for (unsigned i = 0; i < 3; ++i) {
        angle->axis[i] = (float)(axis[i] / sqrt(norm)); angle->bias_dps[i] = bias[i];
    }
    return STEER_OK;
}
steer_result_t steering_angle_update(steering_angle_t *angle,
                                    const steering_imu_sample_t *sample,
                                    double *relative_deg, float *rate_dps)
{
    if (!angle || !sample || !relative_deg || !rate_dps || !angle->max_gap_ms)
        return STEER_ERR_ARGUMENT;
    if (angle->faulted) return STEER_ERR_STALE;
    if (!(sample->flags & STEER_FEEDBACK_VALID) || !sample->session_id ||
        !sample->sample_time_ns) { angle->faulted = 1; return STEER_ERR_STALE; }
    float projected = 0;
    for (unsigned i = 0; i < 3; ++i) {
        if (!isfinite(sample->gyro_dps[i])) { angle->faulted = 1; return STEER_ERR_ARGUMENT; }
        projected += (sample->gyro_dps[i] - angle->bias_dps[i]) * angle->axis[i];
    }
    if (!isfinite(projected)) { angle->faulted = 1; return STEER_ERR_ARGUMENT; }
    if (angle->have_sample) {
        uint32_t delta_sequence = sample->sequence - angle->previous_sequence;
        if (sample->session_id != angle->session_id || !delta_sequence ||
            delta_sequence >= UINT32_C(0x80000000) ||
            sample->sample_time_ns <= angle->previous_time_ns ||
            sample->sample_time_ns - angle->previous_time_ns >
                (uint64_t)angle->max_gap_ms * UINT64_C(1000000)) {
            angle->faulted = 1; return STEER_ERR_STALE;
        }
        double dt = (sample->sample_time_ns - angle->previous_time_ns) / 1e9;
        angle->angle_deg += .5 * ((double)angle->previous_rate_dps + projected) * dt;
    }
    angle->have_sample = 1; angle->session_id = sample->session_id;
    angle->previous_sequence = sample->sequence;
    angle->previous_time_ns = sample->sample_time_ns; angle->previous_rate_dps = projected;
    *relative_deg = angle->angle_deg; *rate_dps = projected;
    return STEER_OK;
}
