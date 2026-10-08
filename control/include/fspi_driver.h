/*
 * fspi_driver.h — FSPI 主机驱动接口（【人员 C · 控制与 FPGA】）
 *
 * 运行于 S 板 RK3568，经 /dev/spidev4.0 与 FPGA FSPI 从机通信，向上层提供
 * 「寄存器读写 + 车辆动作」能力。协议与寄存器映射见 common/include/fpga_regmap.h。
 *
 * 本模块是「设备驱动」：实现依赖 Linux SPI（sys/ioctl + linux/spi/spidev.h），
 * 板端编译；本机（Windows/非 Linux）走桩分支（#if defined(__linux__)），
 * 只做语法/链接自检，所有接口返回 -1。
 *
 * 与旧版 fspi_module.c 的对应（行为一致）：
 *   fspi_init/fspi_write_data/fspi_read_data/vehicle_control/vehicle_emergency_stop
 *   → fspi_driver_init/write_reg/read_reg/move/emergency_stop。
 * 删除了旧版 getopt/SpiTestConfig/TestResult 等测试工具死代码，以及未被
 * 主循环调用的 fspi_read_direct（直接读，读路径本为历史保留）。
 */
#ifndef FSPI_DRIVER_H
#define FSPI_DRIVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 不透明句柄 */
typedef struct fspi_driver fspi_driver_t;

/* 车辆动作方向（与 fpga_regmap.h 的 FSPI_REG_* 一一对应） */
typedef enum {
    FSPI_MOVE_FORWARD  = 0,   /* 前进，寄存器 FSPI_REG_FORWARD */
    FSPI_MOVE_BACKWARD = 1,   /* 后退，寄存器 FSPI_REG_BACKWARD */
    FSPI_MOVE_LEFT     = 2,   /* 左转，寄存器 FSPI_REG_LEFT */
    FSPI_MOVE_RIGHT    = 3    /* 右转，寄存器 FSPI_REG_RIGHT */
} fspi_move_t;

/* 初始化：打开 /dev/spidev4.0 并配置 QUAD/Mode3/100MHz。成功 0，失败 -1 */
int  fspi_driver_init(fspi_driver_t **ctx);

/* 释放资源 */
void fspi_driver_deinit(fspi_driver_t *ctx);

/* 可靠写寄存器：addr 取低字节低 3 位（0..4），内部重试 FSPI_WRITE_RETRY 次 */
int  fspi_driver_write_reg(fspi_driver_t *ctx, uint8_t addr, uint8_t data);

/* 众数读寄存器：采样 FSPI_READ_RETRY 次取出现最多者（读路径历史保留） */
int  fspi_driver_read_reg(fspi_driver_t *ctx, uint8_t addr, uint8_t *data);

/* 车辆动作：enable=1 运动 / 0 停止（内部做低有效编码：运动=0，停止=1） */
int  fspi_driver_move(fspi_driver_t *ctx, fspi_move_t move, uint8_t enable);

/* 触发急停：写 FSPI_REG_STOP = 0（mem4_out[0] 下降沿触发 FPGA 1s 停车脉冲） */
int  fspi_driver_emergency_stop(fspi_driver_t *ctx);

/* 复位急停：写 FSPI_REG_STOP = 1（回到正常，供下次触发重新武装） */
int  fspi_driver_emergency_release(fspi_driver_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* FSPI_DRIVER_H */
