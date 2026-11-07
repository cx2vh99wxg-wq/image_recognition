#ifndef S_CAMERA_CAPTURE_EDGES_H
#define S_CAMERA_CAPTURE_EDGES_H
#include <stdint.h>
#include <string.h>

// The shipped camera-0 processing path puts its final three image rows at
// the head and emits one black sample after the eight transport columns.
// Camera 0 occupies TL and BR; BR is the rear-center view. Restore only
// those copies, before rotation, inference and publication. This standalone
// reference is used by coordinate tests; production fuses it into unpack.
static inline void repair_s_camera0_edges(uint16_t *pixels, int width, int height)
{
    if (!pixels || width != 640 || height != 480)
        return;
    uint16_t saved[3 * 320];
    for (int tile = 0; tile < 2; ++tile)
    {
        const int tx = tile * 320, ty = tile * 240;
        for (int y = 0; y < 3; ++y)
            memcpy(saved + y * 320, pixels + (ty + y) * width + tx, 320 * sizeof(*pixels));
        for (int y = 0; y < 237; ++y)
            memcpy(pixels + (ty + y) * width + tx,
                   pixels + (ty + y + 3) * width + tx, 320 * sizeof(*pixels));
        for (int y = 0; y < 3; ++y)
            memcpy(pixels + (ty + 237 + y) * width + tx,
                   saved + y * 320, 320 * sizeof(*pixels));
        for (int y = 0; y < 240; ++y)
        {
            uint16_t *row = pixels + (ty + y) * width + tx;
            for (int x = 0; x < 9; ++x)
                row[x] = row[9];
        }
    }
}
#endif
