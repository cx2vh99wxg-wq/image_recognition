#ifndef LANE_RULE_H
#define LANE_RULE_H
#include "line_style_ipc.h"
/* 只有已确认实线拦截 HMI 跨线请求；
 * UNKNOWN 保留原人工回退行为，弯道循迹另行处理。 */
static inline unsigned lane_denied_sides(const LineStyleData *d, uint64_t now)
{
    if (!line_fresh(d, now))
        return 0;
    return (d->left == LINE_SOLID && d->left_quality >= 65 ? 1u : 0u) |
           (d->right == LINE_SOLID && d->right_quality >= 65 ? 2u : 0u);
}
static inline unsigned lane_blocked_request(unsigned denied, uint8_t hmi)
{
    return (hmi == 0x0c || hmi == 0xcc)   ? denied & 1u
           : (hmi == 0x0d || hmi == 0xdd) ? denied & 2u
                                          : 0u;
}
#endif
