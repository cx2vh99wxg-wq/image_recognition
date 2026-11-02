#ifndef MPU9250_TRANSPORT_H
#define MPU9250_TRANSPORT_H
#include <stddef.h>
#include "steering_types.h"
#include "fpga_imu_snapshot.h"

#define MPU9250_WHO_AM_I_REGISTER 0x75u
#define MPU9250_EXPECTED_ID 0x71u
typedef struct {
    void *context;
    /* Logical MPU register reads, NOT the existing S motion register protocol.
     * B implements these through the new M FPGA snapshot/register bridge. */
    steer_result_t (*read_registers)(void *context, uint8_t reg,
                                     uint8_t *output, size_t size);
    steer_result_t (*write_register)(void *context, uint8_t reg, uint8_t value);
} mpu9250_bus_t;
steer_result_t mpu9250_read_identity(const mpu9250_bus_t *bus, uint8_t *identity);
/* A host scheduling delay discards this snapshot and permits bounded retry;
 * invalid/reversed monotonic time remains a hard discontinuity. */
steer_result_t mpu9250_capture_timing(uint64_t begin_ns, uint64_t end_ns);
/* Autonomous M FPGA sampler. Callbacks expose identity only; sensor register
 * configuration belongs to the new FPGA state machine, not S motion regs. */
steer_result_t mpu9250_fpga_open(mpu9250_bus_t *bus);
steer_result_t mpu9250_fpga_snapshot(mpu9250_bus_t *bus, fpga_imu_snapshot_t *out,
                                  uint64_t *capture_midpoint_ns);
typedef struct { uint32_t transfers,io_errors,crc_errors,stale_captures,i2c_errors; } mpu9250_link_stats_t;
void mpu9250_fpga_stats(const mpu9250_bus_t *bus, mpu9250_link_stats_t *stats);
void mpu9250_fpga_close(mpu9250_bus_t *bus);
#endif
