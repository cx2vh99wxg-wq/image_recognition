/*
 * stub_pattern.c — 合成"每板 3 路摄像头拼接图"实现（【人员 B】）
 *
 * 本文件生成的是**每块板卡内 FPGA 的输出**（640×480），其内部布局由硬件决定，
 * 与 FPGA 侧 axi4_ctrl_3ch.v 的 2×2 写地址映射一致：
 *   ch0 → 左上   ch1 → 右上
 *   ch2 → 左下   ch3 → 右下（"预留空槽"，本设计未接摄像头，填平色）
 * 输出尺寸 640×480、每个子图 320×240 —— 与论文式(10)、以及 M/S 两端
 * pcie_capture 逐行读取的 PCIe 帧格式完全一致，因此这条模拟通路除了
 * "图像内容"之外，其余环节（UDP 分包/重组、共享内存、X11 渲染）都是真跑。
 *
 * 上屏布局由 render_lcd 决定（3×2 六宫格）：只取每块板的 3 个有效子块，
 * 排成 上排 = M 板 3 路、下排 = S 板 3 路。两板用相反方向的渐变，
 * 于是 6 格互不相同；每格左上角的小方块数量 = 该格在本板内的序号（1/2/3）。
 */
#include "stub_pattern.h"

#include <stddef.h>   /* size_t（put565 里做指针偏移用；不能只靠 stdint.h 传递引入） */

/* 灰度 v(0~255) → RGB565，按小端写入（低字节在前）。
 * 与 render_lcd_x11.c 的 rgb565_line_to_xrgb() 读取顺序严格一致，
 * 否则会出现"图像能出来但颜色/亮度错位"。 */
static void put565(uint8_t *buf, int w, int x, int y, uint8_t v)
{
    const uint16_t r5 = (uint16_t)(v >> 3);
    const uint16_t g6 = (uint16_t)(v >> 2);
    const uint16_t b5 = (uint16_t)(v >> 3);
    const uint16_t px = (uint16_t)((r5 << 11) | (g6 << 5) | b5);
    uint8_t *p = buf + ((size_t)y * (size_t)w + (size_t)x) * 2u;
    p[0] = (uint8_t)(px & 0xFFu);   /* 低字节 */
    p[1] = (uint8_t)(px >> 8);      /* 高字节 */
}

static void fill_rect565(uint8_t *buf, int w, int h,
                         int x0, int y0, int rw, int rh, uint8_t v)
{
    for (int y = y0; y < y0 + rh; y++) {
        if (y < 0 || y >= h) continue;
        for (int x = x0; x < x0 + rw; x++) {
            if (x < 0 || x >= w) continue;
            put565(buf, w, x, y, v);
        }
    }
}

void stub_pattern_fill(uint8_t *rgb565, int width, int height, int board_id)
{
    if (!rgb565 || width < 2 || height < 2) return;

    const int qw = width / 2;          /* 子图宽 320 */
    const int qh = height / 2;         /* 子图高 240 */
    const int maxx = (qw > 1) ? (qw - 1) : 1;
    const int maxy = (qh > 1) ? (qh - 1) : 1;
    const int maxd = maxx + maxy;

    for (int qy = 0; qy < 2; qy++) {
        for (int qx = 0; qx < 2; qx++) {
            const int idx = qy * 2 + qx;
            for (int y = 0; y < qh; y++) {
                for (int x = 0; x < qw; x++) {
                    uint8_t v;
                    if (idx == 3) {
                        /* 第 4 格 = FPGA 侧的"预留空槽"（axi4_ctrl_3ch 只接了 3 路）。
                         * 显示端按 3×2 布局只取 3 个有效子块，本格不会上屏，
                         * 故填平色深灰即可（不再画棋盘格做标记）。 */
                        v = 48;
                    } else if (idx == 0) {
                        v = (uint8_t)((x * 255) / maxx);            /* 水平渐变 */
                    } else if (idx == 1) {
                        v = (uint8_t)((y * 255) / maxy);            /* 垂直渐变 */
                    } else {
                        v = (uint8_t)(((x + y) * 255) / maxd);      /* 对角渐变 */
                    }
                    if (board_id && idx != 3) v = (uint8_t)(255 - v);   /* S 板取反，两板不重样 */
                    put565(rgb565, width, qx * qw + x, qy * qh + y, v);
                }
            }
        }
    }

    /* 通道序号标记：第 1 格 1 个方块、第 2 格 2 个、第 3 格 3 个。
     * 黑框 + 白芯，保证在任何渐变底色上都清晰可辨。 */
    for (int idx = 0; idx < 3; idx++) {
        const int qx = (idx & 1) * qw;
        const int qy = (idx >> 1) * qh;
        for (int k = 0; k <= idx; k++) {
            const int mx = qx + 16 + k * 30;
            const int my = qy + 16;
            fill_rect565(rgb565, width, height, mx - 2, my - 2, 22, 22, 0);
            fill_rect565(rgb565, width, height, mx,     my,     18, 18, 255);
        }
    }
}
