/*
 * test_udp.c — UDP 收发全链路重组测试（【人员 B】）
 *
 * 模拟 M 端发送（帧头+数据块）→ S 端 udp_reassembly 重组，验证：
 *   1. 正常顺序收包重组完整帧（图像字节与车道字段一致）；
 *   2. 乱序收包仍可重组；
 *   3. 重复块被忽略；
 *   4. 新帧头到来后旧帧残块被丢弃；
 *   5. 块校验和损坏的块被丢弃（帧不完整则不出帧）。
 */
#include "udp_proto.h"
#include "udp_receiver.h"
#include "driving_config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int g_fail = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { fprintf(stderr, "FAIL: %s (line %d)\n", msg, __LINE__); g_fail++; } \
} while (0)

/* 与 udp_sender 相同的切块打包逻辑（纯内存版，不建 socket） */
static void fake_sender_split(const uint8_t *img, size_t bytes, const LaneResult *lane,
                              udp_frame_hdr_t *hdr,
                              uint8_t (*block_buf)[sizeof(udp_data_hdr_t) + UDP_BLOCK_SIZE],
                              size_t *block_lens, uint32_t *nblocks)
{
    udp_pack_frame_hdr(hdr, lane, IMG_WIDTH, IMG_HEIGHT, UDP_BLOCK_SIZE);
    uint32_t bs = hdr->block_size;
    uint32_t cnt = hdr->block_count;
    for (uint32_t i = 0; i < cnt; i++) {
        size_t off = (size_t)i * bs;
        size_t len = (off + bs <= bytes) ? bs : (bytes - off);
        udp_data_hdr_t *dh = (udp_data_hdr_t *)block_buf[i];
        dh->magic = UDP_MAGIC_DATA;
        dh->frame_id = hdr->frame_id;
        dh->block_idx = i;
        dh->checksum = udp_block_checksum(img + off, len);
        memcpy(block_buf[i] + sizeof(udp_data_hdr_t), img + off, len);
        block_lens[i] = sizeof(udp_data_hdr_t) + len;
    }
    *nblocks = cnt;
}

