/*
 * 车辆控制程序 (弯道检测自动转向)
 * 作者：科研轮转
 * 功能：弯道检测自动左右转 + 人员检测紧急制动 + 显示控制
 *
 * 控制逻辑：
 *   - 左右转向: 完全由弯道检测自动控制
 *   - 弯道检测: 从UDP共享内存读取路况信息
 *   - 自动转向: 左弯→左转1秒，右弯→右转1秒，直道→停止转向
 *   - 人员检测: PCIe检测到人员时自动紧急制动
 *   - 显示切换: 六个物理视角22/33/44/66/77/88，99六路全景；11/55兼容归并到中间视角
 *
 * 创建时间：2025年10月20日
 * 修改时间：2025年11月18日 - 删除前进后退控制，只保留转向和制动
 */

#define _POSIX_C_SOURCE 200809L
#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#define _BSD_SOURCE
#include "fspi_module.h"
#include "brake_policy.h"
#include "../include/shared_memory.h"
#include "lane_rule.h"
#include "traffic_go_rule.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

// 方向盘转向时间参数(微秒)
#define TIME_NEUTRAL_TO_LEFT   500000   // 中间→左 (0.5秒，保持原参数)
#define TIME_NEUTRAL_TO_RIGHT  600000   // 中间→右 (0.6秒，保持原参数)
#define TIME_LEFT_TO_NEUTRAL   400000   // 左→中间 (0.4秒)
#define TIME_RIGHT_TO_NEUTRAL  400000   // 右→中间 (0.4秒)
#define TIME_LEFT_TO_RIGHT     1500000  // 左→右 (1.5秒)

// 方向盘位置状态
typedef enum {
    WHEEL_LEFT,
    WHEEL_NEUTRAL,
    WHEEL_RIGHT
} WheelPosition;

// 全局变量
static volatile int g_running = 1;
static DisplayControlSharedMemory* display_control_shm = NULL;
static EmergencyBrakeSharedMemory* emergency_brake_shm = NULL;
static WheelPosition wheel_position = WHEEL_NEUTRAL;

// S只发布原始检测请求。AUTO开关、实际STOP输出和锁存均由FPGA决定。
static int last_brake_value = -1;
static uint32_t next_brake_release_refresh = 0;
static int raw_request_active = 0;
static int turn_pulse_active = 0;
static int turn_pulse_interrupted = 0;
static uint32_t next_brake_attach = 0;
static int direction_release_failed = 0;
static LineStyleIPC lane_input={-1,NULL},lane_warning={-1,NULL};
static uint64_t lane_last_poll=0;
static uint8_t lane_heartbeat=0;
static TrafficLightIPC traffic_input={-1,NULL};
static TrafficGoIPC traffic_warning={-1,NULL};
static TrafficGoPolicy traffic_policy={0};
static int traffic_policy_initialized=0;
static SpeedWarningIPC speed_warning={-1,NULL};
static void monitor_speed_warning(uint64_t now) {
    static SpeedWarningData state={0};static int previous=-1,previous_link=-1;
    SpeedSnapshot snapshot={0};uint8_t raw=0;
    int ok=fspi_read_speed_snapshot(&snapshot,&raw)==0;
    state.link_ok=ok;state.read_errno=ok?0:(uint32_t)errno;state.raw_byte=raw;++state.poll_id;
    if(ok) {
        state.gear=snapshot.gear;state.known=snapshot.known;
        state.pressed=snapshot.pressed;state.driving=snapshot.driving;
        speed_set_deadline(&state,now+SPEED_WARNING_TTL_NS);
    } // A failed read never extends the lifetime of the last good snapshot.
    speed_ipc_open(&speed_warning,1);speed_ipc_write(&speed_warning,&state);
    int display_gear=state.known?(int)state.gear:0;
    if(display_gear!=previous||ok!=previous_link) {
        printf("[SPEED_GEAR] estimated_gear=%d link=%s errno=%u warning=%s (no speed sensor; no motion writes)\n",
            display_gear,ok?"OK":"OFFLINE",state.read_errno,display_gear>=2?"HIGH GEAR":"NONE");
        previous=display_gear;previous_link=ok;
    }
}

/* Lane and traffic permissions share ONE writer and ONE atomic reg5 byte.
 * Runs also during the 20ms turn wait. Does not touch motion registers 0..4. */
