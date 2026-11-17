#ifndef M_FRONT_CAMERA_LAYOUT_H
#define M_FRONT_CAMERA_LAYOUT_H

#include "../../../common/image/frame_pixels.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace m_lane
{
// Crop the nine blank columns from the local bottom-right comparison tile.
// Use the same scale on each axis and leave the three other tiles as received.
class LocalFrontPreviewCrop
{
    struct Sample
    {
        int offset;
        unsigned fraction;
    };
    int width_, height_, stride_;
    std::vector<Sample> columns_, rows_;
    std::vector<uint8_t> tile_;

    static Sample sample(double position, int length, int step)
    {
        const int base = std::min(static_cast<int>(position), length - 2);
        return {base * step, static_cast<unsigned>(std::lround((position - base) * 256))};
    }

  public:
    LocalFrontPreviewCrop(int mosaic_width, int mosaic_height)
        : width_(mosaic_width / 2), height_(mosaic_height / 2), stride_(mosaic_width * 3)
    {
        constexpr int left_crop = 9;
        if (width_ <= left_crop + 1 || height_ < 2)
            return;
        const double scale = 1.0 - static_cast<double>(left_crop) / (width_ - 1);
        const double top = (height_ - 1) * (1 - scale) * 0.5;
        columns_.reserve(width_);
        rows_.reserve(height_);
        for (int x = 0; x < width_; ++x)
            columns_.push_back(sample(left_crop + x * scale, width_, 3));
        for (int y = 0; y < height_; ++y)
            rows_.push_back(sample(top + y * scale, height_, stride_));
        tile_.resize(static_cast<size_t>(width_) * height_ * 3);
    }

    void apply(uint8_t *rgb)
    {
        if (!rgb || tile_.empty())
            return;
        uint8_t *start = rgb + static_cast<size_t>(height_) * stride_ + width_ * 3;
        uint8_t *out = tile_.data();
        for (const auto &row : rows_)
        {
            const uint8_t *top = start + row.offset;
            const uint8_t *bottom = top + stride_;
            for (const auto &col : columns_)
            {
                for (int channel = 0; channel < 3; ++channel)
                {
                    const int at = col.offset + channel;
                    const unsigned a = top[at] * (256 - col.fraction) + top[at + 3] * col.fraction;
                    const unsigned b = bottom[at] * (256 - col.fraction) + bottom[at + 3] * col.fraction;
                    *out++ = static_cast<uint8_t>((a * (256 - row.fraction) + b * row.fraction + 32768) >> 16);
                }
            }
        }
        const size_t row_bytes = static_cast<size_t>(width_) * 3;
        for (int y = 0; y < height_; ++y)
            std::memcpy(start + static_cast<size_t>(y) * stride_, tile_.data() + y * row_bytes, row_bytes);
    }
};

// The FPGA writes the same front-center camera to two opposite tiles. A
// half-frame DMA offset changes which diagonal contains that duplicate.
class FrontCameraLayout
{
    bool known_ = false, swapped_ = false;
    unsigned opposing_frames_ = 0;

    static unsigned difference(uint16_t a, uint16_t b)
    {
        return std::abs((a & 31) - (b & 31)) +
               std::abs(((a >> 5) & 63) - ((b >> 5) & 63)) +
               std::abs((a >> 11) - (b >> 11));
    }

  public:
    bool rows_swapped() const { return swapped_; }

    bool observe(const uint16_t *raw, int width, int height)
    {
        const int w = width / 2, h = height / 2;
        unsigned normal = 0, swapped = 0, samples = 0;
        for (int y = h / 8; y < h * 7 / 8; y += 12)
            for (int x = w / 8; x < w * 7 / 8; x += 12)
            {
                const uint16_t *top = raw + y * width + x;
                const uint16_t *bottom = top + h * width;
                normal += difference(top[0], bottom[w]);
                swapped += difference(top[w], bottom[0]);
                ++samples;
            }
        const bool observed_swap = swapped < normal;
        const unsigned match = observed_swap ? swapped : normal;
        const unsigned other = observed_swap ? normal : swapped;
        if (!samples || match > 7 * samples || other < match + 5 * samples || other < 2 * match)
        {
            opposing_frames_ = 0;
            return false;
        }
        if (known_ && observed_swap == swapped_)
        {
            opposing_frames_ = 0;
            return false;
        }
        if (known_ && ++opposing_frames_ < 3)
            return false;
        known_ = true;
        swapped_ = observed_swap;
        opposing_frames_ = 0;
        return true;
    }

    void unpack(const uint16_t *raw, uint8_t *rgb, int width, int height) const
    {
        if (!swapped_)
        {
            frame_unpack_rgb565(raw, rgb, (size_t)width * height);
            return;
        }
        const size_t half = (size_t)width * (height / 2);
        frame_unpack_rgb565(raw + half, rgb, half);
        frame_unpack_rgb565(raw, rgb + 3 * half, half);
    }
};
}

#endif
