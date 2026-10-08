/*
 * main_control.c — S 端控制主循环（【人员 C · 控制与 FPGA】，运行于 S 板 RK3568）
 *
 * 职责：从 B 的共享段读取决策结果并落地到 FSPI 执行器（FPGA → 电机）。
 * 决策不再由 C 自行判断（旧版 main.c 自行读弯道/行人做转向/制动），而是：
 *   1. 读 shm_cmd（ControlCommandMsg，B 写）→ 映射为执行动作并落地 FSPI；
 *   2. 读 shm_person（PersonState，B 写）→ 行人安全兜底（与 B 优先级 1 一致），
 *      检出高置信度行人时强制急停，作为决策之外的第二重保险。
 *
 * 与旧版 udp_rk_rk_yolo/S/fspi/csrc/main.c 的对应：
 *   - 转向时序 FSM（turn_to_left/right/neutral + usleep 时序）→ steer_to()，时序常量不变；
 *   - 急停（人员检测写寄存器 4=0 / 无人写 1）→ 边沿触发 trigger/release；
 *   - 删除旧版 FSPI 寄存器 0 读按钮/显示切换（显示已归 B 的 render_lcd 负责）。
 *
 * 用法：./control_main
 */
#include "fspi_driver.h"
#include "control_logic.h"
#include "fpga_regmap.h"
#include "shm_ipc.h"
#include "driving_config.h"
#include "time_util.h"

#define LOG_TAG "CONTROL"
#include "log.h"

#include <stdio.h>
#include <string.h>
#include <signal.h>

#if defined(__linux__)
#include <sys/types.h>
#include <unistd.h>
#endif

/* ---- 转向时序（微秒，与原 TIME_* 常量完全一致） ---- */
#define STEER_NEUTRAL_TO_LEFT_US   500000u    /* 中间→左 */
#define STEER_NEUTRAL_TO_RIGHT_US  600000u    /* 中间→右 */
#define STEER_LEFT_TO_NEUTRAL_US   400000u    /* 左→中间 */
#define STEER_RIGHT_TO_NEUTRAL_US  400000u    /* 右→中间 */
#define STEER_LEFT_TO_RIGHT_US     1500000u   /* 左→右 */

typedef enum {
    WHEEL_LEFT,
    WHEEL_NEUTRAL,
    WHEEL_RIGHT
} wheel_pos_t;

static volatile int g_running = 1;
static void on_signal(int sig) { (void)sig; g_running = 0; }

static void sleep_us(uint32_t us)
{
#if defined(__linux__)
    usleep((useconds_t)us);
#else
    (void)us;
#endif
}

/* ---- 转向执行（阻塞时序 FSM，与原 turn_to_left/right/neutral 一致） ---- */
static void steer_to(fspi_driver_t *fspi, wheel_pos_t *pos, wheel_pos_t target)
{
    if (*pos == target)
        return;

    switch (target) {
        case WHEEL_LEFT:
            if (*pos == WHEEL_RIGHT) {
                fspi_driver_move(fspi, FSPI_MOVE_LEFT, 1);
                sleep_us(STEER_LEFT_TO_RIGHT_US);
            } else {
                fspi_driver_move(fspi, FSPI_MOVE_LEFT, 1);
                sleep_us(STEER_NEUTRAL_TO_LEFT_US);
            }
            fspi_driver_move(fspi, FSPI_MOVE_LEFT, 0);
            break;

        case WHEEL_RIGHT:
            if (*pos == WHEEL_LEFT) {
                fspi_driver_move(fspi, FSPI_MOVE_RIGHT, 1);
                sleep_us(STEER_LEFT_TO_RIGHT_US);
            } else {
                fspi_driver_move(fspi, FSPI_MOVE_RIGHT, 1);
                sleep_us(STEER_NEUTRAL_TO_RIGHT_US);
            }
            fspi_driver_move(fspi, FSPI_MOVE_RIGHT, 0);
            break;

        case WHEEL_NEUTRAL:
        default:
            if (*pos == WHEEL_LEFT) {
                fspi_driver_move(fspi, FSPI_MOVE_RIGHT, 1);
                sleep_us(STEER_LEFT_TO_NEUTRAL_US);
            } else {
                fspi_driver_move(fspi, FSPI_MOVE_LEFT, 1);
                sleep_us(STEER_RIGHT_TO_NEUTRAL_US);
            }
            fspi_driver_move(fspi, FSPI_MOVE_LEFT, 0);
            fspi_driver_move(fspi, FSPI_MOVE_RIGHT, 0);
            break;
    }
    *pos = target;
}

