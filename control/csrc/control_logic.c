/*
 * control_logic.c — 控制层纯逻辑实现（【人员 C · 控制与 FPGA】）
 */
#include "control_logic.h"

ctrl_action_t ctrl_map_command(ControlCommand cmd)
{
    switch (cmd) {
        case CMD_GO:    return CTRL_ACT_FORWARD;
        case CMD_BACK:  return CTRL_ACT_BACKWARD;
        case CMD_LEFT:  return CTRL_ACT_LEFT;
        case CMD_RIGHT: return CTRL_ACT_RIGHT;
        case CMD_BRAKE:
        case CMD_STOP:  return CTRL_ACT_STOP;
        case CMD_NONE:
        default:        return CTRL_ACT_NONE;
    }
}

const char *ctrl_action_name(ctrl_action_t a)
{
    switch (a) {
        case CTRL_ACT_FORWARD:  return "FORWARD";
        case CTRL_ACT_BACKWARD: return "BACKWARD";
        case CTRL_ACT_LEFT:     return "LEFT";
        case CTRL_ACT_RIGHT:    return "RIGHT";
        case CTRL_ACT_STOP:     return "STOP";
        case CTRL_ACT_NONE:
        default:                return "NONE";
    }
}
