/*
 * perception_pipeline.c — 感知流水线编排（【人员 A · 感知】）
 *
 * 把“采集→转换→分割→判定”串成 perception_step，对 B/C 隐藏内部细节。
 * 桩模式下不碰硬件，直接吐直道结果；真实模式下走完整 PCIe + RKNN 链路。
 */
#include "perception_api.h"
#include "pcie_capture.h"
#include "lane_detect.h"
#include "turn_decide.h"
#include "traffic_light.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if !defined(_WIN32) && !defined(_WIN64)
#include <sys/time.h>
#endif

/* 复位附加感知结果（未检出 / 桩模式时使用） */
static void reset_lane_marks(LaneMarkResult *m)
{
    memset(m, 0, sizeof(*m));
    m->version = LANEMARK_VERSION;
    m->left_x  = -1;
    m->right_x = -1;
}
static void reset_zebra(ZebraResult *z)
{
    memset(z, 0, sizeof(*z));
    z->version  = ZEBRA_VERSION;
    z->center_y = -1;
}
static void reset_tl(TrafficLightResult *t)
{
    memset(t, 0, sizeof(*t));
    t->version = TL_VERSION;
    t->box_x = -1; t->box_y = -1; t->box_w = -1; t->box_h = -1;
    t->state  = TL_UNKNOWN;
}

int perception_init(perception_ctx_t *ctx, const perception_cfg_t *cfg)
{
    if (!ctx || !cfg) return -1;
    memset(ctx, 0, sizeof(*ctx));
    ctx->cfg = *cfg;
    ctx->frame_id = 0;

    const int w = cfg->img_w > 0 ? cfg->img_w : CAP_IMG_W;
    const int h = cfg->img_h > 0 ? cfg->img_h : CAP_IMG_H;
    const int lead = cfg->lead_px > 0 ? cfg->lead_px : CAP_LEAD_PIX;
    ctx->cfg.img_w = w; ctx->cfg.img_h = h; ctx->cfg.lead_px = lead;

    ctx->frame565 = (uint8_t *)malloc((size_t)w * h * 2u);   /* 剥离前导后的干净帧 */
    ctx->rgb888 = (uint8_t *)malloc((size_t)w * h * 3u);
    if (!ctx->frame565 || !ctx->rgb888) { perception_deinit(ctx); return -1; }

    if (lane_seg_alloc(&ctx->seg, w, h) != 0) { perception_deinit(ctx); return -1; }

    if (!cfg->use_stub) {
        frame_grabber_t *g = (frame_grabber_t *)malloc(sizeof(frame_grabber_t));
        if (!g) { perception_deinit(ctx); return -1; }
        ctx->real.grabber = g;

        if (pcie_capture_open(g, w, h, lead) != 0) { perception_deinit(ctx); return -1; }
        if (pcie_capture_start(g) != 0)            { perception_deinit(ctx); return -1; }

        ctx->real.model = (lane_model_t *)malloc(sizeof(lane_model_t));
        if (!ctx->real.model) { perception_deinit(ctx); return -1; }
        if (lane_model_init(ctx->real.model, cfg->model_path) != 0) {
            perception_deinit(ctx); return -1;
        }
    }
    return 0;
}

int perception_step(perception_ctx_t *ctx, LaneResult *out)
{
    if (!ctx || !out) return -1;

    if (ctx->cfg.use_stub) {
        lane_stub_make_result(out, ctx->frame_id, LANE_STRAIGHT, 95u);
        reset_lane_marks(&ctx->lane_marks);   /* 桩模式无分割图 → 无虚实结果 */
        reset_zebra(&ctx->zebra);
        reset_tl(&ctx->tl);
    } else {
        frame_grabber_t *g = (frame_grabber_t *)ctx->real.grabber;
        if (!g || !ctx->real.model) return -1;

        if (pcie_capture_grab(g, ctx->frame565) != 0) return -1;
        rgb565_to_rgb888_pcie((const uint16_t *)ctx->frame565, ctx->rgb888,
                              (size_t)ctx->cfg.img_w * ctx->cfg.img_h);
        if (lane_model_run(ctx->real.model, ctx->rgb888,
                           ctx->cfg.img_w, ctx->cfg.img_h, &ctx->seg) != 0)
            return -1;
        if (turn_decide(&ctx->seg, ctx->cfg.img_w, ctx->cfg.img_h, out) != 0)
            return -1;

        /* 虚实线 / 变道判定（基于同一张车道线概率图；失败不致命） */
        if (lane_marks_analyze(&ctx->seg, ctx->cfg.img_w, ctx->cfg.img_h,
                               &ctx->lane_marks) != 0)
            reset_lane_marks(&ctx->lane_marks);

        /* 斑马线识别（需原始 RGB888；失败不致命） */
        if (zebra_detect(ctx->rgb888, ctx->cfg.img_w, ctx->cfg.img_h,
                         &ctx->zebra) != 0)
            reset_zebra(&ctx->zebra);

        /* 红绿灯识别（需原始 RGB888；失败不致命）—— A 新增感知项 */
        if (traffic_light_detect(ctx->rgb888, ctx->cfg.img_w, ctx->cfg.img_h,
                                 &ctx->tl) != 0)
            reset_tl(&ctx->tl);
    }

    /* 补全契约字段（frame_id 与 timestamp 取自同一帧序号，保持语义一致） */
    out->version  = LANERESULT_VERSION;
    out->frame_id = ctx->frame_id;

#if !defined(__linux__)
    /* 本机桩：用帧号推导一个非零时间戳（板端为真实 gettimeofday） */
    out->timestamp_us = (uint64_t)(ctx->frame_id + 1u) * 1000u;
#else
    struct timeval now;
    gettimeofday(&now, NULL);
    out->timestamp_us = (uint64_t)now.tv_sec * 1000000u + (uint64_t)now.tv_usec;
#endif

    /* 附加结果与主结果共享同一帧号/时间戳，便于 B 端按帧对齐 */
    ctx->lane_marks.frame_id     = ctx->frame_id;
    ctx->lane_marks.timestamp_us = out->timestamp_us;
    ctx->zebra.frame_id          = ctx->frame_id;
    ctx->zebra.timestamp_us      = out->timestamp_us;
    ctx->tl.frame_id             = ctx->frame_id;
    ctx->tl.timestamp_us         = out->timestamp_us;

    ctx->frame_id++;
    return 0;
}

void perception_deinit(perception_ctx_t *ctx)
{
    if (!ctx) return;
    if (!ctx->cfg.use_stub) {
        if (ctx->real.model)      { lane_model_release(ctx->real.model); free(ctx->real.model); ctx->real.model = NULL; }
        if (ctx->real.grabber) {
            frame_grabber_t *g = (frame_grabber_t *)ctx->real.grabber;
            pcie_capture_stop(g);
            pcie_capture_close(g);
            free(g); ctx->real.grabber = NULL;
        }
    }
    lane_seg_free(&ctx->seg);
    free(ctx->rgb888); ctx->rgb888 = NULL;
    free(ctx->frame565); ctx->frame565 = NULL;
}

const LaneMarkResult *perception_lane_marks(const perception_ctx_t *ctx)
{
    return ctx ? &ctx->lane_marks : NULL;
}

const ZebraResult *perception_zebra(const perception_ctx_t *ctx)
{
    return ctx ? &ctx->zebra : NULL;
}

const TrafficLightResult *perception_traffic_light(const perception_ctx_t *ctx)
{
    return ctx ? &ctx->tl : NULL;
}
