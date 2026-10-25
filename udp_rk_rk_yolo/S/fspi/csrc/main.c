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
 *   - 显示切换: 0x11-0x88对应8个摄像头，0x99对应所有摄像头
 *
 * 创建时间：2025年10月20日
 * 修改时间：2025年11月18日 - 删除前进后退控制，只保留转向和制动
 */

#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#include "fspi_module.h"
#include "brake_policy.h"
#include "../include/shared_memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

// 方向盘转向时间参数(微秒)
#define TIME_NEUTRAL_TO_LEFT   500000   // 中间→左 (0.8秒)
#define TIME_NEUTRAL_TO_RIGHT  600000   // 中间→右 (0.8秒)
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

// 上电后默认启用行人制动；原来的 0xff/0xee 指令仍可切换该功能。
static int person_detection_brake_enabled = 1;
static uint8_t last_toggle_command = 0x00;      // 上次处理的切换指令值，避免重复处理
static int last_brake_value = -1;
static uint32_t next_brake_release_refresh = 0;
static int brake_active = 1;
static int turn_pulse_active = 0;
static uint32_t next_brake_attach = 0;

/* 读端只接受检测程序最近两秒写入的状态。检测程序未启动、退出或卡住时保持制动请求。
 * 请求有效时重复写低电平；请求清除时每秒重写高电平，避免无回执的 SPI 写入丢失。
 * STOP 锁存位于 FPGA，请求清除不自动解锁；人工 REARM 可强制解除当前报警。 */
