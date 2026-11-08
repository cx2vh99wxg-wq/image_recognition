#ifndef S_LANE_ANGLE_CUE_H
#define S_LANE_ANGLE_CUE_H
#include <algorithm>
#include <cmath>
#include <cstdint>
#include "../../../../common/perception/include/curve_heading.h"

namespace lane_cue
{
inline bool valid(unsigned type, unsigned confidence, unsigned wire_angle)
{
    const unsigned degrees = curve_angle_degrees(wire_angle);
    return type <= 2 && confidence >= 60 && degrees <= 90 &&
           (type ? degrees > 0 : wire_angle == 0);
}
inline int signed_degrees(unsigned type, unsigned wire_angle)
{
    const int degrees = (int)curve_angle_degrees(wire_angle);
    return type == 1 ? -degrees : type == 2 ? degrees : 0;
}

constexpr uint32_t WHITE = 0xffffffffu, WARNING_RED = 0xffff3030u, UNKNOWN_GRAY = 0xff7d8696u;
constexpr double SEAM = .435; // Reference only; no horizontal guide is drawn.
struct PictureRect { double x, y, width, height; };
inline PictureRect picture_rect(int width, int height)
{
    const double ratio = 1586. / 2643.; // The supplied rear elevated artwork.
    const double h = std::min(double(height), width / ratio), w = h * ratio;
    return {(width - w) * .5, (height - h) * .5, w, h};
}
inline unsigned warning_sides(unsigned blocked, unsigned left, unsigned right)
{
    // The fresh controller warning is authoritative and may outlive a temporary
    // UNKNOWN line classification during its 700ms display hold.
    (void)left; (void)right;
    return blocked & 3u;
}
// Perspective schematic: lower edges stay fixed beside the car; only the
// upper centerline bends. Unknown direction uses two complete gray straight
// edges; a missing side style has a gray placeholder rather than disappearing.
inline void draw(uint32_t *pixels, int stride, int width, int height, int angle,
                 unsigned left_style, unsigned right_style, unsigned blocked = 0,
                 bool red_phase = false, bool angle_known = true)
{
    if (!pixels || width < 8 || height < 8 || stride < width || (angle_known && (angle < -90 || angle > 90)))
        return;
    const auto rect = picture_rect(width, height);
    const double top = rect.y + rect.height * .17;
    const double seam = rect.y + rect.height * SEAM;
    const double bottom = rect.y + rect.height * .91;
    const double theta = (angle_known ? std::max(-85, std::min(85, angle)) : 0) * .017453292519943295;
    const double limit = rect.width * .28;
    const double bend = std::max(-limit, std::min(limit, (seam - top) * .5 * std::tan(theta)));
    const unsigned warning = warning_sides(blocked, left_style, right_style);
    double dash_phase = 0.;
    for (int y = (int)std::floor(bottom); y >= (int)std::ceil(top); --y)
    {
        const double depth = (y - top) / (bottom - top);
        dash_phase += 1. / (12. + 38. * depth);
        const double t = y < seam ? (seam - y) / (seam - top) : 0.;
        const double center = width * .5 + bend * t * t;
        const double half_width = rect.width * (.15 + .27 * depth);
        for (int side = 0; side < 2; ++side)
        {
            const unsigned style = side ? right_style : left_style;
            const bool alarm = (warning & (1u << side)) != 0;
            const bool known = angle_known && (style == 1 || style == 2);
            if (!alarm && known && style == 2 && dash_phase - std::floor(dash_phase) >= .58)
                continue;
            const int x = (int)std::lround(center + (side ? half_width : -half_width));
            const uint32_t color = alarm ? (red_phase ? WARNING_RED : WHITE) : known ? WHITE : UNKNOWN_GRAY;
            const int radius = alarm ? 4 : 2;
            for (int dx = -radius; dx <= radius; ++dx)
                if (x + dx >= 0 && x + dx < width)
                    pixels[y * stride + x + dx] = color;
        }
    }
}
}
#endif
