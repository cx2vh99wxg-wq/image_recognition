/*
 * decision.c — 决策融合状态机实现（【人员 B】）
 *
 * 纯逻辑模块，不依赖系统调用，可在本机单元测试（test/test_decision.c）。
 */
#include "decision.h"

#include <string.h>

int decision_init(decision_ctx_t *ctx)
{
    if (!ctx) return -1;
    memset(ctx, 0, sizeof(*ctx));
    return 0;
}

static int age_ok(uint64_t last_us, uint64_t now_us, uint32_t max_age_ms)
{
    if (last_us == 0) return 0;                        /* 从未收到 */
    if (now_us <= last_us) return 1;                   /* 时钟异常时保守认为新鲜 */
    return (now_us - last_us) <= ((uint64_t)max_age_ms * 1000u);
}

static void cmd_reset(ControlCommandMsg *out, uint32_t seq)
{
    memset(out, 0, sizeof(*out));
    out->version  = CMDMSG_VERSION;
    out->frame_id = seq;
    out->command  = CMD_NONE;
    out->enable   = 0;
    out->priority = CMD_PRI_NORMAL;
    out->confidence = 0;
}

int decision_step(decision_ctx_t *ctx,
                  const LaneResult        *lane,
                  const TrafficLightResult *tl,
                  const LaneMarkResult     *lm,
                  const ZebraResult        *zebra,
                  const PersonState        *person,
                  uint64_t now_us,
                  ControlCommandMsg *out)
{
    (void)lm; (void)zebra;   /* v1 决策暂不消费虚实线/斑马线，接口预留 */
    if (!ctx || !out) return -1;

    ctx->out_seq++;
    cmd_reset(out, ctx->out_seq);

    /* 契约校验 + 缓存更新：版本匹配的输入才缓存，并刷新到达时刻。
     * 版本不匹配的输入按 NULL 忽略（不参与决策、不污染缓存）。 */
    if (lane   && lane->version   == LANERESULT_VERSION) {
        ctx->last_lane     = *lane;
        ctx->last_lane_us  = now_us;
    }
    if (tl     && tl->version     == TL_VERSION) {
        ctx->last_tl       = *tl;
        ctx->last_tl_us    = now_us;
    }
    if (person && person->version == PERSON_VERSION) {
        ctx->last_person       = *person;
        ctx->last_person_us    = now_us;
    }

    /* 基于“缓存是否超龄”判定新鲜度（不再依赖本次是否传入了非 NULL） */
    const int lane_ok   = age_ok(ctx->last_lane_us,   now_us, DEC_LANE_MAX_AGE_MS);
    const int tl_ok     = age_ok(ctx->last_tl_us,     now_us, DEC_TL_MAX_AGE_MS);
    const int person_ok = age_ok(ctx->last_person_us, now_us, DEC_PERSON_MAX_AGE_MS);

    /* ---- 优先级 1：行人紧急停 ---- */
    if (person_ok && ctx->last_person.detected &&
        ctx->last_person.confidence >= DEC_BRAKE_CONF_MIN) {
        out->command    = CMD_STOP;
        out->enable     = 1;
        out->priority   = CMD_PRI_EMERGENCY;
        out->confidence = ctx->last_person.confidence;
        return 0;
    }

    /* ---- 优先级 2：红灯停车（绿灯不阻断弯道/直行） ---- */
    if (tl_ok && ctx->last_tl.state == TL_RED && ctx->last_tl.detected &&
        ctx->last_tl.confidence >= DEC_BRAKE_CONF_MIN) {
        out->command    = CMD_STOP;
        out->enable     = 1;
        out->priority   = CMD_PRI_STOP;
        out->confidence = ctx->last_tl.confidence;
        return 0;
    }

    /* ---- 优先级 3/4：车道弯道/直行 ---- */
    if (lane_ok) {
        switch (ctx->last_lane.direction) {
            case LANE_LEFT:
                out->command    = CMD_LEFT;
                out->enable     = 1;
                out->priority   = CMD_PRI_NORMAL;
                out->confidence = ctx->last_lane.confidence;
                ctx->stall_lane_cnt = 0;
                return 0;
            case LANE_RIGHT:
                out->command    = CMD_RIGHT;
                out->enable     = 1;
                out->priority   = CMD_PRI_NORMAL;
                out->confidence = ctx->last_lane.confidence;
                ctx->stall_lane_cnt = 0;
                return 0;
            case LANE_STRAIGHT:
                if (ctx->last_lane.confidence >= DEC_GO_CONF_MIN) {
                    out->command    = CMD_GO;
                    out->enable     = 1;
                    out->priority   = CMD_PRI_NORMAL;
                    out->confidence = ctx->last_lane.confidence;
                    ctx->stall_lane_cnt = 0;
                } else {
                    out->command = CMD_NONE;   /* 置信度不足，不冒进 */
                    ctx->stall_lane_cnt++;
                }
                return 0;
            case LANE_UNKNOWN:
            default:
                out->command = CMD_NONE;
                ctx->stall_lane_cnt++;
                return 0;
        }
    }

    /* ---- 优先级 5：车道失效/超龄/从未收到 —— 保守不动作 ---- */
    out->command = CMD_NONE;
    out->enable  = 0;
    ctx->stall_lane_cnt++;
    return 0;
}
