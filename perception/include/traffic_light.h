/*
 * traffic_light.h — 红绿灯识别（【人员 A · 感知】）
 *
 * 对应现场测评①「十字路口：红灯停/绿灯行」的感知缺口。
 * 红绿灯是前向路口感知，按 10-05 复核结论由 A 负责识别，B 仅用结果做决策。
 *
 * 原理（传统 CV，不依赖额外模型）：
 *   红绿灯的视觉特征是「暗背景中一个高亮、高饱和的发光色块（红/黄/绿）」。
 *   在画面中上部 ROI 内，对每像素按归一化 RGB + 饱和度判定其最可能属于
 *   红/黄/绿哪一类灯色，生成三色二值掩码；再对每色做连通块分析，
 *   取面积最大的连通团作为「点亮的灯」，并输出包围盒；最后在三色中
 *   选面积最大且超过噪声阈值者作为当前灯色。
 *
 *   该路线无需第二个检测模型（A 端 NPU 已跑 YOLOPv2），实时性好，
 *   且可在桩模式下用合成图单元测试。若实车复杂光照下不够稳，后续可升级
 *   为叠加一个 YOLOv5（COCO 含 traffic light）检测头，接口不变。
 *
 * 输出结构 TrafficLightResult 固定 64 字节（契约见 common/driving_types.h）。
 */
#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <stdint.h>
#include "driving_types.h"   /* TrafficLightResult / TrafficLightState / TL_VERSION 真源 */

#ifdef __cplusplus
extern "C" {
#endif

/* 版本号取自公共契约真源 driving_types.h；此处守卫避免重复定义 */
#ifndef TL_VERSION
#define TL_VERSION 1u
#endif

/* 算法参数（可按现场标定） */
#define TL_ROI_TOP_FRAC 0.05f   /* 灯通常在画面中上部，从近顶开始搜索 */
#define TL_ROI_BOT_FRAC 0.80f   /* 向下搜索到 80% 高度（地面以上） */
#define TL_MIN_BRIGHT   90      /* 发光最低亮度（0~255），低于视为不发光 */
#define TL_MIN_AREA     60      /* 最小点亮面积（像素），过滤噪声 */
#define TL_REF_AREA     600     /* 参考面积：达到该面积置信度接近满分 */
#define TL_SAT_MIN      0.25f   /* 最小饱和度，过滤灰色/白色伪灯 */

/*
 * 对一帧 RGB888（R/G/B 顺序，每像素 3 字节）做红绿灯识别。
 * 成功返回 0；out 的 version/分析字段被填充，frame_id/timestamp 由调用方补全。
 * detected=0（未检出点亮灯）亦为成功。
 */
int traffic_light_detect(const uint8_t *rgb888, int w, int h,
                         TrafficLightResult *out);

#if defined(__cplusplus)
static_assert(sizeof(TrafficLightResult) == 64, "TrafficLightResult must be 64 bytes");
#else
_Static_assert(sizeof(TrafficLightResult) == 64, "TrafficLightResult must be 64 bytes");
#endif

#ifdef __cplusplus
}
#endif

#endif /* TRAFFIC_LIGHT_H */
