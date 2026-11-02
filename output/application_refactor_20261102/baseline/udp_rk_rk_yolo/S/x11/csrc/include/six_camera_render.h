#ifndef SIX_CAMERA_RENDER_H
#define SIX_CAMERA_RENDER_H

#include "camera_views.h"
#include <algorithm>
#include <stdint.h>

namespace six_camera {
constexpr int width = 1280, height = 480, tile_width = 320, tile_height = 240;

static inline uint32_t argb(uint16_t pixel) {
    return 0xff000000u | ((pixel & 31u) << 19) |
           (((pixel >> 5) & 63u) << 10) | (((pixel >> 11) & 31u) << 3);
}

static inline const uint16_t* source_row(const CameraView& view,
        const uint16_t* pcie, const uint16_t* udp, int y) {
    const int row = view.network_source ? y : tile_height - 1 - y;
    return (view.network_source ? udp : pcie) +
           (view.source_y + row) * 640 + view.source_x;
}

static inline void overview(uint32_t* pixels, const uint16_t* pcie,
        const uint16_t* udp, const CameraView* target = nullptr, float dim = 1.f) {
    std::fill_n(pixels, width * height, 0xff000000u);
    for (const CameraView& view : camera_views) {
        const bool darken = target && target->mode != view.mode;
        for (int y = 0; y < tile_height; ++y) {
            const uint16_t* source = source_row(view, pcie, udp, y);
            uint32_t* row = pixels + (view.row * tile_height + y) * width + view.column * tile_width;
            for (int x = 0; x < tile_width; ++x) {
                const uint32_t color = argb(source[x]);
                if (!darken) row[x] = color;
                else row[x] = 0xff000000u |
                    (uint32_t(((color >> 16) & 255) * dim) << 16) |
                    (uint32_t(((color >> 8) & 255) * dim) << 8) |
                    uint32_t((color & 255) * dim);
            }
        }
    }
}

static inline void focus(uint32_t* pixels, const uint16_t* pcie,
        const uint16_t* udp, const CameraView& view) {
    std::fill_n(pixels, width * height, 0xff000000u);
    for (int y = 0; y < height; ++y) {
        const uint16_t* source = source_row(view, pcie, udp, y / 2);
        uint32_t* row = pixels + y * width + 320;
        for (int x = 0; x < 640; ++x) row[x] = argb(source[x / 2]);
    }
}
} // namespace six_camera
#endif
