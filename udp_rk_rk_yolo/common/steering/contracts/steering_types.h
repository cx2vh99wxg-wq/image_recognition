#ifndef STEERING_TYPES_H
#define STEERING_TYPES_H

#include <stdint.h>

/* Shared v1 contract. Left is positive; center is a physical reference.
 * These host structs are NOT wire formats and must never be sent with memcpy. */
typedef enum {
    STEER_OK = 0,
    STEER_ERR_ARGUMENT = -1,
    STEER_ERR_BUSY = -2,
    STEER_ERR_IO = -3,
    STEER_ERR_NOT_READY = -4,
    STEER_ERR_STALE = -5,
    STEER_ERR_DEVICE = -6
} steer_result_t;

typedef enum {
    STEER_RELEASE = 0, /* Release both contacts; does not mean physical center. */
    STEER_LEFT = 1,
    STEER_RIGHT = 2
} steer_direction_t;

enum {
    STEER_FEEDBACK_VALID = 1u,
    STEER_FEEDBACK_ZERO_VALID = 2u,
    STEER_FEEDBACK_SIMULATED = 4u,
    STEER_FEEDBACK_ALL_FLAGS = 7u
};

typedef struct {
    uint32_t session_id; /* Nonzero; change on producer restart/re-zero. */
    uint32_t sequence;   /* Advance only for a new sample, not retransmission. */
    uint32_t flags;
    int32_t angle_mdeg;  /* Relative to calibrated center; signed millidegrees. */
    int32_t rate_mdps;   /* Signed millidegrees/second, same axis/sign as angle. */
    uint32_t source_age_ms; /* Sample age at send time, on the source clock. */
    uint64_t sample_time_ns; /* Source monotonic clock; NOT S's clock. */
} steering_feedback_t;

typedef struct {
    uint32_t session_id, sequence, flags;
    uint64_t sample_time_ns;
    float accel_mps2[3];
    float gyro_dps[3];
    float temperature_c;
} steering_imu_sample_t;

static inline const char *steer_result_name(steer_result_t result)
{
    switch (result) {
    case STEER_OK: return "OK";
    case STEER_ERR_ARGUMENT: return "INVALID_ARGUMENT";
    case STEER_ERR_BUSY: return "BUSY";
    case STEER_ERR_IO: return "IO_ERROR";
    case STEER_ERR_NOT_READY: return "NOT_IMPLEMENTED_OR_NOT_READY";
    case STEER_ERR_STALE: return "STALE_OR_DISCONTINUOUS";
    case STEER_ERR_DEVICE: return "UNEXPECTED_DEVICE";
    default: return "UNKNOWN_ERROR";
    }
}
#endif
