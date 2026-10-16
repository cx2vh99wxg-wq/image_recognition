#include "stereo_local_view.h"
#include "driving_config.h"

int stereo_local_view_render(const uint8_t *frame565, int eye,
                             uint8_t *bgrx, size_t capacity,
                             stereo_view_stats_t *stats)
{
    if (!frame565 || !bgrx || !stats || (eye != 0 && eye != 1) ||
        capacity < STEREO_VIEW_BYTES) return -1;

    uint32_t nonblack = 0, hash = 2166136261u;
    const int src_y = eye ? DISP_CELL_H : 0;
    for (int y = 0; y < DISP_CELL_H; ++y) {
        const uint8_t *src = frame565 + ((size_t)(src_y + y) * IMG_WIDTH) * 2u;
        uint8_t *upper = bgrx + (size_t)(y * 2) * STEREO_VIEW_W * 4u;
        uint8_t *lower = upper + STEREO_VIEW_W * 4u;
        for (int x = 0; x < DISP_CELL_W; ++x) {
            const uint8_t lo = src[2 * x], hi = src[2 * x + 1];
            const uint16_t p = (uint16_t)lo | ((uint16_t)hi << 8);
            const uint8_t r = (uint8_t)(((p >> 11) & 31u) * 255u / 31u);
            const uint8_t g = (uint8_t)(((p >> 5) & 63u) * 255u / 63u);
            const uint8_t b = (uint8_t)((p & 31u) * 255u / 31u);
            uint8_t *a = upper + (size_t)x * 8u;
            uint8_t *c = lower + (size_t)x * 8u;
            for (int i = 0; i < 2; ++i) {
                uint8_t *u = a + 4 * i, *v = c + 4 * i;
                u[0] = v[0] = b; u[1] = v[1] = g;
                u[2] = v[2] = r; u[3] = v[3] = 0;
            }
            nonblack += (p != 0);
            hash = (hash ^ lo) * 16777619u;
            hash = (hash ^ hi) * 16777619u;
        }
    }
    stats->nonblack_pixels = nonblack;
    stats->fnv1a = hash;
    return 0;
}
