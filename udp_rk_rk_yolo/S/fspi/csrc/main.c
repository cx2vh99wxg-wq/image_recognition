/*
 * S 端统一车控入口。
 * 作者：科研轮转
 *
 * 本进程读取共享内存中的车道、行人及交通灯状态，按现有策略更新 FSPI。
 * 自动转向须由串口屏请求并经 FPGA 确认。转角、反馈时效和脉冲期限由
 * steering_runtime 处理。未获授权时，仍处理车道、制动和显示状态。
 *
 * 人员通道发布原始刹停请求；AUTO 使能、STOP 锁存和人工解除仍由 FPGA 仲裁。
 * 实线变道权限、红灯 GO 门控及档位告警继续走各自既有监视入口。
 * 呈现命令保持六路视角 22/33/44/66/77/88、全景 99，以及 11/55 兼容归并。
 *
 * 本版更新：2026年10月20日。
 * 此前调整过前后控制、转向与制动的职责划分。
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

// 本文件共用的运行状态。
static volatile int g_running = 1;
static DisplayControlSharedMemory *display_control_shm = NULL;
static EmergencyBrakeSharedMemory *emergency_brake_shm = NULL;

// 寄存器访问：S只发布原始识别请求。AUTO开关、实际STOP输出和锁存均由FPGA决定。
static int last_brake_value = -1;
static uint32_t next_brake_release_refresh = 0;
static int raw_request_active = 0;
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
static unsigned front_command = FRONT_STOP_REQUEST;
static unsigned front_enabled;
static unsigned front_stop_active;
static uint8_t front_contacts;
static uint8_t center_hmi;
static int center_parked;
static FrontPersonData front_result;
static int front_snapshot_ok = 0;
static int fpga_camera_view_available;

static int park_contacts_released(const ControlSnapshot *status, uint16_t wheel_period)
{
    const unsigned idle_contacts = CONTACT_GO_N | CONTACT_BACK_N | CONTACT_LEFT_N | CONTACT_RIGHT_N;
    /* A valid but unmeasured Hall sensor after power-up leaves parking to
     * the operator's explicit confirmation. Measured motion vetoes it. */
    return (wheel_period == 0 || wheel_period == WHEEL_PERIOD_UNKNOWN) &&
           (status->gates & GATE_RESET_RELEASED) && (status->contacts & idle_contacts) == idle_contacts;
}

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

