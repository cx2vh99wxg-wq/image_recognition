/*
 * test_turn.c — 弯道判定单元测试（【人员 A】自测，不依赖硬件/RKNN）
 *
 * 验证核心算法：对合成车道线概率图做“远/近重心偏移”判定，并断言
 * 置信度为“真实计算值”（绝不出现旧版硬编码的 90/75/60/40）。
 */
#include "turn_decide.h"
#include "lane_detect.h"
#include "driving_config.h"   /* LANE_ROI_TOP_FRAC（与 turn_decide 的 ROI 保持一致） */

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
#define CENTER (TW / 2)

/* 构造车道线概率图：ROI 内画一条竖直线，far_x/near_x 控制远近带位置 */
static void build_seg(lane_seg_t *s, int far_x, int near_x)
{
    lane_seg_alloc(s, TW, TH);
    int roi_top = (int)((float)TH * LANE_ROI_TOP_FRAC);   /* 与 turn_decide 一致 */
    int band_h = TURN_BAND_HEIGHT;
    for (int y = roi_top; y < TH; y++) {
        int band = (y - roi_top) / band_h;     /* 仅前 8 带参与分析 */
        int lx = (band < 4) ? far_x : near_x;  /* 远 4 带 far_x，近 4 带 near_x */
        for (int x = lx - 2; x <= lx + 2; x++) {
            if (x >= 0 && x < TW) s->lane_prob[(size_t)y * TW + x] = 1.0f;
        }
    }
}

static int is_hardcoded(uint32_t c)
{
    return (c == 90u || c == 75u || c == 60u || c == 40u);
}

int main(void)
{
    printf("== test_turn ==\n");

    /* 直道：远近带同列 → 偏移≈0 */
    lane_seg_t straight; memset(&straight, 0, sizeof(straight));
    build_seg(&straight, CENTER, CENTER);
    LaneResult r_straight;
    CHECK(turn_decide(&straight, TW, TH, &r_straight) == 0, "turn_decide straight");
    CHECK(r_straight.direction == LANE_STRAIGHT, "straight -> LANE_STRAIGHT");
    CHECK(r_straight.curve_offset >= -TURN_PIXEL_THRESH &&
          r_straight.curve_offset <=  TURN_PIXEL_THRESH, "straight offset near 0");
    CHECK(!is_hardcoded(r_straight.confidence), "straight confidence not hardcoded");
    CHECK(r_straight.confidence > 50 && r_straight.confidence <= 100,
          "straight confidence computed high");
    lane_seg_free(&straight);

    /* 右弯：远带在右(far_x>CENTER)，近带在左 → 正偏移 */
    lane_seg_t right; memset(&right, 0, sizeof(right));
    build_seg(&right, CENTER + 120, CENTER - 120);
    LaneResult r_right;
    CHECK(turn_decide(&right, TW, TH, &r_right) == 0, "turn_decide right");
    CHECK(r_right.direction == LANE_RIGHT, "right -> LANE_RIGHT");
    CHECK(r_right.curve_offset > 0, "right offset positive");
    CHECK(!is_hardcoded(r_right.confidence), "right confidence not hardcoded");
    lane_seg_free(&right);

    /* 左弯：远带在左，近带在右 → 负偏移 */
    lane_seg_t left; memset(&left, 0, sizeof(left));
    build_seg(&left, CENTER - 120, CENTER + 120);
    LaneResult r_left;
    CHECK(turn_decide(&left, TW, TH, &r_left) == 0, "turn_decide left");
    CHECK(r_left.direction == LANE_LEFT, "left -> LANE_LEFT");
    CHECK(r_left.curve_offset < 0, "left offset negative");
    lane_seg_free(&left);

    /* 空场景：无任何车道线 → 未知，像素计数为 0 */
    lane_seg_t empty; memset(&empty, 0, sizeof(empty));
    lane_seg_alloc(&empty, TW, TH);
    LaneResult r_empty;
    CHECK(turn_decide(&empty, TW, TH, &r_empty) == 0, "turn_decide empty");
    CHECK(r_empty.direction == LANE_UNKNOWN, "empty -> LANE_UNKNOWN");
    CHECK(r_empty.lane_pixel_cnt == 0, "empty pixel count 0");
    CHECK(r_empty.confidence <= 100, "empty confidence within range");
    lane_seg_free(&empty);

    /* 几何分析单测：验证 far/near 重心与带符号偏移符号一致 */
    lane_seg_t geo_seg; memset(&geo_seg, 0, sizeof(geo_seg));
    build_seg(&geo_seg, CENTER + 120, CENTER - 120);
    lane_geometry_t geo;
    CHECK(lane_geometry_analyze(&geo_seg, TW, TH, (int)((float)TH * LANE_ROI_TOP_FRAC), &geo) == 0, "geometry analyze");
    CHECK(geo.far_cx > geo.near_cx, "far centroid right of near (right bend)");
    CHECK(geo.weighted_offset > 0, "weighted offset positive for right");
    CHECK(geo.valid_bands >= 2, "valid bands >= 2");
    lane_seg_free(&geo_seg);

    /* Production uses 320x240 tiles. Equivalent scenes at half resolution must
     * retain all eight sampling bands and the same turn classification. */
    for (int bend=-1;bend<=1;bend++) {
        lane_seg_t small={0};lane_seg_alloc(&small,320,240);
        for(int y=108;y<228;y++) {
            int x=160+(y<168?60:-60)*bend;
            for(int dx=-1;dx<=1;dx++)small.lane_prob[y*320+x+dx]=1;
        }
        LaneResult r={0};lane_geometry_t g={0};
        CHECK(lane_geometry_analyze(&small,320,240,108,&g)==0 && g.valid_bands==8,
              "320x240 uses all eight ROI bands");
        CHECK(turn_decide(&small,320,240,&r)==0 && r.confidence>=DEC_GO_CONF_MIN,
              "320x240 confidence does not lose missing bands");
        CHECK(r.direction==(bend<0?LANE_LEFT:bend>0?LANE_RIGHT:LANE_STRAIGHT),
              "320x240 turn agrees with full-resolution geometry");
        lane_seg_free(&small);
    }
    printf(g_fail == 0 ? "test_turn: PASS\n" : "test_turn: FAIL (%d)\n", g_fail);
    return g_fail == 0 ? 0 : 1;
}
