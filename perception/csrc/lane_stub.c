/*
 * lane_stub.c — 感知桩（【人员 A · 感知】）
 *
 * 不读取任何硬件 / 模型，按请求吐一条确定的 LaneResult，供【人员 B】在
 * 没有相机与 NPU 时做联调。桩与真代码共用 perception_step 签名，通过
 * perception_cfg_t.use_stub 切换。
 */
#include "perception_api.h"

#include <string.h>

void lane_stub_make_result(LaneResult *out, uint32_t frame_id,
                           LaneDirection dir, uint32_t conf)
{
    if (!out) return;
    memset(out, 0, sizeof(*out));
    out->version        = LANERESULT_VERSION;
    out->frame_id       = frame_id;
    out->direction      = dir;
    out->curve_offset   = (dir == LANE_RIGHT) ? 12 : (dir == LANE_LEFT) ? -12 : 0;
    out->confidence     = conf;
    out->lane_pixel_cnt = (dir == LANE_UNKNOWN) ? 0u : 800u;
}
