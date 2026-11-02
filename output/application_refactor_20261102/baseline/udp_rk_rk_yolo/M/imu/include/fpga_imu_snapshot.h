#ifndef FPGA_IMU_SNAPSHOT_H
#define FPGA_IMU_SNAPSHOT_H
#include "steering_types.h"
#include <stddef.h>
#define FPGA_IMU_PACKET_SIZE 64u
#define FPGA_IMU_SPI_PREFIX 8u
typedef struct {
    uint8_t flags, error, identity, address;
    uint32_t sequence, errors, generation;
    uint64_t sample_us, now_us;
    int16_t accel[3], gyro[3], temperature;
} fpga_imu_snapshot_t;
uint32_t imu_crc32(const uint8_t *data, size_t size);
steer_result_t fpga_imu_decode(fpga_imu_snapshot_t *out, const uint8_t *data, size_t size);
#endif
