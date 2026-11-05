#ifndef M_FRONT_RADIAL_H
#define M_FRONT_RADIAL_H

#include <cmath>
#include <cerrno>
#include <cstdio>
#include <cstdlib>

namespace m_lane
{
struct ImagePoint
{
    double x, y;
};

// Division model on the front camera tile. Radius is normalized by half its width.
class FrontRadial
{
    double lambda_;
    bool valid_;

  public:
    explicit FrontRadial(double lambda = 0)
        : lambda_(lambda), valid_(std::isfinite(lambda) && lambda >= -.35 && lambda <= .15)
    {
    }

    static FrontRadial from_text(const char *text)
    {
        if (!text)
            return FrontRadial();
        char *end = nullptr;
        const double lambda = std::strtod(text, &end);
        if (end == text || *end)
            return FrontRadial(1.);
        return FrontRadial(lambda);
    }

    static FrontRadial for_camera(const char *override_text, const char *path)
    {
        if (override_text)
            return from_text(override_text);
        std::FILE *file = std::fopen(path, "r");
        if (!file)
            return FrontRadial(errno == ENOENT ? 0. : 1.);
        char value[48] = {};
        char trailing;
        const bool single_value = std::fscanf(file, "%47s %c", value, &trailing) == 1 &&
                                  !std::ferror(file);
        std::fclose(file);
        return single_value ? from_text(value) : FrontRadial(1.);
    }

    bool valid() const { return valid_; }
    bool enabled() const { return !valid_ || lambda_ != 0; }
    double coefficient() const { return lambda_; }

    bool undistort(ImagePoint &p, int width, int height) const
    {
        if (!valid_ || width < 2 || height < 2)
            return false;
        const double cx = (width - 1) * .5, cy = (height - 1) * .5;
        const double x = p.x - cx, y = p.y - cy;
        const double r2 = (x * x + y * y) / (width * width * .25);
        const double denominator = 1. + lambda_ * r2;
        if (!std::isfinite(denominator) || denominator <= .25 || 1. - lambda_ * r2 <= 0)
            return false;
        p = {cx + x / denominator, cy + y / denominator};
        return true;
    }

    // Pull sampling an output image uses the opposite direction.
    bool distort(ImagePoint &p, int width, int height) const
    {
        if (!valid_ || width < 2 || height < 2)
            return false;
        const double cx = (width - 1) * .5, cy = (height - 1) * .5;
        const double x = p.x - cx, y = p.y - cy;
        const double r2 = (x * x + y * y) / (width * width * .25);
        const double discriminant = 1. - 4. * lambda_ * r2;
        if (!std::isfinite(discriminant) || discriminant <= 0)
            return false;
        const double scale = 2. / (1. + std::sqrt(discriminant));
        p = {cx + x * scale, cy + y * scale};
        return true;
    }
};
}
#endif