static void monitor_lane_rule(void) {
    uint64_t now=line_clock_ns();
    if(lane_last_poll&&now-lane_last_poll<100000000ULL)return;
    lane_last_poll=now;
    line_ipc_open(&lane_input,LINE_STYLE_KEY,0);
    line_ipc_open(&lane_warning,LINE_WARNING_KEY,1);
    LineStyleData data={0};
    if(line_ipc_read(&lane_input,&data)||!line_fresh(&data,now))data=(LineStyleData){0};
    unsigned denied=lane_denied_sides(&data,now);
    traffic_go_open(&traffic_warning,1);
    if(!traffic_policy_initialized) {
        TrafficGoData old={0};
        if(!traffic_go_read(&traffic_warning,&old)&&old.blocked==1)traffic_policy.blocked=1;
        traffic_policy_initialized=1;
    }
    traffic_ipc_open(&traffic_input,0);
    TrafficLightData signal={0};
    int signal_valid=!traffic_ipc_read(&traffic_input,&signal)&&traffic_fresh(&signal,now);
    unsigned color=signal_valid?traffic_go_color(&signal,now):TRAFFIC_UNKNOWN;
    unsigned traffic_command=traffic_go_prepare(&traffic_policy,color);
    // Retry the same next heartbeat after a failed write. Advancing it on
    // failure could make the following successful byte look old to the FPGA.
    uint8_t next_heartbeat=lane_heartbeat^0x80;
    int written=fspi_write_data(5,(uint8_t)(0x40|next_heartbeat|denied|traffic_command))==0;
    if(written)lane_heartbeat=next_heartbeat;
    traffic_go_commit(&traffic_policy,traffic_command,written);
    TrafficGoData go_status={0};
    go_status.blocked=traffic_policy.blocked;go_status.write_ok=written;
    go_status.signal_state=signal_valid?signal.state:TRAFFIC_UNKNOWN;
    go_status.signal_fresh=signal_valid;go_status.session=signal.session;go_status.source_id=signal.source_id;
    traffic_go_set_deadline(&go_status,now+400000000ULL);traffic_go_write(&traffic_warning,&go_status);
    static int last_go_hold=-1,last_go_write=-1;
    if(last_go_hold!=(int)traffic_policy.blocked||last_go_write!=written) {
        printf("[TRAFFIC_GO] signal=%s confirmed=%s go_hold=%u reg5_write=%s (not pin readback); green restores stored GO, STOP/BACK remain independent\n",
            traffic_state_name(go_status.signal_state),traffic_state_name(color),traffic_policy.blocked,written?"OK":"FAIL");
        last_go_hold=(int)traffic_policy.blocked;last_go_write=written;
    }
    uint8_t hmi=0;
    if(fspi_read_hmi_snapshot(&hmi))hmi=0;
    data.hmi_code=hmi;
    data.blocked=written?lane_blocked_request(denied,hmi):0;
    // The warning is a bounded display event; release remains visible for 700ms.
    static unsigned previous=0;static uint64_t warning_until=0;static unsigned held=0;
    if(data.blocked){held=data.blocked;warning_until=now+700000000ULL;}
    if(now<warning_until)data.blocked|=held;
    if(data.blocked!=previous) {
        printf("[LANE_RULE] left=%s right=%s hmi=%02X deny=%u warning=%u reg5_write=%s (not pin readback)\n",
            line_style_name(data.left),line_style_name(data.right),hmi,denied,data.blocked,written?"OK":"FAIL");
        previous=data.blocked;
    }
    line_set_deadline(&data,now+400000000ULL);line_ipc_write(&lane_warning,&data);
    monitor_speed_warning(now);
}

/* 读端只接受检测程序最近两秒写入的状态。检测程序未启动、退出或卡住时保持制动请求。
 * 请求有效时重复写低电平；请求清除时每秒重写高电平，避免无回执的 SPI 写入丢失。
 * 不依赖81状态读回；默认关闭的HMI开关在FPGA最终门控原始请求。
 * STOP 锁存位于 FPGA，请求清除不自动解锁；人工 REARM 可强制解除当前报警。 */
