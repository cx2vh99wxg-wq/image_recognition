#ifndef EGO_LANE_SELECTOR_H
#define EGO_LANE_SELECTOR_H
#include "line_style.h"
#include "line_style_trace.h"
#include "lane_point_selector.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>
namespace m_lane
{
/* 只读的本车道归属层先建立有界车道线对象，
 * 再选择走廊。道路分割只作为支持证据而非车道编号；
 * 本层不改叠加图、旧弯道判定或控制寄存器。 */
class EgoLaneSelector
{
    enum { sample_step = 10, min_sampled_span = 50 };
    double fixed_anchor_;
    bool bottom_up_;
    using Point = LaneRowPoint;
    struct Fit
    {
        double a = 0, b = 0, c = 0;
        double at(double t) const
        {
            return a + b * t + c * t * t;
        }
    };
    struct Track
    {
        Fit fit;
        std::vector<Point> points;
        int first = 0, last = 0;
        unsigned nn = 0;
        double length = 0, error = 0;
    };
    struct Road
    {
        std::vector<uint8_t> mask;
        std::vector<int> left, right;
        int first = 0, last = 0;
        unsigned area = 0;
        double reference = -1, reference_y = 0;
        bool centered = false;
    };
    static double coordinate(int y, int h)
    {
        return (y - h * .95) / (h * .52);
    }
    static bool solve(const std::vector<Point> &p, Fit &f, bool linear = false, size_t min_points = 8)
    {
        if (p.size() < min_points)
            return false;
        const int size = linear ? 2 : 3;
        double m[3][4] = {};
        for (const auto &point : p)
        {
            double v[3] = {1, point.t, point.t * point.t};
            for (int r = 0; r < size; ++r)
            {
                for (int c = 0; c < size; ++c)
                    m[r][c] += v[r] * v[c];
                m[r][size] += v[r] * point.x;
            }
        }
        for (int c = 0; c < size; ++c)
        {
            int pivot = c;
            for (int r = c + 1; r < size; ++r)
                if (std::abs(m[r][c]) > std::abs(m[pivot][c]))
                    pivot = r;
            if (std::abs(m[pivot][c]) < 1e-8)
                return false;
            for (int k = c; k <= size; ++k)
                std::swap(m[pivot][k], m[c][k]);
            double d = m[c][c];
            for (int k = c; k <= size; ++k)
                m[c][k] /= d;
            for (int r = 0; r < size; ++r)
                if (r != c)
                {
                    d = m[r][c];
                    for (int k = c; k <= size; ++k)
                        m[r][k] -= d * m[c][k];
                }
        }
        f.a = m[0][size];
        f.b = m[1][size];
        f.c = linear ? 0 : m[2][size];
        return std::isfinite(f.a) && std::isfinite(f.b) && std::isfinite(f.c);
    }
    static double arc(const Fit &f, int first, int last, int h)
    {
        double length = 0;
        for (int y = first; y < last; ++y)
            length += std::hypot(1., f.at(coordinate(y + 1, h)) - f.at(coordinate(y, h)));
        return length;
    }
    static bool fit_geometry(const std::vector<Point> &p, Fit &f, int w, int h,
                             double curvature_limit = .25, size_t min_points = 8)
    {
        Fit linear;
        if (!solve(p, linear, true, min_points))
            return false;
        f = linear;
        if (p.back().y - p.front().y < h * .10)
            return true;
        Fit quadratic;
        if (!solve(p, quadratic, false, min_points))
            return true;
        double a = 0, b = 0;
        for (const auto &point : p)
        {
            a += std::abs(point.x - linear.at(point.t));
            b += std::abs(point.x - quadratic.at(point.t));
        }
        double span = p.back().t - p.front().t;
        if ((a - b) / p.size() > .35 && std::abs(quadratic.c) * span * span < w * curvature_limit)
            f = quadratic;
        return true;
    }
    static double extended(const Track &t, int y, int h)
    {
        int endpoint = std::max(t.first, std::min(t.last, y));
        double v = coordinate(endpoint, h);
        return t.fit.at(v) + (y - endpoint) * (t.fit.b + 2 * t.fit.c * v) / (h * .52);
    }
    static int light(const uint8_t *p)
    {
        return (p[0] + 2 * p[1] + p[2]) / 4;
    }
    static bool color(const uint8_t *p)
    {
        int hi = std::max(p[0], std::max(p[1], p[2])), lo = std::min(p[0], std::min(p[1], p[2]));
        return (lo >= 75 && hi - lo < 85) ||
               (p[1] >= 95 && std::max(p[0], p[2]) >= 105 && std::min(p[0], p[2]) < p[1] * .8);
    }
    static int ridge(const uint8_t *rgb, int w, int h, int x, int y, int search)
    {
        if (!rgb || y < 0 || y >= h)
            return -1;
        const int margin = std::max(2, w / 40);
        double best = 0;
        int selected = -1;
        for (int px = std::max(margin, x - search); px <= std::min(w - 1 - margin, x + search);
             ++px)
        {
            const uint8_t *p = rgb + ((size_t)y * w + px) * 3;
            if (!color(p))
                continue;
            int background = 0, sides = 0;
            for (int sign : {-1, 1})
            {
                int sum = 0, n = 0;
                for (int d = 6; d <= 10; d += 2)
                {
                    int sx = px + sign * d;
                    if (sx >= margin && sx < w - margin)
                    {
                        sum += light(rgb + ((size_t)y * w + sx) * 3);
                        ++n;
                    }
                }
                if (n)
                {
                    background = std::max(background, sum / n);
                    ++sides;
                }
            }
            if (!sides)
                continue;
            int contrast = light(p) - background;
            if (contrast <= std::max(12, background / 8))
                continue;
            double score = contrast - .65 * std::abs(px - x);
            if (score > best)
            {
                best = score;
                selected = px;
            }
        }
        return selected;
    }
    static bool black_row(const uint8_t *rgb, int w, int h, int x, int y)
    {
        if (y < 0 || y >= h)
            return false;
        int first = std::max(w / 40, x - 8), last = std::min(w - 1 - w / 40, x + 8);
        if (last - first < 6)
            return false;
        for (int px = first; px <= last; ++px)
        {
            const uint8_t *p = rgb + ((size_t)y * w + px) * 3;
            if (p[0] || p[1] || p[2])
                return false;
        }
        return true;
    }
    static Road road_region(const uint8_t *green, int w, int h)
    {
        Road road;
        road.mask.assign((size_t)w * h, 0);
        road.left.assign(h, w);
        road.right.assign(h, -1);
        if (!green)
            return road;
        const int lo = h * 12 / 100, hi = h * 96 / 100;
        std::vector<uint8_t> visited((size_t)w * h, 0);
        std::vector<int> queue, best;
        for (int y = lo; y < hi; ++y)
            for (int x = 0; x < w; ++x)
            {
                int start = y * w + x;
                if (!green[start] || visited[start])
                    continue;
                queue.clear();
                queue.push_back(start);
                visited[start] = 1;
                for (size_t i = 0; i < queue.size(); ++i)
                {
                    int p = queue[i], px = p % w, py = p / w;
                    const int dx[4] = {-1, 1, 0, 0}, dy[4] = {0, 0, -1, 1};
                    for (int d = 0; d < 4; ++d)
                    {
                        int nx = px + dx[d], ny = py + dy[d];
                        if (nx < 0 || nx >= w || ny < lo || ny >= hi)
                            continue;
                        int q = ny * w + nx;
                        if (green[q] && !visited[q])
                        {
                            visited[q] = 1;
                            queue.push_back(q);
                        }
                    }
                }
                if (queue.size() > best.size())
                    best = queue;
            }
        if (best.size() < (size_t)(w * h / 160))
            return road;
        road.first = h;
        road.last = 0;
        road.area = (unsigned)best.size();
        for (int p : best)
        {
            int y = p / w, x = p % w;
            road.mask[p] = 1;
            road.first = std::min(road.first, y);
            road.last = std::max(road.last, y);
            road.left[y] = std::min(road.left[y], x);
            road.right[y] = std::max(road.right[y], x);
        }
        if (road.last - road.first < h * .12)
        {
            road.area = 0;
            return road;
        }
        // 固定前视图使用底部光轴；嵌入的屏幕视频可能在更高处结束，
        // 此时改取其远端入口轴，并由后续走廊置信检查约束，
        // 不直接以任意绿色区域替代本车参考轴。
        int anchor_y = h * 90 / 100, anchor_x = w / 2;
        road.reference_y = road.first;
        if (road.area >= (unsigned)(w * h / 4) && road.last >= anchor_y &&
            road.mask[anchor_y * w + anchor_x])
        {
            road.reference = anchor_x;
            road.centered = true;
        }
        else
        {
            std::vector<double> centers;
            int band = std::max(5, (road.last - road.first) / 8);
            for (int y = road.first; y <= std::min(road.last, road.first + band); ++y)
                if (road.right[y] - road.left[y] >= 2)
                    centers.push_back((road.left[y] + road.right[y]) * .5);
            if (centers.size() >= 3)
            {
                std::sort(centers.begin(), centers.end());
                road.reference = centers[centers.size() / 2];
            }
        }
        return road;
    }
    static bool near_road(const Road &road, int x, int y, int w, int h)
    {
        if (y < 0 || y >= h)
            return false;
        for (int dx = -5; dx <= 5; dx += 2)
        {
            int px = x + dx;
            if (px >= 0 && px < w && road.mask[y * w + px])
                return true;
        }
        return false;
    }
    static std::vector<Track> bottom_up_tracks(const std::vector<std::vector<Point>> &rows, int w,
                                               int h, bool sampled = false)
    {
        struct Link
        {
            size_t chain, point;
            double distance;
        };
        const double limit = std::max(6., w * .06);
        std::vector<Track> chains;
        std::vector<size_t> active;
        std::vector<Link> links;
        for (auto row = rows.rbegin(); row != rows.rend(); ++row)
        {
            if (row->empty())
                continue;
            const int y = row->front().y;
            active.erase(std::remove_if(active.begin(), active.end(), [&](size_t id) {
                const int dy = chains[id].points.back().y - y;
                return sampled ? dy > 2 * sample_step : dy >= limit;
            }), active.end());
            links.clear();
            for (const size_t id : active)
            {
                const Point &end = chains[id].points.back();
                const double dy = end.y - y;
                auto point = std::lower_bound(row->begin(), row->end(), end.x - limit,
                                              [](const Point &p, double x) { return p.x < x; });
                for (; point != row->end() && point->x < end.x + limit; ++point)
                {
                    const double dx = end.x - point->x;
                    const double distance = dx * dx + dy * dy;
                    if (sampled ? std::abs(dx) < limit : distance < limit * limit)
                        links.push_back({id, (size_t)(point - row->begin()), distance});
                }
            }
            // Each point can belong to one track. Resolve conflicts using
            // the closest eligible endpoint before creating new seeds.
            std::stable_sort(links.begin(), links.end(), [](const Link &a, const Link &b) {
                return a.distance < b.distance;
            });
            std::vector<bool> claimed(row->size());
            for (const auto &link : links)
                if (!claimed[link.point] && chains[link.chain].points.back().y > y)
                {
                    chains[link.chain].points.push_back((*row)[link.point]);
                    claimed[link.point] = true;
                }
            for (size_t j = 0; j < row->size(); ++j)
                if (!claimed[j])
                {
                    active.push_back(chains.size());
                    chains.emplace_back();
                    chains.back().points.push_back((*row)[j]);
                }
        }

        std::vector<Track> tracks;
        for (auto &chain : chains)
        {
            auto &points = chain.points;
            if (points.size() < (sampled ? 3u : 8u))
                continue;
            std::reverse(points.begin(), points.end());
            chain.first = points.front().y;
            chain.last = points.back().y;
            const int span = chain.last - chain.first;
            if (sampled ? span < min_sampled_span : span < h * .08 || points.size() * 4 < (size_t)span)
                continue;
            if (!fit_geometry(points, chain.fit, w, h, .8, sampled ? 3u : 8u))
                continue;
            if (!sampled)
            {
                for (const auto &p : points)
                    chain.error += std::abs(p.x - chain.fit.at(p.t));
                chain.error /= points.size();
                if (chain.error > w * .025)
                    continue;
            }
            chain.nn = (unsigned)points.size();
            chain.length = arc(chain.fit, chain.first, chain.last, h);
            tracks.push_back(std::move(chain));
        }
        if (sampled)
            return tracks;
        std::stable_sort(tracks.begin(), tracks.end(), [](const Track &a, const Track &b) {
            return a.nn > b.nn;
        });
        if (tracks.size() > 6)
            tracks.resize(6);
        return tracks;
    }

