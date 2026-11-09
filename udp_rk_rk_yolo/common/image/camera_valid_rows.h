#ifndef CAMERA_VALID_ROWS_H
#define CAMERA_VALID_ROWS_H
#include <stdint.h>
#include <stddef.h>
#include <string.h>

// The shipped 2x2/3-camera FPGA stream has two trailing guard regions per
// tile: an invalid last row, plus an invalid right half of the penultimate
// row. Its values vary (BBWW, partial BBWW, stale pixels, arbitrary words).
// This is a transport-validity boundary, NOT a color-detection problem.
// A >=90% black/white heuristic leaked 122/160 and 142/160 extreme pixels
// on real M frames on 2026-11-09. Always repair that known invalid
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
            memcpy(tail, pixels + (size_t)(end - 3) * width + x,
                   (size_t)(tile_width / 2) * sizeof(*pixels));
        }
        memcpy(pixels + (size_t)(end - 1) * width,
               pixels + (size_t)(end - 2) * width, (size_t)width * sizeof(*pixels));
    }
}
#endif
