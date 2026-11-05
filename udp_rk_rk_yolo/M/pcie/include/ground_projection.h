#ifndef M_GROUND_PROJECTION_H
#define M_GROUND_PROJECTION_H

#include "front_radial.h"
#include <algorithm>

namespace m_lane
{
// Four normalized, undistorted image points in TL, TR, BR, BL order,
// followed by the measured ground rectangle's length / width ratio.
class GroundProjection
{
    double to_ground_[8] = {}, to_image_[8] = {}, aspect_ = 1.;
    bool enabled_ = false, valid_ = true;

    static bool solve(const ImagePoint (&source)[4], const ImagePoint (&target)[4],
                      double (&out)[8])
    {
        double a[8][9] = {};
        for (int i = 0; i < 4; ++i)
        {
            const auto p = source[i], q = target[i];
            double *x = a[i * 2], *y = a[i * 2 + 1];
            x[0] = y[3] = p.x;
            x[1] = y[4] = p.y;
            x[2] = y[5] = 1.;
            x[6] = -q.x * p.x;
            x[7] = -q.x * p.y;
            x[8] = q.x;
            y[6] = -q.y * p.x;
            y[7] = -q.y * p.y;
            y[8] = q.y;
        }
        for (int i = 0; i < 8; ++i)
        {
            int pivot = i;
            for (int j = i + 1; j < 8; ++j)
                if (std::abs(a[j][i]) > std::abs(a[pivot][i]))
                    pivot = j;
            if (std::abs(a[pivot][i]) < 1e-9)
                return false;
            std::swap(a[i], a[pivot]);
            const double divisor = a[i][i];
            for (int k = i; k < 9; ++k)
                a[i][k] /= divisor;
            for (int j = 0; j < 8; ++j)
                if (j != i)
                {
                    const double factor = a[j][i];
                    for (int k = i; k < 9; ++k)
                        a[j][k] -= factor * a[i][k];
                }
        }
        for (int i = 0; i < 8; ++i)
            out[i] = a[i][8];
        return true;
    }
    static bool project(ImagePoint &p, const double (&m)[8])
    {
        const double w = 1. + m[6] * p.x + m[7] * p.y;
        if (!std::isfinite(w) || std::abs(w) < 1e-7)
            return false;
        p = {(m[0] * p.x + m[1] * p.y + m[2]) / w,
             (m[3] * p.x + m[4] * p.y + m[5]) / w};
        return std::isfinite(p.x) && std::isfinite(p.y);
    }

  public:
    explicit GroundProjection(const char *text = nullptr)
    {
        if (!text)
            return;
        enabled_ = true;
        valid_ = false;
        double values[9];
        for (int i = 0; i < 9; ++i)
        {
            char *end = nullptr;
            values[i] = std::strtod(text, &end);
            if (end == text || !std::isfinite(values[i]) || *end != (i == 8 ? '\0' : ','))
                return;
            text = end + 1;
        }
        aspect_ = values[8];
        if (aspect_ < .25 || aspect_ > 10.)
            return;
        ImagePoint corners[4];
        for (int i = 0; i < 4; ++i)
        {
            corners[i] = {values[i * 2], values[i * 2 + 1]};
            if (corners[i].x < -.25 || corners[i].x > 1.25 ||
                corners[i].y < -.25 || corners[i].y > 1.25)
                return;
        }
        if (corners[1].x <= corners[0].x || corners[2].x <= corners[3].x ||
            corners[3].y <= corners[0].y || corners[2].y <= corners[1].y)
            return;
        for (int i = 0; i < 4; ++i)
        {
            const auto a = corners[i], b = corners[(i + 1) % 4], c = corners[(i + 2) % 4];
            if ((b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x) <= .001)
                return;
        }
        const ImagePoint rectangle[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
        valid_ = solve(corners, rectangle, to_ground_) && solve(rectangle, corners, to_image_);
    }

    static GroundProjection for_camera(const char *override_text, const char *path)
    {
        if (override_text)
            return GroundProjection(override_text);
        std::FILE *file = std::fopen(path, "r");
        if (!file)
            return GroundProjection(errno == ENOENT ? nullptr : "");
        char value[256] = {};
        char trailing;
        const bool single_value = std::fscanf(file, "%255s %c", value, &trailing) == 1 &&
                                  !std::ferror(file);
        std::fclose(file);
        return GroundProjection(single_value ? value : "");
    }

    bool enabled() const { return enabled_; }
    bool valid() const { return valid_; }
    double aspect() const { return aspect_; }

    bool ground_at(ImagePoint &p, int width, int height) const
    {
        if (!enabled_ || !valid_ || width < 2 || height < 2)
            return false;
        p = {p.x / (width - 1), p.y / (height - 1)};
        if (!project(p, to_ground_) || p.x < -.25 || p.x > 1.25 || p.y < -.05 || p.y > 1.05)
            return false;
        const double scale = width * .5;
        p = {(p.x - .5) * scale, p.y * scale * aspect_};
        return true;
    }
    bool image_at(ImagePoint &p, int width, int height) const
    {
        if (!enabled_ || !valid_ || width < 2 || height < 2 || !project(p, to_image_))
            return false;
        p = {p.x * (width - 1), p.y * (height - 1)};
        return true;
    }
};
}
#endif
