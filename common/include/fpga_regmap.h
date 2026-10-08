/*
 * fpga_regmap.h — FSPI 寄存器映射契约（【人员 C · 控制与 FPGA】起草，B 集成进 common/）
 *
 * 本文件是「RK3568(S 板) FSPI 主机驱动(control/)」与「FPGA FSPI 从机(fpga/)」之间
 * 的寄存器/协议契约唯一真源。C 端驱动与 FPGA RTL 必须与此表逐位一致；
 * 任何地址、编码、时序改动都须三人 review 后同步两端。
 *
 * 与 driving_config.h 的分工：
 *   - driving_config.h : 图像/共享内存 key/UDP/决策阈值（B 维护）
 *   - fpga_regmap.h    : FSPI 物理层 + 寄存器地址 + 值编码（C 维护）
 * 二者都只放「常量与语义说明」，不放任何函数实现。
 *
 * 【协议要点（相对旧版 fspi_module.c 的梳理）】
 *  1. 物理层：QUAD SPI（4 数据线，每时钟传 4 bit），SPI Mode 3（CPOL=1,CPHA=1），
 *     100 MHz，MSB 优先。两拍拼一个字节（第一拍高 4 位，第二拍低 4 位）。
 *  2. 命令字节：每次传输的**第一个字节**兼作命令与 16 位地址的高字节：
 *       0x00 → 写（FPGA 解码为 cmd=2'b01）
 *       0x80 → 读（FPGA 解码为 cmd=2'b10）
 *     由于当前主机地址高字节恒为 0，实际只发 0x00（写）；0x80（读）路径
 *     为历史保留，主机驱动未启用（见 FSPI_CMD_READ 注释）。
 *  3. 写帧： [0x00(命令/地址高)] [addr_lo(寄存器号)] [data]，共 3 字节。
 *  4. 寄存器：FPGA 内部 mem[0..4] 共 5 个 8-bit 寄存器，地址只取低字节低 3 位。
 *  5. 值编码：**低有效（active-low）**——写 0 表示"运动/使能"，写 1 表示"停止"。
 *  6. 读回：FPGA 读路径返回 rd_data = {4'd0, select_rearview}（select_rearview 即
 *     串口屏 HMI 的 cnn_level 低 4 位），地址越界(≥6)返回 0xFF。该读回为历史
 *     保留语义，主机控制循环并不依赖。
 */
#ifndef FPGA_REGMAP_H
#define FPGA_REGMAP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================
 * 一、FSPI 物理层参数
 * ==================================================================== */
#define FSPI_DEVICE         "/dev/spidev4.0"   /* SPI 设备节点 */
#define FSPI_SPEED_HZ       100000000          /* 100 MHz */
#define FSPI_SPI_MODE       3                  /* CPOL=1, CPHA=1（-m 3 参数） */
#define FSPI_BITS_PER_WORD  8
#define FSPI_DELAY_US       0
#define FSPI_TRANSFER_SIZE  3                  /* 写帧字节数 [cmd][addr_lo][data] */

/* 读写可靠性重试次数（对应旧版 WRITE_RETRY_COUNT / READ_RETRY_COUNT） */
#define FSPI_WRITE_RETRY    15                 /* 写失败重试次数，间隔 1ms */
#define FSPI_READ_RETRY     10                 /* 读采样次数（取众数），间隔 0.5ms */

/* ======================================================================
 * 二、命令字节（每帧首字节）
 * ==================================================================== */
#define FSPI_CMD_WRITE      0x00u              /* 写命令（FPGA cmd=2'b01） */
#define FSPI_CMD_READ       0x80u              /* 读命令（FPGA cmd=2'b10，历史保留） */

/* ======================================================================
 * 三、寄存器地址（FPGA mem[0..4]，低字节低 3 位有效）
 *
 * 与 FPGA 顶层 image_pcie_capture.v 的动作输出一一对应：
 *   mem0_out[0] → go    （前进）
 *   mem1_out[0] → back  （后退）
 *   mem2_out[0] → left  （左转）
 *   mem3_out[0] → right （右转）
 *   mem4_out[0] → stop  （急停，下降沿触发 1s 脉冲）
 * ==================================================================== */
