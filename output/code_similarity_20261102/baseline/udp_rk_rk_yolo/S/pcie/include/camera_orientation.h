#ifndef CAMERA_ORIENTATION_H
#define CAMERA_ORIENTATION_H

#include <stdint.h>

// Rotate each camera tile in place, keeping the tiles in their original slots.
static inline void rotate_rgb565_tiles_180(uint16_t* pixels, int frame_width,
                                            int frame_height, int tile_width,
                                            int tile_height) {
    if (!pixels || frame_width <= 0 || frame_height <= 0 ||
        tile_width <= 0 || tile_height <= 0 ||
        frame_width % tile_width != 0 || frame_height % tile_height != 0) {
        return;
    }

    for (int tile_y = 0; tile_y < frame_height; tile_y += tile_height) {
        for (int tile_x = 0; tile_x < frame_width; tile_x += tile_width) {
            for (int y = 0; y < tile_height / 2; ++y) {
                uint16_t* top = pixels + (tile_y + y) * frame_width + tile_x;
                uint16_t* bottom = pixels + (tile_y + tile_height - 1 - y) * frame_width + tile_x;
                for (int x = 0; x < tile_width; ++x) {
                    uint16_t tmp = top[x];
                    top[x] = bottom[tile_width - 1 - x];
                    bottom[tile_width - 1 - x] = tmp;
                }
            }
            if (tile_height & 1) {
                uint16_t* middle = pixels + (tile_y + tile_height / 2) * frame_width + tile_x;
                for (int x = 0; x < tile_width / 2; ++x) {
                    uint16_t tmp = middle[x];
                    middle[x] = middle[tile_width - 1 - x];
                    middle[tile_width - 1 - x] = tmp;
                }
            }
        }
    }
}

#endif
