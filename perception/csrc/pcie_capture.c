/*
 * pcie_capture.c — 图像采集实现（【人员 A · 感知】）
 *
 * 真实通道仅在 Linux（板端 RK3568）编译；Windows 下硬件 ioctl 不可用，
 * 此时仅保留纯算法（前导剥离 / 色彩转换 / PPM 存盘），便于本机单元测试。
 */
#include "pcie_capture.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(__linux__)
  /* 非 Linux（Windows / 本机）无 PCIe 驱动，硬件函数置为不可用桩 */
  #define PCIE_HW_UNAVAILABLE 1
#else
  #include <fcntl.h>
  #include <unistd.h>
  #include <sys/ioctl.h>
  #include <sys/time.h>
  #include <time.h>
#endif

/* ----------------------------------------------------------------------
 * 纯算法（不依赖硬件，可独立测试）
 * -------------------------------------------------------------------- */

void strip_leading_pixels(const uint8_t *raw_row, int lead_px, int clean_w,
                          uint8_t *clean_row)
{
    if (!raw_row || !clean_row || clean_w <= 0) return;
    /* 每行像素为 RGB565（2 字节），跳过前导部分 */
    memcpy(clean_row, raw_row + (size_t)lead_px * 2u, (size_t)clean_w * 2u);
}

void rgb565_to_rgb888_pcie(const uint16_t *src565, uint8_t *dst888, size_t n)
{
    if (!src565 || !dst888) return;
    for (size_t i = 0; i < n; i++) {
        uint16_t p = src565[i];
        uint8_t r5 = (uint8_t)((p >> 11) & 0x1F);
        uint8_t g6 = (uint8_t)((p >> 5)  & 0x3F);
        uint8_t b5 = (uint8_t)( p        & 0x1F);
        dst888[i * 3]     = (uint8_t)((r5 << 3) | (r5 >> 2));
        dst888[i * 3 + 1] = (uint8_t)((g6 << 2) | (g6 >> 4));
        dst888[i * 3 + 2] = (uint8_t)((b5 << 3) | (b5 >> 2));
    }
}

void rgb888_to_rgb565_pcie(const uint8_t *src888, uint16_t *dst565, size_t n)
{
    if (!src888 || !dst565) return;
    for (size_t i = 0; i < n; i++) {
        uint8_t r = src888[i * 3];
        uint8_t g = src888[i * 3 + 1];
        uint8_t b = src888[i * 3 + 2];
        uint16_t r5 = (uint16_t)((r >> 3) & 0x1F);
        uint16_t g6 = (uint16_t)((g >> 2) & 0x3F);
        uint16_t b5 = (uint16_t)((b >> 3) & 0x1F);
        dst565[i] = (uint16_t)((r5 << 11) | (g6 << 5) | b5);
    }
}

int save_rgb888_to_ppm(const uint8_t *rgb888, int w, int h, const char *path)
{
    if (!rgb888 || !path || w <= 0 || h <= 0) return -1;
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;
    fprintf(fp, "P6\n%d %d\n255\n", w, h);
    fwrite(rgb888, 1, (size_t)w * (size_t)h * 3u, fp);
    fclose(fp);
    return 0;
}

#ifndef PCIE_HW_UNAVAILABLE
double timeval_diff_ms(const void *ta, const void *tb)
{
    const struct timeval *a = (const struct timeval *)ta;
    const struct timeval *b = (const struct timeval *)tb;
    double s = (double)(a->tv_sec - b->tv_sec);
    double us = (double)(a->tv_usec - b->tv_usec);
    return s * 1000.0 + us / 1000.0;
}
#endif

/* ----------------------------------------------------------------------
 * 硬件通道（仅 Linux）
 * -------------------------------------------------------------------- */

#ifdef PCIE_HW_UNAVAILABLE

int pcie_capture_open(frame_grabber_t *g, int img_w, int img_h, int lead_px)
{
    (void)g; (void)img_w; (void)img_h; (void)lead_px;
    return -1; /* 本机无硬件 */
}
int pcie_capture_start(frame_grabber_t *g) { (void)g; return -1; }
int pcie_capture_grab(frame_grabber_t *g, uint8_t *out_rgb565)
{
    (void)g; (void)out_rgb565; return -1;
}
void pcie_capture_stop(frame_grabber_t *g) { (void)g; }
void pcie_capture_close(frame_grabber_t *g) { (void)g; }

#else

int pcie_capture_open(frame_grabber_t *g, int img_w, int img_h, int lead_px)
{
    if (!g) return -1;
    memset(g, 0, sizeof(*g));
    g->img_w = img_w;
    g->img_h = img_h;
    g->lead_px = lead_px;
    g->fd = open(PANGO_PCIE_DEV, O_RDWR);
    if (g->fd < 0) {
        perror("open " PANGO_PCIE_DEV);
        return -1;
    }
    return 0;
}

int pcie_capture_start(frame_grabber_t *g)
{
    if (!g || g->fd < 0) return -1;

    /* 每行 DMA 传输长度（单位 dword）：(前导+有效)*2 字节 / 4 */
    g->dma.current_len = (uint32_t)((g->lead_px + g->img_w) * 2 / 4);
    g->dma.offset_addr = 0;
    memset(g->dma.write_buf, 0, PANGO_DMA_PACKET);

    if (ioctl(g->fd, PANGO_IO_MAP, &g->dma) != 0) {
        perror("ioctl PANGO_IO_MAP");
        return -1;
    }
    g->dma.offset_addr = 0;
    if (ioctl(g->fd, PANGO_IO_DMA_WR, &g->dma) != 0) {
        perror("ioctl PANGO_IO_DMA_WR");
        return -1;
    }
    return 0;
}

int pcie_capture_grab(frame_grabber_t *g, uint8_t *out_rgb565)
{
    if (!g || g->fd < 0 || !out_rgb565) return -1;
    if (g->dma.current_len == 0) return -1;

    const size_t clean_row_bytes = (size_t)g->img_w * 2u;
    const size_t skip_bytes = (size_t)g->lead_px * 2u;

    for (int y = 0; y < g->img_h; y++) {
        /* 逐行设置读偏移并触发 DMA 读 */
        g->dma.offset_addr = (uint32_t)(y * (g->lead_px + g->img_w) * 2);
        if (ioctl(g->fd, PANGO_IO_DMA_WR, &g->dma) != 0) return -1;
        if (ioctl(g->fd, PANGO_IO_RD_KRNL, &g->dma) != 0) return -1;

        /* 拷贝有效像素（跳过前导） */
        memcpy(out_rgb565 + (size_t)y * clean_row_bytes,
               g->dma.read_buf + skip_bytes, clean_row_bytes);
    }
    return 0;
}

void pcie_capture_stop(frame_grabber_t *g)
{
    if (g && g->fd >= 0) {
        ioctl(g->fd, PANGO_IO_UNMAP, &g->dma);
    }
}

void pcie_capture_close(frame_grabber_t *g)
{
    if (g && g->fd >= 0) {
        close(g->fd);
        g->fd = -1;
    }
}

#endif /* PCIE_HW_UNAVAILABLE */