    static void select_sampled_pair(const float *mask, const uint8_t *rgb, const Road &road,
                                    int w, int h, YOLOLineStyleResult &result, LineStyleTrace *trace)
    {
        const double center = w / 2.;
        result.reference_x = center;
        result.road_area = road.area;
        const auto row_has_lane = [&](int y) {
            const float *row = mask + y * w;
            return std::any_of(row, row + w, [](float score) { return score > .3f; });
        };
        int last = h - 1;
        while (last >= 0 && !row_has_lane(last))
            --last;
        if (last < 0)
            return;
        int first = 0;
        while (!row_has_lane(first))
            ++first;
        result.reference_y = last;
        result.left.reason = result.right.reason = LINE_NO_TRACK;
        if (trace)
        {
            trace->reference_x = center;
            trace->near_y = last;
            trace->road_area = road.area;
        }
        if (last - first < min_sampled_span)
            return;

        std::vector<std::vector<Point>> rows;
        for (int y = last - (last - first) / sample_step * sample_step; y <= last; y += sample_step)
        {
            rows.emplace_back();
            const double t = coordinate(y, h);
            for (int x = 0; x < w; ++x)
                if (mask[y * w + x] > .3f)
                    rows.back().push_back({(double)x, t, y, 1, true});
        }
        const auto tracks = bottom_up_tracks(rows, w, h, true);
        result.object_count = (uint32_t)tracks.size();
        int left = -1, right = -1;
        double lx = 0, rx = w;
        for (size_t i = 0; i < tracks.size(); ++i)
        {
            const auto &track = tracks[i];
            const double x = track.points.back().x;
            if (trace)
                trace->objects.push_back({track.fit.a, track.fit.b, track.fit.c, track.first,
                                          track.last, track.nn, x});
            if (x < center && (left < 0 || x > lx))
            {
                left = (int)i;
                lx = x;
            }
            if (x > center && (right < 0 || x < rx))
            {
                right = (int)i;
                rx = x;
            }
        }
        if (trace)
        {
            trace->selected_left = left;
            trace->selected_right = right;
        }
        const int road_first = road.area ? road.first : first;
        const int road_height = road.area ? road.last - road.first : last - first;
        if (left >= 0)
            result.left = measure(tracks[left], rgb, w, h, road_first, road_height, rx - lx,
                                  trace ? &trace->left : nullptr);
        if (right >= 0)
            result.right = measure(tracks[right], rgb, w, h, road_first, road_height, rx - lx,
                                   trace ? &trace->right : nullptr);
    }

