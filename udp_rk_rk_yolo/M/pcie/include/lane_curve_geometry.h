#ifndef M_LANE_CURVE_GEOMETRY_H
#define M_LANE_CURVE_GEOMETRY_H
#include "line_style.h"
#include "front_radial.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
namespace m_lane
{
/* 在已选本车道走廊上估计图像航向候选值，
 * 并非标定后的道路角或方向盘角。
 * 它不修改分割图与实虚判定；几何未知时置信度为零。 */
struct EgoCurve
{
    uint32_t type = 0, confidence = 0, angle = 0;
    float shift_px = 0, span_px = 0;
    unsigned reason = 1;
};
inline double curve_x(const YOLOLineBoundary &b, double y, double height)
{
    double t = (y - height * .95) / (height * .52);
    return b.fit_a + b.fit_b * t + b.fit_c * t * t;
}
inline bool ego_curve_range(const YOLOLineStyleResult &r, double &first, double &last)
{
    if (r.width < 80 || r.height < 60 || !r.road_area || r.object_count < 2)
        return false;
    const auto &l = r.left;
    const auto &q = r.right;
    if (l.last_y <= l.first_y || q.last_y <= q.first_y || l.last_y >= r.height ||
        q.last_y >= r.height)
        return false;
    first = std::max(l.first_y, q.first_y);
    last = std::min(l.last_y, q.last_y);
    return true;
}
inline bool ego_curve_bounds(const YOLOLineStyleResult &r, double y, double &left, double &right)
{
    left = curve_x(r.left, y, r.height);
    right = curve_x(r.right, y, r.height);
    return std::isfinite(left) && std::isfinite(right) && left >= -.15 * r.width &&
           right <= 1.15 * r.width && right - left >= r.width * .025 &&
           right - left <= r.width * 1.1;
}
inline EgoCurve fit_ego_centers(const double (&centers)[5], double span, double source_span,
                                int width, int height)
{
    EgoCurve out;
    out.span_px = (float)source_span;
    const double step = span / 4.;
    // Fit x = a + b*u + c*u*u at u = 2, 1, 0, -1, -2 (positive forward).
    // Symmetry gives closed-form coefficients for the first and second derivatives.
    const double slope = (2. * (centers[0] - centers[4]) + (centers[1] - centers[3])) /
                         (10. * step);
    out.shift_px = (float)(slope * span);
    const double straight_slope = std::max(width * .02 / span, .10);
    const double threshold = straight_slope * span;
    /* 走廊先明显朝一侧、再转向另一侧时，
     * 不赋予高置信的单一弯向；禁止互换远、近采样轨迹来凑方向。 */
    double a = centers[0] - centers[2], b = centers[2] - centers[4];
    if (a * b < 0 && std::min(std::abs(a), std::abs(b)) > threshold)
    {
        out.reason = 4;
        return out;
    }
    const double outer_bend = (centers[0] - centers[2]) + (centers[4] - centers[2]);
    const double inner_bend = (centers[1] - centers[2]) + (centers[3] - centers[2]);
    const double quadratic = (2. * outer_bend - inner_bend) / 14.;
    const double tangent_length = std::sqrt(1. + slope * slope);
    const double curvature = std::abs(2. * quadratic) /
                             (step * step * tangent_length * tangent_length * tangent_length);
    constexpr double low_curvature_limit = .312; // equivalent to 0.0013 px^-1 at 240 px height
    const bool low_curvature = curvature * height <= low_curvature_limit;
    constexpr double degrees_per_radian = 57.29577951308232;
    constexpr double low_curvature_turn_deg = 40.;
    const double heading_deg = std::atan(std::abs(slope)) * degrees_per_radian;
    if (std::abs(slope) >= straight_slope &&
        (!low_curvature || heading_deg > low_curvature_turn_deg))
    {
        out.type = slope < 0 ? 1 : 2;
        out.angle = (uint32_t)std::lround(heading_deg);
    }
    /* 整个小航向带均归为直道，不再用内层半带排除
     * 3.7px 的真实直道样本并反复打断确认；
     * 每次状态转换仍需三个不同的新帧。 */
    out.confidence = source_span >= height * .30 ? 85 : source_span >= height * .20 ? 75 : 60;
    out.reason = 0;
    return out;
}

inline EgoCurve estimate_ego_curve(const YOLOLineStyleResult &r)
{
    double first, last;
    if (!ego_curve_range(r, first, last))
        return {};
    const double span = last - first;
    EgoCurve out;
    out.span_px = (float)span;
    if (span < std::max(24., r.height * .10))
    {
        out.reason = 2;
        return out;
    }
    double centers[5], widths[5];
    out.reason = 3;
    for (int i = 0; i < 5; ++i)
    {
        double left, right;
        if (!ego_curve_bounds(r, first + span * .25 * i, left, right))
            return out;
        centers[i] = (left + right) * .5;
        widths[i] = right - left;
    }
    if (widths[0] > widths[4] * 1.05 + 2)
        return out;
    return fit_ego_centers(centers, span, span, r.width, r.height);
}

// A warp generally moves the two edges to different rows. Interpolate each edge
// separately at the same five corrected rows before taking the center.
template <typename Map>
EgoCurve estimate_ego_curve_mapped(const YOLOLineStyleResult &r, Map map)
{
    EgoCurve out;
    double first, last;
    if (!ego_curve_range(r, first, last))
        return out;
    const double source_span = last - first;
    out.span_px = (float)source_span;
    out.reason = 2;
    if (source_span < std::max(24., r.height * .10))
        return out;
    double far_left, far_right, near_left, near_right;
    if (!ego_curve_bounds(r, first, far_left, far_right) ||
        !ego_curve_bounds(r, last, near_left, near_right) ||
        far_right - far_left > (near_right - near_left) * 1.05 + 2.)
    {
        out.reason = 3;
        return out;
    }
    constexpr int segments = 64;
    std::array<ImagePoint, segments + 1> sides[2];
    out.reason = 7;
    for (int i = 0; i <= segments; ++i)
    {
        const double y = first + source_span * i / segments;
        double left, right;
        if (!ego_curve_bounds(r, y, left, right))
            return out;
        ImagePoint points[2] = {{left, y}, {right, y}};
        for (int side = 0; side < 2; ++side)
        {
            auto &point = points[side];
            auto &edge = sides[side];
            if (!map(point) || !std::isfinite(point.x) || !std::isfinite(point.y) ||
                (i && point.y <= edge[i - 1].y))
                return out;
            edge[i] = point;
        }
    }
    const double top = std::max(sides[0].front().y, sides[1].front().y);
    const double bottom = std::min(sides[0].back().y, sides[1].back().y);
    const double span = bottom - top;
    if (span < r.height * .10)
        return out;
    int indices[2] = {};
    double centers[5];
    for (int i = 0; i < 5; ++i)
    {
        const double y = top + span * .25 * i;
        double x[2];
        for (int side = 0; side < 2; ++side)
        {
            auto &index = indices[side];
            const auto &edge = sides[side];
            while (index + 1 < segments && edge[index + 1].y < y)
                ++index;
            const auto &a = edge[index], &b = edge[index + 1];
            x[side] = a.x + (b.x - a.x) * (y - a.y) / (b.y - a.y);
        }
        if (x[1] - x[0] < r.width * .02 || x[1] - x[0] > r.width * 1.5)
            return out;
        centers[i] = (x[0] + x[1]) * .5;
    }
    return fit_ego_centers(centers, span, source_span, r.width, r.height);
}
class EgoCurveConfirm
{
    uint64_t last_id_ = 0, last_capture_ = 0;
    unsigned candidate_ = 0, count_ = 0;

  public:
    EgoCurve update(EgoCurve raw, uint64_t id, uint64_t captured)
    {
        if (!captured || id == last_id_ || captured <= last_capture_)
        {
            raw.confidence = 0;
            raw.reason = 5;
            return raw;
        }
        if (last_capture_ && captured - last_capture_ > 500000000ULL)
            count_ = 0;
        last_id_ = id;
        last_capture_ = captured;
        if (!raw.confidence)
        {
            count_ = 0;
            return raw;
        }
        if (!count_ || candidate_ != raw.type)
        {
            candidate_ = raw.type;
            count_ = 1;
        }
        else if (count_ < 3)
            ++count_;
        if (count_ < 3)
        {
            raw.confidence = 0;
            raw.reason = 6;
        }
        return raw;
    }
};
}
#endif
