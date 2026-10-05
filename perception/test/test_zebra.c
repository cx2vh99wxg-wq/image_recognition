/*
 * test_zebra.c — 斑马线识别单元测试（【人员 A】自测，不依赖硬件/RKNN）
 *
 * 覆盖：横向明暗条纹（斑马线）能检出；纯色路面不误报；纵向条纹（车道线）不误报。
 */
#include "zebra_detect.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_fail = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { printf("  [FAIL] %s\n", msg); g_fail++; } \
    else { printf("  [ok]   %s\n", msg); } \
} while (0)

#define TW 640
#define TH 480

static void fill(uint8_t *img, int v)
{
    for (size_t i = 0; i < (size_t)TW * TH * 3u; i++) img[i] = (uint8_t)v;
}
static void hline(uint8_t *img, int y, int v)
{
    for (int x = 0; x < TW; x++) {
        uint8_t *p = img + ((size_t)y * TW + x) * 3;
        p[0] = p[1] = p[2] = (uint8_t)v;
    }
}

int main(void)
{
    printf("== test_zebra ==\n");
    uint8_t *img = (uint8_t *)malloc((size_t)TW * TH * 3u);
    if (!img) { printf("  [FAIL] alloc\n"); return 1; }

    /* 1) 斑马线：暗路面 + 5 条横向白条纹 → 检出 */
    fill(img, 60);
    int ys[] = {230, 250, 270, 290, 310};
    for (int k = 0; k < 5; k++)
        for (int dy = 0; dy < 8; dy++) hline(img, ys[k] + dy, 220);
    ZebraResult z1; memset(&z1, 0, sizeof(z1));
    CHECK(zebra_detect(img, TW, TH, &z1) == 0, "detect zebra returns 0");
    CHECK(z1.version == ZEBRA_VERSION, "version set");
    CHECK(z1.detected == 1, "zebra detected");
    CHECK(z1.stripe_count == 5, "stripe count == 5");
    CHECK(z1.contrast >= ZEBRA_MIN_CONTRAST, "contrast above threshold");
    CHECK(z1.center_y > 0 && z1.center_y < TH, "center_y within image");
    CHECK(z1.confidence > 0 && z1.confidence <= 100, "confidence in range");

    /* 2) 纯色路面 → 不检出 */
    fill(img, 128);
    ZebraResult z2; memset(&z2, 0, sizeof(z2));
    CHECK(zebra_detect(img, TW, TH, &z2) == 0, "detect on flat road returns 0");
    CHECK(z2.detected == 0, "flat road not detected");

    /* 3) 纵向条纹（模拟车道线/路面纵向白线）→ 逐行取均值会被抹平，不误报 */
    fill(img, 60);
    for (int y = 0; y < TH; y++)
        for (int x = 0; x < TW; x++)
            if (((x / 4) % 2) == 0) {
                uint8_t *p = img + ((size_t)y * TW + x) * 3;
                p[0] = p[1] = p[2] = 220;
            }
    ZebraResult z3; memset(&z3, 0, sizeof(z3));
    CHECK(zebra_detect(img, TW, TH, &z3) == 0, "detect on vertical stripes returns 0");
    CHECK(z3.detected == 0, "vertical stripes NOT misdetected as zebra");

    /* 4) 单条粗带（对比度够但条纹不足）→ 不检出 */
    fill(img, 60);
    for (int dy = 0; dy < 40; dy++) hline(img, 260 + dy, 220);
    ZebraResult z4; memset(&z4, 0, sizeof(z4));
    CHECK(zebra_detect(img, TW, TH, &z4) == 0, "detect single band returns 0");
    CHECK(z4.detected == 0, "single band not detected (need >=3 stripes)");

    free(img);
    printf(g_fail == 0 ? "test_zebra: PASS\n" : "test_zebra: FAIL (%d)\n", g_fail);
    return g_fail == 0 ? 0 : 1;
}
