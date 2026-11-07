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

// Schematic parallel lane edges. Their far-end tangent follows the received
// angle exactly; this is an angle cue, not a reconstruction of road geometry.
// Style 1 = solid, 2 = dashed, 0 = unknown (muted dotted).
inline void draw(uint32_t *pixels, int stride, int width, int height, int angle,
                 unsigned left_style, unsigned right_style)
{
    if (!pixels || width < 8 || height < 8 || stride < width || std::abs(angle) > 90)
        return;
    const double theta = angle * .017453292519943295;
    const double length = height * .48, start = height * .52, center = width * .5;
    const double half_width = width / 6.;
    const int samples = (int)std::ceil(length * 2);
    for (int i = 0; i <= samples; ++i)
    {
        const double s = length * i / samples, phi = theta * i / samples;
        const double x = center + (angle ? length / theta * (1. - std::cos(phi)) : 0.);
        const double y = start - (angle ? length / theta * std::sin(phi) : s);
        for (int side = 0; side < 2; ++side)
        {
            const unsigned style = side ? right_style : left_style;
            if ((style == 2 && (int)s % 28 >= 16) || (!style && (int)s % 12 >= 4))
                continue;
            const double normal = side ? half_width : -half_width;
            const int px = (int)std::lround(x + normal * std::cos(phi));
            const int py = (int)std::lround(y + normal * std::sin(phi));
            for (int dy = -1; dy <= 1; ++dy)
                for (int dx = -1; dx <= 1; ++dx)
                    if (px + dx >= 0 && px + dx < width && py + dy >= 0 && py + dy < height)
                        pixels[(py + dy) * stride + px + dx] = style ? 0xff00ff00u : 0xff527f69u;
        }
    }
}
}
#endif