static int monitor_emergency_brake(void)
{
    monitor_lane_rule();
    // 转向脉冲的20ms等待也持续发布请求，不等待不可靠的开关读回。
    struct timespec ts;
    int clock_ok = clock_gettime(CLOCK_MONOTONIC, &ts) == 0;
    uint32_t now = clock_ok ? (uint32_t)ts.tv_sec : 0;

    if (!emergency_brake_shm &&
        clock_ok && now >= next_brake_attach) {
        if (emergency_brake_shm_init(&emergency_brake_shm, 0) == 0) {
            printf("[自动刹停] 已连接行人检测共享内存\n");
        }
        next_brake_attach = now + 1;
    }

    int person_detected = 0;
    int brake_enable = 1;
    int status_valid = 0;
    uint32_t last_update = 0;
    if (emergency_brake_shm && emergency_brake_shm != (void*)-1 &&
        emergency_brake_shm->frame_count > 0 &&
        emergency_brake_read_status(emergency_brake_shm,
                                    &person_detected, &brake_enable) == 0) {
        status_valid = 1;
        last_update = emergency_brake_shm->last_update;
    }

    int required = brake_required(1,
                                  status_valid && clock_ok,
                                  person_detected, brake_enable,
                                  last_update, now);

    enum { DETECTOR_UNAVAILABLE, DETECTOR_STALE,
           PERSON_REQUEST, HOLD_OR_INFERENCE_FAULT, CLEAR_REQUEST, DIRECTION_WRITE_FAULT };
    int reason = !status_valid || !clock_ok ? DETECTOR_UNAVAILABLE :
        (last_update == 0 || last_update > now ||
         now - last_update >= BRAKE_STATUS_TIMEOUT_SECONDS) ? DETECTOR_STALE :
        person_detected ? PERSON_REQUEST : brake_enable == 0 ? HOLD_OR_INFERENCE_FAULT : CLEAR_REQUEST;
    static int previous_reason = -1;

    /* 只在制动请求由有效变清除或释放失败重试时丢弃旧方向。
     * reg4 的每秒心跳不能顺便写方向0..3，否则会中断转向或外部前进命令。 */
    int refresh_release = !required &&
        (last_brake_value != 1 || direction_release_failed ||
         (!turn_pulse_active &&
          (!clock_ok || now >= next_brake_release_refresh)));
    if (direction_release_failed || (!required && last_brake_value != 1)) {
        if (turn_pulse_active) turn_pulse_interrupted = 1;
        int directions_released = 1;
        for (int i = 0; i < 4; ++i) {
            if (fspi_write_data(i, 1) != 0) {
                directions_released = 0;
            }
        }
        direction_release_failed = !directions_released;
        if (!directions_released) {
            static uint32_t last_release_error_log = 0;
            if (now != last_release_error_log) {
                fprintf(stderr, "[自动刹停] 方向键释放失败，暂停自动转向并重试；发布原始故障请求，最终由FPGA AUTO门控\n");
                last_release_error_log = now;
            }
            last_brake_value = -1;
            required = 1;
            reason = DIRECTION_WRITE_FAULT;
        }
    }

    if (reason != previous_reason) {
        const char *names[] = {"检测段/首帧/时钟不可用", "检测数据超时", "有效person请求",
            "无人确认期或推理故障（详见PCIe日志）", "有效无人、请求清除", "方向寄存器写入故障"};
        printf("[自动刹停链路] %s: valid=%d person=%d brake_enable=%d raw_request=%d -> reg4=%d；AUTO及实际STOP锁存由FPGA决定\n",
               names[reason], status_valid && clock_ok, person_detected, brake_enable, required, required ? 0 : 1);
        previous_reason = reason;
    }

    if (emergency_brake_shm &&
        (!status_valid || !clock_ok || last_update == 0 ||
         last_update > now || now - last_update >= BRAKE_STATUS_TIMEOUT_SECONDS)) {
        emergency_brake_shm_cleanup(emergency_brake_shm, 0);
        emergency_brake_shm = NULL;
        next_brake_attach = now + 1;
    }

    if (required != raw_request_active) {
        printf("[自动刹停] %s\n", required ? "原始请求有效：发送reg4=0；实际刹停由FPGA AUTO开关和锁存决定"
                                           : "原始请求清除：发送reg4=1；不会解除FPGA已有STOP锁存");
        raw_request_active = required;
    }

    int value_to_write = required ? 0 : 1; // 寄存器4低电平请求制动；高电平只清除请求
    int request_write_failed = 0;
    if (required || refresh_release) {
        if (fspi_write_data(4, (uint8_t)value_to_write) == 0) {
            if (value_to_write != last_brake_value) {
                printf("[自动刹停FSPI] reg4=%d写事务返回成功（未读回FPGA）；AUTO及实际STOP锁存由FPGA决定\n", value_to_write);
            }
            last_brake_value = value_to_write;
            if (!required) {
                next_brake_release_refresh = now + 1;
            }
        } else {
            request_write_failed = 1;
            last_brake_value = -1;
            static uint32_t last_error_log = 0;
            if (now != last_error_log) {
                fprintf(stderr, "[自动刹停] 写制动寄存器失败，下一次轮询将重试\n");
                last_error_log = now;
            }
        }
    }

    // 未读到AUTO状态不能阻断关闭状态下的转向。硬件STOP锁存直接门控方向触点；
    // S只在实际寄存器/方向写入失败时中止软件转向，不把原始请求当作已刹停。
    return request_write_failed || direction_release_failed;
}

