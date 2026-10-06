/*
 * decision.h — 决策融合状态机（【人员 B】）
 *
 * 输入：LaneResult（经 UDP 转发，S 板 shm_lane）
 *        TrafficLightResult / LaneMarkResult / ZebraResult（10-05 新增感知，
 *        v1 预留接口；S 端消费需 UDP 扩展，见 udp_proto.h flags 预留位）
 *        PersonState（S 板本地 YOLOv5s 行人检测）
 * 输出：ControlCommandMsg（写 shm_cmd，C 端 FSPI 执行）
 *
 * 决策优先级（从高到低）：
 *   1. 行人紧急（detected + 置信度达标）        → CMD_STOP（PRI_EMERGENCY=2）
 *   2. 红灯（state==RED + 置信度达标）          → CMD_STOP（PRI_STOP=1）
 *   3. 弯道（LEFT/RIGHT，置信度达标）           → CMD_LEFT / CMD_RIGHT
 *   4. 直道（STRAIGHT + 置信度达标）            → CMD_GO
 *   5. 车道失效 / 超龄 / 未知                   → CMD_NONE（保守不动作）
 *
 * 新鲜度判定基于"S 板本地时钟"（now_us 参数），不使用 M 板时间戳——
 * 两板时钟未同步，跨板时间差不可用于年龄判断。
 */
#ifndef DECISION_H
#define DECISION_H

#include "driving_types.h"
#include "driving_config.h"   /* PersonState / DisplayMode / DEC_* 阈值 */

#ifdef __cplusplus
extern "C" {
#endif

/* ControlCommandMsg.priority 取值约定定义于 driving_config.h（CMD_PRI_*），
 * 本模块引用即可，不重复定义。 */

typedef struct {
    uint64_t last_lane_us;     /* S 板本地时钟：最近一次收到车道结果的时刻 */
    uint64_t last_tl_us;
    uint64_t last_person_us;
    uint32_t out_seq;          /* 决策输出序号（自增，写 CMDMSG.frame_id） */
    uint32_t stall_lane_cnt;   /* 车道连续失效计数（供调试/看门狗） */

    /* 最近一次有效结果缓存：当本次调用输入为 NULL（无新数据）时，
     * 若缓存尚未超龄（见 DEC_*_MAX_AGE_MS），则沿用缓存结果做决策，
     * 避免“单帧丢包 → 决策立即归零 → 车辆顿挫”。超龄后缓存自然失效。 */
    LaneResult         last_lane;
    TrafficLightResult last_tl;
    PersonState        last_person;
} decision_ctx_t;

int decision_init(decision_ctx_t *ctx);

/* 各输入传 NULL 表示"本次无新数据"（调用方在收到新帧时才传非 NULL）。
 * now_us：S 板单调时钟（微秒）。成功 0，失败 -1。 */
int decision_step(decision_ctx_t *ctx,
                  const LaneResult        *lane,    /* NULL=无新车道 */
                  const TrafficLightResult *tl,     /* NULL=无新红绿灯 */
                  const LaneMarkResult     *lm,     /* NULL=无 */
                  const ZebraResult        *zebra,  /* NULL=无 */
                  const PersonState        *person, /* NULL=无新行人 */
                  uint64_t now_us,
                  ControlCommandMsg *out);

#ifdef __cplusplus
}
#endif

#endif /* DECISION_H */
