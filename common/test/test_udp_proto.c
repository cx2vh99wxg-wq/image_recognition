/*
 * test_udp_proto.c — UDP 协议层单元测试（【人员 B】）
 *
 * 覆盖：识别、帧头打包/解包、hdr↔LaneResult 往返、校验和、心跳、命令包。
 */
#include "udp_proto.h"
#include "driving_config.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

static int g_fail = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { fprintf(stderr, "FAIL: %s (line %d)\n", msg, __LINE__); g_fail++; } \
} while (0)

int main(void)
{
    /* 1. 识别 */
    udp_frame_hdr_t hdr;
    memset(&hdr, 0, sizeof(hdr));
    hdr.magic = UDP_MAGIC_FRAME;
    hdr.version = UDP_PROTO_VERSION;
    CHECK(udp_identify((const uint8_t *)&hdr, sizeof(hdr)) == UDP_PKT_FRAME_HDR, "identify frame_hdr");
    CHECK(udp_identify((const uint8_t *)&hdr, 2) == UDP_PKT_UNKNOWN, "identify too short");

    udp_heartbeat_t hb;
    memset(&hb, 0, sizeof(hb));
    hb.magic = UDP_MAGIC_HB;
    hb.version = UDP_PROTO_VERSION;
    CHECK(udp_identify((const uint8_t *)&hb, sizeof(hb)) == UDP_PKT_HEARTBEAT, "identify hb");

    /* 2. 帧头打包/解包 + hdr↔lane 往返 */
    LaneResult lane;
    memset(&lane, 0, sizeof(lane));
    lane.version = LANERESULT_VERSION;
    lane.frame_id = 42;
    lane.timestamp_us = 12345678901ull;
    lane.direction = LANE_LEFT;
    lane.curve_offset = -37;
    lane.confidence = 88;
    lane.lane_pixel_cnt = 12345;

    udp_frame_hdr_t ph;
    CHECK(udp_pack_frame_hdr(&ph, &lane, IMG_WIDTH, IMG_HEIGHT, 0) == 0, "pack hdr");
    CHECK(ph.magic == UDP_MAGIC_FRAME, "hdr magic");
    CHECK(ph.block_size == UDP_BLOCK_SIZE, "default block size");
    CHECK(ph.data_size == (uint32_t)(IMG_WIDTH * IMG_HEIGHT * 2), "data_size");
    CHECK(ph.block_count == (ph.data_size + UDP_BLOCK_SIZE - 1) / UDP_BLOCK_SIZE, "block_count");
    CHECK((ph.flags & UDP_FLAG_LANE_VALID) != 0, "lane valid flag");

    udp_frame_hdr_t uh;
    CHECK(udp_unpack_frame_hdr((const uint8_t *)&ph, sizeof(ph), &uh) == 0, "unpack hdr");
    CHECK(uh.frame_id == 42 && uh.direction == LANE_LEFT && uh.curve_offset == -37, "hdr fields");

    LaneResult back;
    CHECK(udp_hdr_to_lane(&uh, &back) == 0, "hdr_to_lane");
    CHECK(back.frame_id == lane.frame_id, "lane frame_id");
    CHECK(back.direction == lane.direction, "lane direction");
    CHECK(back.curve_offset == lane.curve_offset, "lane offset");
    CHECK(back.confidence == lane.confidence, "lane confidence");
    CHECK(back.timestamp_us == lane.timestamp_us, "lane ts");

    /* 3. 校验和 */
    const uint8_t data[5] = {1, 2, 3, 4, 5};
    CHECK(udp_block_checksum(data, 5) == 15u, "checksum");

    /* 4. 心跳 */
    udp_heartbeat_t hb2;
    CHECK(udp_pack_heartbeat(&hb2, 7) == 0, "pack hb");
    udp_heartbeat_t hb3;
    CHECK(udp_unpack_heartbeat((const uint8_t *)&hb2, sizeof(hb2), &hb3) == 0, "unpack hb");
    CHECK(hb3.seq == 7, "hb seq");

    /* 5. 命令包 */
    ControlCommandMsg cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.version = CMDMSG_VERSION;
    cmd.frame_id = 9;
    cmd.command = CMD_BRAKE;
    cmd.enable = 1;
    cmd.priority = CMD_PRI_EMERGENCY;
    cmd.confidence = 99;
    udp_cmd_t pc;
    CHECK(udp_pack_cmd(&pc, &cmd) == 0, "pack cmd");
    udp_cmd_t uc;
    CHECK(udp_unpack_cmd((const uint8_t *)&pc, sizeof(pc), &uc) == 0, "unpack cmd");
    CHECK(uc.command == CMD_BRAKE && uc.priority == CMD_PRI_EMERGENCY, "cmd fields");

    if (g_fail == 0) {
        printf("test_udp_proto: ALL PASS\n");
        return 0;
    }
    printf("test_udp_proto: %d FAILED\n", g_fail);
    return 1;
}
