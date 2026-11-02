#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <vector>

#ifndef CAMERA_ORIENTATION_SOURCE
#define CAMERA_ORIENTATION_SOURCE "../include/camera_orientation.h"
#endif
#include CAMERA_ORIENTATION_SOURCE

static void verify_frame(int columns, int rows, int tile_width, int tile_height) {
    const int width = columns * tile_width, height = rows * tile_height;
    const int guard = 16;
    std::vector<uint16_t> storage(width * height + 2 * guard, 0xa55a);
    uint16_t* frame = storage.data() + guard;
    for (int i = 0; i < width * height; ++i) frame[i] = (i * 109 + width * 17) & 65535;
    const std::vector<uint16_t> original = storage;
    std::vector<uint16_t> expected = original;
    // Independent destination-to-source mapping, rather than in-place swaps.
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int tile_x = x / tile_width * tile_width;
            const int tile_y = y / tile_height * tile_height;
            const int source_x = tile_x + tile_width - 1 - (x - tile_x);
            const int source_y = tile_y + tile_height - 1 - (y - tile_y);
            expected[guard + y * width + x] = original[guard + source_y * width + source_x];
        }
    }
    rotate_rgb565_tiles_180(frame, width, height, tile_width, tile_height);
    assert(storage == expected);
    rotate_rgb565_tiles_180(frame, width, height, tile_width, tile_height);
    assert(storage == original);
}

int main() {
    int cases = 0;
    for (int w = 1; w <= 11; ++w)
        for (int h = 1; h <= 11; ++h)
            for (int nx = 1; nx <= 3; ++nx)
                for (int ny = 1; ny <= 3; ++ny) {
                    verify_frame(nx, ny, w, h);
                    ++cases;
                }
    verify_frame(2, 2, 320, 240); ++cases;
    verify_frame(3, 2, 320, 240); ++cases;
    uint16_t pixel = 0x1234;
    const int invalid[][4] = {{0,1,1,1},{1,0,1,1},{1,1,0,1},{1,1,1,0},
                              {-1,1,1,1},{3,2,2,1},{2,3,1,2}};
    for (const auto& dimensions : invalid) {
        rotate_rgb565_tiles_180(&pixel, dimensions[0], dimensions[1], dimensions[2], dimensions[3]);
        assert(pixel == 0x1234);
        ++cases;
    }
    rotate_rgb565_tiles_180(nullptr, 640, 480, 320, 240);
    std::printf("PASS camera orientation: %d geometries/invalid inputs; guarded buffers and twice-rotation identity\n", cases);
}
