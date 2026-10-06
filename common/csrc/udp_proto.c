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
