/*
 * S 端统一车控入口。
 * 作者：科研轮转
 *
 * 本进程读取共享内存中的车道、行人及交通灯状态，按现有策略更新 FSPI。
 * 请求闭环模式时，目标角、反馈新鲜度、参考会话和脉冲期限由 steering_runtime
 * 处理；初始化或反馈无效不会退回兼容模式的长时间转向脉冲。
 * 未请求闭环模式时，保留原有位置状态和定时转向分支。
 *
 * 人员通道发布原始刹停请求；AUTO 使能、STOP 锁存和人工解除仍由 FPGA 仲裁。
 * 实线变道权限、红灯 GO 门控及档位告警继续走各自既有监视入口。
 * 呈现命令保持六路视角 22/33/44/66/77/88、全景 99，以及 11/55 兼容归并。
 *
 * 本版更新：2026年10月20日。
 * 此前调整过前后控制、转向与制动的职责划分。
 * 本次仅校正模块说明，不修改上述分支、控制参数或日志字串。
 */

#define _POSIX_C_SOURCE 200809L
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#define _BSD_SOURCE
#include "fspi_module.h"
#include "wheel_speed.h"
#include "brake_policy.h"
#include "../include/shared_memory.h"
#include "lane_rule.h"
#include "traffic_go_rule.h"
#include "front_person_guard.h"
#include "steering_runtime.h"
#include "../../../common/perception/include/curve_heading.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

// 寄存器访问：方向盘转向时间参数(微秒)
#define TIME_NEUTRAL_TO_LEFT 500000  // 寄存器访问：中间→左 (0.5秒，保持原参数)
#define TIME_NEUTRAL_TO_RIGHT 600000 // 寄存器访问：中间→右 (0.6秒，保持原参数)
#define TIME_LEFT_TO_NEUTRAL 400000  // 寄存器访问：左→中间 (0.4秒)
#define TIME_RIGHT_TO_NEUTRAL 400000 // 寄存器访问：右→中间 (0.4秒)
#define TIME_LEFT_TO_RIGHT 1500000   // 寄存器访问：左→右 (1.5秒)

// 保存此时转向机构的位置分类。
typedef enum
{
    WHEEL_LEFT,
    WHEEL_NEUTRAL,
    WHEEL_RIGHT
} WheelPosition;

// 本文件共用的运行状态。
static volatile int g_running = 1;
static DisplayControlSharedMemory *display_control_shm = NULL;
static EmergencyBrakeSharedMemory *emergency_brake_shm = NULL;
static WheelPosition wheel_position = WHEEL_NEUTRAL;

// 寄存器访问：S只发布原始识别请求。AUTO开关、实际STOP输出和锁存均由FPGA决定。
static int last_brake_value = -1;
static uint32_t next_brake_release_refresh = 0;
static int raw_request_active = 0;
static int turn_pulse_active = 0;
static int turn_pulse_interrupted = 0;
static uint32_t next_brake_attach = 0;
static int direction_release_failed = 0;
static LineStyleIPC lane_input = {-1, NULL}, lane_warning = {-1, NULL};
static uint64_t lane_last_poll = 0;
static uint8_t lane_heartbeat = 0;
static TrafficLightIPC traffic_input = {-1, NULL};
static TrafficGoIPC traffic_warning = {-1, NULL};
static TrafficGoPolicy traffic_policy = {0};
static int traffic_policy_initialized = 0;
static FrontPersonIPC front_input = {-1, NULL};
static FrontGuardIPC front_guard_ipc = {-1, NULL};
static FrontPersonGuard front_guard = {0};
static int front_guard_initialized = 0;
static unsigned front_required = 1;
static FrontPersonData front_result;
static int front_snapshot_ok = 0;
static int fpga_camera_view_available;

static int publish_camera_view(uint8_t view)
{
    DisplayMode mode;
    if (!camera_mode_normalize(view, &mode))
        return 0;
    if (display_control_shm && display_control_shm->valid &&
        __atomic_load_n(&display_control_shm->mode, __ATOMIC_ACQUIRE) != (uint32_t)mode)
    {
        __atomic_store_n(&display_control_shm->mode, (uint32_t)mode, __ATOMIC_RELEASE);
        __atomic_add_fetch(&display_control_shm->button_count, 1, __ATOMIC_RELAXED);
        __atomic_store_n(&display_control_shm->last_update, (uint32_t)time(NULL), __ATOMIC_RELEASE);
    }
    return 1;
}

static void monitor_front_person(uint64_t now, unsigned color)
{
    front_guard_open(&front_guard_ipc, 1);
    if (!front_guard_initialized)
    {
        FrontGuardData old = {0};
        if (!front_guard_read(&front_guard_ipc, &old))
        {
            front_guard.held_go = !!old.blocked;
            front_guard.raw_request = !!old.raw_request;
            front_guard.recovery_pending = !!old.raw_request || !!old.blocked;
            front_guard.auto_enabled = !!old.auto_enabled;
        }
        front_guard_initialized = 1;
    }
    if (traffic_policy.blocked) front_guard.recovery_pending=1;
    front_person_ipc_open(&front_input, 0);
    memset(&front_result,0,sizeof(front_result));
    if (front_person_ipc_read(&front_input,&front_result)) memset(&front_result,0,sizeof(front_result));
    ControlSnapshot snapshot = {0};
    uint8_t view = 0xff;
    uint16_t wheel_period = WHEEL_PERIOD_UNKNOWN;
    front_snapshot_ok = fspi_read_control_snapshot_with_wheel(&snapshot, &view, &wheel_period) == 0;
    if (front_snapshot_ok)
    {
        fpga_camera_view_available = publish_camera_view(view);
        if (display_control_shm && display_control_shm->valid && wheel_period != WHEEL_PERIOD_UNKNOWN)
        {
            const uint64_t stamp = (uint32_t)(now / 1000000ULL);
            __atomic_store_n(&display_control_shm->wheel_snapshot,
                             (stamp << 32) | wheel_period, __ATOMIC_RELEASE);
        }
    }
    front_required = front_person_guard_observe(&front_guard,&front_result,now,&snapshot,front_snapshot_ok,color);
}

