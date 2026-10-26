#ifndef FSPI_MODULE_H
#define FSPI_MODULE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 打开固定 SPI 设备并设置模式、位宽和频率。成功返回 0；设备打开或配置失败返回 -1。 */
int fspi_init(void);

/* SMDT会自动添加00前导；用户发送地址高/地址低/数据三字节。
 * 未初始化或最多15次完整写事务均失败时返回-1。 */
int fspi_write_data(uint16_t addr, uint8_t data);

/* 地址0直接接收HMI状态（SMDT自动添加80前导）。非零地址沿用旧接口，
 * 不提供真实方向寄存器回读保证。重复读取并选取出现频率最高的值；
 * 计数相同时优先选取数值较小者。地址 0 的读值按视镜状态协议解释：
 * 0x00 为前视镜，0x11 为后视镜，其余值按异常状态记录。 */
int fspi_read_data(uint16_t addr, uint8_t *data);

/* 独立持久开关：同一次片选发送四线 81 00 05，再四线读取 A1(开)/A0(关)。
 * 只接受带签名的状态，旧 FPGA、无效响应或总线失败返回 -1，且不改输出值。 */
int fspi_read_auto_brake_switch(int *enabled);

/* 获取最近一次独立开关读取的诊断，不进行SPI访问；可空参。
 * errno=0表示成功，transfer_result为消息返回长度，raw_status为原始响应。 */
void fspi_get_auto_brake_diagnostic(int *error_number, int *transfer_result, uint8_t *raw_status);

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

/* 向地址4写0发布STOP请求；写1只清软件请求，已有FPGA锁存仍须人工REARM解除。 */
int vehicle_emergency_stop(void);

/* 关闭已打开的设备句柄并清空内部句柄状态；可安全重复调用。 */
void fspi_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* FSPI_MODULE_H */
