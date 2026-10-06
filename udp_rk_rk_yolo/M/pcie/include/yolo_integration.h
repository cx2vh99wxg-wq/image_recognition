/* ===========================================================================
 * 文件：yolo_integration.h
 * 归属：人员 A（感知与预处理）· 车道线模型（YOLOPv2）
 * 说明：封装 Rockchip NPU 上的 YOLOPv2，输出“车道线分割 + 可行驶区域”。
 *       不再直接画红线再回检，也不再写旧的弯道共享内存；改为从分割概率图
 *       直接计算 LaneResult（真实置信度 + 像素偏移），交给上游拼装。
 * 依赖：rknn_api.h（厂商 NPU SDK，禁止修改）、common/driving_types.h（契约）。
 * ======================================================================== */
#ifndef YOLO_INTEGRATION_H_A
#define YOLO_INTEGRATION_H_A

#include <stdint.h>
#include "rknn_api.h"
#include "driving_types.h"

/* YOLOPv2 输入尺寸（文档 3.③：640x480） */
#define YOLO_IN_W  640
#define YOLO_IN_H  480

#ifndef YOLO_MODEL_PATH
#define YOLO_MODEL_PATH  "model/yolopv2_Nx3x480x640_rk3568.rknn"
#endif

/* YOLOPv2 引擎：负责车道线 + 可行驶区域分割 */
typedef struct {
    rknn_context    ctx;
    rknn_input_output_num io_num;
    rknn_tensor_attr *in_attr;
    rknn_tensor_attr *out_attr;
    rknn_input     *inputs;
    rknn_output    *outputs;
    uint8_t        *input_buf;          /* 模型输入（NHWC UINT8）*/
    float          *lane_buf;           /* 车道线概率图 (1,1,H,W) */
    float          *drivable_buf;       /* 可行驶区域 (1,2,H,W) */
    int             w, h;
    int             ready;
} lane_engine_t;

typedef struct {
    const float *lane;        /* 车道线概率图，长度 w*h */
    const float *drivable;    /* 可行驶区域，长度 w*h*2 */
    int w, h;
} lane_seg_t;

/* ---- 引擎生命周期 ---- */
int  lane_engine_init(lane_engine_t *e, const char *model_path);
int  lane_engine_run(lane_engine_t *e,
                     const uint8_t *rgb888, int w, int h,
                     lane_seg_t *seg);
void lane_engine_free(lane_engine_t *e);

/* ---- 可视化（可选）：把车道线/可行驶区域叠加到 rgb888 上，仅用于显示 ---- */
void lane_overlay_draw(uint8_t *rgb888, int w, int h, const lane_seg_t *seg);

/* ---- 弯道判定（核心优化点） ----
 * 直接从车道线概率图计算 LaneResult：
 *   - curve_offset：远端车道线重心相对画面中心的“带符号像素偏移”（不再是 25°）
 *   - confidence ：由车道线覆盖率 + 远端集中度真实推算（不再硬编码 90/75/60/40）
 *   - direction  ：依据 curve_offset 的符号与阈值判断左/右/直/未知
 * 这实现了文档“与原实现差异”中要求的全部三项算法优化。 */
void lane_result_from_seg(const lane_seg_t *seg, LaneResult *out, uint32_t frame_id);

/* RGB565→RGB888（针对 PCIe 数据流，A 的预处理） */
void rgb565_to_rgb888_yolo(const uint16_t *src565, uint8_t *dst888, int w, int h);

#endif /* YOLO_INTEGRATION_H_A */
