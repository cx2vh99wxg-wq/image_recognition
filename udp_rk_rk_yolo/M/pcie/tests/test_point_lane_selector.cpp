#include "lane_curve_geometry.h"
#include <cassert>
#include <cstdio>

int main()
{
    constexpr int w = 320, h = 240;
    std::vector<float> mask(w * h);
    const auto line = [&](int x, int first, int last, int gap_first = -1, int gap_last = -1) {
        for (int y = first; y <= last; ++y)
            if (y < gap_first || y > gap_last)
                mask[y * w + x] = .9f;
    };
    line(70, 120, 190);
    line(125, 120, 180);
    line(160, 120, 180);
    line(200, 140, 190, 150, 150);
    line(245, 140, 205);
    line(155, 150, 180); // Exactly 30 rows of span.
    auto pair = m_lane::select_point_lanes(mask.data(), w, h);
    assert(pair.candidates == 5 && pair.sampled);
    assert(pair.first == 140 && pair.last == 180);
    assert(pair.left.back().x == 125 && pair.right.back().x == 200);
    assert(pair.left_y[1] == 150 && pair.right_y[1] == 149);
    assert(pair.left_x[1] == 125 && pair.right_x[1] == 200);
    assert(m_lane::estimate_ego_curve(pair).confidence > 0);

    mask.assign(w * h, 0);
    line(120, 120, 180, 148, 152);
    line(210, 140, 190, 147, 152);
    pair = m_lane::select_point_lanes(mask.data(), w, h);
    assert(pair.sampled && pair.left_y[1] == 147 && pair.right_y[1] == 153);
    assert(pair.left_x[1] == 120 && pair.right_x[1] == 210);
    mask[153 * w + 210] = 0;
    pair = m_lane::select_point_lanes(mask.data(), w, h);
    assert(!pair.sampled && m_lane::estimate_ego_curve(pair).confidence == 0);

    mask.assign(w * h, 0);
    line(120, 50, 74);
    line(120, 84, 108); // A distance of exactly 10 must split the track.
    assert(m_lane::select_point_lanes(mask.data(), w, h).candidates == 0);
    mask[83 * w + 120] = .9f; // A distance of 9 joins it.
    assert(m_lane::select_point_lanes(mask.data(), w, h).candidates == 1);

    pair.first = 184;
    pair.last = 222;
    pair.sampled = true;
    pair.left_x = {108.5, 124.5, 130.5, 132., 132.5};
    pair.right_x = {136.5, 164.5, 178.5, 189.5, 201.};
    const auto left = m_lane::estimate_ego_curve(pair, true);
    assert(left.type == 1 && left.confidence == 60 && left.angle == 48);
    m_lane::EgoCurveConfirm confirm;
    assert(confirm.update(left, 1, 100000000).confidence == 0);
    assert(confirm.update(left, 2, 300000000).confidence == 0);
    assert(confirm.update(left, 3, 500000000).confidence == 60);

    for (size_t i = 0; i < pair.left_x.size(); ++i)
    {
        pair.left_x[i] = 110 + 4 * i;
        pair.right_x[i] = 180 + 4 * i;
    }
    const auto straight = m_lane::estimate_ego_curve(pair, true);
    assert(straight.type == 0 && straight.confidence > 0 && straight.curvature == 0);
    pair.right.clear();
    assert(m_lane::estimate_ego_curve(pair, true).confidence == 0);
    std::puts("POINT_LANES endpoint pairing, row lookup, IPM heading and confirmation PASS");
}
