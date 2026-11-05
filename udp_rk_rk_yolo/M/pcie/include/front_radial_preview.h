#ifndef M_FRONT_RADIAL_PREVIEW_H
#define M_FRONT_RADIAL_PREVIEW_H

#include "front_radial.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

namespace m_lane
{
// The first tile contains the front camera. The map samples only that tile;
// the working output is separate so remapping cannot overwrite its own input.
class FrontRadialPreview
{
    struct Sample
    {
        uint32_t offset;
        uint16_t fx, fy;
    };
    int width_, height_, stride_;
    double scale_ = 1.;
    std::vector<Sample> samples_;
    std::vector<uint8_t> tile_;

  public:
    FrontRadialPreview(int mosaic_width, int mosaic_height, const FrontRadial &camera)
        : width_(mosaic_width / 2), height_(mosaic_height / 2), stride_(mosaic_width * 3)
    {
        if (!camera.valid() || !camera.enabled() || width_ < 2 || height_ < 2)
            return;
        const double cx = (width_ - 1) * .5, cy = (height_ - 1) * .5;
        ImagePoint corner{(double)(width_ - 1), (double)(height_ - 1)};
        if (!camera.undistort(corner, width_, height_))
            return;
        // Fit the rectified field of view into the same tile. The unused
        // border is black instead of losing the camera's corner content.
        scale_ = std::min(1., std::min(cx / (corner.x - cx), cy / (corner.y - cy)));
        samples_.reserve((size_t)width_ * height_);
        tile_.resize((size_t)width_ * height_ * 3);
        constexpr double edge_epsilon = 1e-7;
        for (int y = 0; y < height_; ++y)
            for (int x = 0; x < width_; ++x)
            {
                ImagePoint input{cx + (x - cx) / scale_, cy + (y - cy) / scale_};
                if (!camera.distort(input, width_, height_) || input.x < -edge_epsilon ||
                    input.x > width_ - 1 + edge_epsilon || input.y < -edge_epsilon ||
                    input.y > height_ - 1 + edge_epsilon)
                {
                    samples_.push_back({UINT32_MAX, 0, 0});
                    continue;
                }
                input.x = std::max(0., std::min(input.x, (double)(width_ - 1)));
                input.y = std::max(0., std::min(input.y, (double)(height_ - 1)));
                const int ix = std::min((int)input.x, width_ - 2);
                const int iy = std::min((int)input.y, height_ - 2);
                samples_.push_back({(uint32_t)(iy * stride_ + 3 * ix),
                                    (uint16_t)std::lround(256 * (input.x - ix)),
                                    (uint16_t)std::lround(256 * (input.y - iy))});
            }
    }

    bool enabled() const { return !samples_.empty(); }
    double display_scale() const { return scale_; }

    void apply(uint8_t *mosaic)
    {
        if (!mosaic || !enabled())
            return;
        uint8_t *dst = tile_.data();
        for (const auto &s : samples_)
        {
            if (s.offset == UINT32_MAX)
            {
                dst[0] = dst[1] = dst[2] = 0;
                dst += 3;
            }
            else
            {
                const uint8_t *a = mosaic + s.offset, *b = a + stride_;
                for (int c = 0; c < 3; ++c)
                {
                    const unsigned top = a[c] * (256u - s.fx) + a[c + 3] * s.fx;
                    const unsigned bottom = b[c] * (256u - s.fx) + b[c + 3] * s.fx;
                    *dst++ = (uint8_t)((top * (256u - s.fy) + bottom * s.fy + 32768u) >> 16);
                }
            }
        }
        const size_t row = (size_t)width_ * 3;
        for (int y = 0; y < height_; ++y)
            std::memcpy(mosaic + y * stride_, tile_.data() + y * row, row);
    }
};
}

#endif
