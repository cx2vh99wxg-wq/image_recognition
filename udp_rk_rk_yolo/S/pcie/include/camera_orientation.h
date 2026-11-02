#ifndef CAMERA_ORIENTATION_H
#define CAMERA_ORIENTATION_H

#include <stdint.h>

// 成对行与奇数高度中间行共用此镜像区段交换。
static inline void camera_swap_mirrored_span(uint16_t *forward, uint16_t *backward, int pixel_count)
{
    for (int remaining = pixel_count; remaining > 0; --remaining)
    {
        const uint16_t saved = *forward;
        *forward++ = *backward;
        *backward-- = saved;
    }
}

// 每个摄像头格独立原地旋转，格子在拼接图中的位置不变。
static inline void rotate_rgb565_tiles_180(uint16_t *pixels, int frame_width, int frame_height,
                                           int tile_width, int tile_height)
{
    if (!pixels || frame_width <= 0 || frame_height <= 0 || tile_width <= 0 || tile_height <= 0 ||
        frame_width % tile_width != 0 || frame_height % tile_height != 0)
    {
        return;
    }

    for (int tile_y = 0; tile_y < frame_height; tile_y += tile_height)
    {
        for (int tile_x = 0; tile_x < frame_width; tile_x += tile_width)
        {
            for (int y = 0; y < tile_height / 2; ++y)
            {
                uint16_t *top = pixels + (tile_y + y) * frame_width + tile_x;
                uint16_t *bottom = pixels + (tile_y + tile_height - 1 - y) * frame_width + tile_x;
                camera_swap_mirrored_span(top, bottom + tile_width - 1, tile_width);
            }
            if (tile_height & 1)
            {
                uint16_t *middle = pixels + (tile_y + tile_height / 2) * frame_width + tile_x;
                camera_swap_mirrored_span(middle, middle + tile_width - 1, tile_width / 2);
            }
        }
    }
}

#endif
