#ifndef S_VEHICLE_ARTWORK_H
#define S_VEHICLE_ARTWORK_H
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace vehicle_artwork
{
constexpr int WIDTH = 320, HEIGHT = 480;
// Decode once, preserve the supplied image's aspect ratio and center its canvas.
// Replicate background edge pixels into narrow letterbox margins, without
// cropping or stretching the car. Only the small ARGB panel stays resident.
inline std::vector<uint32_t> prepare(const uint8_t *rgba, int width, int height)
{
    if (!rgba || width < 1 || height < 1) return {};
    const double scale = std::min(double(WIDTH) / width, double(HEIGHT) / height);
    const double left = (WIDTH - width * scale) * .5, top = (HEIGHT - height * scale) * .5;
    std::vector<uint32_t> out(WIDTH * HEIGHT);
    for (int y = 0; y < HEIGHT; ++y)
        for (int x = 0; x < WIDTH; ++x)
        {
            int sx = std::max(0, std::min(width - 1, (int)std::floor((x - left) / scale)));
            int sy = std::max(0, std::min(height - 1, (int)std::floor((y - top) / scale)));
            const auto *p = rgba + ((size_t)sy * width + sx) * 4;
            const unsigned a = p[3];
            const unsigned r = (p[0] * a + 8 * (255 - a)) / 255;
            const unsigned g = (p[1] * a + 11 * (255 - a)) / 255;
            const unsigned b = (p[2] * a + 32 * (255 - a)) / 255;
            out[y * WIDTH + x] = 0xff000000u | r << 16 | g << 8 | b;
        }
    return out;
}
inline void person_glow(uint32_t *pixels, int stride, int x, int y, bool bright)
{
    static const auto weights = [] {
        std::vector<unsigned> v(80 * 80 + 1);
        for (size_t i = 0; i < v.size(); ++i)
            v[i] = (unsigned)std::max(0., 255. * (1. - std::sqrt((double)i) / 80.));
        return v;
    }();
    for (int py = std::max(0, y - 80); py < std::min(HEIGHT, y + 80); ++py)
        for (int px = std::max(0, x - 80); px < std::min(WIDTH, x + 80); ++px)
        {
            const int dx = px - x, dy = py - y, distance = dx * dx + dy * dy;
            if (distance >= 80 * 80) continue;
            const unsigned a = weights[distance] * (bright ? 204u : 77u) / 255;
            const uint32_t old = pixels[py * stride + px];
            const unsigned r = (old >> 16) & 255, g = (old >> 8) & 255, b = old & 255;
            pixels[py * stride + px] = 0xff000000u | (r + (255 - r) * a / 255) << 16 |
                                      (g * (255 - a) / 255) << 8 | b * (255 - a) / 255;
        }
}
}
#endif
