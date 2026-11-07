#ifndef CAMERA_ORIENTATION_H
#define CAMERA_ORIENTATION_H

#include <stdint.h>
#include "../../../common/image/frame_pixels.h"

// 成对行与奇数高度中间行共用此镜像区段交换。
static inline void camera_swap_mirrored_span(uint16_t *forward, uint16_t *backward, int pixel_count)
{
    int remaining = pixel_count;
#ifdef FRAME_PIXELS_NEON
    while (remaining >= 8)
    {
        const uint16x8_t front = vld1q_u16(forward);
        const uint16x8_t back = vld1q_u16(backward - 7);
        vst1q_u16(forward, frame_reverse_u16x8(back));
        vst1q_u16(backward - 7, frame_reverse_u16x8(front));
        forward += 8;
        backward -= 8;
        remaining -= 8;
    }
#endif
    for (; remaining > 0; --remaining)
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

// Read each source tile backwards directly into RGB888. This has exactly
// the same coordinates/colors as rotate_rgb565_tiles_180 + unpack, without
// writing and then reading a second full RGB565 frame.
static inline void unpack_rgb565_tiles_180(const uint16_t *source, uint8_t *rgb,
                                          int width, int height, int tile_width, int tile_height,
                                          bool restore_camera0 = false)
{
    if (!source || !rgb || width <= 0 || height <= 0 || tile_width <= 0 || tile_height <= 0 ||
        width % tile_width || height % tile_height)
        return;
    for (int ty = 0; ty < height; ty += tile_height)
        for (int tx = 0; tx < width; tx += tile_width)
            for (int y = 0; y < tile_height; ++y)
            {
                const bool camera0 = restore_camera0 && width == 640 && height == 480 &&
                                     tile_width == 320 && tile_height == 240 && tx / 320 == ty / 240;
                // Recover camera 0's three wrapped rows directly in the
                // read address; avoid copying a full RGB565 tile each frame.
                int source_y = tile_height - 1 - y;
                if (camera0) source_y = (source_y + 3) % tile_height;
                const uint16_t *row = source + (size_t)(ty + source_y) * width + tx;
                uint8_t *out = rgb + ((size_t)(ty + y) * width + tx) * 3;
                int x = 0;
#ifdef FRAME_PIXELS_NEON
                for (; x <= tile_width - 16; x += 16)
                {
                    const uint16_t *p = row + tile_width - x - 16;
                    frame_store_rgb565x16(frame_reverse_u16x8(vld1q_u16(p + 8)),
                                          frame_reverse_u16x8(vld1q_u16(p)), out + (size_t)x * 3);
                }
#endif
                for (; x < tile_width; ++x)
                    frame_unpack_rgb565(row + tile_width - 1 - x, out + (size_t)x * 3, 1);
                if (camera0)
                    for (int edge = tile_width - 9; edge < tile_width; ++edge)
                        memcpy(out + edge * 3, out + (tile_width - 10) * 3, 3);
            }
}

#endif
