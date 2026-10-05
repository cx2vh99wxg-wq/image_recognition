/*
 * turn_decide.c — 弯道判定与置信度（【人员 A · 感知】）
 *
 * 原理（与旧版一致）：把画面下方按水平带切片，逐带做车道线概率的垂直
 * 投影并求重心，比较远带与近带的重心横向位置得到带符号偏移量。
 * 改进（文档强制）：
 *   1) confidence 真实计算，不再硬编码 90/75/60/40；
 *   2) 用像素偏移 curve_offset 取代恒为 25° 的伪角度；
 *   3) 覆盖不足时返回 LANE_UNKNOWN，而非硬给低置信度直道。
 */
#include "turn_decide.h"
#include "driving_config.h"   /* A 只读：阈值由 B 维护并冻结 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* 单带内判定为“有效”的最少车道线像素 */
#define TURN_BAND_MIN_PIXELS 10

int lane_geometry_analyze(const lane_seg_t *seg, int img_w, int img_h,
                          int roi_top, lane_geometry_t *geo)
{
    if (!seg || !seg->lane_prob || img_w <= 0 || img_h <= 0 || !geo) return -1;
    if (roi_top < 0) roi_top = 0;
    if (roi_top >= img_h) return -1;

    memset(geo, 0, sizeof(*geo));
    geo->total_bands = TURN_BAND_COUNT;

    const int band_h = TURN_BAND_HEIGHT;
    const int roi_bottom = img_h;

    int *proj = (int *)calloc((size_t)img_w, sizeof(int));
    if (!proj) return -1;

    float centroids[TURN_BAND_COUNT];
    int   totals[TURN_BAND_COUNT];
    for (int i = 0; i < TURN_BAND_COUNT; i++) { centroids[i] = -1.0f; totals[i] = 0; }

    uint32_t total_lane = 0;
    long long roi_area = 0;

    int band_idx = 0;
    for (int y = roi_top; y < roi_bottom && band_idx < TURN_BAND_COUNT; y += band_h) {
        int y_end = y + band_h;
        if (y_end > roi_bottom) y_end = roi_bottom;
        memset(proj, 0, (size_t)img_w * sizeof(int));

        for (int yy = y; yy < y_end; yy++) {
            const float *row = seg->lane_prob + (size_t)yy * img_w;
            for (int x = 0; x < img_w; x++) {
                if (row[x] > TURN_LANE_THRESH) {
                    proj[x]++;
                    totals[band_idx]++;
                }
            }
        }
        roi_area += (long long)(y_end - y) * img_w;
        total_lane += (uint32_t)totals[band_idx];

        if (totals[band_idx] > TURN_BAND_MIN_PIXELS) {
            long long sumx = 0;
            for (int x = 0; x < img_w; x++) sumx += (long long)x * proj[x];
            centroids[band_idx] = (float)((double)sumx / totals[band_idx]);
            geo->valid_bands++;
        }
        band_idx++;
    }

    geo->lane_pixels = total_lane;
    geo->coverage = roi_area > 0 ? (float)((double)total_lane / roi_area) : 0.0f;

    /* 找最远 / 最近的有效带 */
    int far = -1, near = -1;
    for (int i = 0; i < TURN_BAND_COUNT; i++) {
        if (centroids[i] >= 0.0f) {
            if (far == -1) far = i;
            near = i;
        }
    }
    if (far >= 0) geo->far_cx = centroids[far];
    if (near >= 0) geo->near_cx = centroids[near];

    /* 加权偏移：相邻有效带两两求差，远带−近带方向为正 */
    static const float band_w[TURN_BAND_COUNT] = {2.0f,1.7f,1.5f,1.3f,1.0f,0.8f,0.6f,0.5f};
    float wsum = 0.0f;
    for (int i = 0; i < TURN_BAND_COUNT - 1; i++) {
        if (centroids[i] >= 0.0f && centroids[i + 1] >= 0.0f) {
            float local = centroids[i] - centroids[i + 1];
            geo->weighted_offset += local * band_w[i];
            wsum += band_w[i];
        }
    }
    if (wsum > 0.0f) geo->weighted_offset /= wsum;

    free(proj);
    return 0;
}

int turn_decide(const lane_seg_t *seg, int img_w, int img_h, LaneResult *out)
{
    if (!seg || !out || img_w <= 0 || img_h <= 0) return -1;

    lane_geometry_t geo;
    int roi_top = (int)((float)img_h * LANE_ROI_TOP_FRAC);   /* ROI 起点来自冻结配置 */
    if (roi_top < 0) roi_top = 0;
    if (roi_top >= img_h) roi_top = img_h / 2;               /* 兜底：至少分析下半部 */
    if (lane_geometry_analyze(seg, img_w, img_h, roi_top, &geo) != 0) return -1;

    memset(out->reserved, 0, sizeof(out->reserved));
    out->lane_pixel_cnt = geo.lane_pixels;

    /* curve_offset 带符号像素偏移，饱和到配置上限 */
    int32_t off = (int32_t)lroundf(geo.weighted_offset);
    if (off > (int32_t)LANE_OFFSET_MAX) off = (int32_t)LANE_OFFSET_MAX;
    if (off < -(int32_t)LANE_OFFSET_MAX) off = -(int32_t)LANE_OFFSET_MAX;
    out->curve_offset = off;

    /* 真实置信度：有效带比例 + 车道线密度，封顶 100 */
    float valid_ratio = geo.total_bands > 0
                        ? (float)geo.valid_bands / geo.total_bands : 0.0f;
    float conf = valid_ratio * 60.0f + geo.coverage * 400.0f;
    if (conf > 100.0f) conf = 100.0f;
    if (conf < 0.0f)   conf = 0.0f;
    out->confidence = (uint32_t)(conf + 0.5f);

    /* 方向判定（阈值取 B 冻结的配置：LANE_STRAIGHT_TH） */
    if (geo.coverage < TURN_MIN_COVERAGE || geo.valid_bands < 2) {
        out->direction = LANE_UNKNOWN;
    } else if (off > (int32_t)LANE_STRAIGHT_TH) {
        out->direction = LANE_RIGHT;
    } else if (off < -(int32_t)LANE_STRAIGHT_TH) {
        out->direction = LANE_LEFT;
    } else {
        out->direction = LANE_STRAIGHT;
    }
    return 0;
}