/* 转向期间持续发布原始请求；实际写入故障结束软件脉冲，STOP由FPGA门控。 */
static int wait_with_brake_checks(unsigned int duration_us)
{
    turn_pulse_active = 1;
    turn_pulse_interrupted = 0;
    while (duration_us > 0 && g_running) {
        unsigned int slice = duration_us < 20000 ? duration_us : 20000;
        usleep(slice);
        if (monitor_emergency_brake() || turn_pulse_interrupted) {
            turn_pulse_active = 0;
            return -1;
        }
        duration_us -= slice;
    }
    turn_pulse_active = 0;
    return g_running ? 0 : -1;
}

// 获取当前时间戳（秒，支持小数）
double get_current_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}


// 信号处理函数
void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("\n收到Ctrl+C信号，正在退出...\n");
        g_running = 0;
    } else if (sig == SIGTERM) {
        printf("\n收到SIGTERM信号，正在退出...\n");
        g_running = 0;
    }
}

/* 保持原机械标定时长。先释放左右寄存器，再产生一个新的低电平脉冲，
 * 使FPGA能在STOP/人工接管后重新接受FSPI；失败不得记成已完成转向。
 * wheel_position只是按脉冲估计的位置，并非方向盘反馈。 */
static void apply_turn_pulse(int command, unsigned int duration_us, WheelPosition target)
{
    if (monitor_emergency_brake()) return;
    int released = vehicle_control(2, 0) == 0;
    if (vehicle_control(3, 0) != 0) released = 0;
    if (!released) {
        direction_release_failed = 1;
        return;
    }
    if (vehicle_control(command, 1) != 0) {
        // 无回读的写失败不能证明引脚仍为高；立即尝试释放并安排后续重试。
        vehicle_control(command, 0);
        direction_release_failed = 1;
        return;
    }
    int completed = wait_with_brake_checks(duration_us) == 0;
    if (vehicle_control(command, 0) != 0) {
        direction_release_failed = 1;
        completed = 0;
    }
    if (completed) {
        wheel_position = target;
        printf("[转向链路] reg%d低脉冲%ums完成并释放；估计位置=%s（非硬件反馈）\n",
               command, duration_us / 1000,
               target == WHEEL_LEFT ? "LEFT" : target == WHEEL_RIGHT ? "RIGHT" : "NEUTRAL");
    } else {
        printf("[转向链路] reg%d脉冲中断或写入失败，未更新估计位置，下轮重试\n", command);
    }
}

void turn_to_left(void) {
    if (wheel_position != WHEEL_LEFT)
        apply_turn_pulse(2, wheel_position == WHEEL_RIGHT ? TIME_LEFT_TO_RIGHT : TIME_NEUTRAL_TO_LEFT,
                         WHEEL_LEFT);
}

void turn_to_right(void) {
    if (wheel_position != WHEEL_RIGHT)
        apply_turn_pulse(3, wheel_position == WHEEL_LEFT ? TIME_LEFT_TO_RIGHT : TIME_NEUTRAL_TO_RIGHT,
                         WHEEL_RIGHT);
}