    static std::vector<Track> objects(const float *mask, const uint8_t *rgb, Road &road, int w,
                                      int h, bool ground, bool bottom_up)
    {
        auto rows = lane_mask_rows(mask, w, h, bottom_up ? 0 : h * 12 / 100,
                                   bottom_up ? h : h * 96 / 100);
        if (!road.centered)
        {
            std::vector<double> centers;
            std::vector<int> center_rows;
            for (const auto &row : rows)
            {
                if (row.empty() || row[0].y < road.first - h * .15 ||
                    row[0].y > road.first + h * .1)
                    continue;
                int y = std::max(road.first, std::min(road.last, row[0].y));
                std::vector<double> xs;
                for (const auto &p : row)
                    if (p.x >= road.left[y] - w * .1 && p.x <= road.right[y] + w * .1)
                        xs.push_back(p.x);
                if (xs.size() < 2)
                    continue;
                std::sort(xs.begin(), xs.end());
                double width = xs.back() - xs.front();
                if (width < w * .015 || width > w * .25)
                    continue;
                centers.push_back((xs.front() + xs.back()) * .5);
                center_rows.push_back(row[0].y);
                if (centers.size() >= 8)
                    break;
            }
            if (centers.size() >= 3)
            {
                std::sort(centers.begin(), centers.end());
                road.reference = centers[centers.size() / 2];
                road.reference_y = center_rows[center_rows.size() / 2];
            }
        }
        std::vector<Track> tracks = bottom_up ? bottom_up_tracks(rows, w, h) : std::vector<Track>{};
        std::vector<Fit> rejected_seeds;
        const double tolerance = std::max(3., w * .022);
        for (int object = 0; !bottom_up && object < 6; ++object)
        {
            std::vector<Point> anchors;
            for (size_t r = 0; r < rows.size(); r += 4)
                for (const auto &p : rows[r])
                    anchors.push_back(p);
            if (anchors.size() < 4)
                break;
            double best_score = 0;
            Fit selected;
            bool branch_left = false;
            std::vector<std::pair<size_t, size_t>> pairs;
            if (anchors.size() <= 32)
            {
                for (size_t i = 0; i < anchors.size(); ++i)
                    for (size_t j = i + 1; j < anchors.size(); ++j)
                        pairs.push_back({i, j});
            }
            else
            {
                uint32_t random = 0x51a7u + object;
                for (int k = 0; k < 512; ++k)
                {
                    random = random * 1664525u + 1013904223u;
                    size_t i = (random >> 8) % anchors.size();
                    random = random * 1664525u + 1013904223u;
                    size_t j = (random >> 8) % anchors.size();
                    if (i > j)
                        std::swap(i, j);
                    if (i != j)
                        pairs.push_back({i, j});
                }
            }
            for (const auto &pair : pairs)
            {
                const auto &a = anchors[pair.first];
                const auto &b = anchors[pair.second];
                if (b.y <= a.y || std::hypot(b.x - a.x, (double)b.y - a.y) < h * .06)
                    continue;
                double slope = (b.x - a.x) / (b.y - a.y);
                if (std::abs(slope) > 3.5 * w / h)
                    continue;
                Fit f;
                f.b = slope * h * .52;
                f.a = a.x - slope * (a.y - h * .95);
                if (ground && std::any_of(rejected_seeds.begin(), rejected_seeds.end(),
                                          [&](const Fit &rejected) {
                                              return std::abs(rejected.at(a.t) - a.x) < tolerance &&
                                                     std::abs(rejected.at(b.t) - b.x) < tolerance;
                                          }))
                    continue;
                int hits = 0, first = -1, last = -1;
                double error = 0, first_x = 0, last_x = 0;
                for (size_t r = 0; r < rows.size(); r += 2)
                {
                    const Point *match = nullptr;
                    double distance = tolerance;
                    for (const auto &p : rows[r])
                    {
                        double d = std::abs(p.x - f.at(p.t));
                        if (d < distance)
                        {
                            distance = d;
                            match = &p;
                        }
                    }
                    if (match)
                    {
                        ++hits;
                        error += distance;
                        if (first < 0)
                        {
                            first = match->y;
                            first_x = match->x;
                        }
                        last = match->y;
                        last_x = match->x;
                    }
                }
                if (hits < 4 || std::hypot(last_x - first_x, (double)last - first) < h * .08)
                    continue;
                if (!ground && std::min(first_x, last_x) < road.reference - w * .05 &&
                    std::max(first_x, last_x) > road.reference + w * .05)
                    continue;
                double score = (std::hypot(last_x - first_x, (double)last - first) + hits * 1.5) /
                               (1 + error / hits / tolerance);
                if (score > best_score)
                {
                    best_score = score;
                    selected = f;
                    branch_left = last_x < road.reference;
                }
            }
            if (!best_score)
                break;
            Track track;
            track.fit = selected;
            for (int pass = 0; pass < 4; ++pass)
            {
                track.points.clear();
                for (const auto &row : rows)
                {
                    const Point *match = nullptr;
                    double distance = tolerance * (pass ? 1 : 1.5);
                    for (const auto &p : row)
                    {
                        if (!ground && (branch_left ? p.x > road.reference + w * .05
                                                    : p.x < road.reference - w * .05))
                            continue;
                        double d = std::abs(p.x - track.fit.at(p.t));
                        if (d < distance)
                        {
                            distance = d;
                            match = &p;
                        }
                    }
                    if (match)
                        track.points.push_back(*match);
                }
                if (track.points.size() < 8)
                    break;
                if (!fit_geometry(track.points, track.fit, w, h))
                {
                    track.points.clear();
                    break;
                }
            }
            if (track.points.size() < 8)
            {
                if (!ground)
                    break;
                rejected_seeds.push_back(selected);
                continue;
            }
            track.first = track.points.front().y;
            track.last = track.points.back().y;
            // 用实测漆线峰值细化 NN 走廊，只将真实 RGB 证据用于重拟合，
            // 不能据几何预测填平虚线间隔。
            std::vector<Point> paint_points;
            for (const auto &p : track.points)
            {
                int x = (int)std::lround(track.fit.at(p.t));
                int peak = ridge(rgb, w, h, x, p.y, 5);
                if (peak >= 0)
                    paint_points.push_back({(double)peak, p.t, p.y, p.width, p.nn});
            }
            if (paint_points.size() >= 8 &&
                paint_points.back().y - paint_points.front().y >= (track.last - track.first) * .5)
            {
                Fit refined;
                if (fit_geometry(paint_points, refined, w, h))
                {
                    double movement = 0;
                    for (const auto &p : track.points)
                        movement += std::abs(refined.at(p.t) - track.fit.at(p.t));
                    if (movement / track.points.size() < 5)
                        track.fit = refined;
                }
            }
            for (const auto &p : track.points)
            {
                track.nn += p.nn;
                track.error += std::abs(p.x - track.fit.at(p.t));
            }
            track.error /= track.points.size();
            // 仅保留得到 RGB 验证、且行数受限的延伸部分。
            // 这用于恢复短 NN 线段端点附近的实际漆线；
            // 未观测或外推像素不能计作实线证据。
            for (int direction : {-1, 1})
            {
                int endpoint = direction < 0 ? track.first : track.last, last_good = endpoint,
                    missing = 0;
                int upper_limit = std::min(track.first, road.first);
                for (int step = 1; step <= h / 25; ++step)
                {
                    int y = endpoint + direction * step;
                    if (y < upper_limit || y < h * 12 / 100 || y >= h * 96 / 100)
                        break;
                    int x = (int)std::lround(track.fit.at(coordinate(y, h)));
                    if (x < 0 || x >= w)
                        break;
                    if (ridge(rgb, w, h, x, y, 3) >= 0)
                    {
                        last_good = y;
                        missing = 0;
                    }
                    else if (++missing > 1)
                        break;
                }
                if (direction < 0)
                    track.first = last_good;
                else
                    track.last = last_good;
            }
            track.length = arc(track.fit, track.first, track.last, h);
            // 提取一个对象后从候选点中剥离它，避免相邻长线占满
            // 所有候选名额，遮蔽图像边缘较短的本车边界。
            const auto remove_points = [&]() {
                for (auto &row : rows)
                    row.erase(std::remove_if(row.begin(), row.end(),
                                             [&](const Point &p) {
                                                 return p.y > road.first + h * .06 &&
                                                        std::abs(p.x - track.fit.at(p.t)) < tolerance;
                                             }), row.end());
            };
            if (!ground)
                remove_points();
            double u0 = coordinate(track.first, h), u1 = coordinate(track.last, h);
            double angle0 = std::atan((track.fit.b + 2 * track.fit.c * u0) / (h * .52));
            double angle1 = std::atan((track.fit.b + 2 * track.fit.c * u1) / (h * .52));
            if (track.error > w * .025 || track.length < h * .08 ||
                std::abs(angle1 - angle0) > 1.0472)
            {
                if (ground)
                    rejected_seeds.push_back(selected);
                continue;
            }
            bool duplicate = false;
            for (const auto &old : tracks)
            {
                int lo = std::max(track.first, old.first), hi = std::min(track.last, old.last);
                if (hi < lo)
                    continue;
                double d = 0;
                for (int y = lo; y <= hi; y += 3)
                    d += std::abs(track.fit.at(coordinate(y, h)) - old.fit.at(coordinate(y, h)));
                d /= ((hi - lo) / 3 + 1);
                if (d < tolerance)
                {
                    duplicate = true;
                    break;
                }
            }
            if (!duplicate)
            {
                if (ground)
                    remove_points();
                tracks.push_back(track);
            }
            else if (ground)
                rejected_seeds.push_back(selected);
        }
        // 仅有前景的道路分割可能从真实道路入口下方开始；
        // 利用相互收敛的 NN 线对象修正该入口，
        // 避免底部绿色斑块把本车道选择带向相邻车道。
        auto correct_reference = [&]()
        {
            std::vector<int> entries;
            for (const auto &t : tracks)
                if (t.nn >= 8 && t.first >= road.first - h * .15 && t.first < road.first - h * .025)
                    entries.push_back(t.first);
            if (!road.centered && entries.size() >= 2 && road.area < (unsigned)(w * h / 4))
            {
                std::sort(entries.begin(), entries.end());
                int y = entries[0];
                std::vector<double> xs;
                for (const auto &t : tracks)
                    if (t.nn >= 8 && t.first <= y + h * .05 && t.last >= y)
                        xs.push_back(extended(t, y, h));
                if (xs.size() >= 2)
                {
                    std::sort(xs.begin(), xs.end());
                    if (xs.back() - xs.front() < w * .2 && xs.front() >= 0 && xs.back() < w)
                    {
                        road.reference = xs[xs.size() / 2];
                        road.reference_y = y;
                    }
                }
            }
        };
        correct_reference();
        // 在道路侧边附近，依真实 RGB 恢复被裁切的外侧标线。
        // 这一路与 NN RANSAC 分开；若把拍摄屏幕的底部混入点集，
        // 会形成机盖或屏框伪曲线，并可能在剥离候选时
        // 误删真正较短的车道边界。
        for (int side = 0; side < 2; ++side)
        {
            std::vector<Point> paint;
            for (int y = road.first; y <= road.last; ++y)
            {
                int source = std::max(road.first, y),
                    edge = side ? road.right[source] : road.left[source];
                if (edge < 0 || edge >= w)
                    continue;
                int peak = ridge(rgb, w, h, edge, y, std::max(5, w / 32));
                if (peak < 0 || (side ? peak <= road.reference + 2 : peak >= road.reference - 2))
                    continue;
                paint.push_back({(double)peak, coordinate(y, h), y, 3, false});
            }
            if (paint.size() < 8 || paint.front().y > road.first + h * .1)
                continue;
            Fit f;
            if (!solve(paint, f, true))
                continue;
            for (int pass = 0; pass < 4; ++pass)
            {
                std::vector<Point> selected;
                for (const auto &p : paint)
                    if (std::abs(p.x - f.at(p.t)) < std::max(4., w * .018))
                        selected.push_back(p);
                if (selected.size() < 8)
                    break;
                paint = selected;
                if (!solve(paint, f, paint.back().y - paint.front().y < h * .18))
                    break;
            }
            if (paint.size() < 8 || paint.back().y - paint.front().y < h * .025)
                continue;
            double start = f.at(paint.front().t);
            if (std::abs(start - road.reference) > w * .12)
                continue;
            double end = f.at(paint.back().t);
            if (side ? end < start - 5 : end > start + 5)
                continue;
            Track recovered;
            recovered.fit = f;
            recovered.points = paint;
            recovered.first = paint.front().y;
            recovered.last = paint.back().y;
            recovered.length = arc(f, recovered.first, recovered.last, h);
            for (const auto &p : paint)
                recovered.error += std::abs(p.x - f.at(p.t));
            recovered.error /= paint.size();
            if (recovered.error > w * .018 || recovered.length < h * .08)
                continue;
            bool duplicate = false;
            for (auto &old : tracks)
            {
                int lo = std::max(recovered.first, old.first),
                    hi = std::min(recovered.last, old.last);
                if (hi < lo)
                    continue;
                double distance = 0;
                for (int y = lo; y <= hi; ++y)
                    distance += std::abs(f.at(coordinate(y, h)) - old.fit.at(coordinate(y, h)));
                if (distance / (hi - lo + 1) < tolerance)
                {
                    duplicate = true;
                    if (recovered.length >= old.length * .8 && recovered.error < old.error)
                    {
                        recovered.nn = old.nn;
                        old = recovered;
                    }
                    break;
                }
            }
            if (!duplicate)
                tracks.push_back(recovered);
        }
        correct_reference();
        for (auto &track : tracks)
        {
            for (int direction : {-1, 1})
            {
                int endpoint = direction < 0 ? track.first : track.last, last_good = endpoint,
                    missing = 0;
                int upper_limit = std::min(track.first, road.first);
                for (int step = 1; step <= h / 25; ++step)
                {
                    int y = endpoint + direction * step;
                    if (y < upper_limit || y < h * 12 / 100 || y >= h * 96 / 100)
                        break;
                    int x = (int)std::lround(track.fit.at(coordinate(y, h)));
                    if (x < 0 || x >= w)
                        break;
                    if (ridge(rgb, w, h, x, y, 3) >= 0)
                    {
                        last_good = y;
                        missing = 0;
                    }
                    else if (++missing > 1)
                        break;
                }
                if (direction < 0)
                    track.first = last_good;
                else
                    track.last = last_good;
            }
            track.length = arc(track.fit, track.first, track.last, h);
        }
        return tracks;
    }
    static YOLOLineBoundary measure(const Track &track, const uint8_t *rgb, int w, int h,
                                    int road_first, int road_height, double lane_width,
                                    std::vector<PaintSample> *trace,
                                    const std::vector<Point> *points = nullptr, bool ground = false,
                                    const uint8_t *coverage = nullptr)
    {
        size_t upper = 0;
        const auto x_at = [&](int y) {
            if (!points)
                return track.fit.at(coordinate(y, h));
            while (upper + 1 < points->size() && (*points)[upper].y < y)
                ++upper;
            const auto &b = (*points)[upper];
            if (!upper || b.y == y)
                return b.x;
            const auto &a = (*points)[upper - 1];
            // RGB paint sampling only. Direction uses observed X samples.
            return a.x + (b.x - a.x) * (y - a.y) / (b.y - a.y);
        };
        YOLOLineBoundary out = {};
        out.fit_a = track.fit.a;
        out.fit_b = track.fit.b;
        out.fit_c = track.fit.c;
        out.first_y = track.first;
        out.last_y = track.last;
        out.x_bottom = points ? points->back().x : track.fit.at(0);
        out.x_top = points ? points->front().x : track.fit.at(coordinate(track.first, h));
        out.coverage = std::min(100, (int)(track.length * 100 / (h * .52)));
        std::vector<int> values;
        int valid = 0;
        for (int y = track.first; y <= track.last; ++y)
        {
            int x = (int)std::lround(x_at(y));
            int value = -1;
            if (x >= w / 40 && x < w - w / 40)
            {
                // 采集图中孤立且完全为零的整行表示证据缺失，
                // 不能把它算作车道漆线的暗间隔。
                bool missing = coverage ? !coverage[y * w + x] : black_row(rgb, w, h, x, y) &&
                               (ground || (!black_row(rgb, w, h, x, y - 1) &&
                                           !black_row(rgb, w, h, x, y + 1)));
                if (!missing)
                {
                    value = ridge(rgb, w, h, x, y, 3) >= 0 ? 1 : 0;
                    ++valid;
                }
            }
            values.push_back(value);
            if (trace)
                trace->push_back({y, x, value, value});
        }
        if (valid < 8)
        {
            out.reason = LINE_SHORT_TRACK;
            return out;
        }
        auto cleaned = values;
        for (size_t i = 1; i + 1 < values.size(); ++i)
            if (values[i] >= 0 && values[i - 1] >= 0 && values[i - 1] == values[i + 1])
                cleaned[i] = values[i - 1]; // isolated noise or one missing paint row
        struct Run
        {
            int value, length;
        };
        std::vector<Run> runs;
        int painted = 0, spans = 0, gaps = 0, longest = 0, longest_paint = 0;
        for (size_t i = 0; i < cleaned.size(); ++i)
        {
            const int value = cleaned[i];
            painted += value == 1;
            if (trace)
                (*trace)[i].filtered = value;
            // A single corrupt row surrounded by paint does not split a solid
            // marking. Other unknown rows are evidence barriers, never gaps.
            if (value < 0 && i > 0 && i + 1 < cleaned.size() && cleaned[i - 1] == 1 &&
                cleaned[i + 1] == 1)
                continue;
            if (runs.empty() || runs.back().value != value)
                runs.push_back({value, 1});
            else
                ++runs.back().length;
        }
        for (size_t i = 0; i < runs.size(); ++i)
        {
            const auto &run = runs[i];
            if (run.value == 1 && run.length >= 3)
            {
                ++spans;
                longest_paint = std::max(longest_paint, run.length);
            }
            if (run.value != 0 || run.length < 3 || i == 0 || i + 1 == runs.size() ||
                runs[i - 1].value != 1 || runs[i + 1].value != 1)
                continue;
            longest = std::max(longest, run.length);
            // Compare a gap with its neighboring marks at roughly the same
            // image scale. Tiny specks and very long occlusions are weak evidence.
            const int shorter = std::min(runs[i - 1].length, runs[i + 1].length);
            if (shorter >= 3 && run.length <= 5 * shorter)
                ++gaps;
        }
        out.duty = 100 * painted / valid;
        out.spans = spans;
        out.gaps = gaps;
        double first_x = out.x_top,
               last_x = points ? points->back().x : track.fit.at(coordinate(track.last, h));
        bool clipped = std::min(first_x, last_x) < w * .04 || std::max(first_x, last_x) > w * .96;
        double required_length = ground ? std::max(24., std::min(h * .22, road_height * .7))
                                        : h * .22;
        if (clipped && track.first <= road_first + h * .1)
        {
            // 画面内小型道路的观测长度不能用整个相机格的高度衡量；
            // 仍需满足最小实际可见长度，仅当该线
            // 在道路侧边被裁断时才降低长度要求。
            required_length =
                std::max(24., std::min(h * .20, std::min(road_height * .35, lane_width * .3)));
        }
        bool near_continuous = out.duty >= 82 || (out.duty >= 80 && longest_paint >= valid * .6);
        // 短且侧边被裁断的观测里，一个空隙就可能属于虚线周期；
        // 此处要求漆线近乎连续，不能套用
        // 长距离可见线所允许的磨损容差。
        if (valid < h * .18)
            near_continuous = out.duty >= 95 && gaps == 0;
        if (near_continuous && gaps <= 1 && longest <= std::max(4, h * 3 / 100) &&
            track.length >= required_length)
        {
            out.type = LINE_SOLID;
            out.quality = std::min(95u, 68u + (out.duty - 80) / 2 + out.coverage / 10);
        }
        else if (out.duty >= 15 && spans >= 2 &&
                 ((gaps >= 2 && out.duty <= 88) || (gaps == 1 && out.duty <= 72)))
        {
            out.type = LINE_DASHED;
            out.quality = std::min(94u, 60u + std::min(3, spans) * 5u + out.coverage / 10);
        }
        out.raw_type = out.type;
        out.reason = out.type                         ? LINE_OK
                     : out.duty < 15                  ? LINE_NO_PAINT
                     : track.length < required_length ? LINE_SHORT_TRACK
                                                      : LINE_AMBIGUOUS;
        return out;
    }