static int monitor_emergency_brake(void)
{
    struct timespec ts;
    int clock_ok = clock_gettime(CLOCK_MONOTONIC, &ts) == 0;
    uint32_t now = clock_ok ? (uint32_t)ts.tv_sec : 0;

    if (person_detection_brake_enabled && !emergency_brake_shm &&
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

    int required = brake_required(person_detection_brake_enabled,
                                  status_valid && clock_ok,
                                  person_detected, brake_enable,
                                  last_update, now);

    /* FPGA 复位时方向寄存器默认高电平。每次清除软件制动请求前仍重写
     * 四个方向为高电平，丢弃旧方向命令；任一路失败就保留请求并在下轮重试。 */
    int refresh_release = !required &&
        (last_brake_value != 1 ||
         (!turn_pulse_active &&
          (!clock_ok || now >= next_brake_release_refresh)));
    if (refresh_release) {
        int directions_released = 1;
        for (int i = 0; i < 4; ++i) {
            if (fspi_write_data(i, 1) != 0) {
                directions_released = 0;
            }
        }
        if (!directions_released) {
            static uint32_t last_release_error_log = 0;
            if (now != last_release_error_log) {
                fprintf(stderr, "[自动刹停] 方向键释放失败，保持制动请求并重试\n");
                last_release_error_log = now;
            }
            last_brake_value = -1;
            required = 1;
        }
    }

    if (emergency_brake_shm && person_detection_brake_enabled &&
        (!status_valid || !clock_ok || last_update == 0 ||
         last_update > now || now - last_update >= BRAKE_STATUS_TIMEOUT_SECONDS)) {
        emergency_brake_shm_cleanup(emergency_brake_shm, 0);
        emergency_brake_shm = NULL;
        next_brake_attach = now + 1;
    }

    if (required != brake_active) {
        printf("[自动刹停] %s\n", required ? "请求有效：发现行人或检测数据不可用，FPGA 锁存 STOP（人工可强制 REARM）"
                                           : "请求清除：软件不再请求自动制动，FPGA 锁存仍须人工 REARM 解除");
        brake_active = required;
    }

    int value_to_write = required ? 0 : 1; // 寄存器4低电平请求制动；高电平只清除请求
    if (required || refresh_release) {
        if (fspi_write_data(4, (uint8_t)value_to_write) == 0) {
            last_brake_value = value_to_write;
            if (!required) {
                next_brake_release_refresh = now + 1;
            }
        } else {
            if (!required) {
                last_brake_value = -1;
            }
            static uint32_t last_error_log = 0;
            if (now != last_error_log) {
                fprintf(stderr, "[自动刹停] 写制动寄存器失败，下一次轮询将重试\n");
                last_error_log = now;
            }
        }
    }

    // 软件请求有效或清除请求未成功写入时，不发起自动转向；HMI 解锁由 FPGA 处理。
    return required || last_brake_value != 1;
}

/* 制动请求出现时立即结束当前转向脉冲；清除请求的刷新等脉冲结束后再做。 */
static int wait_with_brake_checks(unsigned int duration_us)
{
    turn_pulse_active = 1;
    while (duration_us > 0 && g_running) {
        unsigned int slice = duration_us < 20000 ? duration_us : 20000;
        usleep(slice);
        if (monitor_emergency_brake()) {
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

// 转到左边
void turn_to_left() {
    if (wheel_position == WHEEL_LEFT) return;

    if (monitor_emergency_brake()) return;

    if (wheel_position == WHEEL_RIGHT) {
        vehicle_control(2, 1);
        int completed = wait_with_brake_checks(TIME_LEFT_TO_RIGHT) == 0;
        vehicle_control(2, 0);
        if (completed) wheel_position = WHEEL_LEFT;
    } else {
        vehicle_control(2, 1);
        int completed = wait_with_brake_checks(TIME_NEUTRAL_TO_LEFT) == 0;
        vehicle_control(2, 0);
        if (completed) wheel_position = WHEEL_LEFT;
    }
}

// 转到右边
void turn_to_right() {
    if (wheel_position == WHEEL_RIGHT) return;

    if (monitor_emergency_brake()) return;

    if (wheel_position == WHEEL_LEFT) {
        vehicle_control(3, 1);
        int completed = wait_with_brake_checks(TIME_LEFT_TO_RIGHT) == 0;
        vehicle_control(3, 0);
        if (completed) wheel_position = WHEEL_RIGHT;
    } else {
        vehicle_control(3, 1);
        int completed = wait_with_brake_checks(TIME_NEUTRAL_TO_RIGHT) == 0;
        vehicle_control(3, 0);
        if (completed) wheel_position = WHEEL_RIGHT;
    }
}

// 回中
void turn_to_neutral() {
    if (wheel_position == WHEEL_NEUTRAL) return;

    if (monitor_emergency_brake()) return;

    if (wheel_position == WHEEL_LEFT) {
        vehicle_control(3, 1);
        if (wait_with_brake_checks(TIME_LEFT_TO_NEUTRAL) != 0) {
            vehicle_control(3, 0);
            return;
        }
    } else {
        vehicle_control(2, 1);
        if (wait_with_brake_checks(TIME_RIGHT_TO_NEUTRAL) != 0) {
            vehicle_control(2, 0);
            return;
        }
    }
    vehicle_control(2, 0);
    vehicle_control(3, 0);
    wheel_position = WHEEL_NEUTRAL;
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

int main(int argc, char *argv[]) {
    printf("=== 车辆控制程序 (弯道检测自动转向) ===\n");
    printf("版本: 8.0 (弯道检测自动转向 + 紧急制动)\n");
    printf("作者: 科研轮转\n");
    printf("功能: 弯道检测自动左右转 + 人员检测紧急制动 + 显示控制\n");
    printf("工作模式说明:\n");
    printf("- 实时读取: 从FSPI寄存器0读取实际模式值\n");
    printf("- 显示控制: 0x11-0x88对应8个摄像头，0x99对应所有摄像头\n");
    printf("- 左右转向: 完全由弯道检测自动控制\n");
    printf("- 紧急制动: 默认开启；有人或检测不可用时寄存器4写0，连续两秒无人写1仅清除请求\n");
    printf("- STOP锁存: FPGA保持低电平直到人工REARM；允许有人时强制解除当前报警\n");
    printf("- 制动控制: 预留0xff开启、0xee关闭指令（当前FPGA读回仅4位）\n");
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

    // 先发布制动请求，再释放四个方向键；请求清除后 STOP 仍须人工 REARM 解锁。
    printf("初始化控制寄存器: 制动4写0，方向0-3写1\n");
    if (fspi_write_data(4, 0) != 0) {
        fprintf(stderr, "无法设置初始制动，停止车辆控制程序\n");
        fspi_cleanup();
        return -1;
    }
    last_brake_value = 0;
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
    
    // 检测程序若尚未启动，轮询函数会继续发布制动请求并定期重连。
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
    printf("  显示控制: 0x11-0x88(单摄像头), 0x99(全部摄像头)\n");
    printf("  左右转向: 完全由弯道检测自动控制\n");
    printf("  制动功能控制: 默认开启；0xff/0xee为当前FPGA尚不支持的预留指令\n");
    printf("  紧急制动: 有人或检测失效写0，连续两秒无人写1；STOP锁存仅由人工REARM解除\n");
    printf("  弯道检测: 自动读取UDP传来的弯道信息并控制车辆转向 [置信度≥60%%时执行]\n");
    if (emergency_brake_shm) {
        printf("  紧急制动状态: 已启用，正在监控PCIe人员检测\n");
    } else {
        printf("  紧急制动状态: 等待PCIe程序，当前软件制动请求有效\n");
    }
    printf("  人员检测停车功能: %s (0xff开启，0xee关闭)\n",
           person_detection_brake_enabled ? "已开启" : "已关闭");
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

        // 2. 制动请求优先于自动转向；检测程序断线或数据过期也保持软件请求。
        int braking = monitor_emergency_brake();

        // 3. 根据弯道检测结果执行自动转向控制（在读取FSPI指令之前）
        if (!braking && curve_data_valid) {
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
            usleep(100000);  // 失败时等待100ms再重试
            continue;
        }

        // 只在值发生变化时记录
        if (current_mode_value != last_read_value) {
            last_read_value = current_mode_value;
        }

        // 5. 检查是否是人员检测停车功能控制指令
        if ((current_mode_value == 0xff || current_mode_value == 0xee) && 
            current_mode_value != last_toggle_command) {
            
            printf("\n=== 人员检测停车功能控制 ===\n");
            printf("接收到指令: 0x%02X", current_mode_value);
            
            // 根据不同指令设置功能状态：0xff=开启，0xee=关闭
            if (current_mode_value == 0xff) {
                if (!person_detection_brake_enabled) {
                    person_detection_brake_enabled = 1;
                    printf(" (开启功能)\n");
                    printf("✅ 人员检测停车功能已开启\n");
                    printf("🛡️  提示: 检测到人员时将自动触发紧急制动\n");
                } else {
                    printf(" (已经开启)\n");
                    printf("✅ 人员检测停车功能保持开启状态\n");
                    printf("🛡️  提示: 功能已经处于开启状态\n");
                }
            } else if (current_mode_value == 0xee) {
                if (person_detection_brake_enabled) {
                    person_detection_brake_enabled = 0;
                    printf(" (关闭功能)\n");
                    printf("🚫 人员检测停车功能已关闭\n");
                    printf("⚠️  注意: 不再发布新的行人制动请求；已有FPGA锁存仍须人工REARM解除\n");
                } else {
                    printf(" (已经关闭)\n");
                    printf("🚫 人员检测停车功能保持关闭状态\n");
                    printf("⚠️  提示: 功能已经处于关闭状态；已有FPGA锁存仍须人工REARM解除\n");
                }
            }
            
            last_toggle_command = current_mode_value;  // 记录当前指令，避免重复处理
            
            printf("🔄 当前状态: %s\n", person_detection_brake_enabled ? "已开启" : "已关闭");
            printf("💡 说明: 发送0xff开启功能，发送0xee关闭功能\n");
            printf("=== 继续监控指令 ===\n\n");
            usleep(100000);
            continue;
        }
        
        // 检查是否是有效的显示模式值（非0x00且非车辆控制指令）
        DisplayMode new_mode = DISPLAY_MODE_ALL_CAMERAS;  // 默认值
        const char* mode_name = "未知模式";
        int valid_mode = 1;
        
        switch (current_mode_value) {
            case 0x11:
                new_mode = DISPLAY_MODE_PCIE_TOP_LEFT;
                mode_name = "PCIe左上角摄像头";
                break;
            case 0x22:
                new_mode = DISPLAY_MODE_PCIE_BOTTOM_LEFT;
                mode_name = "后左摄像头";
                break;
            case 0x33:
                new_mode = DISPLAY_MODE_PCIE_TOP_RIGHT;
                mode_name = "后右摄像头";
                break;
            case 0x44:
                new_mode = DISPLAY_MODE_PCIE_BOTTOM_RIGHT;
                mode_name = "PCIe右下角摄像头";
                break;
            case 0x55:
                new_mode = DISPLAY_MODE_UDP_TOP_LEFT;
                mode_name = "UDP左上角摄像头";
                break;
            case 0x66:
                new_mode = DISPLAY_MODE_UDP_TOP_RIGHT;
                mode_name = "UDP右上角摄像头";
                break;
            case 0x77:
                new_mode = DISPLAY_MODE_UDP_BOTTOM_LEFT;
                mode_name = "UDP左下角摄像头";
                break;
            case 0x88:
                new_mode = DISPLAY_MODE_UDP_BOTTOM_RIGHT;
                mode_name = "UDP右下角摄像头";
                break;
            case 0x99:
                new_mode = DISPLAY_MODE_ALL_CAMERAS;
                mode_name = "所有8个摄像头";
                break;
            default:
                valid_mode = 0;  // 无效模式，跳过处理
                break;
        }
        
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
    
    // 程序退出前的清理工作
    printf("\n=== 程序退出清理 ===\n");

    // 车控进程停止监测前发布制动请求；FPGA 保持 STOP 锁存直到人工 REARM。
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
