/*
 * control_logic.h — 控制层纯逻辑（【人员 C · 控制与 FPGA】）
 *
 * 把 B 的决策命令（ControlCommand，见 common/include/driving_types.h）映射为
 * 执行层动作（转向/前进/后退/急停）。本模块**不依赖**任何系统调用与设备，
 * 可在本机单元测试（test/test_control_logic.c），与 B 的 decision.c 同属
 * 「纯逻辑 + 单测」模式。
 *
 * 映射关系（决策命令 → 执行动作）：
 *   CMD_GO    → CTRL_ACT_FORWARD   （直行：前进 + 转向回中）
 *   CMD_BACK  → CTRL_ACT_BACKWARD  （后退 + 转向回中）
 *   CMD_LEFT  → CTRL_ACT_LEFT      （左转）
 *   CMD_RIGHT → CTRL_ACT_RIGHT     （右转）
 *   CMD_BRAKE → CTRL_ACT_STOP      （制动）
 *   CMD_STOP  → CTRL_ACT_STOP      （急停，最高优先）
 *   CMD_NONE  → CTRL_ACT_NONE      （无动作/保持）
 */
#ifndef CONTROL_LOGIC_H
#define CONTROL_LOGIC_H

#include "driving_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 执行层动作 */
typedef enum {
    CTRL_ACT_NONE     = 0,   /* 无动作（保持当前状态） */
    CTRL_ACT_FORWARD  = 1,   /* 前进（+转向回中） */
    CTRL_ACT_BACKWARD = 2,   /* 后退（+转向回中） */
    CTRL_ACT_LEFT     = 3,   /* 左转 */
    CTRL_ACT_RIGHT    = 4,   /* 右转 */
    CTRL_ACT_STOP     = 5    /* 急停/制动（停所有 + 急停寄存器） */
} ctrl_action_t;

/* 决策命令 → 执行层动作（纯映射，无副作用） */
ctrl_action_t ctrl_map_command(ControlCommand cmd);

/* 执行层动作名（调试打印） */
const char *ctrl_action_name(ctrl_action_t a);

#ifdef __cplusplus
}
#endif

#endif /* CONTROL_LOGIC_H */
