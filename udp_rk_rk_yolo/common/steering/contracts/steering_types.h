#ifndef STEERING_TYPES_H
#define STEERING_TYPES_H

#include <stdint.h>

/* 两端共用 v1 主机契约：左转为正，中位必须来自物理基准。
 * 这里的主机结构不等于线格式，禁止直接 memcpy 后发送。 */
typedef enum
{
    STEER_OK = 0,
    STEER_ERR_ARGUMENT = -1,
    STEER_ERR_BUSY = -2,
    STEER_ERR_IO = -3,
    STEER_ERR_NOT_READY = -4,
    STEER_ERR_STALE = -5,
    STEER_ERR_DEVICE = -6
} steer_result_t;

typedef enum
{
    STEER_RELEASE = 0, /* 同时释放两个触点；该动作不表示机构已经回到物理中位。 */
    STEER_LEFT = 1,
    STEER_RIGHT = 2
} steer_direction_t;

enum
{
    STEER_FEEDBACK_VALID = 1u,
    STEER_FEEDBACK_ZERO_VALID = 2u,
    STEER_FEEDBACK_SIMULATED = 4u,
    STEER_FEEDBACK_ALL_FLAGS = 7u
};

typedef struct
{
    uint32_t session_id; /* 会话号不得为零，生产者重启或重新置零时必须更换。 */
    uint32_t sequence;   /* 仅新采样递增序号，重发旧采样不推进序号。 */
    uint32_t flags;
    int32_t angle_mdeg;      /* 相对已标定中位的有符号角度，单位为毫度。 */
    int32_t rate_mdps;       /* 有符号角速度，单位为毫度/秒；轴向及正负号与角度保持一致。 */
    uint32_t source_age_ms;  /* 以源端时钟度量的发送时采样年龄。 */
    uint64_t sample_time_ns; /* 时间基准属于源端单调时钟，不属于 S 端时钟。 */
} steering_feedback_t;

typedef struct
{
    uint32_t session_id, sequence, flags;
    uint64_t sample_time_ns;
    float accel_mps2[3];
    float gyro_dps[3];
    float temperature_c;
} steering_imu_sample_t;

static inline const char *steer_result_name(steer_result_t result)
{
    switch (result)
    {
    case STEER_OK:
        return "OK";
    case STEER_ERR_ARGUMENT:
        return "INVALID_ARGUMENT";
    case STEER_ERR_BUSY:
        return "BUSY";
    case STEER_ERR_IO:
        return "IO_ERROR";
    case STEER_ERR_NOT_READY:
        return "NOT_IMPLEMENTED_OR_NOT_READY";
    case STEER_ERR_STALE:
        return "STALE_OR_DISCONTINUOUS";
    case STEER_ERR_DEVICE:
        return "UNEXPECTED_DEVICE";
    default:
        return "UNKNOWN_ERROR";
    }
}
#endif
