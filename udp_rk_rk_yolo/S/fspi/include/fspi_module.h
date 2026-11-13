#ifndef FSPI_MODULE_H
#define FSPI_MODULE_H

#include <stdint.h>
#include "speed_status.h"
#include "control_status.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* 寄存器访问：打开固定 SPI 设备并设置模式、位宽和频率。成功返回 0；设备打开或配置失败返回 -1。 */
    int fspi_init(void);

    /* 寄存器访问：SMDT会自动添加00前导；用户发送地址高/地址低/数据三字节。
 * 未初始化或最多15次完整写事务均失败时返回-1。 */
    int fspi_write_data(uint16_t addr, uint8_t data);
    /* 方向启动对截止时间敏感，只发送一次完整三字节事务，不带重试休眠。 */
    int fspi_write_direction_once(uint16_t addr, uint8_t data);

    /* 寄存器访问：地址0直接接收HMI状态（SMDT自动添加80前导）。
 * 非零地址沿用旧接口， 不提供真实方向寄存器回读保证。重复读取并选取出现频率最高的值；
 * 计数相同时优先选取数值较小者。地址 0 的读值按视镜状态协议解释：
 * 0x00 为前视镜，0x11 为后视镜，其余值按异常状态记录。 */
    int fspi_read_data(uint16_t addr, uint8_t *data);

    /* 为车道告警立即读取三个一致 HMI 样本，不插入重试休眠。 */
    int fspi_read_hmi_snapshot(uint8_t *data);

    /* 单次原生四字节只读事务返回旧 HMI、A7、档位、CRC8；
 * 无写入或延时，旧 FPGA 或签名非法返回 -1/EPROTO。 */
    int fspi_read_speed_snapshot(SpeedSnapshot *data, uint8_t *raw_status);
    int fspi_read_control_snapshot(ControlSnapshot *data);
    /* 第九字节为独立视角。旧 FPGA 或延长读取失败时，仍提供原控制快照，视角为 FF。 */
    int fspi_read_control_snapshot_with_view(ControlSnapshot *data, uint8_t *view);
    int fspi_read_control_snapshot_with_wheel(ControlSnapshot *data, uint8_t *view,
                                              uint16_t *period);

    /* 寄存器访问：独立持久开关：同一次片选发送四线 81 00 05，再四线读取 A1(开)/A0(关)。
 * 只接受带签名的状态，旧 FPGA、无效响应或总线失败返回 -1，且不改输出值。 */
    int fspi_read_auto_brake_switch(int *enabled);

    /* 读取最近一次独立开关读取的诊断，不进行SPI访问；可空参。 errno=0表示成功，
 * transfer_result为消息返回长度，raw_status为原始响应。。 */
    void fspi_get_auto_brake_diagnostic(int *error_number, int *transfer_result,
                                        uint8_t *raw_status);

    /* 寄存器访问：不预先发送地址，直接读取一字节；同样通过重复采样筛选结果。 */
    int fspi_read_direct(uint8_t *data);

    /* 控制方向寄存器 0..3：依次为前进、后退、左转和右转。
 * enable=1 代表运动但协议写值为 0；enable=0
 * 代表停止且写值为 1。 该操作只确认写事务成功，不读回控制寄存器。 */
    int vehicle_control(int command, int enable);

    /* 寄存器访问：方向便捷入口与 vehicle_control 共用参数校验及状态输出。 */
    int vehicle_forward(int enable);
    int vehicle_backward(int enable);
    int vehicle_turn_left(int enable);
    int vehicle_turn_right(int enable);

    /* 寄存器访问：向地址4写0发布STOP请求；写1只清软件请求，已有FPGA锁存仍须人工REARM解除。 */
    int vehicle_emergency_stop(void);

    /* 寄存器访问：关闭已打开的设备句柄并清空内部句柄状态；可安全重复调用。 */
    void fspi_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* FSPI_MODULE_H：本节对应的标识或作用域。 */
