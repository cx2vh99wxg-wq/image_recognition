#ifndef STEERING_API_H
#define STEERING_API_H
#include "steering_types.h"
#include "steering_actuator.h"
#include "steering_response.h"

typedef enum
{
    STEERING_NOT_READY,
    STEERING_MOVING,
    STEERING_SETTLED,
    STEERING_CANCELLED,
    STEERING_FAULT,
    STEERING_SETTLING,
    STEERING_RESOLUTION_LIMIT
} steering_status_t;
typedef struct
{
    float min_angle_deg, max_angle_deg, tolerance_deg;
    unsigned feedback_timeout_ms, max_pulse_ms;
} steering_config_t;
typedef struct
{
    float target_deg, angle_deg, error_deg; /* 转向控制：NAN when unknown. */
    steering_status_t status;
    steer_result_t last_error;
    int feedback_valid;
} steering_state_t;

typedef struct
{
    void *context;
    steering_actuator_io_t actuator;
    steer_result_t (*read_feedback)(void *context, steering_feedback_t *feedback);
    /* 启动前及脉冲有效期间均检查，STOP、人工接管或 IO 过期必须否决输出。 */
    steer_result_t (*permission)(void *context, steer_direction_t direction);
    const steering_response_model_t *model;
    unsigned max_pulses, settle_ms, minimum_samples, priority;
} steering_controller_io_t;
steer_result_t steering_bind(steering_controller_io_t io);
/* 仅显式确认过的操作者零点可进入此接口；会话变化即撤销，
 * VALID/ZERO_VALID 标记本身不能悄悄建立中位。 */
steer_result_t steering_confirm_reference(uint32_t session, float zero_raw_deg);
steer_result_t steering_tick(void);
int steering_pulse_active(void);
uint64_t steering_pulse_deadline_ns(void);
void steering_shutdown(void);
const char *steering_status_name(steering_status_t status);

/* 第三阶段闭环接口先要求绑定 IO、映射模型及当前
 * M 会话零点。目标为相对机械中位的绝对角，
 * 左正；设置目标立即返回，不整段等待转向。
 * 取消只释放触点，不宣称机构已物理回中。 */
steer_result_t steering_init(const steering_config_t *config);
steer_result_t steering_set_target_deg(float target_deg);
/* 每个目标可显式给定容差；旧入口继续使用配置容差。
 * 运行时可放宽回中带，而不随之放宽转弯目标。 */
steer_result_t steering_set_target_with_tolerance_deg(float target_deg, float tolerance_deg);
steer_result_t steering_get_state(steering_state_t *state);
steer_result_t steering_cancel(void);
#endif
