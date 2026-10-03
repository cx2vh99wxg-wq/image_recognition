/*
 * 独立车辆控制程序
 * 作者：辉哥大盗 
 * 功能：通过FSPI控制车辆运动，不包含图像识别逻辑
 * 特殊功能：读取寄存器判断小车视镜功能状态
 * 
 * 寄存器地址分配：
 *   地址0: 小车视镜功能状态寄存器
 *          0x00 = 前视镜模式
 *          0x11 = 后视镜模式  
 *          其他值 = 异常状态
 *   地址1-4: 其他功能寄存器
 * 
 * 创建时间：2025年10月20日
 * 修改时间：2025年10月27日 - 添加视镜功能检测注释
 */

#include "fspi_module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

// 全局变量
static volatile int g_running = 1;

// 信号处理函数
void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("\n收到Ctrl+C信号，正在清理资源...\n");
        g_running = 0;
    }
}

// 停止所有车辆运动
void stop_all_vehicle_movement() {
    printf("停止所有车辆运动...\n");
    vehicle_forward(0);
    vehicle_backward(0);
    vehicle_turn_left(0);
    vehicle_turn_right(0);
    printf("所有运动已停止\n");
}

int main(int argc, char *argv[]) {
    printf("=== 独立车辆控制程序 ===\n");
    printf("版本: 1.0\n");
    printf("作者: 辉哥大盗\n");
    printf("说明: 通过FSPI控制车辆运动\n\n");
    
    // 注册信号处理函数
    signal(SIGINT, signal_handler);
    
    // 初始化FSPI模块
    printf("=== FSPI模块初始化 ===\n");
    if (fspi_init() != 0) {
        printf("FSPI初始化失败！请检查:\n");
        printf("1. SPI设备文件是否存在: /dev/spidev4.0\n");
        printf("2. 是否有权限访问SPI设备 (可能需要sudo)\n");
        printf("3. 硬件连接是否正常\n");
        return -1;
    }
    
    printf("FSPI初始化成功！\n\n");
    
    // 执行预设测试序列
    printf("=== 开始车辆控制测试序列 ===\n");
    printf("测试序列: 前进1秒 → 后退1秒 → 左转1秒 → 右转1秒\n\n");
    
    // 前进1秒
    printf("1. 车辆前进 (1秒)...\n");
    if (vehicle_forward(1) == 0) {
        sleep(1);
        vehicle_forward(0);
        printf("   前进完成\n");
    } else {
        printf("   前进失败\n");
        goto cleanup;
    }
    
    // 停止0.5秒
    printf("   暂停 (0.5秒)\n");
    sleep(1);  // 使用1秒暂停，确保动作清晰
    
    // 后退1秒
    printf("2. 车辆后退 (1秒)...\n");
    if (vehicle_backward(1) == 0) {
        sleep(1);
        vehicle_backward(0);
        printf("   后退完成\n");
    } else {
        printf("   后退失败\n");
        goto cleanup;
    }
    
    // 停止0.5秒
    printf("   暂停 (0.5秒)\n");
    sleep(1);
    
    // 左转1秒
    printf("3. 车辆左转 (1秒)...\n");
    if (vehicle_turn_left(1) == 0) {
        sleep(1);
        vehicle_turn_left(0);
        printf("   左转完成\n");
    } else {
        printf("   左转失败\n");
        goto cleanup;
    }
    
    // 停止0.5秒
    printf("   暂停 (0.5秒)\n");
    sleep(1);
    
    // 右转1秒
    printf("4. 车辆右转 (1秒)...\n");
    if (vehicle_turn_right(1) == 0) {
        sleep(1);
        vehicle_turn_right(0);
        printf("   右转完成\n");
    } else {
        printf("   右转失败\n");
        goto cleanup;
    }
    
    printf("\n=== 车辆控制测试序列完成 ===\n");
    
    // 进入手动控制模式
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
    
    // 主循环 - 等待用户输入
    printf("请输入命令: ");
    fflush(stdout);
    
    while (g_running) {
        char buffer[100];
        
        // 获取用户输入
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // 去除换行符
            buffer[strcspn(buffer, "\n")] = 0;
            
            // 处理单字符命令
            if (strlen(buffer) == 1) {
                char cmd = buffer[0];
                
                // 处理数字输入 0-4
                if (cmd >= '0' && cmd <= '4') {
                    int reg_num = cmd - '0';
                    printf("\n=== 测试%d号寄存器 ===\n", reg_num);
                    
                    // 先写0
                    printf("步骤1: 向寄存器%d写入0...\n", reg_num);
                    if (fspi_write_data((uint16_t)reg_num, 0) == 0) {
                        printf("写入0成功\n");
                    } else {
                        printf("写入0失败！\n");
                        goto next_input;
                    }
                    
                    // 等待1秒
                    printf("步骤2: 等待1秒...\n");
                    sleep(1);
                    
                    // 再写1
                    printf("步骤3: 向寄存器%d写入1...\n", reg_num);
                    if (fspi_write_data((uint16_t)reg_num, 1) == 0) {
                        printf("写入1成功\n");
                    } else {
                        printf("写入1失败！\n");
                    }
                    
                    printf("=== 寄存器%d测试完成 ===\n", reg_num);
                }
                // 处理数字输入 5 - 读取0号寄存器检查小车视镜功能状态
                else if (cmd == '5') {
                    printf("\n=== 读取0号寄存器 - 检查小车视镜功能状态 ===\n");
                    uint8_t read_data = 0;
                    
                    // 通过FSPI读取0号寄存器的值
                    // 0号寄存器用于存储小车的视镜功能状态
                    // 寄存器值含义：
                    //   0x00 = 前视镜模式
                    //   0x11 = 后视镜模式
                    //   其他值 = 异常状态或未定义
                    if (fspi_read_data(0, &read_data) == 0) {
                        printf("✓ 读取成功: 寄存器0 = 0x%02X (%d)\n", read_data, read_data);
                        
                        // 根据读取的寄存器值判断视镜功能状态
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
                // 处理退出命令
                else if (cmd == 'q' || cmd == 'Q') {
                    printf("用户请求退出程序\n");
                    g_running = 0;
                    break;
                }
                else {
                    printf("无效命令: %c\n", cmd);
                }
            }
            else if (strlen(buffer) > 1) {
                printf("请输入单个字符命令 (0-5, q)\n");
            }
            
next_input:
            // 显示下一次输入提示
            if (g_running) {
                printf("\n请输入命令: ");
                fflush(stdout);
            }
        }
        else {
            // 输入错误或EOF
            if (feof(stdin)) {
                printf("\n检测到EOF，程序退出\n");
                g_running = 0;
            }
        }
    }
    
cleanup:
    // 程序退出前的清理工作
    printf("\n=== 程序退出清理 ===\n");
    
    // 停止所有车辆运动
    stop_all_vehicle_movement();
    
    // 清理FSPI资源
    fspi_cleanup();
    
    printf("资源清理完成，程序退出\n");
    
    return 0;
}