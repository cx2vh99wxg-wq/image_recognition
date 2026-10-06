/*
 * person_detect.c — S 端行人检测实现（【人员 B】）
 *
 * 桩/真双路径：
 *   - USE_STUB 或非 Linux：桩实现（无行人），本机可编译可单测；
 *   - Linux 板端：RKNN 推理 YOLOv5s（依赖 librknnrt + ../perception/include/rknn_api.h）。
 *
 * 注意：RKNN 真实现需按实际模型输出维度微调（下方输出通道数/网格数
 * 以常见 YOLOv5s 640x480 为例，上板以 rknn_query 实测为准）。
 */
#include "person_detect.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* 默认模型路径（相对运行目录；上板建议 --model 传绝对路径） */
#define PERSON_DEFAULT_MODEL "model/yolov5s_Nx3x480x640_rk3568.rknn"

struct person_detect_ctx {
    int  img_w;
    int  img_h;
    int  use_stub;
#if defined(__linux__) && !defined(USE_STUB)
    int   rknn_ctx;       /* rknn_context 实际为 int 句柄 */
    int   rknn_ready;
#endif
};

#if defined(__linux__) && !defined(USE_STUB)
/* ---------- 板端 RKNN 真实现 ---------- */
#include "rknn_api.h"

static int person_rknn_run(person_detect_ctx_t *ctx, const uint8_t *rgb888,
                           uint32_t frame_id, uint64_t now_us, PersonState *out)
{
    memset(out, 0, sizeof(*out));
    out->version   = PERSON_VERSION;
    out->frame_id  = frame_id;
    out->timestamp_us = now_us;
    out->box_x = out->box_y = out->box_w = out->box_h = -1;
    out->detected = 0;
    out->confidence = 0;

    rknn_context *rk = &ctx->rknn_ctx;
    if (!ctx->rknn_ready || !*rk) return -1;

    rknn_input inputs[1];
    memset(inputs, 0, sizeof(inputs));
    inputs[0].index = 0;
    inputs[0].type = RKNN_TENSOR_UINT8;
    inputs[0].size = (uint32_t)ctx->img_w * (uint32_t)ctx->img_h * 3u;
    inputs[0].fmt = RKNN_TENSOR_NHWC;
    inputs[0].buf = (void *)rgb888;
    if (rknn_inputs_set(*rk, 1, inputs) != 0) return -1;
    if (rknn_run(*rk, NULL) != 0) return -1;

    rknn_output outputs[1];
    memset(outputs, 0, sizeof(outputs));
    outputs[0].want_float = 1;
    if (rknn_outputs_get(*rk, 1, outputs, NULL) != 0) return -1;

    /* 简化解析：假定输出为 1x(5+N)xGxG（YOLOv5 头，N=类别数，class 0=person）。
     * 网格数 G 与通道数以 rknn_query(OUTPUT_ATTR) 实测为准，此处给框架，
     * 上板联调时按真实维度修正。 */
    const float *data = (const float *)outputs[0].buf;
    const size_t total = outputs[0].size / sizeof(float);

    float best_conf = 0.0f;
    int   best_idx = -1;
    for (size_t i = 0; i + 5 < total; i += 6) {   /* 5 头 + 1 类（person）示意 */
        const float obj = data[i + 4];
        const float cls = data[i + 5];
        if (cls < 0.5f) continue;                  /* 仅 person 类 */
        const float conf = obj * cls;
        if (conf > best_conf) { best_conf = conf; best_idx = (int)i; }
    }

    rknn_outputs_release(*rk, 1, outputs);

    if (best_idx >= 0 && best_conf > 0.25f) {
        /* 坐标在归一化输出上，换算回全图（示意，按模型实际尺度修正） */
        out->detected     = 1;
        out->person_count = 1;
        out->confidence   = (uint32_t)(best_conf * 100.0f + 0.5f);
        out->box_x        = 0;   /* TODO(板端): 按 rknn 输出 attr 反算框坐标 */
        out->box_y        = 0;
        out->box_w        = ctx->img_w;
        out->box_h        = ctx->img_h;
    }
    return 0;
}

static int person_rknn_init(person_detect_ctx_t *ctx, const char *model_path)
{
    rknn_context *rk = &ctx->rknn_ctx;
    *rk = 0;
    FILE *fp = fopen(model_path, "rb");
    if (!fp) return -1;
    fseek(fp, 0, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    void *model = malloc((size_t)sz);
    if (!model) { fclose(fp); return -1; }
    if (fread(model, 1, (size_t)sz, fp) != (size_t)sz) { free(model); fclose(fp); return -1; }
    fclose(fp);

    int ret = rknn_init(rk, model, (uint32_t)sz, 0, NULL);
    free(model);
    if (ret != 0) return -1;
    ctx->rknn_ready = 1;
    return 0;
}
#endif /* __linux__ && !USE_STUB */

int person_detect_init(person_detect_ctx_t **ctx, const char *model_path,
                       int img_w, int img_h)
{
    if (!ctx || img_w <= 0 || img_h <= 0) return -1;
    person_detect_ctx_t *c = (person_detect_ctx_t *)calloc(1, sizeof(*c));
    if (!c) return -1;
    c->img_w = img_w;
    c->img_h = img_h;
#if defined(__linux__) && !defined(USE_STUB)
    c->use_stub = 0;
    const char *mp = model_path ? model_path : PERSON_DEFAULT_MODEL;
    if (person_rknn_init(c, mp) != 0) {
        fprintf(stderr, "[PERSON] RKNN 初始化失败: %s（可改用 USE_STUB 联调）\n", mp);
        free(c);
        return -1;
    }
#else
    c->use_stub = 1;
    (void)model_path;
#endif
    *ctx = c;
    return 0;
}

int person_detect_run(person_detect_ctx_t *ctx, const uint8_t *rgb888,
                      uint32_t frame_id, uint64_t now_us, PersonState *out)
{
    if (!ctx || !rgb888 || !out) return -1;

#if defined(__linux__) && !defined(USE_STUB)
    if (!ctx->use_stub && ctx->rknn_ready)
        return person_rknn_run(ctx, rgb888, frame_id, now_us, out);
#endif

    /* 桩：无行人（安全默认值） */
    memset(out, 0, sizeof(*out));
    out->version   = PERSON_VERSION;
    out->frame_id  = frame_id;
    out->timestamp_us = now_us;
    out->box_x = out->box_y = out->box_w = out->box_h = -1;
    out->detected = 0;
    out->confidence = 0;
    return 0;
}

void person_detect_deinit(person_detect_ctx_t *ctx)
{
    if (!ctx) return;
#if defined(__linux__) && !defined(USE_STUB)
    rknn_context *rk = &ctx->rknn_ctx;
    if (ctx->rknn_ready && *rk) { rknn_destroy(*rk); *rk = 0; ctx->rknn_ready = 0; }
#endif
    free(ctx);
}