/* ---- 急停边沿触发：写 0 触发（下降沿），写 1 复位（重新武装） ---- */
static void trigger_stop(fspi_driver_t *fspi, uint8_t *stop_active)
{
    if (!*stop_active) {
        fspi_driver_emergency_stop(fspi);   /* 写 0 */
        *stop_active = 1;
    }
}

static void release_stop(fspi_driver_t *fspi, uint8_t *stop_active)
{
    if (*stop_active) {
        fspi_driver_emergency_release(fspi);   /* 写 1 */
        *stop_active = 0;
    }
}

/* ---- 应用执行动作 ---- */
static void apply_action(fspi_driver_t *fspi, wheel_pos_t *pos,
                         ctrl_action_t action, uint8_t *stop_active)
{
    switch (action) {
        case CTRL_ACT_FORWARD:
            fspi_driver_move(fspi, FSPI_MOVE_BACKWARD, 0);   /* 停后退 */
            steer_to(fspi, pos, WHEEL_NEUTRAL);
            fspi_driver_move(fspi, FSPI_MOVE_FORWARD, 1);    /* 前进 */
            release_stop(fspi, stop_active);
            break;

        case CTRL_ACT_BACKWARD:
            fspi_driver_move(fspi, FSPI_MOVE_FORWARD, 0);    /* 停前进 */
            steer_to(fspi, pos, WHEEL_NEUTRAL);
            fspi_driver_move(fspi, FSPI_MOVE_BACKWARD, 1);   /* 后退 */
            release_stop(fspi, stop_active);
            break;

        case CTRL_ACT_LEFT:
            steer_to(fspi, pos, WHEEL_LEFT);
            release_stop(fspi, stop_active);
            break;

        case CTRL_ACT_RIGHT:
            steer_to(fspi, pos, WHEEL_RIGHT);
            release_stop(fspi, stop_active);
            break;

        case CTRL_ACT_STOP:
            steer_to(fspi, pos, WHEEL_NEUTRAL);
            fspi_driver_move(fspi, FSPI_MOVE_FORWARD, 0);
            fspi_driver_move(fspi, FSPI_MOVE_BACKWARD, 0);
            trigger_stop(fspi, stop_active);
            break;

        case CTRL_ACT_NONE:
        default:
            break;   /* 保持现状 */
    }
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    /* ---- FSPI 初始化 ---- */
    fspi_driver_t *fspi = NULL;
    if (fspi_driver_init(&fspi) != 0) {
        LOGE("FSPI 初始化失败，退出\n");
        return -1;
    }

    /* ---- 初始化所有控制寄存器为停止（写 1，与原实现一致） ---- */
    for (uint8_t r = 0; r < FSPI_REG_COUNT; r++) {
        if (fspi_driver_write_reg(fspi, r, (uint8_t)FSPI_VAL_IDLE) != 0)
            LOGW("寄存器 %u 初始化失败\n", (unsigned)r);
    }

    /* ---- 控制状态 ---- */
    wheel_pos_t wheel = WHEEL_NEUTRAL;
    uint8_t     stop_active = 0;

    LOGI("控制循环启动（周期 %u ms）\n", (unsigned)DEC_LOOP_MS);

    while (g_running) {
        ControlCommandMsg cmd;
        PersonState       person;
        memset(&cmd, 0, sizeof(cmd));
        memset(&person, 0, sizeof(person));

        int have_cmd = (shm_read_cmd(&cmd) == 0 && cmd.version == CMDMSG_VERSION);
        int have_person = (shm_read_person(&person) == 0 &&
                           person.version == PERSON_VERSION);

        ctrl_action_t action = have_cmd ? ctrl_map_command(cmd.command) : CTRL_ACT_NONE;

        /* 行人安全兜底（与 B 优先级 1 一致，双重保险） */
        if (have_person && person.detected &&
            person.confidence >= DEC_BRAKE_CONF_MIN) {
            action = CTRL_ACT_STOP;
        }

        if (action != CTRL_ACT_NONE)
            apply_action(fspi, &wheel, action, &stop_active);

        sleep_us((uint32_t)DEC_LOOP_MS * 1000u);
    }

    /* ---- 退出清理：停止所有运动 + 复位急停 ---- */
    LOGI("退出清理：停止所有运动\n");
    steer_to(fspi, &wheel, WHEEL_NEUTRAL);
    fspi_driver_move(fspi, FSPI_MOVE_FORWARD, 0);
    fspi_driver_move(fspi, FSPI_MOVE_BACKWARD, 0);
    release_stop(fspi, &stop_active);
    fspi_driver_deinit(fspi);

    LOGI("控制进程退出\n");
    return 0;
}
