#ifndef M_LANE_MARKING_PROFILE_H
#define M_LANE_MARKING_PROFILE_H
#include "lane_point_selector.h"

namespace m_lane
{
// Style evidence only: associate visible paint fragments across dash gaps.
// Steering still uses select_point_lanes() and its original 10px links.
inline PointLanePair marking_profile(const float *mask, const PointLanePair &steering,
                                     const LaneMaskRows *shared_rows=nullptr)
{
    PointLanePair out;
    out.width = steering.width;
    out.height = steering.height;
    const int w = out.width, h = out.height;
    if (!mask || w < 80 || h < 60)
        return out;
    if(shared_rows && shared_rows->size()!=(size_t)h)return out;
    const auto owned_rows=shared_rows?LaneMaskRows{}:lane_mask_rows(mask,w,h,0,h);
    const auto& rows=shared_rows?*shared_rows:owned_rows;
    std::vector<std::vector<LaneRowPoint>> tracks;
    std::vector<size_t> active;
    const int max_gap = std::max(12, h / 4);
    for (int y = h - 1; y >= 0; --y)
    {
        active.erase(std::remove_if(active.begin(), active.end(), [&](size_t id) {
            return tracks[id].back().y - y > max_gap;
        }), active.end());
        for (const auto &point : rows[y])
        {
            size_t best = tracks.size();
            double best_distance = 1e9;
            for (size_t id : active)
            {
                const auto &line = tracks[id];
                const auto &end = line.back();
                const int dy = end.y - y;
                if (dy <= 0)
                    continue;
                const auto &older = line[line.size() > 8 ? line.size() - 8 : 0];
                const double slope = older.y == end.y ? 0. :
                    (end.x - older.x) / (end.y - older.y);
                const double expected = end.x - std::max(-3., std::min(3., slope)) * dy;
                const double residual = std::abs(point.x - expected);
                const double tolerance = 5. + std::min(7., .12 * dy);
                const double distance = residual + .035 * dy;
                if (residual <= tolerance && distance < best_distance)
                {
                    best = id;
                    best_distance = distance;
                }
            }
            if (best < tracks.size())
                tracks[best].push_back(point);
            else if (active.size() < 32 && tracks.size() < 512)
            {
                active.push_back(tracks.size());
                tracks.push_back({point});
            }
        }
    }
    double scores[2] = {1e9, 1e9};
    for (auto &line : tracks)
    {
        if (line.size() < 12 || line.front().y - line.back().y <= 30)
            continue;
        ++out.candidates;
        std::reverse(line.begin(), line.end());
        for (int side = 0; side < 2; ++side)
        {
            const auto &reference = side ? steering.right : steering.left;
            double score = 1e9;
            if (!reference.empty())
            {
                unsigned matches = 0;
                double error = 0.;
                for (size_t i = 0; i < reference.size(); i += 4)
                {
                    const auto *p = lane_point_near_row(line, reference[i].y);
                    if (p && std::abs(p->x - reference[i].x) <= 8)
                    {
                        ++matches;
                        error += std::abs(p->x - reference[i].x);
                    }
                }
                if (matches >= 3)
                    score = error / matches - .01 * (line.back().y - line.front().y);
            }
            else if ((side == 0 && line.back().x < w * .5) ||
                     (side == 1 && line.back().x > w * .5))
                score = std::abs(line.back().x - w * .5);
            if (score < scores[side])
            {
                scores[side] = score;
                (side ? out.right : out.left) = line;
            }
        }
    }
    out.first = h;
    for (const auto *line : {&out.left, &out.right})
        if (!line->empty())
        {
            out.first = std::min(out.first, line->front().y);
            out.last = std::max(out.last, line->back().y);
        }
    // Geometry between observed fragments is available even when paint is not.
    // This prevents temporal identity from changing when a dash gap moves over
    // one of the identity-check rows. Virtual points NEVER count as paint:
    // describe_points still measures the real RGB at each row, with coverage.
    for (auto *line : {&out.left, &out.right})
    {
        if (line->empty()) continue;
        std::vector<LaneRowPoint> dense;
        size_t upper = 0;
        for (int y = line->front().y; y <= line->back().y; ++y)
        {
            while ((*line)[upper].y < y) ++upper;
            auto p = (*line)[upper];
            if (p.y != y)
            {
                const auto &a = (*line)[upper - 1];
                p.x = a.x + (p.x - a.x) * (y - a.y) / (p.y - a.y);
                p.y = y;
                p.t = (y - h * .95) / (h * .52);
                p.nn = false;
            }
            dense.push_back(p);
        }
        line->swap(dense);
    }
    return out;
}
}
#endif
