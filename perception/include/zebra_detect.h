/*
 * zebra_detect.h — 斑马线（人行横道）识别（【人员 A · 感知】）
 *
 * 对应基础任务③「道路环境监测与特定场景感知」中的斑马线缺口。
 *
 * 原理（纯几何/灰度统计，不依赖额外模型）：
 *   斑马线的视觉特征是「路面上一组横向（垂直于行车方向）的明暗相间条纹」。
 *   在车头正前方的路面 ROI 内：
 *     1) 逐行对水平方向求灰度均值，得到沿 y 的亮度剖面；
 *     2) 用剖面的极大/极小值确定自适应阈值并二值化；
 *     3) 统计明暗跳变次数与亮条纹段数；
 *   横向条纹会使剖面出现多次周期性跳变，而纵向线条（车道线）因被逐行取
 *   均值而"抹平"，不会误报——这正是斑马线与车道线的天然区别。
 *
 * 输出结构 ZebraResult 固定 64 字节。
 */
#ifndef ZEBRA_DETECT_H
#define ZEBRA_DETECT_H

#include <stdint.h>

#include "driving_types.h"   /* ZebraResult / ZEBRA_VERSION（契约唯一真源） */

#ifdef __cplusplus
extern "C" {
#endif

/* 算法参数（可按现场标定） */
#define ZEBRA_ROI_TOP_FRAC   0.45f   /* 路面 ROI 起始行比例 */
#define ZEBRA_ROI_BOT_FRAC   0.92f   /* 路面 ROI 结束行比例（避开画面最底车头行） */
#define ZEBRA_ROI_L_FRAC     0.25f   /* ROI 左边界比例 */
#define ZEBRA_ROI_R_FRAC     0.75f   /* ROI 右边界比例 */
#define ZEBRA_MIN_CONTRAST   40      /* 明暗对比下限（灰度 0~255），低于视为无条纹 */
#define ZEBRA_MIN_STRIPES    3       /* 亮条纹段数下限 */
#define ZEBRA_MAX_STRIPES    20      /* 亮条纹段数上限（超过视为噪声/非斑马线） */

/* 契约结构体 ZebraResult 定义见 common/include/driving_types.h（A 起草，三人 review） */

/*
 * 对一帧 RGB888（R/G/B 顺序，每像素 3 字节）做斑马线识别。
 * 成功返回 0；out 的 version/分析字段被填充，frame_id/timestamp 由调用方补全。
 * detected=0（未检出）亦为成功。
 */
int zebra_detect(const uint8_t *rgb888, int w, int h, ZebraResult *out);
/* Same adaptive threshold for detection and green stripe annotations, max 16. */
int zebra_detect_rows(const uint8_t *rgb888,int w,int h,ZebraResult *out,
                      uint16_t *stripe_rows,uint32_t *stripe_count);

#ifdef __cplusplus
}
#endif

#endif /* ZEBRA_DETECT_H */