    static void select_ground_pair(const std::vector<Track> &tracks, const Road &road,
                                   const uint8_t *rgb, int w, int h, YOLOLineStyleResult &result,
                                   LineStyleTrace *trace)
    {
        double best = -1.;
        int left_id = -1, right_id = -1, near_y = -1;
        for (size_t i = 0; i < tracks.size(); ++i)
            for (size_t j = i + 1; j < tracks.size(); ++j)
            {
                if (tracks[i].nn + tracks[j].nn < 8)
                    continue;
                const int first = std::max(tracks[i].first, tracks[j].first);
                const int last = std::min(tracks[i].last, tracks[j].last);
                if (last - first < std::max(24., h * .10))
                    continue;
                int l = (int)i, r = (int)j;
                if (extended(tracks[l], last, h) > extended(tracks[r], last, h))
                    std::swap(l, r);
                const double lx = extended(tracks[l], last, h), rx = extended(tracks[r], last, h);
                if (lx >= road.reference - w * .012 || rx <= road.reference + w * .012)
                    continue;
                bool adjacent = true;
                for (size_t k = 0; k < tracks.size(); ++k)
                    if (k != i && k != j && tracks[k].nn >= 8 &&
                        tracks[k].first <= last && tracks[k].last >= last)
                    {
                        const double x = extended(tracks[k], last, h);
                        if (x > lx + w * .025 && x < rx - w * .025)
                            adjacent = false;
                    }
                if (!adjacent)
                    continue;
                double narrow = w, wide = 0.;
                int supported = 0, samples = 0;
                for (int n = 0; n <= 8; ++n)
                {
                    const int y = first + (last - first) * n / 8;
                    const double a = extended(tracks[l], y, h), b = extended(tracks[r], y, h);
                    narrow = std::min(narrow, b - a);
                    wide = std::max(wide, b - a);
                    for (int k = 1; k <= 5; ++k)
                    {
                        const int x = (int)std::lround(a + (b - a) * k / 6);
                        supported += x >= 0 && x < w && road.mask[y * w + x];
                        ++samples;
                    }
                }
                if (narrow < w * .08 || wide > w || wide > narrow * 2. || supported * 2 < samples)
                    continue;
                const double score = (double)supported / samples + (double)(last - first) / h +
                                     (double)last / h;
                if (score > best)
                {
                    best = score;
                    left_id = l;
                    right_id = r;
                    near_y = last;
                }
            }
        if (left_id < 0)
        {
            result.left.reason = result.right.reason = LINE_NO_TRACK;
            return;
        }
        if (trace)
        {
            trace->near_y = near_y;
            trace->selected_left = left_id;
            trace->selected_right = right_id;
        }
        const auto &left = tracks[left_id];
        const auto &right = tracks[right_id];
        const double width = extended(right, near_y, h) - extended(left, near_y, h);
        result.left = measure(left, rgb, w, h, road.first, road.last - road.first, width,
                              trace ? &trace->left : nullptr);
        result.right = measure(right, rgb, w, h, road.first, road.last - road.first, width,
                               trace ? &trace->right : nullptr);
    }

