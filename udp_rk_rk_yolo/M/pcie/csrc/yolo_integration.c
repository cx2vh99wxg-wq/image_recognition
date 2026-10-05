/* ===========================================================================
 * 文件：yolo_integration.c
 * 归属：人员 A（感知与预处理）· 车道线模型（YOLOPv2）
 * 说明：YOLOPv2 引擎实现 + 弯道判定。
 *       相比原实现的三处关键改动（见分工方案“与原实现差异”）：
 *         1) confidence 不再硬编码 90/75/60/40，而是由车道线覆盖率 + 远端集中度真实推算；
 *         2) 不再输出 25° 这种无物理意义的角度，改为 curve_offset（带符号像素偏移）；
 *         3) 不再调用旧的 curve_detection_update() 写弯道共享内存，改为填充 LaneResult，
 *            由 main 经 B 的 shm_write_lane() 写入 key=0x1234567E。
 *       前导像素已在采集端剥离（pcie_capture_grab），此处拿到的是干净的 640x480。
 * ======================================================================== */
#include "yolo_integration.h"
#include "preprocess.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

/* ---------------------------------------------------------------------------
 * 引擎初始化：加载 RKNN 模型，查询 IO，分配输入/输出/概率图缓冲
 * ------------------------------------------------------------------------- */
