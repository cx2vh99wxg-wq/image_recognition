/*
 * demo_real_detect_write.c — 验证「真实识别结果 → 共享内存 → B」
 *
 * 合成一帧含「红灯 + 斑马线 + 中央实线」的画面，依次跑三个真实感知算法
 * （traffic_light_detect / zebra_detect / lane_marks_analyze），把识别结果
 * 写入对应共享内存（shm_write_traffic_light / shm_write_zebra / shm_write_lane_mark）。
 * 之后由 B 端读取器（test_b_side）读出并断言。
 */
#include "traffic_light.h"
#include "zebra_detect.h"
#include "lane_mark.h"
#include "lane_detect.h"
#include "shm_ipc.h"
#include "driving_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define W 640
#define H 480

static void draw_disc(uint8_t *img, int cx, int cy, int r,
                      uint8_t R, uint8_t G, uint8_t B)
{
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            int dx = x - cx, dy = y - cy;
            uint8_t *p = img + ((size_t)y * W + x) * 3u;
            if (dx * dx + dy * dy <= r * r) { p[0] = R; p[1] = G; p[2] = B; }
            else { p[0] = 20; p[1] = 20; p[2] = 20; }
        }
    }
}

int main(void)
{
    uint8_t *img = (uint8_t *)malloc((size_t)W * H * 3u);
    if (!img) { printf("FAIL malloc\n"); return 1; }
    memset(img, 20, (size_t)W * H * 3u);

    /* 场景：红灯 + 5 条斑马线 + 压在中线的实线 */
    draw_disc(img, 200, 150, 30, 255, 0, 0);                       /* 红灯 */
    for (int k = 0; k < 5; k++)                                   /* 斑马线 */
        for (int dy = 0; dy < 8; dy++)
            for (int x = 0; x < W; x++) {
                int y = 250 + k * 20 + dy;
                uint8_t *p = img + ((size_t)y * W + x) * 3u;
                p[0] = p[1] = p[2] = 220;
            }

    /* 1) 红绿灯识别 → 写 shm */
    TrafficLightResult tl; memset(&tl, 0, sizeof(tl));
    if (traffic_light_detect(img, W, H, &tl) != 0) { printf("FAIL tl detect\n"); return 1; }
    tl.frame_id = 100; tl.timestamp_us = 12345678u;
    if (shm_write_traffic_light(&tl) != 0) { printf("FAIL write tl\n"); return 1; }

    /* 2) 斑马线识别 → 写 shm */
    ZebraResult z; memset(&z, 0, sizeof(z));
    if (zebra_detect(img, W, H, &z) != 0) { printf("FAIL zebra detect\n"); return 1; }
    z.frame_id = 100; z.timestamp_us = 12345678u;
    if (shm_write_zebra(&z) != 0) { printf("FAIL write zebra\n"); return 1; }

    /* 3) 虚实线/压线 → 写 shm（压中线实线 → crossing + solid_cross） */
    lane_seg_t seg; memset(&seg, 0, sizeof(seg));
    lane_seg_alloc(&seg, W, H);
    for (int y = (int)((float)H * LM_ROI_TOP_FRAC); y < (int)((float)H * LM_ROI_BOT_FRAC); y++)
        for (int dx = -2; dx <= 2; dx++) {
            int x = W / 2 + dx;
            if (x >= 0 && x < W) seg.lane_prob[(size_t)y * W + x] = 1.0f;
        }
    LaneMarkResult m; memset(&m, 0, sizeof(m));
    if (lane_marks_analyze(&seg, W, H, &m) != 0) { printf("FAIL lane_mark analyze\n"); return 1; }
    m.frame_id = 100; m.timestamp_us = 12345678u;
    if (shm_write_lane_mark(&m) != 0) { printf("FAIL write lane_mark\n"); return 1; }

    printf("A 端注入真实识别结果: tl state=%d conf=%u | zebra det=%u stripes=%u conf=%u | lm cross=%u solid_cross=%d\n",
           (int)tl.state, tl.confidence,
           z.detected, z.stripe_count, z.confidence,
           (unsigned)m.crossing, lane_mark_is_solid_cross(&m));
    return 0;
}