  public:
    static YOLOLineStyleResult describe_points(const PointLanePair &pair, const uint8_t *rgb,
                                               bool ground = false, const uint8_t *coverage = nullptr)
    {
        YOLOLineStyleResult out = {};
        out.width = pair.width;
        out.height = pair.height;
        out.reference_x = pair.width * .5;
        out.object_count = pair.candidates;
        out.left.reason = out.right.reason = LINE_NO_TRACK;
        if (!rgb || pair.last < pair.first)
            return out;
        const auto describe = [&](const std::vector<Point> &points) {
            Track geometry;
            geometry.first = points.front().y;
            geometry.last = points.back().y;
            for (size_t i = 1; i < points.size(); ++i)
                geometry.length += std::hypot(points[i].x - points[i - 1].x,
                                             points[i].y - points[i - 1].y);
            return measure(geometry, rgb, pair.width, pair.height, pair.first,
                           pair.last - pair.first,
                           pair.left.empty() || pair.right.empty() ? pair.width * .5 :
                           pair.right.back().x - pair.left.back().x,
                           nullptr, &points, ground, coverage);
        };
        if (!pair.left.empty()) out.left = describe(pair.left);
        if (!pair.right.empty()) out.right = describe(pair.right);
        return out;
    }

    explicit EgoLaneSelector(double normalized_anchor = -1, bool bottom_up = false)
        : fixed_anchor_(normalized_anchor), bottom_up_(bottom_up)
    {
    }
    YOLOLineStyleResult analyze(const float *mask, const uint8_t *green, const uint8_t *rgb, int w,
                                int h, LineStyleTrace *trace = nullptr, bool ground = false) const
    {
        YOLOLineStyleResult result = {};
        result.width = w;
        result.height = h;
        result.left.reason = result.right.reason = LINE_NO_MASK;
        if (!mask || !rgb || w < 80 || h < 60)
            return result;
        Road road = green ? road_region(green, w, h) : Road{};
        if (bottom_up_ && !ground)
        {
            select_sampled_pair(mask, rgb, road, w, h, result, trace);
            return result;
        }
        if (!road.area || (road.reference < 0 && fixed_anchor_ < 0))
            return result;
        if (fixed_anchor_ >= 0 && fixed_anchor_ <= 1)
        {
            road.reference = fixed_anchor_ * w;
            road.centered = true;
        }
        auto tracks = objects(mask, rgb, road, w, h, ground, bottom_up_);
        result.road_area = road.area;
        result.object_count = (uint32_t)tracks.size();
        result.reference_x = road.reference;
        result.reference_y = road.reference_y;
        int near_y = (int)(road.first + .72 * (road.last - road.first));
        if (!ground)
            std::sort(tracks.begin(), tracks.end(), [&](const Track &a, const Track &b)
                      { return extended(a, near_y, h) < extended(b, near_y, h); });
        if (trace)
        {
            trace->reference_x = road.reference;
            if (!ground)
                trace->near_y = near_y;
            trace->road_area = road.area;
            for (const auto &t : tracks)
                trace->objects.push_back({t.fit.a, t.fit.b, t.fit.c, t.first, t.last, t.nn,
                                          t.points.back().x});
        }
        if (ground)
        {
            select_ground_pair(tracks, road, rgb, w, h, result, trace);
            return result;
        }
        double best = -1;
        int selected = -1;
        for (size_t i = 0; i + 1 < tracks.size(); ++i)
        {
            const Track &left = tracks[i];
            const Track &right = tracks[i + 1];
            if (left.nn + right.nn < 8)
                continue;
            const int far = std::max(left.first, right.first);
            double lx = extended(left, near_y, h), rx = extended(right, near_y, h), width = rx - lx;
            if (width < w * .08 || width > w * 1.8 || lx >= road.reference - w * .012 ||
                rx <= road.reference + w * .012)
                continue;
            double far_width = extended(right, far, h) - extended(left, far, h);
            if (far_width < -w * .025 || far_width > width * .9 + w * .03)
                continue;
            int supported = 0, total = 0, axis_rows = 0;
            for (int y = road.first; y <= road.last; y += 2)
            {
                int a = std::max(road.left[y], std::max(0, (int)std::ceil(extended(left, y, h)))),
                    b = std::min(road.right[y],
                                 std::min(w - 1, (int)std::floor(extended(right, y, h))));
                if (a > b)
                    continue;
                int row_count = 0;
                for (int x = a; x <= b; ++x)
                {
                    ++total;
                    if (road.mask[y * w + x])
                    {
                        ++supported;
                        ++row_count;
                    }
                }
                axis_rows += row_count >= 3;
            }
            if (total < 32 || (unsigned)supported < road.area / 30 || axis_rows < 8)
                continue;
            double coverage = (double)supported / total;
            if (coverage < .3)
                continue;
            // 面积只用于同等候选的优先选择，不能选取
            // 排除了本车参考轴的另一条走廊。
            double balance = std::min(road.reference - lx, rx - road.reference) / width;
            double score =
                coverage + .25 * balance + .1 * std::min(1., (double)supported / road.area);
            if (score > best)
            {
                best = score;
                selected = (int)i;
            }
        }
        if (selected < 0)
        {
            result.left.reason = result.right.reason = LINE_NO_TRACK;
            return result;
        }
        if (trace)
        {
            trace->selected_left = selected;
            trace->selected_right = selected + 1;
        }
        double visible_width = std::min((double)w, extended(tracks[selected + 1], near_y, h)) -
                               std::max(0., extended(tracks[selected], near_y, h));
        result.left = measure(tracks[selected], rgb, w, h, road.first, road.last - road.first,
                              visible_width, trace ? &trace->left : nullptr);
        result.right = measure(tracks[selected + 1], rgb, w, h, road.first, road.last - road.first,
                               visible_width, trace ? &trace->right : nullptr);
        return result;
    }
};
}
#endif
