/* FSPI protocol A6 for drive_6ch. See common/include/fpga_regmap.h. */
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

/* 初始化：打开 /dev/spidev4.0 并配置 QUAD/Mode3/1MHz；支持 FSPI_DEVICE / FSPI_SPEED_HZ 环境变量。成功 0，失败 -1 */
int  fspi_driver_init(fspi_driver_t **ctx);

/* 释放资源 */
void fspi_driver_deinit(fspi_driver_t *ctx);

/* 写寄存器：完整 8 位地址；失败上报，由控制器立即停车 */
int  fspi_driver_write_reg(fspi_driver_t *ctx, uint8_t addr, uint8_t data);

/* 同一片选内发送 80 addr dummy，然后接收一字节 */
int  fspi_driver_read_reg(fspi_driver_t *ctx, uint8_t addr, uint8_t *data);

/* 车辆动作：enable=1 运动 / 0 停止（内部做低有效编码：运动=0，停止=1） */
int  fspi_driver_move(fspi_driver_t *ctx, fspi_move_t move, uint8_t enable);

/* 触发急停：写 FSPI_REG_STOP = 0（FPGA 持续保持低有效停车） */
int  fspi_driver_emergency_stop(fspi_driver_t *ctx);

/* 复位急停：写 FSPI_REG_STOP = 1（仅健康且心跳有效时允许恢复） */
int  fspi_driver_emergency_release(fspi_driver_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* FSPI_DRIVER_H */
