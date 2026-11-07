#ifndef M_FRONT_RADIAL_PREVIEW_H
#define M_FRONT_RADIAL_PREVIEW_H

#include "front_radial.h"
#include "ground_projection.h"
#include "lane_curve_geometry.h"
#include "../../../common/image/frame_pixels.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

namespace m_lane
{
// The normalized mosaic contains front center in the top-left and bottom-right.
// Sample it once and use the same corrected tile in both places.
class FrontRadialPreview
{
    static constexpr uint32_t outside = UINT32_MAX;
    struct Sample
    {
        uint32_t offset;
        uint16_t fx, fy;
    };
    int width_, height_, stride_;
    bool ground_view_;
    double scale_ = 1.;
    std::vector<Sample> samples_;
    std::vector<uint32_t> corrected_at_raw_;
    std::vector<uint8_t> tile_;

  public:
    FrontRadialPreview(int mosaic_width, int mosaic_height, const FrontRadial &camera,
                       const GroundProjection *ground = nullptr, bool inference_input = false)
        : width_(mosaic_width / 2), height_(mosaic_height / 2), stride_(mosaic_width * 3),
          ground_view_(ground != nullptr)
    {
        if (!camera.valid() || width_ < 2 || height_ < 2 ||
            (ground ? !ground->valid() || !ground->enabled() : !camera.enabled() && !inference_input))
            return;
        const double cx = (width_ - 1) * .5, cy = (height_ - 1) * .5;
        const bool barrel = camera.coefficient() < 0.;
        // For barrel distortion the side midpoints limit a centered crop.
        // For pincushion distortion the corner limits both axes.
        ImagePoint x_limit{2 * cx, barrel ? cy : 2 * cy};
        ImagePoint y_limit{barrel ? cx : 2 * cx, 2 * cy};
        if (!camera.undistort(x_limit, width_, height_) ||
            !camera.undistort(y_limit, width_, height_))
            return;
        // One scale keeps the largest centered 4:3 crop in the source and
        // preserves its aspect ratio when filling the 320x240 tile.
        scale_ = std::max(cx / (x_limit.x - cx), cy / (y_limit.y - cy));
        samples_.reserve((size_t)width_ * height_);
        tile_.resize((size_t)width_ * height_ * 3);
        size_t valid_samples = 0;
        constexpr double edge_epsilon = 1e-7;
        for (int y = 0; y < height_; ++y)
            for (int x = 0; x < width_; ++x)
            {
                ImagePoint input{cx + (x - cx) / scale_, cy + (y - cy) / scale_};
                bool projected = true;
                if (ground)
                {
                    // Two calibration lengths, with the same ground scale along both axes.
                    input = {.5 + (x - cx) * 2. * ground->aspect() / (height_ - 1),
                             -.8 + 2. * y / (height_ - 1)};
                    projected = ground->image_at(input, width_, height_);
                }
                if (!projected || !camera.distort(input, width_, height_) || input.x < -edge_epsilon ||
                    input.x > width_ - 1 + edge_epsilon || input.y < -edge_epsilon ||
                    input.y > height_ - 1 + edge_epsilon)
                {
                    if (ground)
                    {
                        samples_.push_back({outside, 0, 0});
                        continue;
                    }
                    samples_.clear();
                    return;
                }
                input.x = std::max(0., std::min(input.x, (double)(width_ - 1)));
                input.y = std::max(0., std::min(input.y, (double)(height_ - 1)));
                const int ix = std::min((int)input.x, width_ - 2);
                const int iy = std::min((int)input.y, height_ - 2);
                samples_.push_back({(uint32_t)(iy * stride_ + 3 * ix),
                                    (uint16_t)std::lround(256 * (input.x - ix)),
                                    (uint16_t)std::lround(256 * (input.y - iy))});
                ++valid_samples;
            }
        if (!valid_samples)
            samples_.clear();
        if (inference_input && !ground && enabled())
        {
            corrected_at_raw_.reserve(samples_.size());
            for (int y = 0; y < height_; ++y)
                for (int x = 0; x < width_; ++x)
                {
                    ImagePoint point{(double)x, (double)y};
                    if (!camera.undistort(point, width_, height_))
                    {
                        corrected_at_raw_.push_back(uint32_t{outside});
                        continue;
                    }
                    point = {cx + (point.x - cx) * scale_, cy + (point.y - cy) * scale_};
                    const int sx = (int)std::lround(point.x), sy = (int)std::lround(point.y);
                    corrected_at_raw_.push_back(sx < 0 || sx >= width_ || sy < 0 || sy >= height_
                                                   ? outside : (uint32_t)(sy * width_ + sx));
                }
        }
    }

    bool enabled() const { return !samples_.empty(); }
    double display_scale() const { return scale_; }

    // Place predictions back in the raw top-left tile for the existing overlay,
    // perspective projection and steering calibration. Other mosaic tiles stay clear.
    void restore_front_masks(const float *lane, const float *road, float *raw_lane,
                             float *raw_road) const
    {
        const size_t tile_size = (size_t)width_ * height_;
        const int w = stride_ / 3;
        const size_t mosaic_size = (size_t)w * height_ * 2;
        for (size_t i = 0; i < corrected_at_raw_.size(); ++i)
        {
            const auto sample = corrected_at_raw_[i];
            const size_t dst = (i / width_) * w + i % width_;
            raw_lane[dst] = sample == outside ? 0.f : lane[sample];
            raw_road[dst] = sample == outside ? 0.f : road[sample];
            raw_road[mosaic_size + dst] = sample == outside ? 0.f : road[tile_size + sample];
        }
    }

