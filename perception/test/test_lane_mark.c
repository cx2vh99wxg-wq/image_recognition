/*
 * test_lane_mark.c — 车道线虚实/变道判定单元测试（【人员 A】自测，不依赖硬件/RKNN）
 *
 * 覆盖：实线、虚线、单侧可见、压线、空场景，以及"跨越实线报警"信号。
 */
#include "lane_mark.h"
#include "lane_detect.h"

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

/* 在近场 ROI 内画一条竖线；dash=1 时按 8on/8off 的占空比画成虚线 */
static void draw_vline(lane_seg_t *s, int x, int dash)
{
    int y0 = (int)((float)TH * LM_ROI_TOP_FRAC);
    int y1 = (int)((float)TH * LM_ROI_BOT_FRAC);
    for (int y = y0; y < y1; y++) {
        if (dash && ((y / 8) % 2) != 0) continue;   /* 虚线：一半的行留空 */
        for (int dx = -2; dx <= 2; dx++) {
            int xx = x + dx;
            if (xx >= 0 && xx < TW) s->lane_prob[(size_t)y * TW + xx] = 1.0f;
        }
    }
}

static void make_seg(lane_seg_t *s) { memset(s, 0, sizeof(*s)); lane_seg_alloc(s, TW, TH); }

int main(void)
{
    printf("== test_lane_mark ==\n");

    /* 1) 左右皆实线、位于车道两侧 → 双实线，无压线，偏移≈0 */
    lane_seg_t solid; make_seg(&solid);
    draw_vline(&solid, 160, 0);
    draw_vline(&solid, 480, 0);
    LaneMarkResult m1; memset(&m1, 0, sizeof(m1));
    CHECK(lane_marks_analyze(&solid, TW, TH, &m1) == 0, "analyze solid");
    CHECK(m1.version == LANEMARK_VERSION, "version set");
    CHECK(m1.left_type  == LANE_MARK_SOLID, "left is SOLID");
    CHECK(m1.right_type == LANE_MARK_SOLID, "right is SOLID");
    CHECK(m1.left_x == 160 && m1.right_x == 480, "line positions correct");
    CHECK(m1.ego_offset_px >= -3 && m1.ego_offset_px <= 3, "ego centered (offset near 0)");
    CHECK(m1.crossing == 0, "no crossing when lines at sides");
    CHECK(m1.confidence >= 90, "high confidence for both visible");
    lane_seg_free(&solid);

    /* 2) 左虚线 + 右实线 → 分别定性 */
    lane_seg_t mix; make_seg(&mix);
    draw_vline(&mix, 160, 1);   /* 虚 */
    draw_vline(&mix, 480, 0);   /* 实 */
    LaneMarkResult m2; memset(&m2, 0, sizeof(m2));
    CHECK(lane_marks_analyze(&mix, TW, TH, &m2) == 0, "analyze mixed");
    CHECK(m2.left_type  == LANE_MARK_DASHED, "left is DASHED");
    CHECK(m2.right_type == LANE_MARK_SOLID,  "right is SOLID");
    CHECK(m2.left_gap_pm > m2.right_gap_pm, "dashed has larger gap ratio than solid");
    CHECK(m2.left_gap_pm >= 350, "left gap ratio high (dashed)");
    CHECK(m2.right_gap_pm <= 150, "right gap ratio low (solid)");
    lane_seg_free(&mix);

    /* 3) 一条实线压在中线（车辆下方）→ 压线 + 跨越实线报警 */
    lane_seg_t cross; make_seg(&cross);
    draw_vline(&cross, TW / 2, 0);
    LaneMarkResult m3; memset(&m3, 0, sizeof(m3));
    CHECK(lane_marks_analyze(&cross, TW, TH, &m3) == 0, "analyze crossing");
    CHECK(m3.crossing == 1, "crossing detected");
    CHECK(m3.crossing_left || m3.crossing_right, "crossing side flagged");
    CHECK(m3.lane_change == 1, "lane_change flagged while crossing");
    CHECK(lane_mark_is_solid_cross(&m3) == 1, "crossing a SOLID line → alarm signal");
    lane_seg_free(&cross);

    /* 4) 一条虚线压在中线 → 变道但不是"实线报警" */
    lane_seg_t crossd; make_seg(&crossd);
    draw_vline(&crossd, TW / 2, 1);
    LaneMarkResult m4; memset(&m4, 0, sizeof(m4));
    CHECK(lane_marks_analyze(&crossd, TW, TH, &m4) == 0, "analyze dashed crossing");
    CHECK(m4.crossing == 1, "dashed crossing detected");
    CHECK(lane_mark_is_solid_cross(&m4) == 0, "crossing a DASHED line → no solid alarm");
    lane_seg_free(&crossd);

    /* 5) 仅右侧可见 → 左侧 NONE */
    lane_seg_t one; make_seg(&one);
    draw_vline(&one, 480, 0);
    LaneMarkResult m5; memset(&m5, 0, sizeof(m5));
    CHECK(lane_marks_analyze(&one, TW, TH, &m5) == 0, "analyze single side");
    CHECK(m5.left_type  == LANE_MARK_NONE, "left NONE when absent");
    CHECK(m5.right_type == LANE_MARK_SOLID, "right SOLID when present");
    CHECK(m5.left_x == -1, "left_x = -1 when absent");
    lane_seg_free(&one);

    /* 6) 空场景 → 双 NONE、置信度 0、无压线 */
    lane_seg_t empty; make_seg(&empty);
    LaneMarkResult m6; memset(&m6, 0, sizeof(m6));
    CHECK(lane_marks_analyze(&empty, TW, TH, &m6) == 0, "analyze empty");
    CHECK(m6.left_type == LANE_MARK_NONE && m6.right_type == LANE_MARK_NONE, "both NONE when empty");
    CHECK(m6.confidence == 0, "empty confidence 0");
    CHECK(m6.crossing == 0, "empty no crossing");
    lane_seg_free(&empty);

    printf(g_fail == 0 ? "test_lane_mark: PASS\n" : "test_lane_mark: FAIL (%d)\n", g_fail);
    return g_fail == 0 ? 0 : 1;
}