void turn_to_neutral(void) {
    if (wheel_position == WHEEL_LEFT)
        apply_turn_pulse(3, TIME_LEFT_TO_NEUTRAL, WHEEL_NEUTRAL);
    else if (wheel_position == WHEEL_RIGHT)
        apply_turn_pulse(2, TIME_RIGHT_TO_NEUTRAL, WHEEL_NEUTRAL);
}

// 停止所有车辆运动
void stop_all_vehicle_movement() {
    printf("释放所有方向控制...\n");
    vehicle_control(0, 0);
    vehicle_control(1, 0);
    vehicle_control(2, 0);
    vehicle_control(3, 0);
    wheel_position = WHEEL_NEUTRAL;
    printf("所有方向控制已释放\n");
}

/* 复用显示控制共享段的预留字节发布当前 HMI 按键提示。
 * 每个方向只对应当前按下的键；松开或读取失败时立即发布 NONE。
 * 该值不是 FPGA 触点或车轮速度反馈，过期后显示端也会自动隐藏。 */
static void publish_motion_hint(uint8_t hmi_code)
{
    if (!display_control_shm || !display_control_shm->valid) return;

    VehicleMotionHint motion = VEHICLE_MOTION_NONE;
    /* FPGA 的四线读口将 4 位 HMI 选择码重复送出两次，因此
     * 串口屏功能 04 的 A/B/C/D 在 RK 端分别读作 AA/BB/CC/DD。 */
    if (hmi_code == 0xaa) {
        motion = VEHICLE_MOTION_FORWARD;
    } else if (hmi_code == 0xbb) {
        motion = VEHICLE_MOTION_REVERSE;
    } else if (hmi_code == 0xcc) {
        motion = VEHICLE_MOTION_LEFT;
    } else if (hmi_code == 0xdd) {
        motion = VEHICLE_MOTION_RIGHT;
    }

    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) return;
    uint32_t timestamp_ms = (uint32_t)((uint64_t)ts.tv_sec * 1000u +
                                       (uint32_t)ts.tv_nsec / 1000000u);
    uint64_t snapshot = ((uint64_t)timestamp_ms << 32) | (uint32_t)motion;
    __atomic_store_n(&display_control_shm->motion_snapshot, snapshot, __ATOMIC_RELEASE);
}

