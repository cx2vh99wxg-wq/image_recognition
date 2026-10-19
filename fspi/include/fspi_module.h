#ifndef FSPI_MODULE_H
#define FSPI_MODULE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 打开固定 SPI 设备并设置模式、位宽和频率。成功返回 0；设备打开或配置失败返回 -1。 */
int fspi_init(void);

/* 向 16 位地址写入一个字节。未初始化或最多 15 次写事务均失败时返回 -1。 */
int fspi_write_data(uint16_t addr, uint8_t data);

/* 先发送 16 位寄存器地址，再读取一字节。重复读取并选取出现频率最高的值；
 * 计数相同时优先选取数值较小者。地址 0 的读值用于视镜状态说明：
 * 0x00 为前视镜，0x11 为后视镜，其余值按异常状态记录。 */
int fspi_read_data(uint16_t addr, uint8_t *data);

/* 不预先发送地址，直接读取一字节；同样通过重复采样筛选结果。 */
int fspi_read_direct(uint8_t *data);

/* 控制方向寄存器 0..3：依次为前进、后退、左转和右转。
 * enable=1 代表运动但协议写值为 0；enable=0 代表停止且写值为 1。
 * 该操作只确认写事务成功，不读回控制寄存器。 */
int vehicle_control(int command, int enable);

/* 方向便捷入口与 vehicle_control 共用参数校验及状态输出。 */
int vehicle_forward(int enable);
int vehicle_backward(int enable);
int vehicle_turn_left(int enable);
int vehicle_turn_right(int enable);

/* 向地址 4 写入 1 触发主动紧急停止，不做寄存器读回验证。 */
int vehicle_emergency_stop(void);

/* 关闭已打开的设备句柄并清空内部句柄状态；可安全重复调用。 */
void fspi_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* FSPI_MODULE_H */
