#include "stereo_local_view.h"
#include "driving_config.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)

static uint8_t source[IMG_FRAME_BYTES];
static uint8_t display[STEREO_VIEW_BYTES + 16];

static void fill_quadrant(int q, uint16_t rgb565)
{
    for (int y = 0; y < DISP_CELL_H; ++y) {
        for (int x = 0; x < DISP_CELL_W; ++x) {
            size_t offset = ((size_t)(y + (q / 2) * DISP_CELL_H) * IMG_WIDTH +
                             x + (q % 2) * DISP_CELL_W) * 2u;
            source[offset] = (uint8_t)rgb565;
            source[offset + 1] = (uint8_t)(rgb565 >> 8);
        }
    }
}

int main(void)
{
    /* The unused quadrants have deliberately different colors. If either is
     * selected by mistake, the complete output comparison detects it. */
    fill_quadrant(0, 0xf800u); /* CAM1 red */
    fill_quadrant(1, 0x001fu); /* unused blue */
    fill_quadrant(2, 0x07e0u); /* CAM2 green */
    fill_quadrant(3, 0xffffu); /* unused white */
    const uint8_t expected[2][4] = {{0, 0, 255, 0}, {0, 255, 0, 0}};
    for (int eye = 0; eye < 2; ++eye) {
        memset(display, 0xa5, sizeof(display));
        stereo_view_stats_t stats;
        CHECK(stereo_local_view_render(source, eye, display, sizeof(display), &stats) == 0);
        CHECK(stats.nonblack_pixels == STEREO_VIEW_PIXELS);
        for (size_t i = 0; i < (size_t)STEREO_VIEW_W * STEREO_VIEW_H; ++i)
            CHECK(memcmp(display + i * 4, expected[eye], 4) == 0);
        for (size_t i = STEREO_VIEW_BYTES; i < sizeof(display); ++i)
            CHECK(display[i] == 0xa5);
    }
    CHECK(stereo_local_view_render(source, 2, display, sizeof(display), &(stereo_view_stats_t){0}) == -1);
    CHECK(stereo_local_view_render(source, 0, display, STEREO_VIEW_BYTES - 1,
                                   &(stereo_view_stats_t){0}) == -1);
    memset(source, 0, sizeof(source));
    stereo_view_stats_t blank;
    CHECK(stereo_local_view_render(source, 0, display, sizeof(display), &blank) == 0);
    CHECK(blank.nonblack_pixels == 0);
    puts("stereo local view: CAM1/CAM2 selection, scale, stats and bounds PASS");
    return 0;
}
