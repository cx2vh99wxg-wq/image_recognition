/*
 * udp_proto.c — UDP 协议编解码实现（【人员 B】）
 *
 * 纯逻辑模块，不依赖 socket/系统调用，可在本机单元测试；
 * 实际收发在 planning/udp_sender.c / udp_receiver.c。
 */
#include "udp_proto.h"
#include "driving_config.h"   /* UDP_BLOCK_SIZE 等 */

#include <string.h>

udp_pkt_type_t udp_identify(const uint8_t *buf, size_t len)
{
    if (!buf) return UDP_PKT_UNKNOWN;
    uint32_t magic;
    if (len < 4) return UDP_PKT_UNKNOWN;
    memcpy(&magic, buf, 4);

    switch (magic) {
        case UDP_MAGIC_FRAME:    return len >= sizeof(udp_frame_hdr_t) ? UDP_PKT_FRAME_HDR : UDP_PKT_UNKNOWN;
        case UDP_MAGIC_DATA:     return len >= sizeof(udp_data_hdr_t)  ? UDP_PKT_DATA      : UDP_PKT_UNKNOWN;
        case UDP_MAGIC_HB:       return len >= sizeof(udp_heartbeat_t) ? UDP_PKT_HEARTBEAT : UDP_PKT_UNKNOWN;
        case UDP_MAGIC_CMD:      return len >= sizeof(udp_cmd_t)       ? UDP_PKT_CMD       : UDP_PKT_UNKNOWN;
        default:                 return UDP_PKT_UNKNOWN;
    }
}

int udp_pack_frame_hdr(udp_frame_hdr_t *hdr, const LaneResult *lane,
                       uint32_t width, uint32_t height, uint32_t block_size)
{
    if (!hdr) return -1;

    memset(hdr, 0, sizeof(*hdr));
    hdr->magic       = UDP_MAGIC_FRAME;
    hdr->version     = UDP_PROTO_VERSION;
    hdr->width       = width;
    hdr->height      = height;
    hdr->data_size   = width * height * 2u;   /* RGB565 */
    if (block_size == 0) block_size = UDP_BLOCK_SIZE;
    hdr->block_size  = block_size;
    hdr->block_count = (hdr->data_size + block_size - 1u) / block_size;

    if (lane) {
        hdr->frame_id        = lane->frame_id;
        hdr->timestamp_s     = (uint32_t)(lane->timestamp_us / 1000000u);
        hdr->timestamp_us_lo = (uint32_t)(lane->timestamp_us % 1000000u);
        hdr->direction       = (uint32_t)lane->direction;
        hdr->curve_offset    = lane->curve_offset;
        hdr->confidence      = lane->confidence;
        hdr->lane_pixel_cnt  = lane->lane_pixel_cnt;
        hdr->flags          |= UDP_FLAG_LANE_VALID;
    } else {
        hdr->frame_id = 0;
        hdr->flags    = 0;
    }
    return 0;
}

int udp_unpack_frame_hdr(const uint8_t *buf, size_t len, udp_frame_hdr_t *out)
{
    if (!buf || !out || len < sizeof(udp_frame_hdr_t)) return -1;
    memcpy(out, buf, sizeof(udp_frame_hdr_t));
    if (out->magic != UDP_MAGIC_FRAME || out->version != UDP_PROTO_VERSION)
        return -1;
    return 0;
}

int udp_hdr_to_lane(const udp_frame_hdr_t *hdr, LaneResult *lane)
{
    if (!hdr || !lane) return -1;
    memset(lane, 0, sizeof(*lane));
    lane->version        = LANERESULT_VERSION;
    lane->frame_id       = hdr->frame_id;
    lane->timestamp_us   = (uint64_t)hdr->timestamp_s * 1000000u +
                           (uint64_t)hdr->timestamp_us_lo;
    lane->direction      = (LaneDirection)hdr->direction;
    lane->curve_offset   = hdr->curve_offset;
    lane->confidence     = hdr->confidence;
    lane->lane_pixel_cnt = hdr->lane_pixel_cnt;
    return 0;
}

uint32_t udp_block_checksum(const uint8_t *data, size_t n)
{
    uint32_t sum = 0;
    for (size_t i = 0; i < n; i++) sum += data[i];
    return sum;
}

/* ---- 三感知摘要：帧头 reserved[4]（16B）编解码，布局见 udp_proto.h ---- */

static uint32_t conf01(uint32_t c) { return c > 100u ? 100u : c; }

void udp_hdr_set_aux(udp_frame_hdr_t *hdr,
                     const TrafficLightResult *tl,
                     const LaneMarkResult     *lm,
                     const ZebraResult        *zebra)
{
    if (!hdr) return;

    if (tl) {
        uint32_t v = 0;
        v |= (uint32_t)((uint8_t)tl->state & 0xFFu);          /* bit 0..7  */
        v |= conf01(tl->confidence) << 8;                     /* bit 8..15 */
        v |= (uint32_t)(tl->detected ? 1u : 0u) << 16;        /* bit 16    */
        hdr->reserved[0] = v;
        hdr->flags |= UDP_FLAG_TL_VALID;
    }

    if (lm) {
        uint32_t v = 0;
        uint32_t lmf = 0;
        if (lm->crossing)       lmf |= 0x01u;
        if (lm->crossing_left)  lmf |= 0x02u;
        if (lm->crossing_right) lmf |= 0x04u;
        if (lm->lane_change)    lmf |= 0x08u;
        v |= (uint32_t)((uint8_t)lm->left_type  & 0xFFu);          /* bit 0..7  */
        v |= (uint32_t)((uint8_t)lm->right_type & 0xFFu) << 8;     /* bit 8..15 */
        v |= lmf << 16;                                            /* bit 16..19*/
        v |= conf01(lm->confidence) << 20;                         /* bit 20..27*/
        hdr->reserved[1] = v;
        /* 车辆中心相对车道中心偏移（有符号），供决策做车道保持微调 */
        hdr->reserved[3] = (uint32_t)lm->ego_offset_px;
        hdr->flags |= UDP_FLAG_LM_VALID;
    }

    if (zebra) {
        uint32_t v = 0;
        v |= (uint32_t)(zebra->detected ? 1u : 0u);           /* bit 0    */
        v |= conf01(zebra->confidence) << 1;                  /* bit 1..8 */
        hdr->reserved[2] = v;
        hdr->flags |= UDP_FLAG_ZEBRA_VALID;
    }
}

