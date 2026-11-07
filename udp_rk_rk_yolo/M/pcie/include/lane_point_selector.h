#ifndef M_LANE_POINT_SELECTOR_H
#define M_LANE_POINT_SELECTOR_H
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <vector>

namespace m_lane
{
struct LaneRowPoint
{
    double x, t;
    int y, width;
    bool nn;
};

inline std::vector<std::vector<LaneRowPoint>> lane_mask_rows(const float *mask, int w, int h,
                                                            int first, int end)
{
    std::vector<std::vector<LaneRowPoint>> rows(end - first);
    for (int y = first; y < end; ++y)
    {
        auto &row = rows[y - first];
        const float *line = mask + (size_t)y * w;
        for (int x = 0; x < w;)
        {
            if (!(line[x] > .3f))
            {
                ++x;
                continue;
            }
            const int begin = x;
            while (x < w && line[x] > .3f)
                ++x;
            if (x - begin <= w / 10)
                row.push_back({(begin + x - 1) * .5, (y - h * .95) / (h * .52), y, x - begin, true});
        }
        if (row.size() > 12)
            row.resize(12);
    }
    return rows;
}

struct PointLanePair
{
    int width = 0, height = 0;
    unsigned candidates = 0;
    int first = 0, last = -1;
    std::vector<LaneRowPoint> left, right;
    std::array<double, 5> left_x = {}, right_x = {};
    std::array<int, 5> left_y = {}, right_y = {};
    bool sampled = false;
};

inline const LaneRowPoint *lane_point_near_row(const std::vector<LaneRowPoint> &line, int y)
{
    const auto upper = std::lower_bound(line.begin(), line.end(), y,
                                       [](const LaneRowPoint &p, int row) { return p.y < row; });
    const auto lower = upper == line.begin() ? line.end() : upper - 1;
    const int upper_gap = upper == line.end() ? 4 : upper->y - y;
    const int lower_gap = lower == line.end() ? 4 : y - lower->y;
    if (lower_gap <= 3 && lower_gap <= upper_gap)
        return &*lower;
    return upper_gap <= 3 ? &*upper : nullptr;
}

inline PointLanePair select_point_lanes(const float *mask, int w, int h)
{
    PointLanePair result;
    result.width = w;
    result.height = h;
    if (!mask || w < 80 || h < 60)
        return result;

    constexpr int distance_limit = 10;
    constexpr int min_span = 30;
    const auto rows = lane_mask_rows(mask, w, h, 0, h);
    struct Link
    {
        size_t track, point;
        double squared_distance;
    };
    std::vector<std::vector<LaneRowPoint>> tracks;
    std::vector<size_t> active;
    std::vector<Link> links;
    for (int y = h - 1; y >= 0; --y)
    {
        const auto &row = rows[y];
        if (row.empty())
            continue;
        active.erase(std::remove_if(active.begin(), active.end(), [&](size_t id) {
            return tracks[id].back().y - y >= distance_limit;
        }), active.end());
        links.clear();
        for (const size_t id : active)
        {
            const auto &end = tracks[id].back();
            const int dy = end.y - y;
            for (size_t i = 0; i < row.size(); ++i)
            {
                const double dx = end.x - row[i].x;
                const double squared_distance = dx * dx + dy * dy;
                if (squared_distance < distance_limit * distance_limit)
                    links.push_back({id, i, squared_distance});
            }
        }
        std::stable_sort(links.begin(), links.end(), [](const Link &a, const Link &b) {
            return a.squared_distance < b.squared_distance;
        });
        std::array<bool, 12> claimed = {};
        for (const auto &link : links)
            if (!claimed[link.point] && tracks[link.track].back().y > y)
            {
                tracks[link.track].push_back(row[link.point]);
                claimed[link.point] = true;
            }
        for (size_t i = 0; i < row.size(); ++i)
            if (!claimed[i])
            {
                active.push_back(tracks.size());
                tracks.push_back({row[i]});
            }
    }

    const double center = w * .5; // X=160 in the 320x240 front tile.
    double left_x = -1, right_x = w;
    for (auto &track : tracks)
    {
        if (track.front().y - track.back().y <= min_span)
            continue;
        ++result.candidates;
        const double x = track.front().x;
        if (x < center && x > left_x)
        {
            result.left.swap(track);
            left_x = x;
        }
        else if (x > center && x < right_x)
        {
            result.right.swap(track);
            right_x = x;
        }
    }
    std::reverse(result.left.begin(), result.left.end());
    std::reverse(result.right.begin(), result.right.end());
    if (result.left.empty() || result.right.empty())
        return result;
    result.first = std::max(result.left.front().y, result.right.front().y);
    result.last = std::min(result.left.back().y, result.right.back().y);
    const int span = result.last - result.first;
    if (span < std::max(24., h * .10))
        return result;
    for (size_t i = 0; i < result.left_x.size(); ++i)
    {
        const int target = (int)std::lround(result.first + span * .25 * i);
        const auto *left = lane_point_near_row(result.left, target);
        const auto *right = lane_point_near_row(result.right, target);
        if (!left || !right)
            return result;
        result.left_x[i] = left->x;
        result.right_x[i] = right->x;
        result.left_y[i] = left->y;
        result.right_y[i] = right->y;
    }
    result.sampled = true;
    return result;
}
}
#endif
