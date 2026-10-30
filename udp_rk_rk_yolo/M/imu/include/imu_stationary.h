#ifndef IMU_STATIONARY_H
#define IMU_STATIONARY_H
#include "steering_angle.h"
#define IMU_STATIONARY_WINDOW 200
typedef struct {
    float value[IMU_STATIONARY_WINDOW][6];
    uint64_t stamp[IMU_STATIONARY_WINDOW], previous_time;
    double sum[6], square[6];
    unsigned count, next, fast_count;
    int holding;
} imu_stationary_t;
void imu_stationary_init(imu_stationary_t *s);
/* Optional bench constraint. Near-zero constant yaw cannot be distinguished
 * from bias with this IMU alone. Never changes bias or grants ZERO_VALID. */
steer_result_t steering_angle_update_with_hold(steering_angle_t *angle,
    imu_stationary_t *hold, const steering_imu_sample_t *sample,
    double *relative_deg, float *rate_dps);
#endif
