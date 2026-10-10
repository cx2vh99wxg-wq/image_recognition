/*
 * lane_mark.c — 车道线虚实判定与变道/压线判定实现（【人员 A · 感知】）
 *
 * 见 lane_mark.h 的算法说明。核心是纯几何统计：
 *   近场 ROI 内按左右半幅逐行查车道线像素 → 行出现率 → 断空率 → 虚实定性；
 *   再以近场线位与图像中线距离判压线、以线的虚实判「是否违法跨越实线」。
 */
#include "lane_mark.h"

#include <stdlib.h>
#include <string.h>

/* 断空率 + 出现率 → 形态定性 */
static LaneMarkType lm_classify(float gap, float presence)
{
    if (presence < LM_MIN_PRESENCE) return LANE_MARK_NONE;    /* 该侧基本无车道线 */
    if (gap <= LM_SOLID_MAX_GAP)    return LANE_MARK_SOLID;   /* 几乎每行都有 → 实线 */
    if (gap >= LM_DASH_MIN_GAP)     return LANE_MARK_DASHED;  /* 大量断空 → 虚线 */
    return LANE_MARK_UNKNOWN;                                 /* 落于模糊区 */
}

int lane_marks_analyze(const lane_seg_t *seg, int img_w, int img_h,
                       LaneMarkResult *out)
{
    if (!seg || !seg->lane_prob || !out || img_w <= 0 || img_h <= 0) return -1;

    memset(out, 0, sizeof(*out));
    out->version = LANEMARK_VERSION;
    out->left_x  = -1;
    out->right_x = -1;

    /* 近场 ROI（车头正前方一段，避开最底车头行） */
    int y0 = (int)((float)img_h * LM_ROI_TOP_FRAC);
    int y1 = (int)((float)img_h * LM_ROI_BOT_FRAC);
    if (y0 < 0) y0 = 0;
    if (y1 > img_h) y1 = img_h;
    if (y1 - y0 < 4) return -1;

    const int mid = img_w / 2;
    int cross_margin = (LM_CROSS_MARGIN * img_w + 320) / 640;
    if (cross_margin < 1) cross_margin = 1;
    int rows = 0;
    int left_cnt = 0, right_cnt = 0;     /* 左/右半幅"有车道线"的行数 */
    long left_sum = 0, right_sum = 0;    /* 左/右车道线 x 累加（求均值） */

    for (int y = y0; y < y1; y++) {
        const float *row = seg->lane_prob + (size_t)y * img_w;

        /* 左半幅：最左车道线像素所在连续段的中心（取线中心而非外边缘） */
        int lx = -1;
        for (int x = 0; x < mid; x++) {
            if (row[x] > LM_THRESH) {
                int a = x, b = x;
                while (a - 1 >= 0 && row[a - 1] > LM_THRESH) a--;
                while (b + 1 < img_w && row[b + 1] > LM_THRESH) b++;
                lx = (a + b) / 2;
                break;
            }
        }
        /* 右半幅：最右车道线像素所在连续段的中心 */
        int rx = -1;
        for (int x = img_w - 1; x >= mid; x--) {
            if (row[x] > LM_THRESH) {
                int a = x, b = x;
                while (a - 1 >= 0 && row[a - 1] > LM_THRESH) a--;
                while (b + 1 < img_w && row[b + 1] > LM_THRESH) b++;
                rx = (a + b) / 2;
                break;
            }
        }

        if (lx >= 0) { left_cnt++;  left_sum  += lx; }
        if (rx >= 0) { right_cnt++; right_sum += rx; }
        rows++;
    }
    if (rows <= 0) return -1;

    float lp = (float)left_cnt  / (float)rows;   /* 左半幅行出现率 */
    float rp = (float)right_cnt / (float)rows;   /* 右半幅行出现率 */
    float lg = 1.0f - lp;                        /* 左断空率 */
    float rg = 1.0f - rp;                        /* 右断空率 */

    out->left_type  = lm_classify(lg, lp);
    out->right_type = lm_classify(rg, rp);
    out->left_gap_pm  = (uint32_t)(lg * 1000.0f + 0.5f);
    out->right_gap_pm = (uint32_t)(rg * 1000.0f + 0.5f);

    if (out->left_type  != LANE_MARK_NONE && left_cnt  > 0)
        out->left_x  = (int32_t)((double)left_sum  / (double)left_cnt);
    if (out->right_type != LANE_MARK_NONE && right_cnt > 0)
        out->right_x = (int32_t)((double)right_sum / (double)right_cnt);

    /* 车辆中心相对车道中心的横向偏移（正 = 车辆偏右） */
    if (out->left_x >= 0 && out->right_x >= 0) {
        int lane_center = (out->left_x + out->right_x) / 2;
        int off = mid - lane_center;
        if (off >  LM_OFFSET_MAX) off =  LM_OFFSET_MAX;
        if (off < -LM_OFFSET_MAX) off = -LM_OFFSET_MAX;
        out->ego_offset_px = off;
    }

    /* 压线 / 变道：近场车道线贴近车辆中心（图像中线） */
    if (out->left_x  >= 0 && abs(out->left_x  - mid) <= cross_margin) {
        out->crossing = 1; out->crossing_left  = 1;
    }
    if (out->right_x >= 0 && abs(out->right_x - mid) <= cross_margin) {
        out->crossing = 1; out->crossing_right = 1;
    }
    out->lane_change = out->crossing;

    /* 可信度：两侧可见度加权（两侧都清晰→接近 100） */
    float conf = 0.0f;
    if (left_cnt  > 0) conf += 50.0f * lp;
    if (right_cnt > 0) conf += 50.0f * rp;
    if (conf > 100.0f) conf = 100.0f;
    if (conf < 0.0f)   conf = 0.0f;
    out->confidence = (uint32_t)(conf + 0.5f);

    return 0;
}
