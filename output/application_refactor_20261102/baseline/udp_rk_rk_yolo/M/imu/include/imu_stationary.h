#ifndef IMU_STATIONARY_H
#define IMU_STATIONARY_H
#include "steering_angle.h"
#define IMU_STATIONARY_WINDOW 200
typedef struct {
    float value[IMU_STATIONARY_WINDOW][6], filtered_rate[3], residual_bias[3];
    uint64_t stamp[IMU_STATIONARY_WINDOW], previous_time;
    double sum[6], square[6];
    unsigned count, next, fast_count, quiet_count, bad_count, transitions;
    int holding, filter_ready;
} imu_stationary_t;
void imu_stationary_init(imu_stationary_t *s);
/* Explicit stationary constraint, not an absolute heading reference. A bounded
 * in-memory residual estimate follows thermal drift only while already held.
 * Calibration files/axis/ZERO_VALID never change; very slow yaw is ambiguous. */
steer_result_t steering_angle_update_with_hold(steering_angle_t *angle,
    imu_stationary_t *hold, const steering_imu_sample_t *sample,
    double *relative_deg, float *rate_dps);
#endif
