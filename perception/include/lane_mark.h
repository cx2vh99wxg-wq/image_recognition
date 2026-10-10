/*
 * lane_mark.h — 车道线虚实判定与变道/压线判定（【人员 A · 感知】）
 *
 * 对应现场测评②「虚线变道、实线直行、强制实线变道报警」：
 *   1) 区分左侧 / 右侧车道线是虚线还是实线；
 *   2) 判定车辆是否正压在 / 跨越某条车道线（压线、变道）；
 *   3) 给出「跨越的是实线还是虚线」信号：
 *        - 跨越虚线 → 合法变道（lane_change 置位，crossing_solid 为 0）
 *        - 跨越实线 → 违法变道（crossing_solid 置位，供决策层触发报警）
 *
 * 原理（与 turn_decide 同源，均基于 YOLOPv2 车道线概率图，不依赖模型）：
 *   - 取画面下部「近场」ROI（车头正前方），把图像按中线分左右半幅；
 *   - 逐行统计该半幅内是否存在车道线像素，得到「行出现率 presence」；
 *       · 实线：每行几乎都有 → 断空率≈0（presence≈1）
 *       · 虚线：按占空比间断 → 断空率较高（presence≈0.4~0.7）
 *   - 近场车道线的横向 x 与图像中线（≈车头中心）距离小于阈值 → 判为压线；
 *   - 纯几何统计，可完全离线单元测试。
 *
 * 输出结构 LaneMarkResult 固定 64 字节，附编译期尺寸断言，与 LaneResult 同规格。
 */
#ifndef LANE_MARK_H
#define LANE_MARK_H

#include <stdint.h>

#include "driving_types.h"   /* LaneMarkType / LaneMarkResult / LANEMARK_VERSION（契约唯一真源） */
#include "lane_detect.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 契约版本与结果结构体定义见 common/include/driving_types.h（A 起草，三人 review） */

/* ----------------------------------------------------------------------
 * 算法参数（集中于此，便于现场标定；均为纯几何阈值）
 * -------------------------------------------------------------------- */
#define LM_THRESH          0.30f   /* 车道线像素概率阈值（与 turn_decide 一致） */
#define LM_ROI_TOP_FRAC    0.68f   /* 近场 ROI 起始行比例（自顶向下） */
#define LM_ROI_BOT_FRAC    0.95f   /* 近场 ROI 结束行比例（避开画面最底车头行） */
#define LM_MIN_PRESENCE    0.30f   /* 该侧行出现率低于此 → 视为无车道线 */
#define LM_SOLID_MAX_GAP   0.15f   /* 断空率 ≤ 此 → 实线 */
#define LM_DASH_MIN_GAP    0.35f   /* 断空率 ≥ 此 → 虚线 */
#define LM_CROSS_MARGIN    20      /* 640 宽参考尺度；实际按图宽缩放 → 压线 */
#define LM_OFFSET_MAX      200     /* ego_offset_px 饱和上限（像素） */

/* 便捷判定：当帧是否发生「跨越实线」（强制实线变道报警信号） */
static inline int lane_mark_is_solid_cross(const LaneMarkResult *m)
{
    if (!m || !m->crossing) return 0;
    if (m->crossing_left  && m->left_type  == LANE_MARK_SOLID) return 1;
    if (m->crossing_right && m->right_type == LANE_MARK_SOLID) return 1;
    return 0;
}

/*
 * 对一帧车道线概率图做形态与变道分析。
 * seg 需已由 lane_seg_alloc 分配（w×h 车道线概率）。
 * 成功返回 0；out 的 version/分析字段被填充，frame_id/timestamp 由调用方补全。
 */
int lane_marks_analyze(const lane_seg_t *seg, int img_w, int img_h,
                       LaneMarkResult *out);

#ifdef __cplusplus
}
#endif

#endif /* LANE_MARK_H */
