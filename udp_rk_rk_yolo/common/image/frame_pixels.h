#ifndef FRAME_PIXELS_H
#define FRAME_PIXELS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline uint8_t frame_expand_5bit(unsigned channel)
{
    return (uint8_t)((channel << 3) | (channel >> 2));
}

static inline void frame_unpack_rgb565(const uint16_t *source, uint8_t *rgb, size_t count)
{
    if (!source || !rgb)
        return;

    for (; count; --count, rgb += 3)
    {
        const uint16_t pixel = *source++;
        const unsigned green = (pixel >> 5) & 0x3Fu;
        rgb[0] = frame_expand_5bit(pixel >> 11);
        rgb[1] = (uint8_t)((green << 2) | (green >> 4));
        rgb[2] = frame_expand_5bit(pixel & 0x1Fu);
    }
}

static inline void frame_pack_rgb565(const uint8_t *rgb, uint16_t *target, size_t count)
{
    if (!rgb || !target)
        return;

    for (; count; --count, rgb += 3)
        *target++ = (uint16_t)(((rgb[0] & 0xF8u) << 8) |
                              ((rgb[1] & 0xFCu) << 3) | (rgb[2] >> 3));
}

// Rotate the suffix so the selected inclusive column range ends the row.
static inline void frame_move_rgb565_columns(uint16_t *row, int width, int first, int last)
{
    if (!row || first < 0 || first > last || width <= 0 || last >= width - 1)
        return;

    const size_t count = (size_t)(last - first + 1);
    const size_t saved_bytes = count * sizeof(*row);
    const size_t tail_bytes = (size_t)(width - last - 1) * sizeof(*row);
    uint16_t *saved = (uint16_t *)malloc(saved_bytes);
    if (!saved)
    {
        printf("列重排失败：内存分配失败\n");
        return;
    }

    memcpy(saved, row + first, saved_bytes);
    memmove(row + first, row + last + 1, tail_bytes);
    memcpy(row + width - count, saved, saved_bytes);
    free(saved);
}

// Each affected row contributes one contiguous span. Copy rows in ascending
// order so a source span is consumed before it can become a destination.
static inline void frame_rotate_column_rows(uint8_t *image, int width, int height, int first,
                                            int last, int shift, size_t pixel_bytes)
{
    if (!image || width <= 0 || height <= 0 || first < 0 || last >= width ||
        first > last || shift <= 0 || shift >= height)
        return;

    const size_t pitch = (size_t)width * pixel_bytes;
    const size_t span = (size_t)(last - first + 1) * pixel_bytes;
    uint8_t *saved = (uint8_t *)malloc((size_t)shift * span);
    if (!saved)
    {
        printf("列垂直移动失败：内存分配失败\n");
        return;
    }

    uint8_t *column = image + (size_t)first * pixel_bytes;
    for (int row = 0; row < shift; ++row)
        memcpy(saved + row * span, column + row * pitch, span);

    const int retained_rows = height - shift;
    for (int row = 0; row < retained_rows; ++row)
        memcpy(column + row * pitch, column + (row + shift) * pitch, span);

    for (int row = 0; row < shift; ++row)
        memcpy(column + (retained_rows + row) * pitch, saved + row * span, span);
    free(saved);
}

#endif
