/*
 * test_control_logic.c — 控制层纯逻辑单元测试（【人员 C】）
 *
 * 覆盖：
 *   1. 决策命令 → 执行动作映射（ctrl_map_command）；
 *   2. 低有效编码（fspi_encode_enable）；
 *   3. fpga_regmap.h 寄存器地址契约。
 * 本测试不依赖任何设备，可在本机（Linux/WSL）直接运行。
 */
#include "control_logic.h"
#include "fpga_regmap.h"

#include <stdio.h>

static int g_failures = 0;

#define CHECK(cond) do { \
    if (!(cond)) { \
        printf("  FAIL: %s (line %d)\n", #cond, __LINE__); \
        g_failures++; \
    } \
} while (0)

int main(void)
{
    printf("== control_logic 单元测试 ==\n");

    /* 1. 决策命令 → 执行动作映射 */
    printf("[1] 命令映射\n");
    CHECK(ctrl_map_command(CMD_GO)    == CTRL_ACT_FORWARD);
    CHECK(ctrl_map_command(CMD_BACK)  == CTRL_ACT_BACKWARD);
    CHECK(ctrl_map_command(CMD_LEFT)  == CTRL_ACT_LEFT);
    CHECK(ctrl_map_command(CMD_RIGHT) == CTRL_ACT_RIGHT);
    CHECK(ctrl_map_command(CMD_BRAKE) == CTRL_ACT_STOP);
    CHECK(ctrl_map_command(CMD_STOP)  == CTRL_ACT_STOP);
    CHECK(ctrl_map_command(CMD_NONE)  == CTRL_ACT_NONE);
    CHECK(ctrl_map_command((ControlCommand)999) == CTRL_ACT_NONE);   /* 未知→NONE */

    /* 2. 低有效编码（运动=0，停止=1） */
    printf("[2] 低有效编码\n");
    CHECK(fspi_encode_enable(1) == FSPI_VAL_ACTIVE);   /* enable=1 → 0 */
    CHECK(fspi_encode_enable(0) == FSPI_VAL_IDLE);     /* enable=0 → 1 */

    /* 3. 寄存器地址契约 */
    printf("[3] 寄存器地址契约\n");
    CHECK(FSPI_REG_FORWARD  == 0u);
    CHECK(FSPI_REG_BACKWARD == 1u);
    CHECK(FSPI_REG_LEFT     == 2u);
    CHECK(FSPI_REG_RIGHT    == 3u);
    CHECK(FSPI_REG_STOP     == 4u);
    CHECK(FSPI_REG_COUNT    == 5u);
    CHECK(FSPI_CMD_WRITE    == 0x00u);
    CHECK(FSPI_CMD_READ     == 0x80u);

    /* 4. 动作名（调试打印，不为 NULL） */
    printf("[4] 动作名\n");
    CHECK(ctrl_action_name(CTRL_ACT_NONE)     != NULL);
    CHECK(ctrl_action_name(CTRL_ACT_FORWARD)  != NULL);
    CHECK(ctrl_action_name(CTRL_ACT_STOP)     != NULL);

    if (g_failures == 0) {
        printf("ALL PASS\n");
        return 0;
    }
    printf("%d failure(s)\n", g_failures);
    return 1;
}
