#include "mpu9250_transport.h"
#include <string.h>

steer_result_t mpu9250_read_identity(const mpu9250_bus_t *bus, uint8_t *identity)
{
    if (!bus || !bus->read_registers || !identity) return STEER_ERR_ARGUMENT;
    uint8_t value = 0;
    steer_result_t rc = bus->read_registers(bus->context, MPU9250_WHO_AM_I_REGISTER,
                                          &value, 1);
    if (rc != STEER_OK) return rc;
    if (value != MPU9250_EXPECTED_ID) return STEER_ERR_DEVICE;
    *identity = value;
    return STEER_OK;
}
steer_result_t mpu9250_fpga_open(mpu9250_bus_t *bus)
{
    if (!bus) return STEER_ERR_ARGUMENT;
    memset(bus, 0, sizeof(*bus));
    /* Do not open /dev/spidev or invent a register address/opcode here.
     * The current bitstream has no MPU bridge. B replaces this boundary. */
    return STEER_ERR_NOT_READY;
}
void mpu9250_fpga_close(mpu9250_bus_t *bus)
{
    if (bus) memset(bus, 0, sizeof(*bus));
}
