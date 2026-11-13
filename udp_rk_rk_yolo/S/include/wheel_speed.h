#ifndef WHEEL_SPEED_H
#define WHEEL_SPEED_H

#include "control_status.h"

#define WHEEL_PERIOD_UNKNOWN UINT16_MAX
#define WHEEL_PERIOD_MAX 30000u

static inline uint16_t wheel_decode_period(const uint8_t bytes[4])
{
    const uint16_t ticks = (uint16_t)((bytes[1] << 8) | bytes[2]);
    const uint8_t crc = control_crc_byte(control_crc_byte(
        control_crc_byte(0, bytes[0]), bytes[1]), bytes[2]);
    return bytes[0] == 0xb6 && crc == bytes[3] && ticks <= WHEEL_PERIOD_MAX
               ? ticks : WHEEL_PERIOD_UNKNOWN;
}

/* A period tick is 100 us. Zero means the FPGA expired a prior measurement. */
static inline unsigned wheel_speed_tenths(unsigned ticks, unsigned circumference_mm,
                                          unsigned pulses_per_turn)
{
    if (!ticks || ticks > WHEEL_PERIOD_MAX || !pulses_per_turn)
        return 0;
    const uint64_t denominator = (uint64_t)ticks * pulses_per_turn;
    return (unsigned)((360ULL * circumference_mm + denominator / 2) / denominator);
}

static inline unsigned wheel_rpm(unsigned ticks, unsigned pulses_per_turn)
{
    if (!ticks || ticks > WHEEL_PERIOD_MAX || !pulses_per_turn)
        return 0;
    const uint64_t denominator = (uint64_t)ticks * pulses_per_turn;
    return (unsigned)((600000ULL + denominator / 2) / denominator);
}

#endif
