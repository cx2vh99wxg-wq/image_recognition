/*
 * lane_detect.h — 车道线与可行驶区域分割（【人员 A · 感知】）
 *
 * 使用 YOLOPv2（RKNN NPU）做语义分割，输出两类概率图：
 *   - lane_prob  : 车道线概率 (1,1,H,W)
 *   - drivable   : 可行驶区域 2 分类 (1,2,H,W)
 * 不在此做弯道判定（那属于 turn_decide 的职责），只产出原始概率图。
 */
#ifndef LANE_DETECT_H
#define LANE_DETECT_H

#include <stdint.h>

#include "rknn_api.h"   /* RKNN 运行时头（板端提供，本机语法检查用同名桩头） */

#ifdef __cplusplus
extern "C" {
#endif

/* 模型输入分辨率（与训练一致） */
#define LANE_MODEL_W 640
#define LANE_MODEL_H 480

/* 分割结果容器：全部为浮点概率，未做阈值化 */
typedef struct {
    int    w;
    int    h;
    float *lane_prob;   /* w*h，车道线概率 0~1 */
    float *drivable;    /* w*h*2，可行驶区域两分类 */
} lane_seg_t;

/* RKNN 引擎句柄 */
typedef struct {
    rknn_context     ctx;
    rknn_input_output_num io_num;
    rknn_tensor_attr *in_attr;
    rknn_tensor_attr *out_attr;
    rknn_input      *inputs;
    rknn_output     *outputs;
    uint8_t         *in_buf;     /* 模型输入 RGB */
    float           *lane_buf;   /* 车道线输出缓存 */
    float           *drive_buf;  /* 可行驶区域输出缓存 */
    int              m_w;
    int              m_h;
    int              ready;
} lane_model_t;

/* 分配/释放分割容器 */
int  lane_seg_alloc(lane_seg_t *s, int w, int h);
void lane_seg_free(lane_seg_t *s);

/* 加载 RKNN 模型并分配缓存；成功返回 0 */
int  lane_model_init(lane_model_t *m, const char *model_path);
/* 对一帧 RGB888 运行推理，结果写入 out；成功返回 0 */
int  lane_model_run(lane_model_t *m, const uint8_t *rgb888, int w, int h, lane_seg_t *out);
/* 释放引擎 */
void lane_model_release(lane_model_t *m);

/*
 * 将 RKNN 原始输出映射到 lane_seg（不依赖 RKNN 库，可单元测试）：
 *   drivable_raw : 模型输出 0 节点 (1,2,H,W)
 *   lane_raw     : 模型输出 1 节点 (1,1,H,W)
 * 两分类整段拷贝到 drivable（w*h*2），车道线概率整段拷贝到 lane_prob。
 */
int lane_model_extract(const float *drivable_raw, const float *lane_raw,
                       int w, int h, lane_seg_t *out);

#ifdef __cplusplus
}
#endif

#endif /* LANE_DETECT_H */
