#ifndef FSPI_MODULE_H
#define FSPI_MODULE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief FSPI模块初始化
 * @return 0表示成功，-1表示失败
 */
int fspi_init(void);

/**
 * @brief FSPI写数据
 * @param addr 写入地址 (16位)
 * @param data 写入数据 (8位)
 * @return 0表示成功，-1表示失败
 */
int fspi_write_data(uint16_t addr, uint8_t data);

/**
 * @brief FSPI读数据 - 用于读取指定地址的寄存器值
 * @param addr 读取地址 (16位)
 *             地址0: 小车视镜功能状态寄存器
 *                   0x00 = 前视镜模式
 *                   0x11 = 后视镜模式
 *                   其他值 = 异常状态
 *             地址1-4: 其他功能寄存器
 * @param data 读取数据指针 (8位) - 用于存储读取到的寄存器值
 * @return 0表示成功，-1表示失败
 * @note 此函数会进行多次读取重试，返回出现频率最高的值以提高可靠性
 */
int fspi_read_data(uint16_t addr, uint8_t *data);

/**
 * @brief FSPI直接读数据 - 不需要先写地址
 * @param data 读取数据指针 (8位)
 * @return 0表示成功，-1表示失败
 */
int fspi_read_direct(uint8_t *data);

/**
 * @brief 车辆控制函数（无读回验证）
 * @param command 控制命令 (0=前进, 1=后退, 2=左转, 3=右转)
 * @param enable 运动/停止 (1=运动, 0=停止) -> 实际发送值：运动=0, 停止=1
 * @return 0表示成功，-1表示失败
 */
int vehicle_control(int command, int enable);

/**
 * @brief 车辆前进控制
 * @param enable 运动/停止 (1=运动, 0=停止) -> 实际发送值：运动=0, 停止=1
 * @return 0表示成功，-1表示失败
 */
int vehicle_forward(int enable);

/**
 * @brief 车辆后退控制
 * @param enable 运动/停止 (1=运动, 0=停止) -> 实际发送值：运动=0, 停止=1
 * @return 0表示成功，-1表示失败
 */
int vehicle_backward(int enable);

/**
 * @brief 车辆左转控制
 * @param enable 运动/停止 (1=运动, 0=停止) -> 实际发送值：运动=0, 停止=1
 * @return 0表示成功，-1表示失败
 */
int vehicle_turn_left(int enable);

/**
 * @brief 车辆右转控制
 * @param enable 运动/停止 (1=运动, 0=停止) -> 实际发送值：运动=0, 停止=1
 * @return 0表示成功，-1表示失败
 */
int vehicle_turn_right(int enable);

/**
 * @brief 立即主动停止 - 地址4
 * @return 0表示成功，-1表示失败
 */
int vehicle_emergency_stop(void);

/**
 * @brief FSPI清理资源
 */
void fspi_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif // FSPI_MODULE_H