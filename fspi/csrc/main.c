/*
 * 独立 FSPI 车辆控制与寄存器检查程序
 * 作者：科研轮转
 *
 * 启动后先执行前进、后退、左转、右转的固定测试序列，再进入单字符命令循环。
 * 运动命令通过 fspi_module 写入 FPGA；本文件不处理摄像头数据或目标识别。
 * 输入 0-4 用于检查对应寄存器的写入，输入 5 读取地址 0 的视镜状态：
 * 0x00 表示前视，0x11 表示后视，其余值按异常值报告。
 * 收到 Ctrl+C、输入结束或退出命令后，统一停止四个方向并释放 FSPI 资源。
 */

#include "fspi_module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

// SIGINT 到来时只修改此标志；主循环在下一次检查时退出并统一清理设备。
static volatile int g_running = 1;

// Ctrl+C 触发有序退出，避免在信号处理函数里直接操作 SPI 设备。
void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("\n收到Ctrl+C信号，正在清理资源...\n");
        g_running = 0;
    }
}

// 退出路径逐项撤销四个方向的运动命令，保持原有停止顺序。
void stop_all_vehicle_movement() {
    printf("停止所有车辆运动...\n");
    vehicle_forward(0);
    vehicle_backward(0);
    vehicle_turn_left(0);
    vehicle_turn_right(0);
    printf("所有运动已停止\n");
}

