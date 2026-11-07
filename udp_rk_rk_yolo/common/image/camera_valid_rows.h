#ifndef CAMERA_VALID_ROWS_H
#define CAMERA_VALID_ROWS_H
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// The shipped 2x2/3-camera FPGA stream has two trailing guard regions per
// tile: an invalid last row, plus alternating 0xffff/0x0000 words in the
// right half of the penultimate row (152/160 pixels observed). Repair that
// half from the preceding valid row BEFORE extending the final row. Using
// the original penultimate row as the source propagates its checkerboard.
// Apply before rotation/lens correction/inference/publishing so both boards
// use the same raw-camera coordinates and keep the existing image size.
// Each tile's first eight columns are outside its own valid 312-column
// payload: white transport padding on the left tile, left-camera spill on
// the right tile. Extend each tile's OWN first valid pixel into this margin
// instead of allowing a neighboring camera into its border. X>=8 is intact,
// so the current lens/IPM calibration keeps its existing coordinate grid.
static inline void camera_extend_padding_rows(uint16_t *pixels, int width, int height,
                                              int tile_height)
{
    if (!pixels || width < 4 || width % 4 != 0 || tile_height < 3 ||
        height < tile_height || height % tile_height != 0)
        return;
    const int tile_width = width / 2;
    if (tile_width >= 16)
        for (int y = 0; y < height; ++y)
            for (int tile_x = 0; tile_x < width; tile_x += tile_width)
            {
                uint16_t *row = pixels + (size_t)y * width + tile_x;
                const uint16_t own_edge = row[8];
                for (int x = 0; x < 8; ++x)
                    row[x] = own_edge;
            }
    for (int end = tile_height; end <= height; end += tile_height)
    {
        for (int tile_x = 0; tile_x < width; tile_x += tile_width)
        {
            const size_t x = (size_t)tile_x + tile_width / 2;
            uint16_t *tail = pixels + (size_t)(end - 2) * width + x;
            const int count = tile_width / 2;
            int black = 0, white = 0, transitions = 0;
            for (int i = 0; i < count; ++i)
            {
                black += tail[i] == 0;
                white += tail[i] == 65535;
                if (i && (tail[i] == 0 || tail[i] == 65535) &&
                    (tail[i - 1] == 0 || tail[i - 1] == 65535))
                    transitions += tail[i] != tail[i - 1];
            }
            // Observed signature: eight valid pixels followed by BBWW repeats.
            // A dark/bright real road edge alone is not this transport pattern.
            if (black + white >= count * 9 / 10 && black >= count / 4 &&
                white >= count / 4 && transitions >= count / 3)
                memcpy(tail, pixels + (size_t)(end - 3) * width + x,
                       (size_t)count * sizeof(*pixels));
        }
        memcpy(pixels + (size_t)(end - 1) * width,
               pixels + (size_t)(end - 2) * width, (size_t)width * sizeof(*pixels));
    }
}
#endif
