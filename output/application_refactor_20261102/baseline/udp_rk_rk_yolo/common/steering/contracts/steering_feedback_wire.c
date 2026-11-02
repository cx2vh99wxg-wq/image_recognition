#include "steering_feedback_wire.h"
#include <string.h>

static void put32(uint8_t *p, uint32_t value)
{
    p[0] = (uint8_t)(value >> 24); p[1] = (uint8_t)(value >> 16);
    p[2] = (uint8_t)(value >> 8); p[3] = (uint8_t)value;
}
static uint32_t get32(const uint8_t *p)
{
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 |
           (uint32_t)p[2] << 8 | (uint32_t)p[3];
}
static int32_t get_signed32(const uint8_t *p)
{
    uint32_t value = get32(p);
    return value <= INT32_MAX ? (int32_t)value :
        (int32_t)((int64_t)value - INT64_C(4294967296));
}
static uint32_t crc32(const uint8_t *p, size_t size)
{
    uint32_t crc = UINT32_MAX;
    for (size_t n = 0; n < size; ++n) {
        crc ^= p[n];
        for (unsigned bit = 0; bit < 8; ++bit)
            crc = (crc >> 1) ^ ((crc & 1u) ? UINT32_C(0xedb88320) : 0u);
    }
    return ~crc;
}
static int valid(const steering_feedback_t *f)
{
    return f && f->session_id && !(f->flags & ~STEER_FEEDBACK_ALL_FLAGS) &&
        (!(f->flags & STEER_FEEDBACK_ZERO_VALID) || (f->flags & STEER_FEEDBACK_VALID));
}
steer_result_t steering_feedback_encode(uint8_t *p, size_t capacity,
                                      const steering_feedback_t *f)
{
    if (!p || capacity < STEERING_FEEDBACK_WIRE_SIZE || !valid(f))
        return STEER_ERR_ARGUMENT;
    memset(p, 0, STEERING_FEEDBACK_WIRE_SIZE);
    memcpy(p, "STF1", 4); p[5] = STEERING_FEEDBACK_WIRE_VERSION;
    p[7] = STEERING_FEEDBACK_WIRE_SIZE;
    put32(p + 8, f->session_id); put32(p + 12, f->sequence);
    put32(p + 16, f->flags); put32(p + 20, (uint32_t)f->angle_mdeg);
    put32(p + 24, (uint32_t)f->rate_mdps); put32(p + 28, f->source_age_ms);
    put32(p + 32, (uint32_t)(f->sample_time_ns >> 32));
    put32(p + 36, (uint32_t)f->sample_time_ns);
    put32(p + 44, crc32(p, 44));
    return STEER_OK;
}
steer_result_t steering_feedback_decode(steering_feedback_t *f,
                                      const uint8_t *p, size_t size)
{
    if (!f || !p || size != STEERING_FEEDBACK_WIRE_SIZE || memcmp(p, "STF1", 4) ||
        p[4] || p[5] != STEERING_FEEDBACK_WIRE_VERSION || p[6] ||
        p[7] != STEERING_FEEDBACK_WIRE_SIZE || get32(p + 40) ||
        get32(p + 44) != crc32(p, 44)) return STEER_ERR_ARGUMENT;
    steering_feedback_t value = {0};
    value.session_id = get32(p + 8); value.sequence = get32(p + 12);
    value.flags = get32(p + 16); value.angle_mdeg = get_signed32(p + 20);
    value.rate_mdps = get_signed32(p + 24); value.source_age_ms = get32(p + 28);
    value.sample_time_ns = (uint64_t)get32(p + 32) << 32 | get32(p + 36);
    if (!valid(&value)) return STEER_ERR_ARGUMENT;
    *f = value;
    return STEER_OK;
}