static SpeedWarningIPC speed_warning = {-1, NULL};
static void monitor_speed_warning(uint64_t now)
{
    static SpeedWarningData state = {0};
    static int previous = -1, previous_link = -1;
    SpeedSnapshot snapshot = {0};
    uint8_t raw = 0;
    int ok = fspi_read_speed_snapshot(&snapshot, &raw) == 0;
    state.link_ok = ok;
    state.read_errno = ok ? 0 : (uint32_t)errno;
    state.raw_byte = raw;
    ++state.poll_id;
    if (ok)
    {
        state.gear = snapshot.gear;
        state.known = snapshot.known;
        state.pressed = snapshot.pressed;
        state.driving = snapshot.driving;
        speed_set_deadline(&state, now + SPEED_WARNING_TTL_NS);
    } // 读取未成功时，不更新上一次有效快照的存活期限。
    speed_ipc_open(&speed_warning, 1);
    speed_ipc_write(&speed_warning, &state);
    int display_gear = state.known ? (int)state.gear : 0;
    if (display_gear != previous || ok != previous_link)
    {
        printf(
            "[SPEED_GEAR] estimated_gear=%d link=%s errno=%u warning=%s (gear estimate; no motion writes)\n",
            display_gear, ok ? "OK" : "OFFLINE", state.read_errno,
            display_gear >= 2 ? "HIGH GEAR" : "NONE");
        previous = display_gear;
        previous_link = ok;
    }
}

/* 车道与交通灯权限由同一写者合成为一个原子 reg5 字节；
 * 20ms 转向等待期间也刷新，但不写运动寄存器 0..4。 */
static void monitor_lane_rule(void)
{
    uint64_t now = line_clock_ns();
    if (lane_last_poll && now - lane_last_poll < 100000000ULL)
        return;
    lane_last_poll = now;
    line_ipc_open(&lane_input, LINE_STYLE_KEY, 0);
    line_ipc_open(&lane_warning, LINE_WARNING_KEY, 1);
    LineStyleData data = {0};
    if (line_ipc_read(&lane_input, &data) || !line_fresh(&data, now))
        data = (LineStyleData){0};
    unsigned denied = lane_denied_sides(&data, now);
    traffic_go_open(&traffic_warning, 1);
    if (!traffic_policy_initialized)
    {
        TrafficGoData old = {0};
        if (!traffic_go_read(&traffic_warning, &old) && old.blocked == 1)
            traffic_policy.blocked = 1;
        traffic_policy_initialized = 1;
    }
    traffic_ipc_open(&traffic_input, 0);
    TrafficLightData signal = {0};
    int signal_valid = !traffic_ipc_read(&traffic_input, &signal) && traffic_fresh(&signal, now);
    unsigned color = signal_valid ? traffic_go_color(&signal, now) : TRAFFIC_UNKNOWN;
    unsigned traffic_command = traffic_go_prepare(&traffic_policy, color);
    // Sample contacts BEFORE inhibiting GO; left/right do not hide forward motion.
    monitor_front_person(now,color);
    // Front-person recovery participates only while HMI Auto Brake is ON.
    // OFF must not let a detected person prevent a confirmed GREEN from
    // clearing an independent red-light hold. Raw STOP stays FPGA-gated.
    if (traffic_command==TRAFFIC_GO_CLEAR && front_guard.auto_enabled &&
        front_required && traffic_policy.blocked)
        traffic_command=TRAFFIC_GO_SET;
    unsigned permission = front_person_guard_permission(&front_guard,front_required,
                                                        traffic_command,traffic_policy.blocked);
    // 写未成功后继续重试同一个待发出心跳值；若未成功时仍推进，
    // 下一次已完成写入的值可能被 FPGA 当作旧心跳。
    uint8_t next_heartbeat = lane_heartbeat ^ 0x80;
    int written =
        fspi_write_data(5, (uint8_t)(0x40 | next_heartbeat | denied | permission)) == 0;
    if (written)
        lane_heartbeat = next_heartbeat;
    traffic_go_commit(&traffic_policy, traffic_command, written);
    if (written) front_guard.held_go = front_guard.auto_enabled && front_required;
    FrontGuardData front_status = {0};
    front_status.blocked = front_guard.held_go;
    front_status.raw_request = front_guard.raw_request;
    front_status.auto_enabled = front_guard.auto_enabled; front_status.link_ok = front_snapshot_ok;
    front_status.person = front_person_fresh(&front_result,now) && front_person_hazard(&front_result);
    front_status.valid = front_person_fresh(&front_result,now) && ((front_result.flags & 3)==3);
    front_guard_set_deadline(&front_status,now+400000000ULL);
    front_guard_write(&front_guard_ipc,&front_status);
    static int previous_front = -1;
    int front_state = (front_status.person?1:0) | (front_guard.raw_request?2:0) |
                      (front_guard.held_go?4:0) | (front_status.valid?8:0);
    if (front_state != previous_front)
    {
        printf("[FRONT_PERSON] detected=%u valid=%u auto=%u forward_hold=%u raw_stop=%u read_ok=%d reg5_write=%d clear_count=%u\n",
               !!front_status.person,front_status.valid,front_guard.auto_enabled,
               front_guard.held_go,front_guard.raw_request,front_snapshot_ok,written,front_guard.clear_count);
        previous_front=front_state;
    }
    TrafficGoData go_status = {0};
    go_status.blocked = traffic_policy.blocked;
    go_status.write_ok = written;
    go_status.signal_state = signal_valid ? signal.state : TRAFFIC_UNKNOWN;
    go_status.signal_fresh = signal_valid;
    go_status.session = signal.session;
    go_status.source_id = signal.source_id;
    traffic_go_set_deadline(&go_status, now + 400000000ULL);
    traffic_go_write(&traffic_warning, &go_status);
    static int last_go_hold = -1, last_go_write = -1;
    if (last_go_hold != (int)traffic_policy.blocked || last_go_write != written)
    {
        printf(
            "[TRAFFIC_GO] signal=%s confirmed=%s go_hold=%u reg5_write=%s (not pin readback); green restores stored GO, STOP/BACK remain independent\n",
            traffic_state_name(go_status.signal_state), traffic_state_name(color),
            traffic_policy.blocked, written ? "OK" : "FAIL");
        last_go_hold = (int)traffic_policy.blocked;
        last_go_write = written;
    }
    uint8_t hmi = 0;
    if (fspi_read_hmi_snapshot(&hmi))
        hmi = 0;
    data.hmi_code = hmi;
    data.blocked = written ? lane_blocked_request(denied, hmi) : 0;
    // 呈现告警具有有限持续时间，按键释放后仍展示 700ms。
    static unsigned previous = 0;
    static uint64_t warning_until = 0;
    static unsigned held = 0;
    if (data.blocked)
    {
        held = data.blocked;
        warning_until = now + 700000000ULL;
    }
    if (now < warning_until)
        data.blocked |= held;
    if (data.blocked != previous)
    {
        printf(
            "[LANE_RULE] left=%s right=%s hmi=%02X deny=%u warning=%u reg5_write=%s (not pin readback)\n",
            line_style_name(data.left), line_style_name(data.right), hmi, denied, data.blocked,
            written ? "OK" : "FAIL");
        previous = data.blocked;
    }
    line_set_deadline(&data, now + 400000000ULL);
    line_ipc_write(&lane_warning, &data);
    monitor_speed_warning(now);
}

