#ifndef M_FRONT_CAMERA_LAYOUT_H
#define M_FRONT_CAMERA_LAYOUT_H

#include "../../../common/image/frame_pixels.h"
#include <cstdlib>

namespace m_lane
{
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