int main(void)
{
    /* 构造假图像：pattern 逐字节递增 */
    static uint8_t img[IMG_FRAME_BYTES];
    for (size_t i = 0; i < sizeof(img); i++) img[i] = (uint8_t)(i & 0xFF);

    LaneResult lane;
    memset(&lane, 0, sizeof(lane));
    lane.version = LANERESULT_VERSION;
    lane.frame_id = 100;
    lane.direction = LANE_RIGHT;
    lane.curve_offset = 66;
    lane.confidence = 91;

    /* 切块 */
    udp_frame_hdr_t hdr;
    static uint8_t blocks[512][sizeof(udp_data_hdr_t) + UDP_BLOCK_SIZE];
    static size_t blen[512];
    uint32_t nb = 0;
    fake_sender_split(img, sizeof(img), &lane, &hdr, blocks, blen, &nb);
    CHECK(nb > 1, "multi blocks");
    CHECK(hdr.block_count == nb, "block count match");

    /* 接收端重组 */
    static uint8_t frame[IMG_FRAME_BYTES];
    udp_reassembly_t r;
    CHECK(udp_reassembly_init(&r, frame, sizeof(frame)) == 0, "reass init");

    /* 测试 1：乱序 + 重复块（先喂帧头，再乱序补块） */
    static const uint32_t order[9] = {5, 3, 7, 0, 3, 1, 8, 2, 4};  /* 重复 3，且乱序 */
    udp_frame_hdr_t oh;
    uint8_t *of = NULL; size_t ofl = 0;
    int done = 0;
    udp_reassembly_feed(&r, (const uint8_t *)&hdr, sizeof(hdr), &oh, &of, &ofl);
    for (int pass = 0; pass < 2; pass++) {          /* 两轮补齐 */
        for (uint32_t k = 0; k < nb; k++) {
            uint32_t idx = (pass == 0) ? order[k % 9] : k;
            int rc = udp_reassembly_feed(&r, blocks[idx], blen[idx], &oh, &of, &ofl);
            if (rc == 1) { done = 1; break; }
        }
        if (done) break;
    }
    CHECK(done == 1, "frame reassembled despite reorder+dup");
    if (done) {
        CHECK(of == frame && ofl == sizeof(img), "frame out");
        CHECK(memcmp(frame, img, sizeof(img)) == 0, "image bytes identical");
        CHECK(oh.frame_id == 100 && oh.direction == LANE_RIGHT &&
              oh.curve_offset == 66, "hdr via reassembly");
    }

    /* 测试 2：新帧头丢弃旧帧残块 */
    udp_reassembly_reset(&r);
    udp_reassembly_feed(&r, (const uint8_t *)&hdr, sizeof(hdr), &oh, &of, &ofl);
    /* 发第 0 块 */
    udp_reassembly_feed(&r, blocks[0], blen[0], &oh, &of, &ofl);
    /* 新帧头（frame_id+1）到来 */
    LaneResult lane2 = lane; lane2.frame_id = 101;
    udp_frame_hdr_t hdr2;
    udp_pack_frame_hdr(&hdr2, &lane2, IMG_WIDTH, IMG_HEIGHT, UDP_BLOCK_SIZE);
    udp_reassembly_feed(&r, (const uint8_t *)&hdr2, sizeof(hdr2), &oh, &of, &ofl);
    /* 旧帧第 0 块应被丢弃（frame_id 不匹配） */
    udp_reassembly_feed(&r, blocks[0], blen[0], &oh, &of, &ofl);
    CHECK(r.received_blocks == 0, "old-frame block dropped after new hdr");

    /* 测试 3：校验和损坏的块被丢弃 */
    udp_reassembly_reset(&r);
    udp_reassembly_feed(&r, (const uint8_t *)&hdr, sizeof(hdr), &oh, &of, &ofl);
    uint8_t bad[sizeof(udp_data_hdr_t) + UDP_BLOCK_SIZE];
    memcpy(bad, blocks[0], blen[0]);
    bad[sizeof(udp_data_hdr_t)] ^= 0xFF;   /* 破坏 payload 首字节 */
    int rc = udp_reassembly_feed(&r, bad, blen[0], &oh, &of, &ofl);
    CHECK(rc != 1, "corrupt block rejected");

    /* 测试 4：正常顺序完整走通 */
    udp_reassembly_reset(&r);
    udp_reassembly_feed(&r, (const uint8_t *)&hdr, sizeof(hdr), &oh, &of, &ofl);
    for (uint32_t k = 0; k < nb; k++) {
        int rc = udp_reassembly_feed(&r, blocks[k], blen[k], &oh, &of, &ofl);
        if (rc == 1) break;
    }
    CHECK(memcmp(frame, img, sizeof(img)) == 0, "full frame after ordered feed");

    udp_reassembly_reset(&r);

    /* 测试 5：三感知摘要随帧头 reserved[4] 往返（红绿灯/虚实线/斑马线）。
     * 这是"红绿灯停驶 / 实线变道报警"能生效的前提：M 端把 A 的三项感知塞进
     * 帧头 16B 预留区，S 端解出后喂 decision —— 全程不改包尺寸。 */
    {
        TrafficLightResult tl_in, tl_out;
        LaneMarkResult     lm_in, lm_out;
        ZebraResult        zb_in, zb_out;
        memset(&tl_in, 0, sizeof(tl_in));
        memset(&lm_in, 0, sizeof(lm_in));
        memset(&zb_in, 0, sizeof(zb_in));

        tl_in.version    = TL_VERSION;
        tl_in.state      = TL_RED;
        tl_in.detected   = 1;
        tl_in.confidence = 88;
        lm_in.version       = LANEMARK_VERSION;
        lm_in.left_type     = LANE_MARK_SOLID;
        lm_in.right_type    = LANE_MARK_DASHED;
        lm_in.lane_change   = 1;
        lm_in.crossing_left = 1;
        lm_in.confidence    = 76;
        lm_in.ego_offset_px = -37;
        zb_in.version    = ZEBRA_VERSION;
        zb_in.detected   = 1;
        zb_in.confidence = 64;

        udp_frame_hdr_t h_aux;
        udp_pack_frame_hdr(&h_aux, &lane, IMG_WIDTH, IMG_HEIGHT, UDP_BLOCK_SIZE);
        /* pack 之后才可写摘要（pack 会 memset 整帧头） */
        udp_hdr_set_aux(&h_aux, &tl_in, &lm_in, &zb_in);

        uint32_t has = udp_hdr_get_aux(&h_aux, &tl_out, &lm_out, &zb_out);
        CHECK(has == (UDP_AUX_HAS_TL | UDP_AUX_HAS_LM | UDP_AUX_HAS_ZEBRA),
              "aux: all three flags present");
        CHECK(tl_out.state == TL_RED && tl_out.detected == 1 &&
              tl_out.confidence == 88, "aux: traffic light round-trip");
        CHECK(lm_out.left_type == LANE_MARK_SOLID &&
              lm_out.right_type == LANE_MARK_DASHED &&
              lm_out.lane_change == 1 && lm_out.crossing_left == 1 &&
              lm_out.confidence == 76 && lm_out.ego_offset_px == -37,
              "aux: lane-mark round-trip");
        CHECK(zb_out.detected == 1 && zb_out.confidence == 64,
              "aux: zebra round-trip");
        /* 帧号/时间戳需与车道结果同源，供决策按帧对齐 */
        CHECK(tl_out.frame_id == lane.frame_id && lm_out.frame_id == lane.frame_id,
              "aux: frame_id aligned with lane");

        /* 不携带任一项时（旧发送端），不应误报有效 */
        udp_frame_hdr_t h_plain;
        udp_pack_frame_hdr(&h_plain, &lane, IMG_WIDTH, IMG_HEIGHT, UDP_BLOCK_SIZE);
        uint32_t has2 = udp_hdr_get_aux(&h_plain, &tl_out, &lm_out, &zb_out);
        CHECK(has2 == 0u, "aux: absent when not set (backward compatible)");
    }

    if (g_fail == 0) {
        printf("test_udp: ALL PASS\n");
        return 0;
    }
    printf("test_udp: %d FAILED\n", g_fail);
    return 1;
}