/* 仅接受识别程序两秒内更新的状态；未运行、退出或卡住均维持制动请求。
 * 请求有效时重写低，无请求时每秒重写高，以补偿无回执写入可能丢失。
 * 无需 81 回读；默认关闭的 HMI 开关由 FPGA 最终门控。
 * 软件清除请求不会解除 STOP 锁存，人工 REARM 可强制解锁。 */
static int monitor_emergency_brake(void)
{
    monitor_lane_rule();
    // 寄存器访问：转向脉冲的20ms等待也持续发布请求，不等待不可靠的开关读回。
    struct timespec ts;
    int clock_ok = clock_gettime(CLOCK_MONOTONIC, &ts) == 0;
    uint32_t now = clock_ok ? (uint32_t)ts.tv_sec : 0;

    if (!emergency_brake_shm && clock_ok && now >= next_brake_attach)
    {
        if (emergency_brake_shm_init(&emergency_brake_shm, 0) == 0)
        {
            printf("[自动刹停] 已连接行人检测共享内存\n");
        }
        next_brake_attach = now + 1;
    }

    int person_detected = 0;
    int brake_enable = 1;
    int status_valid = 0;
    uint32_t last_update = 0;
    if (emergency_brake_shm && emergency_brake_shm != (void *)-1 &&
        emergency_brake_shm->frame_count > 0 &&
        emergency_brake_read_status(emergency_brake_shm, &person_detected, &brake_enable) == 0)
    {
        status_valid = 1;
        last_update = emergency_brake_shm->last_update;
    }

    int required = brake_required(1, status_valid && clock_ok, person_detected, brake_enable,
                                  last_update, now);
    required = required || front_guard.raw_request;

    enum
    {
        DETECTOR_UNAVAILABLE,
        DETECTOR_STALE,
        PERSON_REQUEST,
        FRONT_PERSON_REQUEST,
        HOLD_OR_INFERENCE_FAULT,
        CLEAR_REQUEST,
        DIRECTION_WRITE_FAULT
    };
    int reason = !status_valid || !clock_ok ? DETECTOR_UNAVAILABLE
                 : (last_update == 0 || last_update > now ||
                    now - last_update >= BRAKE_STATUS_TIMEOUT_SECONDS)
                     ? DETECTOR_STALE
                 : person_detected   ? PERSON_REQUEST
                 : front_guard.raw_request ? FRONT_PERSON_REQUEST
                 : brake_enable == 0 ? HOLD_OR_INFERENCE_FAULT
                                     : CLEAR_REQUEST;
    static int previous_reason = -1;

    /* 只在制动请求由有效变清除或释放未成功重试时丢弃旧方向。 reg4
     * 的每秒心跳不能顺便写方向0..3，其他情形下会中断转向或外部前进命令。 */
    int refresh_release =
        !required && (last_brake_value != 1 || direction_release_failed ||
                      (!turn_pulse_active && (!clock_ok || now >= next_brake_release_refresh)));
    if (direction_release_failed || (!required && last_brake_value != 1))
    {
        if (turn_pulse_active)
            turn_pulse_interrupted = 1;
        int directions_released = 1;
        for (int i = 0; i < 4; ++i)
        {
            if (fspi_write_data(i, 1) != 0)
            {
                directions_released = 0;
            }
        }
        direction_release_failed = !directions_released;
        if (!directions_released)
        {
            static uint32_t last_release_error_log = 0;
            if (now != last_release_error_log)
            {
                fprintf(
                    stderr,
                    "[自动刹停] 方向键释放失败，暂停自动转向并重试；发布原始故障请求，最终由FPGA AUTO门控\n");
                last_release_error_log = now;
            }
            last_brake_value = -1;
            required = 1;
            reason = DIRECTION_WRITE_FAULT;
        }
    }

    if (reason != previous_reason)
    {
        const char *names[] = {"检测段/首帧/时钟不可用", "检测数据超时",
                               "有效person请求", "前方行人/通路故障，前进刹停请求", "无人确认期或推理故障（详见PCIe日志）",
                               "有效无人、请求清除",     "方向寄存器写入故障"};
        printf(
            "[自动刹停链路] %s: valid=%d person=%d brake_enable=%d raw_request=%d -> reg4=%d；AUTO及实际STOP锁存由FPGA决定\n",
            names[reason], status_valid && clock_ok, person_detected, brake_enable, required,
            required ? 0 : 1);
        previous_reason = reason;
    }

    if (emergency_brake_shm &&
        (!status_valid || !clock_ok || last_update == 0 || last_update > now ||
         now - last_update >= BRAKE_STATUS_TIMEOUT_SECONDS))
    {
        emergency_brake_shm_cleanup(emergency_brake_shm, 0);
        emergency_brake_shm = NULL;
        next_brake_attach = now + 1;
    }

    if (required != raw_request_active)
    {
        printf("[自动刹停] %s\n",
               required ? "原始请求有效：发送reg4=0；实际刹停由FPGA AUTO开关和锁存决定"
                        : "原始请求清除：发送reg4=1；不会解除FPGA已有STOP锁存");
        raw_request_active = required;
    }

    int value_to_write = required ? 0 : 1; // 寄存器访问：寄存器4低电平请求制动；高电平只清除请求
    int request_write_failed = 0;
    if (required || refresh_release)
    {
        if (fspi_write_data(4, (uint8_t)value_to_write) == 0)
        {
            if (value_to_write != last_brake_value)
            {
                printf(
                    "[自动刹停FSPI] reg4=%d写事务返回成功（未读回FPGA）；AUTO及实际STOP锁存由FPGA决定\n",
                    value_to_write);
            }
            last_brake_value = value_to_write;
            if (!required)
            {
                next_brake_release_refresh = now + 1;
            }
        }
        else
        {
            request_write_failed = 1;
            last_brake_value = -1;
            static uint32_t last_error_log = 0;
            if (now != last_error_log)
            {
                fprintf(stderr, "[自动刹停] 写制动寄存器失败，下一次轮询将重试\n");
                last_error_log = now;
            }
        }
    }

    // 寄存器访问：未读到AUTO状态不能阻断关闭状态下的转向。硬件STOP锁存直接门控方向触点；
    // 寄存器访问：S只在实际寄存器/方向写入未成功时中止软件转向，不把原始请求当作已刹停。
    return request_write_failed || direction_release_failed;
}

