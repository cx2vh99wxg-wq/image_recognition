/*
 * pcie_capture.h — 图像采集模块（【人员 A · 感知】）
 *
 * 职责：通过 PCIe DMA 从 FPGA 端取一帧原始图像，逐行剥离每行前导像素，
 * 对外只暴露干净的 640×480 RGB565。颜色空间转换（RGB565↔RGB888）、
 * 前导剥离、行重排等纯算法也都在此提供，供单元测试覆盖。
 */
#ifndef PCIE_CAPTURE_H
#define PCIE_CAPTURE_H

#include <stdint.h>
#include <stddef.h>

#include "pango_pcie_abi.h"

/* 图像固定尺寸（与 FPGA 输出一致） */
#define CAP_IMG_W      640
#define CAP_IMG_H      480
#define CAP_LEAD_PIX   120   /* 每行前端冗余像素，必须剥离 */

#ifdef __cplusplus
extern "C" {
#endif

/* 采集器上下文 */
typedef struct {
    int             fd;        /* /dev/pango_pci_driver 句柄 */
    int             img_w;
    int             img_h;
    int             lead_px;
    pango_dma_xfer  dma;       /* DMA 描述符（驱动 ABI） */
} frame_grabber_t;

/* 打开设备并准备 DMA 映射；成功返回 0 */
int  pcie_capture_open(frame_grabber_t *g, int img_w, int img_h, int lead_px);
/* 启动连续采集（映射 DMA 地址、复位读指针） */
int  pcie_capture_start(frame_grabber_t *g);
/* 抓取一帧：输出干净 RGB565，长度 img_w*img_h*2；成功返回 0 */
int  pcie_capture_grab(frame_grabber_t *g, uint8_t *out_rgb565);
/* 停止采集（取消映射） */
void pcie_capture_stop(frame_grabber_t *g);
/* 关闭设备 */
void pcie_capture_close(frame_grabber_t *g);

/* ---- 纯算法（可独立单元测试，不依赖硬件） ---- */

/* 将一行“带前导”的 RGB565 数据剥离前导像素，写回干净行 */
void strip_leading_pixels(const uint8_t *raw_row, int lead_px, int clean_w,
                          uint8_t *clean_row);

/* RGB565 → RGB888（每像素 3 字节，R/G/B 顺序） */
void rgb565_to_rgb888_pcie(const uint16_t *src565, uint8_t *dst888, size_t n);
/* RGB888 → RGB565（用于回写共享内存 / 显示） */
void rgb888_to_rgb565_pcie(const uint8_t *src888, uint16_t *dst565, size_t n);

/* 毫秒级时间差（仅 Linux 实现；Windows 本机无此函数） */
#if !defined(_WIN32) && !defined(_WIN64)
double timeval_diff_ms(const void *ta, const void *tb);
#endif

/* 将 RGB888 存为 PPM（P6）文件，调试用 */
int  save_rgb888_to_ppm(const uint8_t *rgb888, int w, int h, const char *path);

#ifdef __cplusplus
}
#endif

#endif /* PCIE_CAPTURE_H */