int lane_engine_init(lane_engine_t *e, const char *model_path)
{
    if (!e || !model_path) {
        fprintf(stderr, "[LANE] 参数无效\n");
        return -1;
    }
    memset(e, 0, sizeof(*e));

    FILE *fp = fopen(model_path, "rb");
    if (!fp) {
        fprintf(stderr, "[LANE] 无法打开模型: %s\n", model_path);
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    size_t sz = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    void *blob = malloc(sz);
    if (!blob || fread(blob, 1, sz, fp) != sz) {
        fprintf(stderr, "[LANE] 读取模型失败\n");
        free(blob); fclose(fp);
        return -1;
    }
    fclose(fp);

    int ret = rknn_init(&e->ctx, blob, sz, 0, NULL);
    free(blob);
    if (ret < 0) {
        fprintf(stderr, "[LANE] rknn_init 失败: %d\n", ret);
        return -1;
    }

    ret = rknn_query(e->ctx, RKNN_QUERY_IN_OUT_NUM, &e->io_num, sizeof(e->io_num));
    if (ret < 0) {
        fprintf(stderr, "[LANE] 查询 IO 数量失败\n");
        rknn_destroy(e->ctx); e->ctx = 0;
        return -1;
    }

    e->in_attr  = malloc(e->io_num.n_input  * sizeof(rknn_tensor_attr));
    e->out_attr = malloc(e->io_num.n_output * sizeof(rknn_tensor_attr));
    e->inputs   = malloc(e->io_num.n_input  * sizeof(rknn_input));
    e->outputs  = malloc(e->io_num.n_output * sizeof(rknn_output));
    if (!e->in_attr || !e->out_attr || !e->inputs || !e->outputs) {
        lane_engine_free(e);
        return -1;
    }
    memset(e->in_attr, 0,  e->io_num.n_input  * sizeof(rknn_tensor_attr));
    memset(e->out_attr, 0, e->io_num.n_output * sizeof(rknn_tensor_attr));
    memset(e->inputs, 0,   e->io_num.n_input  * sizeof(rknn_input));
    memset(e->outputs, 0,  e->io_num.n_output * sizeof(rknn_output));

    for (uint32_t i = 0; i < e->io_num.n_input; i++) {
        e->in_attr[i].index = i;
        rknn_query(e->ctx, RKNN_QUERY_INPUT_ATTR, &e->in_attr[i], sizeof(rknn_tensor_attr));
    }
    for (uint32_t i = 0; i < e->io_num.n_output; i++) {
        e->out_attr[i].index = i;
        rknn_query(e->ctx, RKNN_QUERY_OUTPUT_ATTR, &e->out_attr[i], sizeof(rknn_tensor_attr));
    }

    e->w = YOLO_IN_W;
    e->h = YOLO_IN_H;
    e->input_buf    = malloc((size_t)e->w * e->h * 3);
    e->lane_buf     = malloc((size_t)e->w * e->h       * sizeof(float));
    e->drivable_buf = malloc((size_t)e->w * e->h * 2   * sizeof(float));
    if (!e->input_buf || !e->lane_buf || !e->drivable_buf) {
        lane_engine_free(e);
        return -1;
    }
    e->ready = 1;
    printf("[LANE] YOLOPv2 引擎初始化成功 (%dx%d)\n", e->w, e->h);
    return 0;
}

/* ---------------------------------------------------------------------------
 * 引擎推理：把 rgb888 送入模型，取出车道线/可行驶区域概率图
 * ------------------------------------------------------------------------- */
int lane_engine_run(lane_engine_t *e,
                    const uint8_t *rgb888, int w, int h,
                    lane_seg_t *seg)
{
    if (!e || !e->ready || !rgb888 || !seg) return -1;

    /* 模型输入固定 640x480；尺寸一致直接拷贝，否则做 letterbox */
    if (w == e->w && h == e->h) {
        memcpy(e->input_buf, rgb888, (size_t)w * h * 3);
    } else {
        letterbox_t box;
        perception_letterbox(rgb888, w, h, e->input_buf, e->w, e->h, &box);
    }

    e->inputs[0].index = 0;
    e->inputs[0].type  = RKNN_TENSOR_UINT8;
    e->inputs[0].size  = (size_t)e->w * e->h * 3;
    e->inputs[0].fmt   = RKNN_TENSOR_NHWC;
    e->inputs[0].buf   = e->input_buf;
    for (uint32_t i = 0; i < e->io_num.n_output; i++)
        e->outputs[i].want_float = 1;

    if (rknn_inputs_set(e->ctx, e->io_num.n_input, e->inputs) < 0) return -1;
    if (rknn_run(e->ctx, NULL) < 0) return -1;
    if (rknn_outputs_get(e->ctx, e->io_num.n_output, e->outputs, NULL) < 0) return -1;

    /* 输出顺序（与原实现一致）：[0]=可行驶区域(1,2,H,W)，[1]=车道线(1,1,H,W) */
    size_t drivable_bytes = (size_t)e->w * e->h * 2 * sizeof(float);
    size_t lane_bytes     = (size_t)e->w * e->h     * sizeof(float);
    if (e->io_num.n_output >= 2 && e->outputs[0].buf && e->outputs[1].buf) {
        size_t dcopy = (e->outputs[0].size < drivable_bytes) ? e->outputs[0].size : drivable_bytes;
        size_t lcopy = (e->outputs[1].size < lane_bytes)     ? e->outputs[1].size : lane_bytes;
        memcpy(e->drivable_buf, e->outputs[0].buf, dcopy);
        memcpy(e->lane_buf,     e->outputs[1].buf, lcopy);
    }
    rknn_outputs_release(e->ctx, e->io_num.n_output, e->outputs);

    seg->lane     = e->lane_buf;
    seg->drivable = e->drivable_buf;
    seg->w = e->w; seg->h = e->h;
    return 0;
}

/* ---------------------------------------------------------------------------
 * 引擎释放
 * ------------------------------------------------------------------------- */
void lane_engine_free(lane_engine_t *e)
{
    if (!e) return;
    free(e->lane_buf);     e->lane_buf = NULL;
    free(e->drivable_buf); e->drivable_buf = NULL;
    free(e->input_buf);    e->input_buf = NULL;
    free(e->in_attr);      e->in_attr = NULL;
    free(e->out_attr);     e->out_attr = NULL;
    free(e->inputs);       e->inputs = NULL;
    free(e->outputs);      e->outputs = NULL;
    if (e->ctx > 0) rknn_destroy(e->ctx);
    e->ctx = 0; e->ready = 0;
}

/* ---------------------------------------------------------------------------
 * 可视化（仅用于显示，不影响判定）：叠加可行驶区域(绿) + 车道线(红)
 * ------------------------------------------------------------------------- */
void lane_overlay_draw(uint8_t *rgb888, int w, int h, const lane_seg_t *seg)
{
    if (!rgb888 || !seg || !seg->lane || !seg->drivable) return;
    int area = seg->w * seg->h;
    for (int y = 0; y < h; y++) {
        int sy = (int)((float)y / h * seg->h); if (sy >= seg->h) sy = seg->h - 1;
        for (int x = 0; x < w; x++) {
            int sx = (int)((float)x / w * seg->w); if (sx >= seg->w) sx = seg->w - 1;
            int si = sy * seg->w + sx;
            /* 可行驶区域：通道1 > 通道0 → 绿色半透明 */
            if (seg->drivable[area + si] > seg->drivable[si]) {
                int di = (y * w + x) * 3;
                rgb888[di + 0] = (uint8_t)(rgb888[di + 0] * 0.6f);
                rgb888[di + 1] = (uint8_t)(rgb888[di + 1] * 0.6f + 255 * 0.4f);
                rgb888[di + 2] = (uint8_t)(rgb888[di + 2] * 0.6f);
            }
            /* 车道线：概率 > 0.3 → 纯红 */
            if (seg->lane[si] > 0.3f) {
                int di = (y * w + x) * 3;
                rgb888[di + 0] = 0; rgb888[di + 1] = 0; rgb888[di + 2] = 255;
            }
        }
    }
}

/* ---------------------------------------------------------------------------
 * 弯道判定（核心优化）：从车道线概率图直接算 LaneResult
 *   - 把画面按高度均分为若干水平带，逐带求车道线重心 x 与概率质量
 *   - curve_offset = 远端带重心_x − 近端带重心_x（带符号像素偏移，正值=路向右弯）
 *   - confidence  = 覆盖率(0.6权重) + 远端带质量占比(0.4权重)，真实推算，封顶100
 *   - direction   = 依据 |curve_offset| 阈值判断左/右/直；覆盖率过低判 UNKNOWN
 * ------------------------------------------------------------------------- */
void lane_result_from_seg(const lane_seg_t *seg, LaneResult *out, uint32_t frame_id)
{
    if (!out) return;
    memset(out, 0, sizeof(*out));
    out->version  = LANERESULT_VERSION;
    out->frame_id = frame_id;

    struct timeval now;
    gettimeofday(&now, NULL);
    out->timestamp_us = (uint64_t)now.tv_sec * 1000000u + (uint64_t)now.tv_usec;

    if (!seg || !seg->lane || seg->w <= 0 || seg->h <= 0) {
        out->direction = LANE_UNKNOWN;
        out->confidence = 0;
        out->curve_offset = 0;
        out->lane_pixel_cnt = 0;
        return;
    }

    const int W = seg->w, H = seg->h;
    const float *lane = seg->lane;
    const float TH = 0.3f;

    /* 水平带数量（远处多采样，权重更大） */
    const int BANDS = 8;
    const int band_h = H / BANDS;
    long band_mass[BANDS];
    double band_cx[BANDS];     /* 带内车道线重心 x（概率加权） */
    long total_lane = 0;

    for (int b = 0; b < BANDS; b++) {
        band_mass[b] = 0;
        band_cx[b]   = 0.0;
        int y0 = b * band_h, y1 = (b == BANDS - 1) ? H : (b + 1) * band_h;
        for (int y = y0; y < y1; y++) {
            for (int x = 0; x < W; x++) {
                float p = lane[y * W + x];
                if (p > TH) {
                    long wgt = (long)(p * 100.0f);
                    band_mass[b] += wgt;
                    band_cx[b]   += (double)x * wgt;
                    total_lane   += wgt;
                }
            }
        }
        if (band_mass[b] > 0)
            band_cx[b] /= (double)band_mass[b];
        else
            band_cx[b] = -1.0;   /* 该带无效 */
    }

    out->lane_pixel_cnt = (uint16_t)(total_lane / 100);   /* 近似像素数 */

    /* 覆盖率：车道线像素 / 全图像素（用阈值过后的存在性近似） */
    long exist = 0;
    for (int i = 0; i < W * H; i++) if (lane[i] > TH) exist++;
    float coverage = (float)exist / (float)(W * H);

    /* 远端质量占比：最上 3 带 / 全图（远端有车道线 → 判定更可信） */
    long far_mass = band_mass[0] + band_mass[1] + band_mass[2];
    float far_ratio = (total_lane > 0) ? (float)far_mass / (float)total_lane : 0.0f;

    /* 真实置信度：覆盖率为主、远端质量为辅 */
    int conf = (int)(coverage * 600.0f + far_ratio * 40.0f);
    if (conf > 100) conf = 100;
    if (conf < 0)   conf = 0;
    out->confidence = (uint8_t)conf;

    /* 拿远/近有效带重心算偏移 */
    int far_cx = -1, near_cx = -1;
    for (int b = 0; b < BANDS; b++) if (band_cx[b] >= 0) { far_cx = (int)band_cx[b]; break; }
    for (int b = BANDS - 1; b >= 0; b--) if (band_cx[b] >= 0) { near_cx = (int)band_cx[b]; break; }

    if (far_cx < 0 || near_cx < 0 || coverage < 0.01f) {
        /* 车道线太弱或缺失 → 未知 */
        out->direction = LANE_UNKNOWN;
        out->curve_offset = 0;
        return;
    }

    /* 带符号像素偏移：远端重心相对近端重心的横向位移。
     * 远端重心偏右 → 路向右弯 → 正值（与下游“右转”语义一致）。 */
    int offset = far_cx - near_cx;
    out->curve_offset = (int16_t)offset;

    const int STRAIGHT_TH = 8;   /* 像素阈值：小于此视为直道 */
    if (offset >  STRAIGHT_TH) out->direction = LANE_RIGHT;
    else if (offset < -STRAIGHT_TH) out->direction = LANE_LEFT;
    else out->direction = LANE_STRAIGHT;
}

/* ---------------------------------------------------------------------------
 * RGB565→RGB888（PCIe 数据流专用，A 的预处理）
 * ------------------------------------------------------------------------- */
void rgb565_to_rgb888_yolo(const uint16_t *src565, uint8_t *dst888, int w, int h)
{
    if (!src565 || !dst888) return;
    int n = w * h;
    for (int i = 0; i < n; i++) {
        uint16_t p = src565[i];
        uint8_t r = (uint8_t)((p >> 11) & 0x1F);
        uint8_t g = (uint8_t)((p >> 5)  & 0x3F);
        uint8_t b = (uint8_t)( p        & 0x1F);
        dst888[i * 3 + 0] = (uint8_t)((r << 3) | (r >> 2));
        dst888[i * 3 + 1] = (uint8_t)((g << 2) | (g >> 4));
        dst888[i * 3 + 2] = (uint8_t)((b << 3) | (b >> 2));
    }
}
