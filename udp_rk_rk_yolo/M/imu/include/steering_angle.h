#ifndef STEERING_ANGLE_H
#define STEERING_ANGLE_H
#include "steering_types.h"
typedef struct
{
    float axis[3], bias_dps[3];
    double angle_deg;
    float previous_rate_dps;
    uint64_t previous_time_ns;
    uint32_t session_id, previous_sequence;
    unsigned max_gap_ms;
    int have_sample, faulted;
} steering_angle_t;

/* 台架用相对积分器不提供车身补偿 AHRS，也不自行置
 * ZERO_VALID；物理中位须由调用方另行建立。 */
steer_result_t steering_angle_init(steering_angle_t *angle, const float axis[3],
                                   const float bias_dps[3], unsigned max_gap_ms);
/* 重复、倒序、时钟或会话不连续以及长采样间隔均被拒绝；
 * 连续性故障锁存后必须显式重新初始化。 */
steer_result_t steering_angle_update(steering_angle_t *angle, const steering_imu_sample_t *sample,
                                     double *relative_deg, float *rate_dps);
#endif