static void monitor_front_person(uint64_t now)
{
    front_guard_open(&front_guard_ipc, 1);
    front_person_ipc_open(&front_input, 0);
    memset(&front_result,0,sizeof(front_result));
    if (front_person_ipc_read(&front_input,&front_result)) memset(&front_result,0,sizeof(front_result));
    ControlSnapshot snapshot = {0};
    uint8_t view = 0xff;
    uint16_t wheel_period = WHEEL_PERIOD_INVALID;
    front_snapshot_ok = fspi_read_control_snapshot_with_wheel(&snapshot, &view, &wheel_period) == 0;
    center_parked = 0;
    if (front_snapshot_ok)
    {
        front_enabled = (snapshot.gates & GATE_AUTO) != 0;
        front_stop_active = (snapshot.gates & GATE_STOP) != 0;
        front_contacts = snapshot.contacts;
        center_hmi = snapshot.hmi;
        center_parked = park_contacts_released(&snapshot, wheel_period);
        fpga_camera_view_available = publish_camera_view(view);
        if (display_control_shm && display_control_shm->valid && wheel_period <= WHEEL_PERIOD_MAX)
        {
            const uint64_t stamp = (uint32_t)(now / 1000000ULL);
            __atomic_store_n(&display_control_shm->wheel_snapshot,
                             (stamp << 32) | wheel_period, __ATOMIC_RELEASE);
        }
    }
    front_command = front_person_guard_observe(&front_guard, &front_result, now, front_snapshot_ok);
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
    if (line_ipc_read(&lane_input, &data) || !line_fresh(&data, now) ||
        (data.diagnostic & LINE_HDMI_DISPLAY_ONLY))
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
    monitor_front_person(now);
    // 写未成功后继续重试同一个待发出心跳值；若未成功时仍推进，
    // 下一次已完成写入的值可能被 FPGA 当作旧心跳。
    uint8_t next_heartbeat = lane_heartbeat ^ 0x80;
    int written =
        fspi_write_data(5, (uint8_t)(0x40 | next_heartbeat | denied | traffic_command | front_command)) == 0;
    if (written)
        lane_heartbeat = next_heartbeat;
    traffic_go_commit(&traffic_policy, traffic_command, written);
    FrontGuardData front_status = {0};
    front_status.raw_request = front_command == FRONT_STOP_REQUEST;
    front_status.auto_enabled = front_enabled;
    front_status.blocked = front_enabled && (front_status.raw_request || front_stop_active);
    front_status.link_ok = front_snapshot_ok;
    front_status.person = front_person_fresh(&front_result,now) && front_person_hazard(&front_result);
    front_status.valid = front_person_fresh(&front_result,now) && ((front_result.flags & 3)==3);
    front_guard_set_deadline(&front_status,now+400000000ULL);
    front_guard_write(&front_guard_ipc,&front_status);
    static int previous_front = -1;
    int front_state = (front_status.person ? 1 : 0) | (front_status.blocked ? 2 : 0) |
        (front_status.valid ? 4 : 0) | (front_enabled ? 8 : 0) | (int)front_command |
        (front_snapshot_ok ? 0x40 | ((front_contacts & (CONTACT_GO_N | CONTACT_STOP_N)) << 8) : 0);
    if (front_state != previous_front)
    {
        printf("[FRONT_PERSON] detected=%u valid=%u auto=%u forward_hold=%u raw_stop=%u resume_ready=%u read_ok=%d go_n=%u sr_n=%u reg5_write=%d\n",
               !!front_status.person, front_status.valid, front_enabled, front_status.blocked,
               front_status.raw_request, front_command == FRONT_RESUME_ALLOWED, front_snapshot_ok,
               !!(front_contacts & CONTACT_GO_N), !!(front_contacts & CONTACT_STOP_N), written);
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

    enum
    {
        DETECTOR_UNAVAILABLE,
        DETECTOR_STALE,
        PERSON_REQUEST,
        HOLD_OR_INFERENCE_FAULT,
        CLEAR_REQUEST,
        DIRECTION_WRITE_FAULT
    };
    int reason = !status_valid || !clock_ok ? DETECTOR_UNAVAILABLE
                 : (last_update == 0 || last_update > now ||
                    now - last_update >= BRAKE_STATUS_TIMEOUT_SECONDS)
                     ? DETECTOR_STALE
                 : person_detected   ? PERSON_REQUEST
                 : brake_enable == 0 ? HOLD_OR_INFERENCE_FAULT
                                     : CLEAR_REQUEST;
    static int previous_reason = -1;

    /* 只在制动请求由有效变清除或释放未成功重试时丢弃旧方向。 reg4
     * 的每秒心跳不能顺便写方向0..3，其他情形下会中断转向或外部前进命令。 */
    int refresh_release =
        !required && (last_brake_value != 1 || direction_release_failed ||
                      (!steering_pulse_active() && (!clock_ok || now >= next_brake_release_refresh)));
    if (direction_release_failed || (!required && last_brake_value != 1))
    {
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
                               "有效person请求", "无人确认期或推理故障（详见PCIe日志）",
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

// 寄存器访问：结束所有车辆运动
void stop_all_vehicle_movement()
{
    printf("释放所有方向控制...\n");
    vehicle_control(0, 0);
    vehicle_control(1, 0);
    vehicle_control(2, 0);
    vehicle_control(3, 0);
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

/* 共享段读取器检查结果时效。原闭环接收 0 至 90 度的角度。 */
static int read_steering_lane(void *context, uint32_t *frame, unsigned *type, unsigned *confidence,
                              unsigned *angle)
{
    (void)context;
    CurveType kind = CURVE_NONE;
    uint32_t quality = 0, road_angle = 0;
    int rc = curve_detection_read(frame, &kind, &quality, &road_angle);
    if (quality & CURVE_HDMI_DISPLAY_ONLY)
        return -1;
    *type = (unsigned)kind;
    *confidence = quality;
    *angle = curve_angle_degrees(road_angle);
    return rc;
}

enum
{
    AUTOPILOT_HEARTBEAT_REG = 7,
    AUTOPILOT_REQUEST_ON = 0x04,
    AUTOPILOT_ACTIVE = 0x08,
    AUTOPILOT_SEQUENCE_MASK = 0x03
};

typedef struct
{
    uint64_t next_poll_ns;
    uint8_t counter;
    int active;
    int switch_off;
    int inhibit_until_off;
} AutopilotGate;

/* A failed poll pauses the software target. Leave the FPGA lease alone so
 * a brief SPI error can recover before the hardware timeout. */
static int poll_autopilot_gate(AutopilotGate *gate, uint64_t now, int control_ok)
{
    if (!control_ok || !now)
    {
        gate->next_poll_ns = 0;
        gate->active = 0;
        gate->switch_off = 0;
        return 0;
    }
    if (now < gate->next_poll_ns)
        return gate->active;

    gate->next_poll_ns = now + 40000000ULL;
    gate->active = 0;
    gate->switch_off = 0;
    uint8_t status;
    if (fspi_read_autopilot_status(&status))
        return 0;
    gate->switch_off = !(status & (AUTOPILOT_REQUEST_ON | AUTOPILOT_ACTIVE));
    if (gate->inhibit_until_off)
    {
        if (gate->switch_off && !steering_runtime_center_sampling())
            gate->inhibit_until_off = 0;
        return 0;
    }
    if (!(status & AUTOPILOT_REQUEST_ON))
        return 0;

    uint8_t counter = (gate->counter + 1) & 0x0f;
    uint8_t heartbeat = 0x80 | ((status & AUTOPILOT_SEQUENCE_MASK) << 4) | counter;
    if (fspi_write_data(AUTOPILOT_HEARTBEAT_REG, heartbeat))
        return 0;
    gate->counter = counter;
    gate->active = (status & AUTOPILOT_ACTIVE) != 0;
    return gate->active;
}

static void publish_center_input(AutopilotGate *gate, uint8_t key, int parked)
{
    unsigned status = steering_runtime_center_input(key, parked, gate->switch_off);
    if (steering_runtime_center_sampling())
        gate->inhibit_until_off = 1;
    if (display_control_shm && display_control_shm->valid)
    {
        uint64_t stamp = (uint32_t)(line_clock_ns() / 1000000ULL);
        __atomic_store_n(&display_control_shm->steering_center_snapshot,
                         (stamp << 32) | status, __ATOMIC_RELEASE);
    }
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
    if (argc != 1 && !brake_only)
    {
        fprintf(stderr, "Usage: %s [--brake-only]\n", argv[0]);
        return 2;
    }
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
    printf("- 自动转向: 串口屏 Autopilot 授权后，由原有 IMU 闭环跟随车道\n");
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
    (void)fspi_write_data(AUTOPILOT_HEARTBEAT_REG, 0);
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
    steer_result_t result = steering_runtime_init(0);
    printf("[STEER_MODE] closed-loop result=%s\n",
           steer_result_name(result));
    printf("将持续监控FSPI寄存器0的模式值变化...\n");
    printf("按 Ctrl+C 退出程序\n\n");

    // 保存此时模式识别所需状态。
    uint8_t last_read_value = 0xee;    // 上一轮保存的寄存器读值。

    printf("=== 开始监控FSPI寄存器0 ===\n");
    printf("按钮逻辑: 按下时产生指令值，松开时回到0x00\n");
    printf("支持的指令值:\n");
    printf("  呈现控制: 22/33/44/66/77/88(六个物理视角), 99(六路全景)\n");
    printf("  自动转向: Autopilot 开启后才允许有效车道、IMU 反馈和机械零位触发输出\n");
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
    printf("\n");

    // 寄存器访问：弯道识别状态变量（在循环外定义，供循环内多个地方使用）
    uint32_t current_curve_frame_id = 0;
    CurveType current_curve_type = CURVE_NONE;
    uint32_t current_curve_confidence = 0;
    uint32_t current_curve_angle = 0;

    uint64_t next_policy_ns = 0, next_center_touch_ns = 0;
    int control_io_failed = 1; /* 完成首次 IO 检查前，不触发自动动作。 */
    AutopilotGate autopilot = {0};
    int autopilot_enabled = 0;
    while (g_running)
    {
        /* Avoid an extra SPI exchange while an actuator release deadline is active. */
        if (!steering_pulse_active())
        {
            uint64_t now = line_clock_ns();
            int active = poll_autopilot_gate(&autopilot, now, !control_io_failed);
            if (active != autopilot_enabled)
            {
                if (!active)
                    steering_runtime_cancel();
                printf("[AUTOPILOT] steering_output=%s\n", active ? "ON" : "OFF");
                autopilot_enabled = active;
            }
            if (!control_io_failed && now >= next_center_touch_ns && steering_runtime_center_waiting_for_touch())
            {
                ControlSnapshot current = {0};
                uint8_t view;
                uint16_t period;
                next_center_touch_ns = now + 10000000ULL;
                int valid = fspi_read_control_snapshot_with_wheel(&current, &view, &period) == 0;
                publish_center_input(&autopilot, current.hmi, valid && park_contacts_released(&current, period));
            }
        }
        steering_runtime_poll_lane(!control_io_failed && autopilot_enabled, read_steering_lane, NULL);
        steering_runtime_tick();
        uint64_t tick_now = line_clock_ns();
        if (steering_pulse_active() || tick_now < next_policy_ns)
        {
            steering_runtime_wait();
            continue;
        }
        next_policy_ns = tick_now + 100000000ULL;

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
                steering_state_t measured;
                steering_get_state(&measured);
                printf(
                    "[SHM Read] Frame%u: %s | Conf:%u%% | RoadAngle:%u | WheelDeg:%.3f | Feedback:%d\n",
                    current_curve_frame_id, curve_name, current_curve_confidence,
                    curve_angle_degrees(current_curve_angle), measured.angle_deg,
                    measured.feedback_valid);
            }
        }

        // 寄存器访问：2. 发布原始制动请求。FPGA决定实际STOP；仅实际写故障暂停软件转向。
        control_io_failed = monitor_emergency_brake();

        if (control_io_failed)
            steering_runtime_cancel();
        publish_center_input(&autopilot, center_hmi, center_parked && !control_io_failed);

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
        steering_runtime_wait();
    }

    publish_motion_hint(0);
    (void)fspi_write_data(AUTOPILOT_HEARTBEAT_REG, 0);
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