int main(void) {
    printf("=== 独立车辆控制程序 ===\n");
    printf("版本: 1.0\n");
    printf("作者: 科研轮转\n");
    printf("说明: 通过FSPI控制车辆运动\n\n");
    
    // 仅注册 SIGINT；其余退出路径仍由输入循环处理。
    signal(SIGINT, signal_handler);
    
    // 在发送任何运动指令前打开并配置 FSPI；失败时直接返回。
    printf("=== FSPI模块初始化 ===\n");
    if (fspi_init() != 0) {
        printf("FSPI初始化失败！请检查:\n");
        printf("1. SPI设备文件是否存在: /dev/spidev4.0\n");
        printf("2. 是否有权限访问SPI设备 (可能需要sudo)\n");
        printf("3. 硬件连接是否正常\n");
        return -1;
    }
    
    printf("FSPI初始化成功！\n\n");
    
    // 启动后按固定次序运行四个方向的动作，随后才允许手动输入。
    printf("=== 开始车辆控制测试序列 ===\n");
    printf("测试序列: 前进1秒 → 后退1秒 → 左转1秒 → 右转1秒\n\n");
    
    // 依次验证四个运动方向。每一步都保持“启动、运行一秒、停止”的调用顺序；
    // 前三步之后再等待一秒，让下一动作开始前留出与原程序相同的停顿。
    typedef struct {
        const char* begin_message;
        const char* done_message;
        const char* error_message;
        int (*set_enabled)(int);
    } MovementStep;
    const MovementStep steps[] = {
        {"1. 车辆前进 (1秒)...\n", "   前进完成\n", "   前进失败\n", vehicle_forward},
        {"2. 车辆后退 (1秒)...\n", "   后退完成\n", "   后退失败\n", vehicle_backward},
        {"3. 车辆左转 (1秒)...\n", "   左转完成\n", "   左转失败\n", vehicle_turn_left},
        {"4. 车辆右转 (1秒)...\n", "   右转完成\n", "   右转失败\n", vehicle_turn_right},
    };

    for (size_t i = 0; i < sizeof(steps) / sizeof(steps[0]); ++i) {
        printf("%s", steps[i].begin_message);
        if (steps[i].set_enabled(1) != 0) {
            printf("%s", steps[i].error_message);
            goto cleanup;
        }

        sleep(1);
        steps[i].set_enabled(0);
        printf("%s", steps[i].done_message);

        if (i + 1 < sizeof(steps) / sizeof(steps[0])) {
            printf("   暂停 (0.5秒)\n");
            sleep(1);
        }
    }

    printf("\n=== 车辆控制测试序列完成 ===\n");
    
    // 预设动作结束后进入单字符命令模式，提供寄存器读写检查。
    printf("\n=== 进入手动寄存器测试模式 ===\n");
    printf("输入命令:\n");
    printf("  0 - 测试0号寄存器 (先写0，等1秒，再写1)\n");
    printf("  1 - 测试1号寄存器 (先写0，等1秒，再写1)\n");
    printf("  2 - 测试2号寄存器 (先写0，等1秒，再写1)\n");
    printf("  3 - 测试3号寄存器 (先写0，等1秒，再写1)\n");
    printf("  4 - 测试4号寄存器 (先写0，等1秒，再写1)\n");
    printf("  5 - 读取0号寄存器检查小车视镜功能状态\n");
    printf("      (0x00=前视镜模式, 0x11=后视镜模式, 其他值=异常)\n");
    printf("  q - 退出程序\n");
    printf("=======================================\n\n");
    
    // 循环读取一行输入；只有单个字符会被解释为命令。
    printf("请输入命令: ");
    fflush(stdout);
    
    while (g_running) {
        char buffer[100];
        
        // fgets 限制最大输入长度，避免超出本地命令缓冲区。
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // 去掉首个换行符，使带回车的单字符输入仍能被识别。
            buffer[strcspn(buffer, "\n")] = 0;
            
            // 根据去掉换行符后的长度区分有效命令和多字符输入。
            size_t command_length = strlen(buffer);
            if (command_length == 1) {
                char cmd = buffer[0];
                
                // 数字 0-4 对应同号寄存器；先写 0，等待后再写 1。
                if (cmd >= '0' && cmd <= '4') {
                    int reg_num = cmd - '0';
                    printf("\n=== 测试%d号寄存器 ===\n", reg_num);
                    
                    // 第一次写入 0；失败后跳过本次测试，返回输入提示。
                    printf("步骤1: 向寄存器%d写入0...\n", reg_num);
                    if (fspi_write_data((uint16_t)reg_num, 0) == 0) {
                        printf("写入0成功\n");
                    } else {
                        printf("写入0失败！\n");
                        goto next_input;
                    }
                    
                    // 两次写入间保留一秒间隔，维持原有寄存器测试时序。
                    printf("步骤2: 等待1秒...\n");
                    sleep(1);
                    
                    // 第二次写入 1；无论结果如何，都报告本次测试结束。
                    printf("步骤3: 向寄存器%d写入1...\n", reg_num);
                    if (fspi_write_data((uint16_t)reg_num, 1) == 0) {
                        printf("写入1成功\n");
                    } else {
                        printf("写入1失败！\n");
                    }
                    
                    printf("=== 寄存器%d测试完成 ===\n", reg_num);
                }
                // 命令 5 查询地址 0，按协议值解释前视、后视或异常状态。
                else if (cmd == '5') {
                    printf("\n=== 读取0号寄存器 - 检查小车视镜功能状态 ===\n");
                    uint8_t read_data = 0;
                    
                    // 地址 0 保存视镜模式：0x00 为前视，0x11 为后视；其他值交给异常分支。
                    if (fspi_read_data(0, &read_data) == 0) {
                        printf("✓ 读取成功: 寄存器0 = 0x%02X (%d)\n", read_data, read_data);
                        
                        // 仅对两个已定义的状态值显示正常模式说明。
                        switch (read_data) {
                            case 0x00:
                                printf("� 视镜功能状态: 前视镜模式 (FRONT VIEW)\n");
                                printf("   说明: 小车当前使用前视镜，用于前方视野观察\n");
                                break;
                            case 0x11:
                                printf("📱 视镜功能状态: 后视镜模式 (REAR VIEW)\n");
                                printf("   说明: 小车当前使用后视镜，用于后方视野观察\n");
                                break;
                            default:
                                printf("⚠️  视镜功能状态: 异常 (值=0x%02X)\n", read_data);
                                printf("   说明: 寄存器值异常，应为0x00(前视镜)或0x11(后视镜)\n");
                                break;
                        }
                    } else {
                        printf("✗ 读取失败: 寄存器0\n");
                        printf("   原因: 可能是FSPI通信故障或硬件连接问题\n");
                        printf("   建议: 检查SPI连接线路和FPGA状态\n");
                    }
                }
                // q/Q 设置退出标志，并在离开循环后走统一清理路径。
                else if (cmd == 'q' || cmd == 'Q') {
                    printf("用户请求退出程序\n");
                    g_running = 0;
                    break;
                }
                else {
                    printf("无效命令: %c\n", cmd);
                }
            }
            else if (command_length > 1) {
                printf("请输入单个字符命令 (0-5, q)\n");
            }
            
next_input:
            // 继续运行时刷新提示符，保证交互终端立即显示它。
            if (g_running) {
                printf("\n请输入命令: ");
                fflush(stdout);
            }
        }
        else {
            // 标准输入到达 EOF 时结束程序；其他读取错误保持原有重试行为。
            if (feof(stdin)) {
                printf("\n检测到EOF，程序退出\n");
                g_running = 0;
            }
        }
    }
    
cleanup:
    // 所有正常退出和测试失败路径都汇合于此，先停机再释放 FSPI。
    printf("\n=== 程序退出清理 ===\n");
    
    // 退出路径逐项撤销四个方向的运动命令，保持原有停止顺序。
    stop_all_vehicle_movement();
    
    // 停止动作后关闭 FSPI 设备并释放模块内部资源。
    fspi_cleanup();
    
    printf("资源清理完成，程序退出\n");
    
    return 0;
}