int main(int argc, char *argv[]) {
    // 调试日志重定向到文件时也逐行输出，便于实机核对制动请求。
    setvbuf(stdout, NULL, _IOLBF, 0);
    (void)argc;
    (void)argv;
    printf("=== 车辆控制程序 (弯道检测自动转向) ===\n");
    printf("版本: 8.2 (SMDT原生三字节FSPI + 按轴转向 + 制动心跳隔离)\n");
    printf("作者: 科研轮转\n");
    printf("功能: 弯道检测自动左右转 + 人员检测紧急制动 + 显示控制\n");
    printf("工作模式说明:\n");
    printf("- 实时读取: 从FSPI寄存器0读取实际模式值\n");
    printf("- 显示控制: 六个物理视角22/33/44/66/77/88，99六路全景；11/55兼容归并到中间视角\n");
    printf("- 左右转向: 完全由弯道检测自动控制\n");
    printf("- 原始制动请求: 有人或检测不可用写4=0，连续两秒无人写1仅清除请求\n");
    printf("- STOP锁存: FPGA保持低电平直到人工REARM；允许有人时强制解除当前报警\n");
    printf("- 制动开关: HMI功能0F的FF开启/EE关闭，默认关闭和最终门控由FPGA负责；S不依赖开关读回\n");
    printf("- 弯道检测: 自动读取UDP传来的弯道信息，智能控制车辆转向\n");
    printf("  * 智能转向: 左弯→左转，右弯→右转，直道→回中\n");
    printf("- 检测间隔: 每100ms检测一次模式变化\n");
    printf("- 实时响应: 检测到指令时立即执行对应操作\n\n");
    

    /* 转向函数已在文件作用域定义；此处原有的同内容嵌套定义及旧注释代码
     * 不参与实际控制流程，直接使用上方统一实现。 */
    // 注册信号处理函数
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // 初始化FSPI模块
    printf("=== FSPI模块初始化 ===\n");
    if (fspi_init() != 0) {
        printf("FSPI初始化失败！请检查:\n");
        printf("1. SPI设备文件是否存在: /dev/spidev4.0\n");
        printf("2. 是否有权限访问SPI设备 (可能需要sudo)\n");
        printf("3. 硬件连接是否正常\n");
        return -1;
    }
    printf("FSPI初始化成功！\n");

    // 先初始化原始请求与方向为释放；写1不解除已有的FPGA锁存。
    printf("初始化控制寄存器: 制动4写1，方向0-3写1\n");
    if (fspi_write_data(4, 1) != 0) {
        fprintf(stderr, "无法清除初始软件制动请求，停止车辆控制程序\n");
        fspi_cleanup();
        return -1;
    }
    last_brake_value = 1;
    int control_init_ok = 1;
    for (int i = 0; i < 4; i++) {
        if (fspi_write_data(i, 1) == 0) {
            printf("✓ 寄存器%d初始化成功\n", i);
        } else {
            printf("✗ 寄存器%d初始化失败\n", i);
            control_init_ok = 0;
        }
    }
    if (!control_init_ok) {
        fprintf(stderr, "控制寄存器初始化失败，停止车辆控制程序\n");
        fspi_cleanup();
        return -1;
    }
    printf("\n");
    
    // 初始化显示控制共享内存（写入者）
    printf("=== 显示控制共享内存初始化 ===\n");
    if (display_control_shm_init(&display_control_shm, 1) != 0) {
        printf("显示控制共享内存初始化失败！\n");
        fspi_cleanup();
        return -1;
    }
    printf("显示控制共享内存初始化成功\n\n");
    
    // 始终连接检测段并发布原始请求，是否生效由FPGA的HMI开关最终决定。
    printf("=== 紧急制动共享内存初始化 ===\n");
    monitor_emergency_brake();
    printf("\n");

    // 初始化弯道检测共享内存（读取者）
    printf("=== 弯道检测共享内存初始化 ===\n");
    if (curve_detection_shm_init(0) != 0) {  // 0表示读取者
        printf("弯道检测共享内存初始化失败！\n");
        printf("注意: 这可能是因为UDP程序尚未启动\n");
        printf("弯道检测功能将被禁用，但其他功能正常\n");
    } else {
        printf("弯道检测共享内存初始化成功，FSPI将读取UDP传来的弯道信息\n");
    }
    printf("\n");
    
    printf("=== 开始实际模式读取 ===\n");
    printf("将持续监控FSPI寄存器0的模式值变化...\n");
    printf("按 Ctrl+C 退出程序\n\n");
    
    // 模式检测变量
    uint8_t last_read_value = 0xee;  // 上次读取的值
    uint8_t current_valid_mode = 0x00;  // 当前有效的模式值（非0x00）
    uint8_t mode_update_count = 0;

    printf("=== 开始监控FSPI寄存器0 ===\n");
    printf("按钮逻辑: 按下时产生指令值，松开时回到0x00\n");
    printf("支持的指令值:\n");
    printf("  显示控制: 22/33/44/66/77/88(六个物理视角), 99(六路全景)\n");
    printf("  左右转向: 完全由弯道检测自动控制\n");
    printf("  制动功能控制: S持续发布原始请求；HMI AUTO开关和实际STOP锁存由FPGA决定\n");
    printf("  紧急制动: 有人或检测失效写0，连续两秒无人写1；STOP锁存仅由人工REARM解除\n");
    printf("  弯道检测: 自动读取UDP传来的弯道信息并控制车辆转向 [置信度≥60%%时执行]\n");
    if (emergency_brake_shm) {
        printf("  检测通道: 已连接PCIe人员检测；FPGA实际AUTO/STOP状态不作软件推断\n");
    } else {
        printf("  检测通道: 等待PCIe程序；发布原始故障请求，最终由FPGA AUTO门控\n");
    }
    printf("  人员检测停车功能: 开关状态以HMI/FPGA为准，S不伪造开启或关闭状态\n");
    printf("  弯道检测功能: 已启用，自动控制车辆转向\n");
    printf("\n");
    
    // 弯道检测状态变量（在循环外定义，供循环内多个地方使用）
    uint32_t current_curve_frame_id = 0;
    CurveType current_curve_type = CURVE_NONE;
    uint32_t current_curve_confidence = 0;
    uint32_t current_curve_angle = 0;
    int curve_data_valid = 0;  // 标记当前周期的弯道数据是否有效

    while (g_running) {
        // 重置弯道数据有效标志
        curve_data_valid = 0;

        // 1. 读取弯道检测信息（只读取，不控制）
        {
            // 静态变量跟踪状态
            static uint32_t curve_check_count = 0;
            static int read_fail_count = 0;
            static int first_read = 1;  // 首次读取标志

            curve_check_count++;

            // 尝试读取弯道检测结果
            int read_result = curve_detection_read(&current_curve_frame_id, &current_curve_type,
                                     &current_curve_confidence, &current_curve_angle);

            // 首次读取时立即显示状态
            if (first_read) {
                if (read_result != 0) {
                    printf("[Road Detection] WARNING: Shared memory read failed - UDP program may not be running\n");
                } else {
                    printf("[Road Detection] Initialized successfully\n");
                }
                first_read = 0;
            }

            // 读取失败时的处理
            if (read_result != 0) {
                read_fail_count++;
                // 前10次失败立即打印，之后每5秒打印一次
                if (read_fail_count <= 10 || read_fail_count % 50 == 1) {
                    printf("[Road Detection] Shared memory read failed (count: %d) - Check if UDP is running\n", read_fail_count);
                }
            } else {
                // 读取成功，重置失败计数
                if (read_fail_count > 0) {
                    printf("[Road Detection] Shared memory read recovered\n");
                    read_fail_count = 0;
                }

                // 标记数据有效
                curve_data_valid = 1;

                // 每次读取成功都打印读到的路况信息
                const char* curve_name = "";
                switch(current_curve_type) {
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
                const char* pos_name = wheel_position == WHEEL_LEFT ? "LEFT" :
                                       wheel_position == WHEEL_RIGHT ? "RIGHT" : "NEUTRAL";
                printf("[SHM Read] Frame%u: %s | Conf:%u%% | Angle:%u | Wheel:%s\n",
                       current_curve_frame_id, curve_name, current_curve_confidence, current_curve_angle, pos_name);
            }
        }

        // 2. 发布原始制动请求。FPGA决定实际STOP；仅实际写故障暂停软件转向。
        int control_io_failed = monitor_emergency_brake();

        // 3. 根据弯道检测结果执行自动转向控制（在读取FSPI指令之前）
        if (!control_io_failed && curve_data_valid) {
            // 静态变量跟踪上次的弯道类型，用于避免重复执行
            static CurveType last_applied_curve_type = CURVE_NONE;
            static uint32_t last_applied_confidence = 0;

            // 检测弯道类型变化用于日志输出
            int should_log = (current_curve_type != last_applied_curve_type) ||
                            (abs((int)current_curve_confidence - (int)last_applied_confidence) > 20);

            // 根据当前弯道类型执行转向控制（每次都检查）
            if (current_curve_confidence >= 60) {  // 只有置信度足够高时才执行
                switch(current_curve_type) {
                    case CURVE_NONE:
                        if (should_log) {
                            printf("\n========================================\n");
                            printf("Road: STRAIGHT (Frame %u, Conf:%u%%)\n", current_curve_frame_id, current_curve_confidence);
                            printf("========================================\n\n");
                        }
                        turn_to_neutral();
                        break;

                    case CURVE_LEFT:
                        if (should_log) {
                            printf("\n========================================\n");
                            printf("Road: LEFT (Frame %u, Conf:%u%%, Angle:%u)\n",
                                   current_curve_frame_id, current_curve_confidence, current_curve_angle);
                            printf("========================================\n\n");
                        }
                        turn_to_left();
                        break;

                    case CURVE_RIGHT:
                        if (should_log) {
                            printf("\n========================================\n");
                            printf("Road: RIGHT (Frame %u, Conf:%u%%, Angle:%u)\n",
                                   current_curve_frame_id, current_curve_confidence, current_curve_angle);
                            printf("========================================\n\n");
                        }
                        turn_to_right();
                        break;

                    case CURVE_BOTH:
                        if (should_log) {
                            printf("\n========================================\n");
                            printf("Road: S-CURVE (Frame %u, Conf:%u%%)\n", current_curve_frame_id, current_curve_confidence);
                            printf("   Action: Maintain current turn state\n");
                            printf("========================================\n\n");
                        }
                        break;
                }
            }

            // 更新记录值用于下次日志判断
            last_applied_curve_type = current_curve_type;
            last_applied_confidence = current_curve_confidence;
        }

        // 4. 实际从FSPI寄存器0读取模式值
        uint8_t current_mode_value = 0;
        if (fspi_read_data(0, &current_mode_value) != 0) {
            // 读不到最新触摸状态时立即撤销提示，避免继续显示上一次按键。
            publish_motion_hint(0);
            usleep(100000);  // 失败时等待100ms再重试
            continue;
        }
        publish_motion_hint(current_mode_value);

        // 只在值发生变化时记录
        if (current_mode_value != last_read_value) {
            printf("[转向链路] HMI选择码=0x%02X；自动转向写reg2/3，最终引脚由FPGA手动优先及STOP仲裁\n",
                   current_mode_value);
            last_read_value = current_mode_value;
        }

        // 检查是否是有效的显示模式值（非0x00且非车辆控制指令）
        DisplayMode new_mode = DISPLAY_MODE_ALL_CAMERAS;  // 默认值
        const char* mode_name = "未知模式";
        int valid_mode = camera_mode_normalize(current_mode_value, &new_mode);
        if (valid_mode) mode_name = camera_mode_name(new_mode);

        // 只在有效模式且与当前有效模式不同时更新
        if (valid_mode && current_mode_value != current_valid_mode) {
            time_t current_time = time(NULL);
            mode_update_count++;
            current_valid_mode = current_mode_value;  // 更新当前有效模式值
            
            printf("\n=== 检测到按钮按下 ===\n");
            printf("按下按钮值: 0x%02X\n", current_mode_value);
            printf("切换到模式: %s (0x%02X)\n", mode_name, new_mode);
            printf("累计切换次数: %d\n", mode_update_count);
            
            // 更新显示控制共享内存
            if (display_control_shm && display_control_shm->valid) {
                __sync_lock_test_and_set(&display_control_shm->mode, new_mode);
                __sync_lock_test_and_set(&display_control_shm->button_count, mode_update_count);
                __sync_lock_test_and_set(&display_control_shm->last_update, (uint32_t)current_time);
                
                printf("✓ 显示模式已切换到: %s\n", mode_name);
                printf("✓ 共享内存已更新 (模式=0x%02X, 时间戳=%ld)\n", new_mode, current_time);
                printf("✓ 当前有效模式值保存为: 0x%02X\n", current_valid_mode);
            } else {
                printf("✗ 警告: 显示控制共享内存无效，无法更新模式\n");
            }
            
            printf("=== 继续监控按钮状态 ===\n\n");
        } else if (valid_mode && current_mode_value == current_valid_mode) {
            // 相同的有效值，不需要更新，静默处理
        } else if (!valid_mode) {
            // 静默忽略无效值
        }
        
        // 每100ms检测一次，响应更快
        usleep(100000);
    }
    
    publish_motion_hint(0);
    // 程序退出前的清理工作
    printf("\n=== 程序退出清理 ===\n");

    // 退出沿用原始故障请求；AUTO开关与已有STOP锁存均由FPGA决定。
    if (fspi_write_data(4, 0) != 0) {
        fprintf(stderr, "警告: 退出时无法写入制动寄存器，请检查车辆状态\n");
    }

    // 停止所有车辆运动
    stop_all_vehicle_movement();

    // 清理弯道检测共享内存
    printf("清理弯道检测共享内存...\n");
    curve_detection_shm_cleanup();

    // 清理紧急制动共享内存
    if (emergency_brake_shm) {
        printf("清理紧急制动共享内存...\n");
        emergency_brake_shm_cleanup(emergency_brake_shm, 0);
        emergency_brake_shm = NULL;
    }
    
    // 清理显示控制共享内存
    if (display_control_shm) {
        printf("清理显示控制共享内存...\n");
        display_control_shm_cleanup(display_control_shm, 1);
    }
    
    // 清理FSPI资源
    printf("清理FSPI资源...\n");
    fspi_cleanup();
    
    printf("所有资源清理完成，程序退出\n");
    return 0;
}
