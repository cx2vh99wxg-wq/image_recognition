/* ===========================================================================
 * 文件：yolop_opencv_integration.cpp
 * 归属：人员 A（感知与预处理）· YOLOP 多任务分割（C++ 实现）
 * 说明：YOLOP 引擎实现。相比旧版：
 *   - 删除行人检测分支与 5x7 点阵字体（属 B 的显示/行人检测）；
 *   - 删除硬编码 25° 角度，改由 extractLaneResult() 输出带符号像素偏移；
 *   - 置信度由分割覆盖率真实推算，不再写旧弯道共享内存。
 * ======================================================================== */
#include "yolop_opencv_integration.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <fstream>
#include <sys/time.h>

#define YOLO_IN_W 640
#define YOLO_IN_H 480
#define LANE_TH   0.3f

YOLOPEngine::YOLOPEngine(const std::string& model_path)
    : ctx(0), in_attr(nullptr), out_attr(nullptr),
      inpW(YOLO_IN_W), inpH(YOLO_IN_H), segW(0), segH(0)
{
    if (!loadModel(model_path)) {
        fprintf(stderr, "[YOLOP] 模型初始化失败: %s\n", model_path.c_str());
    } else {
        printf("[YOLOP] 引擎初始化成功\n");
    }
}

YOLOPEngine::~YOLOPEngine()
{
    delete[] in_attr;  in_attr = nullptr;
    delete[] out_attr; out_attr = nullptr;
    if (ctx) { rknn_destroy(ctx); ctx = 0; }
}

bool YOLOPEngine::loadModel(const std::string& path)
{
    FILE* fp = fopen(path.c_str(), "rb");
    if (!fp) { fprintf(stderr, "[YOLOP] 无法打开模型\n"); return false; }
    fseek(fp, 0, SEEK_END);
    size_t sz = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    void* blob = malloc(sz);
    if (!blob || fread(blob, 1, sz, fp) != sz) { free(blob); fclose(fp); return false; }
    fclose(fp);

    int ret = rknn_init(&ctx, blob, sz, 0, nullptr);
    free(blob);
    if (ret != RKNN_SUCC) { fprintf(stderr, "[YOLOP] rknn_init 失败 %d\n", ret); return false; }

    ret = rknn_query(ctx, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num));
    if (ret != RKNN_SUCC) { rknn_destroy(ctx); ctx = 0; return false; }

    in_attr  = new rknn_tensor_attr[io_num.n_input];
    out_attr = new rknn_tensor_attr[io_num.n_output];
    memset(in_attr, 0, sizeof(rknn_tensor_attr) * io_num.n_input);
    memset(out_attr, 0, sizeof(rknn_tensor_attr) * io_num.n_output);
    for (uint32_t i = 0; i < io_num.n_input; i++) {
        in_attr[i].index = i;
        rknn_query(ctx, RKNN_QUERY_INPUT_ATTR, &in_attr[i], sizeof(rknn_tensor_attr));
    }
    for (uint32_t i = 0; i < io_num.n_output; i++) {
        out_attr[i].index = i;
        rknn_query(ctx, RKNN_QUERY_OUTPUT_ATTR, &out_attr[i], sizeof(rknn_tensor_attr));
    }
    return true;
}

void YOLOPEngine::preprocess(const uint8_t* rgb, int w, int h)
{
    input_buf.resize((size_t)inpW * inpH * 3);
    if (w == inpW && h == inpH) {
        memcpy(input_buf.data(), rgb, input_buf.size());
    } else {
        float scale = (inpW / (float)w < inpH / (float)h) ? inpW / (float)w : inpH / (float)h;
        for (int y = 0; y < inpH; y++) {
            int sy = (int)(y / scale); if (sy >= h) sy = h - 1;
            for (int x = 0; x < inpW; x++) {
                int sx = (int)(x / scale); if (sx >= w) sx = w - 1;
                const uint8_t* s = rgb + (sy * w + sx) * 3;
                uint8_t* d = input_buf.data() + (y * inpW + x) * 3;
                d[0] = s[0]; d[1] = s[1]; d[2] = s[2];
            }
        }
    }
}

