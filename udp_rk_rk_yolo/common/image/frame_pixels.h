#ifndef FRAME_PIXELS_H
#define FRAME_PIXELS_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h>
#define FRAME_PIXELS_NEON 1

static inline uint16x8_t frame_reverse_u16x8(uint16x8_t pixels)
{
    const uint16x8_t reversed = vrev64q_u16(pixels);
    return vextq_u16(reversed, reversed, 4);
}

static inline uint8x8x3_t frame_expand_rgb565x8(uint16x8_t pixels)
{
    const uint16x8_t red = vshrq_n_u16(pixels, 11);
    const uint16x8_t green = vandq_u16(vshrq_n_u16(pixels, 5), vdupq_n_u16(63));
    const uint16x8_t blue = vandq_u16(pixels, vdupq_n_u16(31));
    uint8x8x3_t output;
    output.val[0] = vmovn_u16(vorrq_u16(vshlq_n_u16(red, 3), vshrq_n_u16(red, 2)));
    output.val[1] = vmovn_u16(vorrq_u16(vshlq_n_u16(green, 2), vshrq_n_u16(green, 4)));
    output.val[2] = vmovn_u16(vorrq_u16(vshlq_n_u16(blue, 3), vshrq_n_u16(blue, 2)));
    return output;
}

static inline void frame_store_rgb565x8(uint16x8_t pixels, uint8_t *rgb)
{
    vst3_u8(rgb, frame_expand_rgb565x8(pixels));
}

static inline void frame_store_rgb565x16(uint16x8_t first, uint16x8_t second, uint8_t *rgb)
{
    const uint8x8x3_t lo = frame_expand_rgb565x8(first);
    const uint8x8x3_t hi = frame_expand_rgb565x8(second);
    uint8x16x3_t output;
    for (int channel = 0; channel < 3; ++channel)
        output.val[channel] = vcombine_u8(lo.val[channel], hi.val[channel]);
    vst3q_u8(rgb, output);
}

static inline uint16x8_t frame_pack_rgb888x8(uint8x8_t red, uint8x8_t green, uint8x8_t blue)
{
    return vorrq_u16(vorrq_u16(vshlq_n_u16(vmovl_u8(vand_u8(red, vdup_n_u8(248))), 8),
                              vshlq_n_u16(vmovl_u8(vand_u8(green, vdup_n_u8(252))), 3)),
                     vmovl_u8(vshr_n_u8(blue, 3)));
}
#endif

static inline uint8_t frame_expand_5bit(unsigned channel)
{
    return (uint8_t)((channel << 3) | (channel >> 2));
}

static inline void frame_unpack_rgb565(const uint16_t *source, uint8_t *rgb, size_t count)
{
    if (!source || !rgb)
        return;
#ifdef FRAME_PIXELS_NEON
    while (count >= 16)
    {
        frame_store_rgb565x16(vld1q_u16(source), vld1q_u16(source + 8), rgb);
        source += 16;
        rgb += 48;
        count -= 16;
    }
    while (count >= 8)
    {
        frame_store_rgb565x8(vld1q_u16(source), rgb);
        source += 8;
        rgb += 24;
        count -= 8;
    }
#endif

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
#ifdef FRAME_PIXELS_NEON
    while (count >= 16)
    {
        const uint8x16x3_t input = vld3q_u8(rgb);
        vst1q_u16(target, frame_pack_rgb888x8(vget_low_u8(input.val[0]),
                                             vget_low_u8(input.val[1]), vget_low_u8(input.val[2])));
        vst1q_u16(target + 8, frame_pack_rgb888x8(vget_high_u8(input.val[0]),
                                                 vget_high_u8(input.val[1]), vget_high_u8(input.val[2])));
        rgb += 48;
        target += 16;
        count -= 16;
    }
    while (count >= 8)
    {
        const uint8x8x3_t input = vld3_u8(rgb);
        const uint16x8_t red = vshlq_n_u16(vmovl_u8(vand_u8(input.val[0], vdup_n_u8(248))), 8);
        const uint16x8_t green = vshlq_n_u16(vmovl_u8(vand_u8(input.val[1], vdup_n_u8(252))), 3);
        const uint16x8_t blue = vmovl_u8(vshr_n_u8(input.val[2], 3));
        vst1q_u16(target, vorrq_u16(vorrq_u16(red, green), blue));
        rgb += 24;
        target += 8;
        count -= 8;
    }
#endif

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