/* 寄存器访问：转向期间持续发布原始请求；实际写入故障结束软件脉冲，STOP由FPGA门控。 */
static int wait_with_brake_checks(unsigned int duration_us)
{
    turn_pulse_active = 1;
    turn_pulse_interrupted = 0;
    while (duration_us > 0 && g_running)
    {
        unsigned int slice = duration_us < 20000 ? duration_us : 20000;
        usleep(slice);
        if (monitor_emergency_brake() || turn_pulse_interrupted)
        {
            turn_pulse_active = 0;
            return -1;
        }
        duration_us -= slice;
    }
    turn_pulse_active = 0;
    return g_running ? 0 : -1;
}

// 读取此时时间戳（秒，支持小数）
double get_current_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

// 处理进程信号的回调入口。
void signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        printf("\n收到Ctrl+C信号，开始退出...\n");
        g_running = 0;
    }
    else if (sig == SIGTERM)
    {
        printf("\n收到SIGTERM信号，开始退出...\n");
        g_running = 0;
    }
}

/* 保留既有机械标定时长。先释放左右触点，再给出新的低有效脉冲，让 FPGA
 * 在 STOP 或人工接管后重新收取 FSPI。未成功不得记录为已转到位；
 * wheel_position 是按脉冲推测的位置，并非方向盘实测反馈。 */