    void render(const uint8_t *mosaic, uint8_t *dst) const
    {
        for (const auto &s : samples_)
        {
            if (s.offset == outside)
            {
                *dst++ = 0;
                *dst++ = 0;
                *dst++ = 0;
                continue;
            }
            const uint8_t *a = mosaic + s.offset, *b = a + stride_;
            for (int c = 0; c < 3; ++c)
            {
                const unsigned top = a[c] * (256u - s.fx) + a[c + 3] * s.fx;
                const unsigned bottom = b[c] * (256u - s.fx) + b[c + 3] * s.fx;
                *dst++ = (uint8_t)((top * (256u - s.fy) + bottom * s.fy + 32768u) >> 16);
            }
        }
    }

    bool remap_masks(const float *lane, const float *drivable, int w, int h, float *out_lane,
                     uint8_t *out_green) const
    {
        if (!ground_view_ || !enabled() || w != stride_ / 3 || h != height_ * 2)
            return false;
        const size_t image_size = (size_t)w * h;
        for (size_t i = 0; i < samples_.size(); ++i)
        {
            const auto &s = samples_[i];
            if (s.offset == outside)
            {
                out_lane[i] = 0;
                out_green[i] = 0;
                continue;
            }
            const size_t at = s.offset / 3;
            const auto sample = [&](const float *p) {
                p += at;
                const float fx = s.fx * (1.f / 256.f), fy = s.fy * (1.f / 256.f);
                const float a = p[0] + (p[1] - p[0]) * fx;
                const float b = p[w] + (p[w + 1] - p[w]) * fx;
                return a + (b - a) * fy;
            };
            out_lane[i] = sample(lane);
            out_green[i] = sample(drivable + image_size) > sample(drivable);
        }
        return true;
    }

    void apply(uint8_t *mosaic, uint16_t *udp_frame = nullptr,
               const YOLOLineStyleResult *pair = nullptr, const PointLanePair *points = nullptr)
    {
        if (!mosaic || !enabled())
            return;
        render(mosaic, tile_.data());
        const auto mark = [&](int x, int y, uint8_t b, uint8_t g, uint8_t r) {
            if (x < 0 || x >= width_ || y < 0 || y >= height_)
                return;
            uint8_t *pixel = tile_.data() + ((size_t)y * width_ + x) * 3;
            pixel[0] = b;
            pixel[1] = g;
            pixel[2] = r;
        };
        const auto mark_center = [&](double x, double y) {
            for (int dy = -2; dy <= 2; ++dy)
                for (int dx = -2; dx <= 2; ++dx)
                    mark((int)std::lround(x) + dx, (int)std::lround(y) + dy, 255, 255, 255);
        };
        if (ground_view_ && points && points->sampled)
        {
            for (const auto &p : points->left)
                if (p.y >= points->first && p.y <= points->last)
                    for (int dx = -1; dx <= 1; ++dx)
                        mark((int)std::lround(p.x) + dx, p.y, 255, 240, 48);
            for (const auto &p : points->right)
                if (p.y >= points->first && p.y <= points->last)
                    for (int dx = -1; dx <= 1; ++dx)
                        mark((int)std::lround(p.x) + dx, p.y, 58, 222, 255);
            for (size_t i = 0; i < points->left_x.size(); ++i)
                mark_center((points->left_x[i] + points->right_x[i]) * .5,
                            points->first + (points->last - points->first) * .25 * i);
        }
        else if (ground_view_ && pair)
        {
            double first, last;
            if (ego_curve_range(*pair, first, last) && last > first)
            {
                for (int y = (int)first; y <= (int)last; ++y)
                {
                    double left, right;
                    if (!ego_curve_bounds(*pair, y, left, right))
                        continue;
                    const int l = (int)std::lround(left), r = (int)std::lround(right);
                    for (int dx = -1; dx <= 1; ++dx)
                    {
                        mark(l + dx, y, 255, 240, 48);
                        mark(r + dx, y, 58, 222, 255);
                    }
                }
                for (int i = 0; i < 5; ++i)
                {
                    const double y = first + (last - first) * i / 4.;
                    double left, right;
                    if (!ego_curve_bounds(*pair, y, left, right))
                        continue;
                    mark_center((left + right) * .5, y);
                }
            }
        }
        if (ground_view_ && width_ >= 42 && height_ >= 22)
        {
            // Visible test badge, also a deterministic RGB565 marker for the S receiver probe.
            static const uint16_t letters[7] = {0x7451, 0x457b, 0x4555, 0x4451,
                                                 0x4451, 0x4451, 0x7451};
            for (int y = 4; y < 22; ++y)
                for (int x = 4; x < 42; ++x)
                {
                    const int row = (y - 6) / 2, col = (x - 7) / 2;
                    const bool lit = y >= 6 && y < 20 && x >= 7 && x < 39 &&
                                     (letters[row] & (1u << (15 - col)));
                    uint8_t *p = tile_.data() + ((size_t)y * width_ + x) * 3;
                    p[0] = p[1] = p[2] = lit ? 255 : 0;
                }
        }
        const size_t row = (size_t)width_ * 3;
        for (int y = 0; y < height_; ++y)
        {
            const uint8_t *line = tile_.data() + y * row;
            std::memcpy(mosaic + y * stride_, line, row);
            if (udp_frame)
                frame_pack_rgb565(line, udp_frame + (height_ + y) * (stride_ / 3) + width_, width_);
            else
                std::memcpy(mosaic + (height_ + y) * stride_ + row, line, row);
        }
    }
};
}

#endif