int YOLOPEngine::inference(const uint8_t* rgb, int w, int h)
{
    if (!rgb || !isValid()) return -1;
    preprocess(rgb, w, h);

    rknn_input inputs[1];
    memset(inputs, 0, sizeof(inputs));
    inputs[0].index = 0;
    inputs[0].type  = RKNN_TENSOR_UINT8;
    inputs[0].size  = input_buf.size();
    inputs[0].fmt   = RKNN_TENSOR_NHWC;
    inputs[0].buf   = input_buf.data();
    if (rknn_inputs_set(ctx, io_num.n_input, inputs) != RKNN_SUCC) return -1;
    if (rknn_run(ctx, nullptr) != RKNN_SUCC) return -1;

    rknn_output* outs = new rknn_output[io_num.n_output];
    memset(outs, 0, sizeof(rknn_output) * io_num.n_output);
    for (uint32_t i = 0; i < io_num.n_output; i++) outs[i].want_float = 1;
    if (rknn_outputs_get(ctx, io_num.n_output, outs, nullptr) != RKNN_SUCC) {
        delete[] outs; return -1;
    }

    /* 输出顺序：[0]=可行驶区域(1,2,H,W)，[1]=车道线(1,1,H,W) */
    segW = inpW; segH = inpH;
    if (io_num.n_output >= 2 && outs[0].buf && outs[1].buf) {
        size_t dsize = (size_t)segW * segH * 2;
        size_t lsize = (size_t)segW * segH;
        float* d = (float*)outs[0].buf;
        float* l = (float*)outs[1].buf;
        drivable_seg.assign(d, d + dsize);
        lane_seg.assign(l, l + lsize);
    }
    rknn_outputs_release(ctx, io_num.n_output, outs);
    delete[] outs;
    return 0;
}

void YOLOPEngine::drawSegmentation(uint8_t* rgb, int w, int h) const
{
    if (!rgb || lane_seg.empty() || drivable_seg.empty()) return;
    int area = segW * segH;
    for (int y = 0; y < h; y++) {
        int sy = (int)((float)y / h * segH); if (sy >= segH) sy = segH - 1;
        for (int x = 0; x < w; x++) {
            int sx = (int)((float)x / w * segW); if (sx >= segW) sx = segW - 1;
            int si = sy * segW + sx;
            if (drivable_seg[area + si] > drivable_seg[si]) {
                int di = (y * w + x) * 3;
                rgb[di + 0] = (uint8_t)(rgb[di + 0] * 0.6f);
                rgb[di + 1] = (uint8_t)(rgb[di + 1] * 0.6f + 255 * 0.4f);
                rgb[di + 2] = (uint8_t)(rgb[di + 2] * 0.6f);
            }
            if (lane_seg[si] > LANE_TH) {
                int di = (y * w + x) * 3;
                rgb[di + 0] = 0; rgb[di + 1] = 0; rgb[di + 2] = 255;
            }
        }
    }
}

void YOLOPEngine::extractLaneResult(LaneResult* out, uint32_t frame_id) const
{
    if (!out) return;
    memset(out, 0, sizeof(*out));
    out->version = LANERESULT_VERSION;
    out->frame_id = frame_id;
    struct timeval tv; gettimeofday(&tv, nullptr);
    out->timestamp_us = (uint64_t)tv.tv_sec * 1000000u + (uint64_t)tv.tv_usec;

    if (lane_seg.empty() || segW <= 0 || segH <= 0) {
        out->direction = LANE_UNKNOWN;
        return;
    }

    const int W = segW, H = segH;
    const int BANDS = 8;
    const int band_h = H / BANDS;
    long band_mass[BANDS]; double band_cx[BANDS]; long total = 0; long exist = 0;

    for (int b = 0; b < BANDS; b++) {
        band_mass[b] = 0; band_cx[b] = 0.0;
        int y0 = b * band_h, y1 = (b == BANDS - 1) ? H : (b + 1) * band_h;
        for (int y = y0; y < y1; y++)
            for (int x = 0; x < W; x++) {
                float p = lane_seg[(size_t)y * W + x];
                if (p > LANE_TH) {
                    long wgt = (long)(p * 100.0f);
                    band_mass[b] += wgt; band_cx[b] += (double)x * wgt; total += wgt; exist++;
                }
            }
        if (band_mass[b] > 0) band_cx[b] /= (double)band_mass[b]; else band_cx[b] = -1.0;
    }

    out->lane_pixel_cnt = (uint16_t)exist;
    float coverage = (float)exist / (float)(W * H);
    long far_mass = band_mass[0] + band_mass[1] + band_mass[2];
    float far_ratio = (total > 0) ? (float)far_mass / (float)total : 0.0f;

    int conf = (int)(coverage * 600.0f + far_ratio * 40.0f);
    if (conf > 100) conf = 100;
    if (conf < 0)   conf = 0;
    out->confidence = (uint8_t)conf;

    int far_cx = -1, near_cx = -1;
    for (int b = 0; b < BANDS; b++) if (band_cx[b] >= 0) { far_cx = (int)band_cx[b]; break; }
    for (int b = BANDS - 1; b >= 0; b--) if (band_cx[b] >= 0) { near_cx = (int)band_cx[b]; break; }

    if (far_cx < 0 || near_cx < 0 || coverage < 0.01f) {
        out->direction = LANE_UNKNOWN; out->curve_offset = 0; return;
    }
    int offset = far_cx - near_cx;            /* 正值=路向右弯 */
    out->curve_offset = (int16_t)offset;
    const int STRAIGHT_TH = 8;
    if (offset >  STRAIGHT_TH) out->direction = LANE_RIGHT;
    else if (offset < -STRAIGHT_TH) out->direction = LANE_LEFT;
    else out->direction = LANE_STRAIGHT;
}
