/*
 * test_decision.c — 决策状态机单元测试（【人员 B】）
 *
 * 覆盖优先级链：行人紧急 > 红灯 > 弯道 > 直道 > 无数据/超龄。
 */
#include "decision.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

static int g_fail = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { fprintf(stderr, "FAIL: %s (line %d)\n", msg, __LINE__); g_fail++; } \
} while (0)

static LaneResult mk_lane(uint32_t fid, LaneDirection dir, uint32_t conf)
{
    LaneResult l;
    memset(&l, 0, sizeof(l));
    l.version = LANERESULT_VERSION;
    l.frame_id = fid;
    l.direction = dir;
    l.confidence = conf;
    return l;
}

static TrafficLightResult mk_tl(TrafficLightState s, uint32_t conf, uint32_t detected)
{
    TrafficLightResult t;
    memset(&t, 0, sizeof(t));
    t.version = TL_VERSION;
    t.state = s;
    t.confidence = conf;
    t.detected = detected;
    return t;
}

static PersonState mk_person(uint32_t detected, uint32_t conf)
{
    PersonState p;
    memset(&p, 0, sizeof(p));
    p.version = PERSON_VERSION;
    p.detected = detected;
    p.confidence = conf;
    return p;
}

static ControlCommandMsg run(decision_ctx_t *ctx,
                             const LaneResult *l, const TrafficLightResult *t,
                             const PersonState *p, uint64_t now)
{
    ControlCommandMsg out;
    memset(&out, 0, sizeof(out));
    int rc = decision_step(ctx, l, t, NULL, NULL, p, now, &out);
    if (rc != 0) { out.command = (ControlCommand)0xEE; out.enable = 0; }
    return out;
}

int main(void)
{
    decision_ctx_t ctx;
    decision_init(&ctx);
    uint64_t t0 = 1000000000ull;   /* 基时刻 */

    /* 1. 无任何输入 → CMD_NONE */
    ControlCommandMsg out = run(&ctx, NULL, NULL, NULL, t0);
    CHECK(out.command == CMD_NONE, "no input -> NONE");
    CHECK(out.enable == 0, "no input -> disable");

    /* 2. 直道高置信 → GO */
    LaneResult l = mk_lane(1, LANE_STRAIGHT, 95);
    out = run(&ctx, &l, NULL, NULL, t0 + 1000);
    CHECK(out.command == CMD_GO, "straight high -> GO");
    CHECK(out.enable == 1, "go enable");
    CHECK(out.priority == CMD_PRI_NORMAL, "go pri");

    /* 3. 直道低置信 → NONE（不冒进） */
    l = mk_lane(2, LANE_STRAIGHT, 20);
    out = run(&ctx, &l, NULL, NULL, t0 + 2000);
    CHECK(out.command == CMD_NONE, "straight low -> NONE");

    /* 4. 左弯 → LEFT */
    l = mk_lane(3, LANE_LEFT, 90);
    out = run(&ctx, &l, NULL, NULL, t0 + 3000);
    CHECK(out.command == CMD_LEFT, "left -> LEFT");

    /* 5. 右弯 → RIGHT */
    l = mk_lane(4, LANE_RIGHT, 90);
    out = run(&ctx, &l, NULL, NULL, t0 + 4000);
    CHECK(out.command == CMD_RIGHT, "right -> RIGHT");

    /* 6. 未知 → NONE */
    l = mk_lane(5, LANE_UNKNOWN, 90);
    out = run(&ctx, &l, NULL, NULL, t0 + 5000);
    CHECK(out.command == CMD_NONE, "unknown -> NONE");

    /* 7. 红灯 → STOP pri=1（压过弯道） */
    l = mk_lane(6, LANE_RIGHT, 90);
    TrafficLightResult tl = mk_tl(TL_RED, 80, 1);
    out = run(&ctx, &l, &tl, NULL, t0 + 6000);
    CHECK(out.command == CMD_STOP, "red -> STOP");
    CHECK(out.priority == CMD_PRI_STOP, "red pri=1");

    /* 8. 绿灯不阻断行驶 */
    tl = mk_tl(TL_GREEN, 80, 1);
    out = run(&ctx, &l, &tl, NULL, t0 + 7000);
    CHECK(out.command == CMD_RIGHT, "green -> keep RIGHT");

    /* 9. 行人紧急 → STOP pri=2（压过红灯） */
    tl = mk_tl(TL_RED, 80, 1);
    PersonState p = mk_person(1, 90);
    out = run(&ctx, &l, &tl, &p, t0 + 8000);
    CHECK(out.command == CMD_STOP, "person -> STOP");
    CHECK(out.priority == CMD_PRI_EMERGENCY, "person pri=2");

    /* 9b. 行人离开 + 绿灯 → 恢复按车道行驶（新帧 detected=0 覆盖行人缓存） */
    PersonState p_clear = mk_person(0, 0);
    tl = mk_tl(TL_GREEN, 80, 1);
    out = run(&ctx, &l, &tl, &p_clear, t0 + 8100);
    CHECK(out.command == CMD_RIGHT, "person clear + green -> RIGHT");

    /* 10. 超龄降级：车道结果 500ms 后失效 → NONE */
    l = mk_lane(7, LANE_STRAIGHT, 95);
    out = run(&ctx, &l, NULL, NULL, t0 + 9000);
    CHECK(out.command == CMD_GO, "fresh lane -> GO");
    out = run(&ctx, NULL, NULL, NULL, t0 + 9000 + (uint64_t)DEC_LANE_MAX_AGE_MS * 1000u + 1);
    CHECK(out.command == CMD_NONE, "aged lane -> NONE");

    /* 11. 版本不匹配输入被忽略（不覆盖缓存；此时车道已超龄 → NONE） */
    LaneResult bad = mk_lane(8, LANE_LEFT, 90);
    bad.version = 0xFF;
    out = run(&ctx, &bad, NULL, NULL,
              t0 + 9000 + (uint64_t)DEC_LANE_MAX_AGE_MS * 1000u + 2);
    CHECK(out.command == CMD_NONE, "bad version ignored");

    if (g_fail == 0) {
        printf("test_decision: ALL PASS\n");
        return 0;
    }
    printf("test_decision: %d FAILED\n", g_fail);
    return 1;
}