uint32_t udp_hdr_get_aux(const udp_frame_hdr_t *hdr,
                         TrafficLightResult *tl,
                         LaneMarkResult     *lm,
                         ZebraResult        *zebra)
{
    if (!hdr) return 0u;
    uint32_t has = 0u;

    /* 同一帧的帧号/时间戳，保证三感知与车道结果可按帧对齐 */
    const uint64_t ts_us = (uint64_t)hdr->timestamp_s * 1000000u +
                           (uint64_t)hdr->timestamp_us_lo;

    if ((hdr->flags & UDP_FLAG_TL_VALID) && tl) {
        const uint32_t v = hdr->reserved[0];
        memset(tl, 0, sizeof(*tl));
        tl->version     = TL_VERSION;
        tl->frame_id    = hdr->frame_id;
        tl->timestamp_us = ts_us;
        tl->state       = (TrafficLightState)(v & 0xFFu);
        tl->confidence  = (v >> 8) & 0xFFu;
        tl->detected    = (v >> 16) & 0x1u;
        tl->box_x = tl->box_y = tl->box_w = tl->box_h = -1;   /* 摘要不带包围盒 */
        has |= UDP_AUX_HAS_TL;
    }
    if ((hdr->flags & UDP_FLAG_LM_VALID) && lm) {
        const uint32_t v = hdr->reserved[1];
        memset(lm, 0, sizeof(*lm));
        lm->version      = LANEMARK_VERSION;
        lm->frame_id     = hdr->frame_id;
        lm->timestamp_us = ts_us;
        lm->left_type    = (LaneMarkType)(v & 0xFFu);
        lm->right_type   = (LaneMarkType)((v >> 8) & 0xFFu);
        lm->crossing        = (uint8_t)((v >> 16) & 0x1u);
        lm->crossing_left   = (uint8_t)((v >> 17) & 0x1u);
        lm->crossing_right  = (uint8_t)((v >> 18) & 0x1u);
        lm->lane_change     = (uint8_t)((v >> 19) & 0x1u);
        lm->confidence   = (v >> 20) & 0xFFu;
        lm->left_x  = -1;                     /* 摘要不带近场坐标 */
        lm->right_x = -1;
        lm->ego_offset_px = (int32_t)hdr->reserved[3];
        has |= UDP_AUX_HAS_LM;
    }
    if ((hdr->flags & UDP_FLAG_ZEBRA_VALID) && zebra) {
        const uint32_t v = hdr->reserved[2];
        memset(zebra, 0, sizeof(*zebra));
        zebra->version      = ZEBRA_VERSION;
        zebra->frame_id     = hdr->frame_id;
        zebra->timestamp_us = ts_us;
        zebra->detected     = v & 0x1u;
        zebra->confidence   = (v >> 1) & 0xFFu;
        zebra->center_y     = -1;
        has |= UDP_AUX_HAS_ZEBRA;
    }
    return has;
}

int udp_pack_heartbeat(udp_heartbeat_t *hb, uint32_t seq)
{
    if (!hb) return -1;
    hb->magic   = UDP_MAGIC_HB;
    hb->version = UDP_PROTO_VERSION;
    hb->seq     = seq;
    hb->status  = 0;
    return 0;
}

int udp_unpack_heartbeat(const uint8_t *buf, size_t len, udp_heartbeat_t *out)
{
    if (!buf || !out || len < sizeof(udp_heartbeat_t)) return -1;
    memcpy(out, buf, sizeof(udp_heartbeat_t));
    if (out->magic != UDP_MAGIC_HB || out->version != UDP_PROTO_VERSION)
        return -1;
    return 0;
}

int udp_pack_cmd(udp_cmd_t *cmd, const ControlCommandMsg *m)
{
    if (!cmd || !m) return -1;
    cmd->magic      = UDP_MAGIC_CMD;
    cmd->version    = UDP_PROTO_VERSION;
    cmd->frame_id   = m->frame_id;
    cmd->command    = (uint32_t)m->command;
    cmd->enable     = m->enable;
    cmd->priority   = m->priority;
    cmd->confidence = m->confidence;
    cmd->reserved   = 0;
    return 0;
}

int udp_unpack_cmd(const uint8_t *buf, size_t len, udp_cmd_t *out)
{
    if (!buf || !out || len < sizeof(udp_cmd_t)) return -1;
    memcpy(out, buf, sizeof(udp_cmd_t));
    if (out->magic != UDP_MAGIC_CMD || out->version != UDP_PROTO_VERSION)
        return -1;
    return 0;
}

const char *udp_type_name(udp_pkt_type_t t)
{
    switch (t) {
        case UDP_PKT_FRAME_HDR: return "FRAME_HDR";
        case UDP_PKT_DATA:      return "DATA";
        case UDP_PKT_HEARTBEAT: return "HEARTBEAT";
        case UDP_PKT_CMD:       return "CMD";
        default:                return "UNKNOWN";
    }
}
