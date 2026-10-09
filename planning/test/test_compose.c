/* Contract test: UDP reassembly -> production six-camera compositor -> PPM.
 * No X11, FPGA, RKNN or network is mocked into a claim of hardware validation. */
#include "frame_compose.h"
#include "stub_pattern.h"
#include "stereo_test_frame.h"
#include "udp_receiver.h"
#include "udp_proto.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)
static uint8_t m[IMG_FRAME_BYTES], s[IMG_FRAME_BYTES], received[IMG_FRAME_BYTES];
static uint8_t out[(size_t)DISP_WIN_W * DISP_WIN_H * 4u + 16];

static void quadrant(uint8_t *frame, int q, uint16_t color)
{
    for (int y = 0; y < 240; ++y) for (int x = 0; x < 320; ++x) {
        size_t off = ((size_t)(y + (q / 2) * 240) * 640 + x + (q % 2) * 320) * 2;
        frame[off] = (uint8_t)color; frame[off + 1] = (uint8_t)(color >> 8);
    }
}

int main(int argc, char **argv)
{
    /* 6 distinct primary/secondary colors. Reserved slots use a seventh color. */
    const uint16_t colors[6] = {0xf800, 0x07e0, 0x001f, 0xffe0, 0x07ff, 0xf81f};
    const uint8_t bgr[6][3] = {{0,0,255},{0,255,0},{255,0,0},{0,255,255},{255,255,0},{255,0,255}};
    for (int q = 0; q < 3; ++q) { quadrant(m,q,colors[q]); quadrant(s,q,colors[q+3]); }
    quadrant(m,3,0xffff); quadrant(s,3,0xffff);
    memset(out, 0xa5, sizeof(out));
    CHECK(frame_compose_6ch(s,m,DISPLAY_MODE_SPLIT,out,sizeof(out),960,480) == 0);
    /* Check all pixels, including every tile boundary, and buffer guard bytes. */
    for (int y = 0; y < 480; ++y) for (int x = 0; x < 960; ++x) {
        int ch = (y / 240) * 3 + x / 320;
        CHECK(memcmp(out + ((size_t)y*960+x)*4, bgr[ch], 3) == 0);
    }
    for (size_t i = sizeof(out)-16; i < sizeof(out); ++i) CHECK(out[i] == 0xa5);
    CHECK(frame_compose_6ch(s,m,DISPLAY_MODE_SPLIT,out,1,960,480) == -1);
    CHECK(frame_compose_6ch(s,m,DISPLAY_MODE_SPLIT,out,sizeof(out),1280,480) == -1);
    CHECK(frame_compose_6ch(NULL,m,DISPLAY_MODE_SPLIT,out,sizeof(out),960,480) == 0);
    for (size_t i = 960u*240*4; i < 960u*480*4; ++i) CHECK(out[i] == 0);
    CHECK(frame_compose_6ch(s,m,DISPLAY_MODE_LOCAL_ONLY,out,sizeof(out),960,480) == 0);
    for (size_t i = 0; i < 960u*240*4; ++i) CHECK(out[i] == 0);
    CHECK(frame_compose_6ch(s,m,DISPLAY_MODE_BLANK,out,sizeof(out),960,480) == 0);
    for (size_t i = 0; i < 960u*480*4; ++i) CHECK(out[i] == 0);

    /* Stereo bitstream contract: live at q0/q2, no stale center/reserved pixels. */
    for (int q = 0; q < 4; ++q) quadrant(m, q, colors[q]);
    stereo_test_frame_prepare(m, 0);
    CHECK(frame_compose_6ch(NULL,m,DISPLAY_MODE_SPLIT,out,sizeof(out),960,480) == 0);
    for (int y = 0; y < 240; ++y) for (int x = 0; x < 960; ++x) {
        const uint8_t black[3] = {0,0,0};
        const uint8_t *expected = x < 320 ? bgr[0] : x < 640 ? black : bgr[2];
        CHECK(memcmp(out + ((size_t)y*960+x)*4, expected, 3) == 0);
    }
    stereo_test_frame_prepare(m, 1);
    CHECK(frame_compose_6ch(NULL,m,DISPLAY_MODE_SPLIT,out,sizeof(out),960,480) == 0);
    CHECK(memcmp(out,bgr[2],3) == 0);
    CHECK(memcmp(out+640*4,bgr[0],3) == 0);
    stereo_test_frame_prepare(NULL, 0);

    stub_pattern_fill(m,640,480,0); stub_pattern_fill(s,640,480,1);
    if (argc > 2 && strcmp(argv[2], "--stereo") == 0) stereo_test_frame_prepare(m, 0);
    LaneResult lane = {0}; lane.version = LANERESULT_VERSION; lane.frame_id = 42;
    udp_frame_hdr_t hdr, got;
    udp_pack_frame_hdr(&hdr,&lane,640,480,UDP_BLOCK_SIZE);
    udp_reassembly_t r;
    CHECK(udp_reassembly_init(&r,received,sizeof(received)) == 0);
    uint8_t *frame = NULL; size_t len = 0;
    CHECK(udp_reassembly_feed(&r,(uint8_t *)&hdr,sizeof(hdr),&got,&frame,&len) == 0);
    uint8_t packet[sizeof(udp_data_hdr_t) + UDP_BLOCK_SIZE];
    int done = 0;
    for (int i = (int)hdr.block_count-1; i >= 0; --i) {
        size_t offset = (size_t)i * UDP_BLOCK_SIZE;
        size_t n = sizeof(m)-offset; if (n > UDP_BLOCK_SIZE) n = UDP_BLOCK_SIZE;
        udp_data_hdr_t block = {UDP_MAGIC_DATA, hdr.frame_id, (uint32_t)i, udp_block_checksum(m+offset,n)};
        memcpy(packet,&block,sizeof(block)); memcpy(packet+sizeof(block),m+offset,n);
        done = udp_reassembly_feed(&r,packet,sizeof(block)+n,&got,&frame,&len);
    }
    CHECK(done == 1 && len == sizeof(m) && got.frame_id == 42);
    CHECK(memcmp(frame,m,sizeof(m)) == 0);
    CHECK(frame_compose_6ch(s,frame,DISPLAY_MODE_SPLIT,out,sizeof(out),960,480) == 0);
    if (argc > 1) {
        FILE *fp = fopen(argv[1],"wb"); CHECK(fp != NULL);
        fprintf(fp,"P6\n960 480\n255\n");
        for (size_t i = 0; i < 960u*480; ++i) {
            uint8_t rgb[3] = {out[i*4+2],out[i*4+1],out[i*4]};
            CHECK(fwrite(rgb,1,3,fp) == 3);
        }
        CHECK(fclose(fp) == 0);
    }
    udp_reassembly_reset(&r);
    puts("test_compose: PASS (six tiles, boundaries, colors, missing source, bounds, UDP -> preview)");
    return 0;
}
