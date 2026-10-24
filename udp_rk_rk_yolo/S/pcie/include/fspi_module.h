#ifndef FSPI_MODULE_H
#define FSPI_MODULE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 打开固定 SPI 设备并设置总线模式、字宽和频率；成功返回 0，失败返回 -1。 */
int fspi_init(void);

/* 将 16 位地址与一个数据字节写入设备；最多尝试 15 次。 */
int fspi_write_data(uint16_t addr, uint8_t data);

/* 先发送地址，再重复读取十次，返回成功样本中出现次数最多的字节。
 * 计数相同时选择数值较小者；返回值 0 为成功，-1 为失败。 */
int fspi_read_data(uint16_t addr, uint8_t *data);

/* 控制方向寄存器 0..3，分别对应前进、后退、左转和右转。
 * enable=1 时写 0 表示运行，enable=0 时写 1 表示停止。
 * 每次写入后读回核对，最多执行十轮验证。 */
int vehicle_control(int command, int enable);

/* 四个便捷入口只固定方向编号，仍由 vehicle_control 执行参数校验和读回。 */
int vehicle_forward(int enable);
int vehicle_backward(int enable);
int vehicle_turn_left(int enable);
int vehicle_turn_right(int enable);

/* 关闭已打开的 SPI 句柄并重置内部状态；可重复调用。 */
void fspi_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* FSPI_MODULE_H */
