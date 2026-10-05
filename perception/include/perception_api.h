/*
 * perception_api.h — 感知流水线对外接口（【人员 A · 感知】）
 *
 * 这是 A 模块对外的唯一入口。B/C 只需调用 perception_init / perception_step
 * / perception_deinit，无需关心内部是 PCIe 还是桩、是 RKNN 还是占位。
 * perception_step 的签名保持“桩兼容”：use_stub=1 时不碰硬件/模型，
 * 直接吐一条直道假结果，便于 B 在没有相机和 NPU 时联调。
 */
#ifndef PERCEPTION_API_H
#define PERCEPTION_API_H

#include <stdint.h>

#include "driving_types.h"
#include "lane_detect.h"
#include "lane_mark.h"
#include "zebra_detect.h"
#include "traffic_light.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int  use_stub;       /* 1=桩模式（不读硬件），0=真实采集 */
    int  img_w;
    int  img_h;
    int  lead_px;
    const char *model_path;
} perception_cfg_t;

typedef struct {
    perception_cfg_t cfg;

    /* 真实通道 */
    struct {
        int            fd;     /* 采集句柄（桩模式下未用） */
        void          *grabber; /* frame_grabber_t* */
        lane_model_t  *model;   /* RKNN 引擎 */
    } real;

    /* 缓冲 */
    uint8_t  *frame565;  /* 剥离前导后的干净 640x480 RGB565 帧 */
    uint8_t  *rgb888;    /* 转换后 RGB888 */
    lane_seg_t seg;      /* 分割结果 */

    LaneMarkResult lane_marks;  /* 虚实线/变道判定结果（每帧更新） */
    ZebraResult    zebra;       /* 斑马线识别结果（每帧更新） */
    TrafficLightResult tl;      /* 红绿灯识别结果（每帧更新） */

    uint32_t  frame_id;
} perception_ctx_t;

/* 初始化流水线（桩/真实由 cfg.use_stub 决定） */
int  perception_init(perception_ctx_t *ctx, const perception_cfg_t *cfg);
/* 桩：按请求生成一条确定 LaneResult（不碰硬件/模型） */
void lane_stub_make_result(LaneResult *out, uint32_t frame_id,
                           LaneDirection dir, uint32_t conf);
/* 推进一帧：采集→转换→分割→判定，结果写入 out；成功返回 0。
 * 同时刷新 ctx->lane_marks（虚实线/变道）与 ctx->zebra（斑马线）。 */
int  perception_step(perception_ctx_t *ctx, LaneResult *out);
/* 释放资源 */
void perception_deinit(perception_ctx_t *ctx);

/* 取最近一帧的虚实线/变道判定结果（未初始化时返回 NULL） */
const LaneMarkResult *perception_lane_marks(const perception_ctx_t *ctx);
/* 取最近一帧的斑马线识别结果（未初始化时返回 NULL） */
const ZebraResult    *perception_zebra(const perception_ctx_t *ctx);
/* 取最近一帧的红绿灯识别结果（未初始化时返回 NULL） */
const TrafficLightResult *perception_traffic_light(const perception_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* PERCEPTION_API_H */