static void apply_turn_pulse(int command, unsigned int duration_us, WheelPosition target)
{
    if (monitor_emergency_brake())
        return;
    int released = vehicle_control(2, 0) == 0;
    if (vehicle_control(3, 0) != 0)
        released = 0;
    if (!released)
    {
        direction_release_failed = 1;
        return;
    }
    if (vehicle_control(command, 1) != 0)
    {
        // 寄存器访问：无回读的写未成功不能证明引脚仍为高；立即尝试释放并安排后续重试。
        vehicle_control(command, 0);
        direction_release_failed = 1;
        return;
    }
    int completed = wait_with_brake_checks(duration_us) == 0;
    if (vehicle_control(command, 0) != 0)
    {
        direction_release_failed = 1;
        completed = 0;
    }
    if (completed)
    {
        wheel_position = target;
        printf("[转向链路] reg%d低脉冲%ums完成并释放；估计位置=%s（非硬件反馈）\n", command,
               duration_us / 1000,
               target == WHEEL_LEFT    ? "LEFT"
               : target == WHEEL_RIGHT ? "RIGHT"
                                       : "NEUTRAL");
    }
    else
    {
        printf("[转向链路] reg%d脉冲中断或写入失败，未更新估计位置，下轮重试\n", command);
    }
}

void turn_to_left(void)
{
    if (wheel_position != WHEEL_LEFT)
        apply_turn_pulse(2,
                         wheel_position == WHEEL_RIGHT ? TIME_LEFT_TO_RIGHT : TIME_NEUTRAL_TO_LEFT,
                         WHEEL_LEFT);
}

void turn_to_right(void)
{
    if (wheel_position != WHEEL_RIGHT)
        apply_turn_pulse(3,
                         wheel_position == WHEEL_LEFT ? TIME_LEFT_TO_RIGHT : TIME_NEUTRAL_TO_RIGHT,
                         WHEEL_RIGHT);
}

void turn_to_neutral(void)
{
    if (wheel_position == WHEEL_LEFT)
        apply_turn_pulse(3, TIME_LEFT_TO_NEUTRAL, WHEEL_NEUTRAL);
    else if (wheel_position == WHEEL_RIGHT)
        apply_turn_pulse(2, TIME_RIGHT_TO_NEUTRAL, WHEEL_NEUTRAL);
}

// 寄存器访问：结束所有车辆运动
void stop_all_vehicle_movement()
{
    printf("释放所有方向控制...\n");
    vehicle_control(0, 0);
    vehicle_control(1, 0);
    vehicle_control(2, 0);
    vehicle_control(3, 0);
    wheel_position = WHEEL_NEUTRAL;
    printf("所有方向控制已释放\n");
}

/* 寄存器访问：复用呈现控制共享段的预留字节发布此时 HMI 按键提示。
 * 每个方向只对应此时按下的键；松开或读取未成功时立即发布 NONE。
 * 该值不是 FPGA 触点或车轮速度反馈，过期后呈现端也会自动隐藏。 */
static void publish_motion_hint(uint8_t hmi_code)
{
    if (!display_control_shm || !display_control_shm->valid)
        return;

    VehicleMotionHint motion = VEHICLE_MOTION_NONE;
    /* FPGA 的四线读口将 4 位 HMI 选择码重复送出两次，因此 串口屏功能
     * 04 的 A/B/C/D 在 RK 端分别读作 AA/BB/CC/DD。 */
    if (hmi_code == 0xaa)
    {
        motion = VEHICLE_MOTION_FORWARD;
    }
    else if (hmi_code == 0xbb)
    {
        motion = VEHICLE_MOTION_REVERSE;
    }
    else if (hmi_code == 0xcc)
    {
        motion = VEHICLE_MOTION_LEFT;
    }
    else if (hmi_code == 0xdd)
    {
        motion = VEHICLE_MOTION_RIGHT;
    }

    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
        return;
    uint32_t timestamp_ms =
        (uint32_t)((uint64_t)ts.tv_sec * 1000u + (uint32_t)ts.tv_nsec / 1000000u);
    uint64_t snapshot = ((uint64_t)timestamp_ms << 32) | (uint32_t)motion;
    __atomic_store_n(&display_control_shm->motion_snapshot, snapshot, __ATOMIC_RELEASE);
}

/* 道路事件轮询独立于较慢的 SPI/策略/日志轮次。
 * 回调只读已有元数据，实际输出仍归转向运行时，
 * 并受 STOP、人工优先及零点确认约束。 */
static int read_steering_lane(void *context, uint32_t *frame, unsigned *type, unsigned *confidence,
                              unsigned *angle)
{
    (void)context;
    CurveType kind = CURVE_NONE;
    uint32_t quality = 0, road_angle = 0;
    int rc = curve_detection_read(frame, &kind, &quality, &road_angle);
    *type = (unsigned)kind;
    *confidence = quality;
    *angle = road_angle;
    return rc;
}

/* Algorithm-test profile: SAME production brake/permission policies, without
 * starting IMU, auto-centering or steering pulses. Never asserts reg0..3. */
static void run_brake_only(unsigned max_iterations)
{
    unsigned iterations = 0;
    printf("[BRAKE_ONLY] production front/rear/traffic/lane monitors; automatic steering and IMU disabled\n");
    while (g_running && (!max_iterations || iterations < max_iterations))
    {
        monitor_emergency_brake();
        ++iterations;
        if (!max_iterations || iterations < max_iterations) usleep(100000);
    }
}

