/*
 * test_capture.c — 采集纯算法单元测试（【人员 A】自测，不依赖硬件）
 *
 * 覆盖：RGB565↔RGB888 往返一致性、前导像素剥离、PPM 存盘。
 */
#include "pcie_capture.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static int g_fail = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { printf("  [FAIL] %s\n", msg); g_fail++; } \
    else { printf("  [ok]   %s\n", msg); } \
} while (0)

static uint16_t make565(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}

int main(void)
{
    printf("== test_capture ==\n");

    /* 1) RGB565 -> RGB888 -> RGB565 往返必须一致（无损还原高位） */
    uint16_t samples[] = {0x0000, 0xFFFF, make565(255,128,64),
                          make565(10,200,30), make565(123,45,200), 0x07E0, 0xF800};
    for (size_t i = 0; i < sizeof(samples)/sizeof(samples[0]); i++) {
        uint8_t out888[3];
        uint16_t back565;
        rgb565_to_rgb888_pcie(&samples[i], out888, 1);
        rgb888_to_rgb565_pcie(out888, &back565, 1);
        char buf[64];
        snprintf(buf, sizeof(buf), "roundtrip px #%zu", i);
        CHECK(back565 == samples[i], buf);
    }

    /* 2) 批量往返 */
    const int N = 1000;
    uint16_t *src = (uint16_t *)malloc(N * 2);
    uint8_t *mid = (uint8_t *)malloc(N * 3);
    uint16_t *dst = (uint16_t *)malloc(N * 2);
    for (int i = 0; i < N; i++) src[i] = (uint16_t)(rand() & 0xFFFF);
    rgb565_to_rgb888_pcie(src, mid, N);
    rgb888_to_rgb565_pcie(mid, dst, N);
    int same = 1;
    for (int i = 0; i < N; i++) if (dst[i] != src[i]) { same = 0; break; }
    CHECK(same, "bulk roundtrip (1000 px)");
    free(src); free(mid); free(dst);

    /* 3) 前导像素剥离：前导填 0xAA，有效区填递增，剥离后只保留有效区 */
    int lead = 120, w = 640, rowbytes = (lead + w) * 2;
    uint8_t *raw = (uint8_t *)malloc(rowbytes);
    uint8_t *clean = (uint8_t *)malloc(w * 2);
    for (int i = 0; i < rowbytes; i++) raw[i] = (i < lead*2) ? 0xAA : (uint8_t)(i & 0xFF);
    strip_leading_pixels(raw, lead, w, clean);
    int ok = 1;
    for (int i = 0; i < w * 2; i++) {
        int expect = (lead*2 + i) & 0xFF;
        if (clean[i] != (uint8_t)expect) { ok = 0; break; }
    }
    CHECK(ok, "strip_leading_pixels keeps only clean region");
    free(raw); free(clean);

    /* 4) PPM 存盘可写（写完即删） */
    uint8_t *img = (uint8_t *)malloc(4*4*3);
    for (int i = 0; i < 4*4*3; i++) img[i] = (uint8_t)i;
    CHECK(save_rgb888_to_ppm(img, 4, 4, "test_capture_tmp.ppm") == 0, "save_rgb888_to_ppm writes file");
    free(img);
    remove("test_capture_tmp.ppm");

    printf(g_fail == 0 ? "test_capture: PASS\n" : "test_capture: FAIL (%d)\n", g_fail);
    return g_fail == 0 ? 0 : 1;
}
