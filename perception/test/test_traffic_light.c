/*
 * test_traffic_light.c — 红绿灯识别单元测试（【人员 A · 感知】）
 *
 * 在暗背景上合成红/黄/绿实心圆块与全暗图，验证 traffic_light_detect
 * 的颜色判定与包围盒输出。纯逻辑、零硬件依赖，可在本机 / WSL 直接跑。
 */
#include "traffic_light.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TW 640
#define TH 480

/* 在暗背景图上画一个中心 (cx,cy)、半径 r 的纯色圆 */
static void draw_disc(uint8_t *img, int cx, int cy, int r,
                      uint8_t R, uint8_t G, uint8_t B)
{
    for (int y = 0; y < TH; y++) {
        for (int x = 0; x < TW; x++) {
            int dx = x - cx, dy = y - cy;
            uint8_t *p = img + ((size_t)y * TW + x) * 3u;
            if (dx * dx + dy * dy <= r * r) {
                p[0] = R; p[1] = G; p[2] = B;
            } else {
                p[0] = 20; p[1] = 20; p[2] = 20;   /* 暗背景，不发光 */
            }
        }
    }
}

static int fail = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { printf("  [FAIL] %s\n", msg); fail++; } \
    else { printf("  [ok]   %s\n", msg); } \
} while (0)

static void run_case(const char *name, uint8_t R, uint8_t G, uint8_t B,
                     TrafficLightState expect)
{
    printf("用例: %s\n", name);
    uint8_t *img = (uint8_t *)malloc((size_t)TW * TH * 3u);
    if (!img) { printf("  [FAIL] malloc\n"); fail++; return; }
    draw_disc(img, 200, 150, 30, R, G, B);

    TrafficLightResult out;
    int rc = traffic_light_detect(img, TW, TH, &out);
    CHECK(rc == 0, "调用返回 0");
    CHECK(out.detected == 1, "检出点亮灯");
    CHECK(out.state == expect, "灯色判定正确");
    CHECK(out.box_x >= 0 && out.box_w > 0 && out.box_h > 0, "包围盒有效");
    CHECK(out.confidence > 0 && out.confidence <= 100, "置信度 0~100");
    CHECK(out.version == TL_VERSION, "version 正确");

    printf("    -> state=%d conf=%u box=(%d,%d,%d,%d) r/g/b_area=%u/%u/%u\n",
           (int)out.state, out.confidence, out.box_x, out.box_y,
           out.box_w, out.box_h, out.red_area, out.yellow_area, out.green_area);
    free(img);
}

int main(void)
{
    printf("==== traffic_light 单元测试 ====\n");
    CHECK(sizeof(TrafficLightResult) == 64, "TrafficLightResult 为 64 字节");

    run_case("红灯",   255, 0,   0,   TL_RED);
    run_case("绿灯",   0,   255, 0,   TL_GREEN);
    run_case("黄灯",   255, 255, 0,   TL_YELLOW);

    /* 全暗图：应无检出 */
    printf("用例: 全暗（无灯）\n");
    uint8_t *dark = (uint8_t *)malloc((size_t)TW * TH * 3u);
    if (dark) {
        memset(dark, 20, (size_t)TW * TH * 3u);
        TrafficLightResult out;
        int rc = traffic_light_detect(dark, TW, TH, &out);
        CHECK(rc == 0, "调用返回 0");
        CHECK(out.detected == 0, "未检出点亮灯");
        CHECK(out.state == TL_UNKNOWN, "状态为 UNKNOWN");
        free(dark);
    } else { CHECK(0, "malloc dark"); }

    if (fail == 0) { printf("==== ALL PASS ====\n"); return 0; }
    printf("==== %d 项失败 ====\n", fail);
    return 1;
}