int main(int argc, char *argv[])
{
    const int brake_only = argc == 2 && !strcmp(argv[1], "--brake-only");
    if (argc != 1 && !brake_only) { fprintf(stderr,"Usage: %s [--brake-only]\n",argv[0]); return 2; }
    /* 显式台架会话应能跨越人工观察停顿；
     * 在任何可能写触点的 SPI 初始化或移除前先检查会话。 */
    if (access("/tmp/steering-bench.active", F_OK) == 0)
    {
        fprintf(stderr, "STEERING_BENCH_ACTIVE: legacy vehicle_control is paused. "
                        "Finish the bench session and remove /tmp/steering-bench.active "
                        "before starting normal vehicle control.\n");
        return 75;
    }
    // 寄存器访问：调试日志重定向到文件时也逐行输出，便于实机核对制动请求。
    setvbuf(stdout, NULL, _IOLBF, 0);
    printf("=== 车辆控制程序 (弯道检测自动转向) ===\n");
    printf("版本: 8.2 (SMDT原生三字节FSPI + 按轴转向 + 制动心跳隔离)\n");
    printf("作者: 科研轮转\n");
    printf("功能: 弯道识别自动左右转 + 人员识别紧急制动 + 呈现控制\n");
    printf("工作模式说明:\n");
    printf("- 实时读取: 从FSPI寄存器0读取实际模式值\n");
    printf("- 呈现控制: 六个物理视角22/33/44/66/77/88，99六路全景；11/55兼容归并到中间视角\n");
    printf("- 左右转向: 完全由弯道识别自动控制\n");
    printf("- 原始制动请求: 有人或识别不可用写4=0，连续两秒无人写1仅清除请求\n");
    printf("- STOP锁存: FPGA保持低电平直到人工REARM；允许有人时强制解除此时报警\n");
    printf("- 制动开关: HMI功能0F的FF开启/EE关闭，默认关闭和最终门控由FPGA负责；S不依赖开关读回\n");
    printf("- 弯道识别: 自动读取UDP传来的弯道信息，智能控制车辆转向\n");
    printf("  * 智能转向: 左弯→左转，右弯→右转，直道→回中\n");
    printf("- 识别间隔: 每100ms识别一次模式变化\n");
    printf("- 实时响应: 识别到指令时立即执行对应操作\n\n");

    /* 寄存器访问：转向函数已在文件作用域定义；此处原有的同内容嵌套定义及旧注释代码
     * 不参与实际控制流程，直接使用上方统一实现。 */
    // 处理注册信号处理的接口。
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // 为FSPI模块建立初始状态。
    printf("=== FSPI模块初始化 ===\n");
    if (fspi_init() != 0)
    {
        printf("FSPI初始化未完成！请核对:\n");
        printf("1. SPI设备文件是否存在: /dev/spidev4.0\n");
        printf("2. 是否有权限访问SPI设备 (可能需要sudo)\n");
        printf("3. 硬件接入是否正常\n");
        return -1;
    }
    printf("FSPI初始化完成！\n");
    if (brake_only)
    {
        if (display_control_shm_init(&display_control_shm, 1) != 0)
            fprintf(stderr, "无法初始化视角共享内存，继续运行制动监控\n");
        run_brake_only(0);
        fspi_write_data(4, 0); /* Existing fault request; hardware AUTO gate still applies. */
        if (emergency_brake_shm) emergency_brake_shm_cleanup(emergency_brake_shm, 0);
        if (display_control_shm) display_control_shm_cleanup(display_control_shm, 1);
        fspi_cleanup();
        return 0;
    }
    int closed_loop = steering_runtime_requested();

    // 寄存器访问：先初始化原始请求与方向为释放；写1不解除已有的FPGA锁存。
    printf("初始化控制寄存器: 制动4写1，方向0-3写1\n");
    if (fspi_write_data(4, 1) != 0)
    {
        fprintf(stderr, "无法清除初始软件制动请求，结束车辆控制程序\n");
        fspi_cleanup();
        return -1;
    }
    last_brake_value = 1;
    int control_init_ok = 1;
    for (int i = 0; i < 4; i++)
    {
        if (fspi_write_data(i, 1) == 0)
        {
            printf("✓ 寄存器%d初始化完成\n", i);
        }
        else
        {
            printf("✗ 寄存器%d初始化未完成\n", i);
            control_init_ok = 0;
        }
    }
    if (!control_init_ok)
    {
        fprintf(stderr, "控制寄存器初始化未完成，结束车辆控制程序\n");
        fspi_cleanup();
        return -1;
    }
    printf("\n");

    // 为呈现控制共享内存（写入者）建立初始状态。
    printf("=== 显示控制共享内存初始化 ===\n");
    if (display_control_shm_init(&display_control_shm, 1) != 0)
    {
        printf("呈现控制共享内存初始化未完成！\n");
        fspi_cleanup();
        return -1;
    }
    printf("呈现控制共享内存初始化完成\n\n");

    // 寄存器访问：始终接入识别段并发布原始请求，是否生效由FPGA的HMI开关最终决定。
    printf("=== 紧急制动共享内存初始化 ===\n");
    monitor_emergency_brake();
    printf("\n");

    // 为弯道识别共享内存（读取者）建立初始状态。
    printf("=== 弯道检测共享内存初始化 ===\n");
    if (curve_detection_shm_init(0) != 0)
    { // 参数 0 选择只读消费者角色。
        printf("弯道识别共享内存初始化未完成！\n");
        printf("注意: 这可能是因为UDP程序尚未运行\n");
        printf("弯道识别功能将被禁用，但其他功能正常\n");
    }
    else
    {
        printf("弯道识别共享内存初始化完成，FSPI将读取UDP传来的弯道信息\n");
    }
    printf("\n");

    printf("=== 开始实际模式读取 ===\n");
    if (closed_loop)
    {
        steer_result_t result = steering_runtime_init(0);
        printf("[STEER_MODE] closed-loop requested result=%s; legacy turn pulses disabled\n",
               steer_result_name(result));
    }
    printf("将持续监控FSPI寄存器0的模式值变化...\n");
    printf("按 Ctrl+C 退出程序\n\n");

    // 保存此时模式识别所需状态。
    uint8_t last_read_value = 0xee;    // 上一轮保存的寄存器读值。

    printf("=== 开始监控FSPI寄存器0 ===\n");
    printf("按钮逻辑: 按下时产生指令值，松开时回到0x00\n");
    printf("支持的指令值:\n");
    printf("  呈现控制: 22/33/44/66/77/88(六个物理视角), 99(六路全景)\n");
    printf("  左右转向: 完全由弯道识别自动控制\n");
    printf("  制动功能控制: S持续发布原始请求；HMI AUTO开关和实际STOP锁存由FPGA决定\n");
    printf("  紧急制动: 有人或识别失效写0，连续两秒无人写1；STOP锁存仅由人工REARM解除\n");
    printf("  弯道识别: 自动读取UDP传来的弯道信息并控制车辆转向 [置信度≥60%%时执行]\n");
    if (emergency_brake_shm)
    {
        printf("  识别通道: 已接入PCIe人员识别；FPGA实际AUTO/STOP状态不作软件推断\n");
    }
    else
    {
        printf("  识别通道: 等待PCIe程序；发布原始故障请求，最终由FPGA AUTO门控\n");
    }
    printf("  人员识别停车功能: 开关状态以HMI/FPGA为准，S不伪造开启或关闭状态\n");
    printf("  弯道识别功能: 已启用，自动控制车辆转向\n");
    printf("\n");

    // 寄存器访问：弯道识别状态变量（在循环外定义，供循环内多个地方使用）
    uint32_t current_curve_frame_id = 0;
    CurveType current_curve_type = CURVE_NONE;
    uint32_t current_curve_confidence = 0;
    uint32_t current_curve_angle = 0;
    int curve_data_valid = 0; // 寄存器访问：标记此时周期的弯道数据是否有效

    uint64_t next_policy_ns = 0;
    int control_io_failed = 1; /* 完成首次 IO 检查前，不触发自动动作。 */
    while (g_running)
    {
        if (closed_loop)
        {
            steering_runtime_poll_lane(!control_io_failed, read_steering_lane, NULL);
            steering_runtime_tick();
            uint64_t tick_now = line_clock_ns();
            if (steering_pulse_active() || tick_now < next_policy_ns)
            {
                steering_runtime_wait();
                continue;
            }
            next_policy_ns = tick_now + 100000000ULL;
        }
        // 标记重置弯道数据是否有效。
        curve_data_valid = 0;

        // 寄存器访问：1. 读取弯道识别信息（只读取，不控制）
        {
            // 寄存器访问：静态变量跟踪状态
            static uint32_t curve_check_count = 0;
            static int read_fail_count = 0;
            static int first_read = 1; // 记录是否尚未读取过有效值。

            curve_check_count++;

            // 寄存器访问：尝试读取弯道识别结果
            int read_result = curve_detection_read(&current_curve_frame_id, &current_curve_type,
                                                   &current_curve_confidence, &current_curve_angle);

            // 寄存器访问：首次读取时立即呈现状态
            if (first_read)
            {
                if (read_result != 0)
                {
                    printf(
                        "[Road Detection] WARNING: Shared memory read failed - UDP program may not be running\n");
                }
                else
                {
                    printf("[Road Detection] Initialized successfully\n");
                }
                first_read = 0;
            }

            // 取得未成功时的处理的此时内容。
            if (read_result != 0)
            {
                read_fail_count++;
                // 前10次未成功立即打印，以后每5秒打印一次
                if (read_fail_count <= 10 || read_fail_count % 50 == 1)
                {
                    printf(
                        "[Road Detection] Shared memory read failed (count: %d) - Check if UDP is running\n",
                        read_fail_count);
                }
            }
            else
            {
                // 取得已完成，重置未成功计数的此时内容。
                if (read_fail_count > 0)
                {
                    printf("[Road Detection] Shared memory read recovered\n");
                    read_fail_count = 0;
                }

                // 发布此时数据的有效标志。
                curve_data_valid = 1;

                // 寄存器访问：每次读取已完成都打印读到的路况信息
                const char *curve_name = "";
                switch (current_curve_type)
                {
                case CURVE_NONE:
                    curve_name = "STRAIGHT";
                    break;
                case CURVE_LEFT:
                    curve_name = "LEFT";
                    break;
                case CURVE_RIGHT:
                    curve_name = "RIGHT";
                    break;
                case CURVE_BOTH:
                    curve_name = "S-CURVE";
                    break;
                }
                if (closed_loop)
                {
                    steering_state_t measured;
                    steering_get_state(&measured);
                    printf(
                        "[SHM Read] Frame%u: %s | Conf:%u%% | RoadAngle:%u | WheelDeg:%.3f | Feedback:%d\n",
                        current_curve_frame_id, curve_name, current_curve_confidence,
                        curve_angle_degrees(current_curve_angle), measured.angle_deg,
                        measured.feedback_valid);
                }
                else
                {
                    const char *pos_name = wheel_position == WHEEL_LEFT    ? "LEFT"
                                           : wheel_position == WHEEL_RIGHT ? "RIGHT"
                                                                           : "NEUTRAL";
                    printf("[SHM Read] Frame%u: %s | Conf:%u%% | Angle:%u | Wheel:%s\n",
                           current_curve_frame_id, curve_name, current_curve_confidence,
                           curve_angle_degrees(current_curve_angle), pos_name);
                }
            }
        }

        // 寄存器访问：2. 发布原始制动请求。FPGA决定实际STOP；仅实际写故障暂停软件转向。
        control_io_failed = monitor_emergency_brake();

        // 3. 根据弯道识别结果执行自动转向控制（在读取FSPI指令以前）
        if (closed_loop)
        {
            if (control_io_failed)
                steering_runtime_cancel();
        }
        else if (!control_io_failed && curve_data_valid &&
                 !curve_is_image_heading(current_curve_angle))
        {
            // 静态变量跟踪上次的弯道类型，用来避免重复执行
            static CurveType last_applied_curve_type = CURVE_NONE;
            static uint32_t last_applied_confidence = 0;

            // 识别弯道类型变化用来日志输出
            int should_log =
                (current_curve_type != last_applied_curve_type) ||
                (abs((int)current_curve_confidence - (int)last_applied_confidence) > 20);

            // 寄存器访问：根据此时弯道类型执行转向控制（每次都检查）
            if (current_curve_confidence >= 60)
            { // 寄存器访问：只有置信度足够高时才执行
                switch (current_curve_type)
                {
                case CURVE_NONE:
                    if (should_log)
                    {
                        printf("\n========================================\n");
                        printf("Road: STRAIGHT (Frame %u, Conf:%u%%)\n", current_curve_frame_id,
                               current_curve_confidence);
                        printf("========================================\n\n");
                    }
                    turn_to_neutral();
                    break;

                case CURVE_LEFT:
                    if (should_log)
                    {
                        printf("\n========================================\n");
                        printf("Road: LEFT (Frame %u, Conf:%u%%, Angle:%u)\n",
                               current_curve_frame_id, current_curve_confidence,
                               current_curve_angle);
                        printf("========================================\n\n");
                    }
                    turn_to_left();
                    break;

                case CURVE_RIGHT:
                    if (should_log)
                    {
                        printf("\n========================================\n");
                        printf("Road: RIGHT (Frame %u, Conf:%u%%, Angle:%u)\n",
                               current_curve_frame_id, current_curve_confidence,
                               current_curve_angle);
                        printf("========================================\n\n");
                    }
                    turn_to_right();
                    break;

                case CURVE_BOTH:
                    if (should_log)
                    {
                        printf("\n========================================\n");
                        printf("Road: S-CURVE (Frame %u, Conf:%u%%)\n", current_curve_frame_id,
                               current_curve_confidence);
                        printf("   Action: Maintain current turn state\n");
                        printf("========================================\n\n");
                    }
                    break;
                }
            }

            // 按本轮结果推进记录值用于下次日志判断。
            last_applied_curve_type = current_curve_type;
            last_applied_confidence = current_curve_confidence;
        }

        // 寄存器访问：4. 实际从FSPI寄存器0读取模式值
        uint8_t current_mode_value = 0;
        if (fspi_read_data(0, &current_mode_value) != 0)
        {
            // 寄存器访问：读不到最新触摸状态时立即撤销提示，避免继续呈现上一次按键。
            publish_motion_hint(0);
            usleep(100000); // 寄存器访问：未成功时等待100ms再重试
            continue;
        }
        publish_motion_hint(current_mode_value);

        // 寄存器访问：只在值发生变化时记录
        if (current_mode_value != last_read_value)
        {
            printf(
                "[转向链路] HMI选择码=0x%02X；自动转向写reg2/3，最终引脚由FPGA手动优先及STOP仲裁\n",
                current_mode_value);
            last_read_value = current_mode_value;
        }

        // Older FPGA images only provide the original HMI code.
        if (!fpga_camera_view_available)
            publish_camera_view(current_mode_value);

        // 寄存器访问：每100ms识别一次，响应更快
        if (closed_loop)
            steering_runtime_wait();
        else
            usleep(100000);
    }

    publish_motion_hint(0);
    if (closed_loop)
        steering_runtime_close();
    // 寄存器访问：程序退出前的移除工作
    printf("\n=== 程序退出移除 ===\n");

    // 寄存器访问：退出沿用原始故障请求；AUTO开关与已有STOP锁存均由FPGA决定。
    if (fspi_write_data(4, 0) != 0)
    {
        fprintf(stderr, "注意: 退出时无法写入制动寄存器，请核对车辆状态\n");
    }

    // 寄存器访问：结束所有车辆运动
    stop_all_vehicle_movement();

    // 寄存器访问：移除弯道识别共享内存
    printf("移除弯道识别共享内存...\n");
    curve_detection_shm_cleanup();

    // 寄存器访问：移除紧急制动共享内存
    if (emergency_brake_shm)
    {
        printf("移除紧急制动共享内存...\n");
        emergency_brake_shm_cleanup(emergency_brake_shm, 0);
        emergency_brake_shm = NULL;
    }

    // 寄存器访问：移除呈现控制共享内存
    if (display_control_shm)
    {
        printf("移除呈现控制共享内存...\n");
        display_control_shm_cleanup(display_control_shm, 1);
    }

    // 退出时释放 FSPI 所占资源。
    printf("移除FSPI资源...\n");
    fspi_cleanup();

    printf("所有资源移除完成，程序退出\n");
    return 0;
}
