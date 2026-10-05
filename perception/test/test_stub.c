/*
 * test_stub.c — 感知桩模式集成测试（【人员 A】自测，不依赖硬件/RKNN/共享内存）
 *
 * 覆盖：perception_init 桩模式初始化、perception_step 桩模式产出契约合法的
 * LaneResult（version/帧号自增/时间戳/直道方向），以及 deinit 正常回收。
 * 该测试验证 B/C 联调依赖的"桩兼容"签名在整条流水线上可用。
 */
#include "perception_api.h"
#include "pcie_capture.h"

#include <stdio.h>
#include <string.h>

static int g_fail = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { printf("  [FAIL] %s\n", msg); g_fail++; } \
    else { printf("  [ok]   %s\n", msg); } \
} while (0)

int main(void)
{
    printf("== test_stub ==\n");

    /* 1) 桩函数直接产出合法结果 */
    LaneResult r0;
    lane_stub_make_result(&r0, 7u, LANE_LEFT, 88u);
    CHECK(r0.version == LANERESULT_VERSION, "stub version set");
    CHECK(r0.frame_id == 7u, "stub frame_id set");
    CHECK(r0.direction == LANE_LEFT, "stub direction set");
    CHECK(r0.curve_offset < 0, "stub left offset negative");
    CHECK(r0.confidence == 88u, "stub confidence set");

    /* 2) 桩模式下整条流水线可跑（不碰硬件/模型） */
    perception_cfg_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.use_stub = 1;
    cfg.img_w = 640;
    cfg.img_h = 480;
    cfg.lead_px = 120;
    cfg.model_path = "dummy.rknn";   /* 桩模式不应触达该文件 */

    perception_ctx_t ctx;
    CHECK(perception_init(&ctx, &cfg) == 0, "perception_init stub");
    CHECK(ctx.real.grabber == NULL && ctx.real.model == NULL, "stub skips hw/model");

    LaneResult r1, r2;
    CHECK(perception_step(&ctx, &r1) == 0, "perception_step #1");
    CHECK(r1.version == LANERESULT_VERSION, "step #1 version");
    CHECK(r1.direction == LANE_STRAIGHT, "step #1 straight");
    CHECK(r1.frame_id == 0u, "step #1 frame_id starts at 0");
    CHECK(r1.timestamp_us > 0u, "step #1 timestamp set");

    CHECK(perception_step(&ctx, &r2) == 0, "perception_step #2");
    CHECK(r2.frame_id == 1u, "step #2 frame_id increments");

    perception_deinit(&ctx);
    CHECK(ctx.frame565 == NULL && ctx.rgb888 == NULL, "deinit frees buffers");
    CHECK(ctx.real.grabber == NULL && ctx.real.model == NULL, "deinit stub clean");

    printf(g_fail == 0 ? "test_stub: PASS\n" : "test_stub: FAIL (%d)\n", g_fail);
    return g_fail == 0 ? 0 : 1;
}
