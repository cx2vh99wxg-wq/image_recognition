/*
 * test_lane.c — 车道分割纯逻辑单元测试（【人员 A】自测，不依赖 RKNN）
 *
 * 覆盖：lane_seg_alloc/free、lane_model_extract 的节点语义映射。
 */
#include "lane_detect.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_fail = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { printf("  [FAIL] %s\n", msg); g_fail++; } \
    else { printf("  [ok]   %s\n", msg); } \
} while (0)

int main(void)
{
    printf("== test_lane ==\n");
    const int w = 64, h = 48;

    /* 1) 容器分配/释放 */
    lane_seg_t seg;
    CHECK(lane_seg_alloc(&seg, w, h) == 0, "lane_seg_alloc");
    CHECK(seg.lane_prob != NULL && seg.drivable != NULL, "buffers allocated");
    CHECK(seg.w == w && seg.h == h, "dimensions stored");
    lane_seg_free(&seg);
    CHECK(seg.lane_prob == NULL && seg.drivable == NULL, "buffers freed");

    /* 2) 节点映射：drivable_raw(1,2,H,W) 与 lane_raw(1,1,H,W) 正确拷贝 */
    float *drive_raw = (float *)malloc((size_t)w * h * 2 * sizeof(float));
    float *lane_raw  = (float *)malloc((size_t)w * h * sizeof(float));
    for (int i = 0; i < w*h*2; i++) drive_raw[i] = (float)(i % 7) * 0.1f;
    for (int i = 0; i < w*h; i++)   lane_raw[i]  = (float)(i % 5) * 0.2f;

    lane_seg_t s2;
    memset(&s2, 0, sizeof(s2));
    CHECK(lane_model_extract(drive_raw, lane_raw, w, h, &s2) == 0, "lane_model_extract");
    CHECK(s2.w == w && s2.h == h, "extracted dims");
    /* 抽样验证：lane 通道逐点一致 */
    int ok = 1;
    for (int i = 0; i < w*h; i++) if (s2.lane_prob[i] != lane_raw[i]) { ok = 0; break; }
    CHECK(ok, "lane_prob copied faithfully");
    /* drivable 通道整段一致 */
    ok = 1;
    for (int i = 0; i < w*h*2; i++) if (s2.drivable[i] != drive_raw[i]) { ok = 0; break; }
    CHECK(ok, "drivable copied faithfully");

    /* 3) 二次提取到同一容器（尺寸不变）应复用而非重分配 */
    float *old_lane = s2.lane_prob;
    CHECK(lane_model_extract(drive_raw, lane_raw, w, h, &s2) == 0, "re-extract same size");
    CHECK(s2.lane_prob == old_lane, "reuses existing buffer when size matches");

    lane_seg_free(&s2);
    free(drive_raw); free(lane_raw);

    printf(g_fail == 0 ? "test_lane: PASS\n" : "test_lane: FAIL (%d)\n", g_fail);
    return g_fail == 0 ? 0 : 1;
}
