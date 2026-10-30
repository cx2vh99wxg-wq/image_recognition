#ifndef STEERING_IMU_SOURCE_H
#define STEERING_IMU_SOURCE_H
#include "mpu9250_transport.h"
typedef enum { IMU_SOURCE_MOCK, IMU_SOURCE_FPGA } imu_source_kind_t;
typedef struct {
    imu_source_kind_t kind;
    int opened;
    float mock_rate_dps;
    uint32_t sequence;
    mpu9250_bus_t bus;
    uint32_t session_id, generation;
    uint64_t anchor_fpga_us, anchor_host_ns;
    uint64_t captured_age_ns, capture_host_ns;
    int anchored;
} imu_source_t;
steer_result_t imu_source_open(imu_source_t *source, imu_source_kind_t kind,
                               float mock_rate_dps);
steer_result_t imu_source_read(imu_source_t *source, steering_imu_sample_t *sample);
void imu_source_close(imu_source_t *source);
#endif
