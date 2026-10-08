/*
 * 车辆控制程序 (弯道检测自动转向)
 * 作者：辉哥大盗
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

// 人员检测停车功能控制
static int person_detection_brake_enabled = 0;  // 人员检测停车功能开关：1=开启，0=关闭
static uint8_t last_toggle_command = 0x00;      // 上次处理的切换指令值，避免重复处理

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

    if (wheel_position == WHEEL_RIGHT) {
        vehicle_control(2, 1);
        usleep(TIME_LEFT_TO_RIGHT);
        wheel_position = WHEEL_LEFT;
    } else {
        vehicle_control(2, 1);
        usleep(TIME_NEUTRAL_TO_LEFT);
        wheel_position = WHEEL_LEFT;
    }
    vehicle_control(2, 0);
}

// 转到右边
void turn_to_right() {
    if (wheel_position == WHEEL_RIGHT) return;

    if (wheel_position == WHEEL_LEFT) {
        vehicle_control(3, 1);
        usleep(TIME_LEFT_TO_RIGHT);
        wheel_position = WHEEL_RIGHT;
    } else {
        vehicle_control(3, 1);
        usleep(TIME_NEUTRAL_TO_RIGHT);
        wheel_position = WHEEL_RIGHT;
    }
    vehicle_control(3, 0);
}

// 回中
void turn_to_neutral() {
    if (wheel_position == WHEEL_NEUTRAL) return;

    if (wheel_position == WHEEL_LEFT) {
        vehicle_control(3, 1);
        usleep(TIME_LEFT_TO_NEUTRAL);
    } else {
        vehicle_control(2, 1);
        usleep(TIME_RIGHT_TO_NEUTRAL);
    }
    vehicle_control(2, 0);
    vehicle_control(3, 0);
    wheel_position = WHEEL_NEUTRAL;
}

// 停止所有车辆运动
void stop_all_vehicle_movement() {
    printf("停止所有转向运动...\n");
    vehicle_control(2, 0);
    vehicle_control(3, 0);
    wheel_position = WHEEL_NEUTRAL;
    printf("所有转向运动已停止\n");
}

int main(int argc, char *argv[]) {
    printf("=== 车辆控制程序 (弯道检测自动转向) ===\n");
    printf("版本: 8.0 (弯道检测自动转向 + 紧急制动)\n");
    printf("作者: 辉哥大盗\n");
    printf("功能: 弯道检测自动左右转 + 人员检测紧急制动 + 显示控制\n");
    printf("工作模式说明:\n");
    printf("- 实时读取: 从FSPI寄存器0读取实际模式值\n");
    printf("- 显示控制: 0x11-0x88对应8个摄像头，0x99对应所有摄像头\n");
    printf("- 左右转向: 完全由弯道检测自动控制\n");
    printf("- 紧急制动: PCIe检测到人员时写0到寄存器4，无人时写1\n");
    printf("- 制动控制: 0xff开启人员检测停车功能，0xee关闭该功能\n");
    printf("- 弯道检测: 自动读取UDP传来的弯道信息，智能控制车辆转向\n");
    printf("  * 智能转向: 左弯→左转，右弯→右转，直道→回中\n");
    printf("- 检测间隔: 每100ms检测一次模式变化\n");
    printf("- 实时响应: 检测到指令时立即执行对应操作\n\n");
    

// 转到左边
void turn_to_left() {
    if (wheel_position == WHEEL_LEFT) return;

    if (wheel_position == WHEEL_RIGHT) {
        vehicle_control(2, 1);
        usleep(TIME_LEFT_TO_RIGHT);
        wheel_position = WHEEL_LEFT;
    } else {
        vehicle_control(2, 1);
        usleep(TIME_NEUTRAL_TO_LEFT);
        wheel_position = WHEEL_LEFT;
    }
    vehicle_control(2, 0);
}

// 转到右边
// void turn_to_right() {
//     if (wheel_position == WHEEL_RIGHT) return;

//     if (wheel_position == WHEEL_LEFT) {
//         vehicle_control(3, 1);
//         usleep(TIME_LEFT_TO_RIGHT);
//         wheel_position = WHEEL_RIGHT;
//     } else {
//         vehicle_control(3, 1);
//         usleep(TIME_NEUTRAL_TO_RIGHT);
//         wheel_position = WHEEL_RIGHT;
//     }
//     vehicle_control(3, 0);
// }

// // 回中
// void turn_to_neutral() {
//     if (wheel_position == WHEEL_NEUTRAL) return;

//     if (wheel_position == WHEEL_LEFT) {
//         vehicle_control(3, 1);
//         usleep(TIME_LEFT_TO_NEUTRAL);
//     } else {
//         vehicle_control(2, 1);
//         usleep(TIME_RIGHT_TO_NEUTRAL);
//     }
//     vehicle_control(2, 0);
//     vehicle_control(3, 0);
//     wheel_position = WHEEL_NEUTRAL;
// }
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

    // 初始化所有控制寄存器为1
    printf("初始化所有控制寄存器: 写1到寄存器0-4\n");
    for (int i = 0; i <= 4; i++) {
        if (fspi_write_data(i, 1) == 0) {
            printf("✓ 寄存器%d初始化成功\n", i);
        } else {
            printf("✗ 寄存器%d初始化失败\n", i);
        }
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
    
    // 初始化紧急制动共享内存（读取者）
    printf("=== 紧急制动共享内存初始化 ===\n");
    if (emergency_brake_shm_init(&emergency_brake_shm, 0) != 0) {
        printf("紧急制动共享内存初始化失败！请先启动PCIe检测程序\n");
        printf("提示: 紧急制动功能需要PCIe程序先启动创建共享内存\n");
        emergency_brake_shm = NULL;  // 设为NULL，程序继续运行但没有紧急制动功能
    } else {
        printf("紧急制动共享内存初始化成功\n");
    }
    printf("\n");
    
    // 初始化紧急制动共享内存（读取者）
    printf("=== 紧急制动共享内存初始化 ===\n");
    if (emergency_brake_shm_init(&emergency_brake_shm, 0) != 0) {
        printf("紧急制动共享内存初始化失败！\n");
        printf("注意: 这可能是因为PCIe程序尚未启动或已退出\n");
        printf("紧急制动功能将被禁用，但其他功能正常\n");
        emergency_brake_shm = NULL;  // 设置为NULL表示不可用
    } else {
        printf("紧急制动共享内存初始化成功\n");
    }
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
    printf("  制动功能控制: 0xff(开启人员检测停车功能), 0xee(关闭人员检测停车功能)\n");
    printf("  紧急制动: 自动检测人员控制寄存器4 [有人:写0, 无人:写1]\n");
    printf("  弯道检测: 自动读取UDP传来的弯道信息并控制车辆转向 [置信度≥60%%时执行]\n");
    if (emergency_brake_shm) {
        printf("  紧急制动状态: 已启用，正在监控PCIe人员检测\n");
    } else {
        printf("  紧急制动状态: 未启用（PCIe程序未运行）\n");
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
        printf("test\n");

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

        // 2. 检查紧急制动状态
        if (emergency_brake_shm) {
            int person_detected = 0;
            int brake_enable = 1;

            if (emergency_brake_read_status(emergency_brake_shm, &person_detected, &brake_enable) == 0) {
                static int last_person_detected = -1;  // 记录上次人员检测状态
                static int last_written_value = -1;    // 记录上次写入寄存器4的值

                // 记录人员检测状态变化
                if (person_detected != last_person_detected) {
                    printf("📡 人员检测状态: %s → %s\n",
                           last_person_detected == -1 ? "初始化" : (last_person_detected ? "有人员" : "无人员"),
                           person_detected ? "有人员" : "无人员");
                    last_person_detected = person_detected;
                }

                // 计算需要写入的值
                int value_to_write;
                if (!person_detection_brake_enabled) {
                    // 功能已关闭，保持寄存器4为1（正常运行）
                    value_to_write = 1;
                } else {
                    // 功能已开启，根据人员检测状态控制
                    value_to_write = person_detected ? 0 : 1;
                }

                // 只在值需要改变时才写入
                if (value_to_write != last_written_value) {
                    fspi_write_data(4, value_to_write);
                    last_written_value = value_to_write;

                    // 打印状态变化信息
                    if (person_detection_brake_enabled) {
                        if (value_to_write == 0) {
                            printf("\n🚨 紧急制动: 检测到人员，写0到寄存器4\n");
                        } else {
                            printf("\n✅ 正常运行: 无人员，写1到寄存器4\n");
                        }
                    } else {
                        printf("\n📊 制动功能已关闭: 写1到寄存器4保持正常运行\n");
                    }
                }
            } else {
                // 如果读取失败，可能PCIe程序已退出
                static int shm_error_count = 0;
                if (++shm_error_count >= 50) {  // 5秒后报告一次
                    printf("警告: 紧急制动共享内存读取失败，PCIe程序可能已退出\n");
                    shm_error_count = 0;
                }
            }
        }

        // 3. 根据弯道检测结果执行自动转向控制（在读取FSPI指令之前）
        if (curve_data_valid) {
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
                    printf("⚠️  注意: 即使检测到人员，车辆也不会自动停车\n");
                } else {
                    printf(" (已经关闭)\n");
                    printf("🚫 人员检测停车功能保持关闭状态\n");
                    printf("⚠️  提示: 功能已经处于关闭状态\n");
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