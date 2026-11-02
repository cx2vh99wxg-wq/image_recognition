#ifndef MPU9250_TRANSPORT_H
#define MPU9250_TRANSPORT_H
#include <stddef.h>
#include "steering_types.h"
#include "fpga_imu_snapshot.h"

#define MPU9250_WHO_AM_I_REGISTER 0x75u
#define MPU9250_EXPECTED_ID 0x71u
typedef struct
{
    void *context;
    /* 这里读取 MPU 逻辑寄存器，通过 M FPGA
     * 新增的快照/寄存器桥接实现，与 S 端原有运动寄存器协议分离。 */
    steer_result_t (*read_registers)(void *context, uint8_t reg, uint8_t *output, size_t size);
    steer_result_t (*write_register)(void *context, uint8_t reg, uint8_t value);
} mpu9250_bus_t;
steer_result_t mpu9250_read_identity(const mpu9250_bus_t *bus, uint8_t *identity);
/* 若仅因主机调度延迟错过快照，可丢弃后作有限重试；
 * 单调时间非法或倒退仍按硬连续性故障处理。 */
steer_result_t mpu9250_capture_timing(uint64_t begin_ns, uint64_t end_ns);
/* M FPGA 自主采样，回调仅暴露器件身份；传感器寄存器配置归
 * FPGA 新状态机管理，不经 S 运动寄存器。 */
steer_result_t mpu9250_fpga_open(mpu9250_bus_t *bus);
steer_result_t mpu9250_fpga_snapshot(mpu9250_bus_t *bus, fpga_imu_snapshot_t *out,
                                     uint64_t *capture_midpoint_ns);
typedef struct
{
    uint32_t transfers, io_errors, crc_errors, stale_captures, i2c_errors;
} mpu9250_link_stats_t;
void mpu9250_fpga_stats(const mpu9250_bus_t *bus, mpu9250_link_stats_t *stats);
void mpu9250_fpga_close(mpu9250_bus_t *bus);
#endif