#define FSPI_REG_FORWARD    0u                 /* 前进控制 */
#define FSPI_REG_BACKWARD   1u                 /* 后退控制 */
#define FSPI_REG_LEFT       2u                 /* 左转控制 */
#define FSPI_REG_RIGHT      3u                 /* 右转控制 */
#define FSPI_REG_STOP       4u                 /* 急停（写 0 触发 / 写 1 复位） */

/* 寄存器数量（FPGA mem 数组深度，地址 < FSPI_REG_COUNT 才有效） */
#define FSPI_REG_COUNT      5u

/* ======================================================================
 * 四、值编码（低有效）
 *
 * C 端入参语义为「enable=1 运动 / enable=0 停止」，落到总线上反相：
 *   运动(enable=1) → 写 0
 *   停止(enable=0) → 写 1
 *
 * 急停寄存器(FSPI_REG_STOP)特殊：它是**边沿触发**，与低有效编码一致——
 *   写 0（mem4_out[0] 1→0）→ 触发 FPGA 1s 停车脉冲（stop_pulse）
 *   写 1（mem4_out[0] 0→1）→ 复位到正常
 * 这与旧版主循环「人员检测：有人写 0 / 无人写 1」一致；旧版 fspi_module.c
 * 里孤立的 vehicle_emergency_stop()（写 1，注释"立即停止"）有误且未被主循环
 * 调用，属死代码，本契约以 FPGA 实际下降沿触发语义为准。
 * ==================================================================== */
#define FSPI_VAL_ACTIVE     0u                 /* 运动/使能（总线值 0） */
#define FSPI_VAL_IDLE       1u                 /* 停止/关闭（总线值 1） */

/* 把 C 端 enable 语义转换为总线值（低有效） */
static inline uint8_t fspi_encode_enable(uint8_t enable)
{
    return enable ? (uint8_t)FSPI_VAL_ACTIVE : (uint8_t)FSPI_VAL_IDLE;
}

/* ======================================================================
 * 五、读回语义（历史保留，主机控制循环不依赖）
 *
 * FSPI_REG_FORWARD(地址0) 读回：
 *   0x00 = 前视镜模式；0x11 = 后视镜模式（旧版注释，实际读回 select_rearview）。
 * 实际读回值 = {4'd0, select_rearview}，select_rearview 由串口屏 HMI 的
 * cnn_level（uart_lcd.v 的 st_cnn 状态捕获 uart_rx[3:0]）驱动。
 * ==================================================================== */
#define FSPI_READ_MIRROR_FRONT  0x00u         /* 前视镜（历史注释值） */
#define FSPI_READ_MIRROR_REAR   0x11u         /* 后视镜（历史注释值） */
#define FSPI_READ_OUT_OF_RANGE  0xFFu         /* 地址越界(≥6)读回 0xFF */

/* ======================================================================
 * 六、ControlCommand → 寄存器映射（决策层命令 → 执行层寄存器）
 *
 * 映射关系（B 的 ControlCommand 定义于 driving_types.h）：
 *   CMD_GO    → FSPI_REG_FORWARD   （前进）
 *   CMD_BACK  → FSPI_REG_BACKWARD  （后退）
 *   CMD_LEFT  → FSPI_REG_LEFT      （左转）
 *   CMD_RIGHT → FSPI_REG_RIGHT     （右转）
 *   CMD_BRAKE → FSPI_REG_STOP      （制动）
 *   CMD_STOP  → FSPI_REG_STOP      （急停，最高优先）
 *   CMD_NONE  → 无动作（转向回中 + 停所有）
 * ==================================================================== */

/* ======================================================================
 * 七、串口屏（陶晶驰 HMI）UART 显示选择字节（与 uart_lcd.v / isp_params.c 对应）
 *
 * 串口屏经 FPGA UART 下发 0x30 0x90 + sel + data 调参帧；sel 字节语义：
 *   0x01 mode / 0x02 blc / 0x03 awb / 0x04 cnn_level / 0x05 saturation
 *   0x06 brightness / 0x07 awb_en / 0x08 binarization / 0x09 sobel
 *   0x0A isp_judge / 0x0B~0x0E cb/cr min/max
 * 详细参数表见 perception/csrc/isp_params.c（A 维护）。
 * ==================================================================== */
#define UART_LCD_PREAMBLE0  0x30u              /* 串口屏帧前导第 1 字节 */
#define UART_LCD_PREAMBLE1  0x90u              /* 串口屏帧前导第 2 字节 */

#ifdef __cplusplus
}
#endif

#endif /* FPGA_REGMAP_H */
