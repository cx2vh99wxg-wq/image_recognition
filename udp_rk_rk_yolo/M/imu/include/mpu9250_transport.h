#ifndef MPU9250_TRANSPORT_H
#define MPU9250_TRANSPORT_H
#include <stddef.h>
#include "steering_types.h"

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
/* Extension point: returns NOT_READY until FPGA + readback protocol exist. */
steer_result_t mpu9250_fpga_open(mpu9250_bus_t *bus);
void mpu9250_fpga_close(mpu9250_bus_t *bus);
#endif
