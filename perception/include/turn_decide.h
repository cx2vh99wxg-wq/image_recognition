/*
 * turn_decide.h — 弯道判定与置信度（【人员 A · 感知】）
 *
 * 原理（与旧版一致，但指标更诚实）：
 *   把图像下半部分按水平带分为若干采样区，逐区求“车道线概率”的垂直
 *   投影重心；比较远区与近区的重心横向位置，得到带符号偏移量。
 *   - curve_offset = 远区重心x − 近区重心x（正=右弯，负=左弯）
 *   - confidence   = 真实计算：由“有效带比例”与“车道线密度”合成，封顶 100
 *   - 覆盖不足或有效带 < 2 时判 LANE_UNKNOWN（不再硬给 40 置信度）
 * 不再输出任何 25° 之类的伪角度。
 */
#ifndef TURN_DECIDE_H
#define TURN_DECIDE_H

#include <stdint.h>

#include "lane_detect.h"
#include "driving_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 几何分析结果（turn_decide 的中间量，单独可测） */
typedef struct {
    int    total_bands;
    int    valid_bands;     /* 概率质量足够、参与加权的带数 */
    float  far_cx;          /* 最远有效带重心 x */
    float  near_cx;         /* 最近有效带重心 x */
    float  weighted_offset; /* 远−近 加权偏移（像素） */
    float  coverage;        /* ROI 内车道线像素占比 0~1 */
    uint32_t lane_pixels;   /* ROI 内超过阈值的车道线像素数 */
} lane_geometry_t;

/* 采样参数：8 带 × 30 行 = 240 行，从 ROI 起点（LANE_ROI_TOP_FRAC）向下
 * 覆盖图像主体；与旧版"仅分析顶部 64 行"相比，近处车道线也参与判定。
 * 图像最底部约 24 行通常为车头/保险杠区域，不参与。 */
#define TURN_BAND_COUNT    8
#define TURN_BAND_HEIGHT   30
#define TURN_LANE_THRESH   0.30f   /* 车道线概率阈值（与绘制阈值一致） */
#define TURN_MIN_COVERAGE  0.005f  /* 低于此判定为未知 */
#define TURN_PIXEL_THRESH  8.0f    /* 偏移超过此像素判弯 */

/*
 * 只做几何分析，不写结果结构；供单元测试逐字段断言。
 * roi_top 指定只分析图像下部的起始 y（默认图像下半）。
 */
int lane_geometry_analyze(const lane_seg_t *seg, int img_w, int img_h,
                          int roi_top, lane_geometry_t *geo);

/* 综合判定：填充 LaneResult（version/frame_id/timestamp 由调用方填或此处填） */
int turn_decide(const lane_seg_t *seg, int img_w, int img_h, LaneResult *out);

#ifdef __cplusplus
}
#endif

#endif /* TURN_DECIDE_H */
