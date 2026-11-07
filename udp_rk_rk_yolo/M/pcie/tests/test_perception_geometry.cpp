#include "line_style_analyzer.h"
#include "traffic_frame_input.h"
#include <cassert>
#include <cstdio>

int main()
{
    constexpr int w = 320, h = 240;
    std::vector<float> lane(w * h);
    std::vector<uint8_t> bird(w * h * 3, 45);
    for (int y = 60; y <= 220; ++y)
        for (int x : {100, 220})
        {
            lane[y * w + x] = 1;
            if (x == 100 || (y - 60) % 32 < 16)
                for (int dx = -2; dx <= 2; ++dx)
                    for (int c = 0; c < 3; ++c)
                        bird[(y * w + x + dx) * 3 + c] = 230;
        }
    const auto points = m_lane::select_point_lanes(lane.data(), w, h);
    assert(points.sampled && points.candidates == 2);
    const auto raw = m_lane::EgoLaneSelector::describe_points(points, bird.data(), true);
    assert(raw.left.type == LINE_SOLID && raw.right.type == LINE_DASHED);
    assert(raw.road_area == 0);
    m_lane::LineStyleAnalyzer confirm;
    for (unsigned i = 1; i <= 12; ++i)
    {
        const auto stable = confirm.confirm(raw, i, 1000000000ULL + i * 200000000ULL, &points);
        if (i < 2)
            assert(!stable.left.type && !stable.right.type);
        else
            assert(stable.left.type == LINE_SOLID && stable.right.type == LINE_DASHED);
    }
    auto shifted = points;
    for (auto &p : shifted.right)
        p.x += 45;
    auto changed = raw;
    changed.right.x_bottom += 45;
    changed.right.x_top += 45;
    auto stable = confirm.confirm(changed, 13, 3600000000ULL, &shifted);
    assert(stable.left.type == LINE_SOLID && stable.right.type == LINE_UNKNOWN);
    stable = confirm.confirm(changed, 14, 3800000000ULL, &shifted);
    assert(stable.right.type == LINE_DASHED);
    stable = confirm.confirm(changed, 15, 4000000000ULL, &shifted);
    assert(stable.right.type == LINE_DASHED);
    assert(!confirm.confirm(changed, 15, 4000000000ULL, &shifted).left.type);

    // Missing map pixels inside a marking must not create a dashed gap.
    for (int y = 60; y <= 220; ++y)
        for (int x = 212; x <= 228; ++x)
            for (int c = 0; c < 3; ++c)
                bird[(y * w + x) * 3 + c] = y >= 115 && y <= 135 ? 0 : 230;
    const auto black_gap = m_lane::EgoLaneSelector::describe_points(points, bird.data(), true);
    assert(black_gap.right.type != LINE_DASHED);

    std::fill(lane.begin(), lane.end(), 0.f);
    std::fill(bird.begin(), bird.end(), 45);
    for (int y = 180; y <= 220; ++y)
        for (int x : {100, 220})
        {
            lane[y * w + x] = 1;
            for (int dx = -2; dx <= 2; ++dx)
                for (int c = 0; c < 3; ++c)
                    bird[(y * w + x + dx) * 3 + c] = 230;
        }
    const auto short_points = m_lane::select_point_lanes(lane.data(), w, h);
    const auto short_line =
        m_lane::EgoLaneSelector::describe_points(short_points, bird.data(), true);
    assert(short_line.left.type == LINE_SOLID && short_line.right.type == LINE_SOLID);

    constexpr int mw = w * 2, mh = h * 2;
    std::vector<uint8_t> mosaic(mw * mh * 3, 0);
    for (int y = 0; y < mh; ++y)
        for (int x = 0; x < mw; ++x)
        {
            auto *p = mosaic.data() + (y * mw + x) * 3;
            p[0] = x < w && y < h ? (uint8_t)(x / 2) : 250;
            p[1] = x < w && y < h ? (uint8_t)(y / 2) : 0;
            p[2] = x < w && y < h ? 20 : 5;
        }
    const auto before = mosaic;
    const m_lane::FrontRadial camera(-.188218);
    const m_lane::FrontRadialPreview correction(mw, mh, camera, nullptr, true);
    assert(correction.enabled());
    std::vector<uint8_t> rgb;
    assert(traffic::copy_input_rgb(mosaic.data(), mw, mh,
                                  traffic::InputView(0, 0, traffic::PixelOrder::BGR),
                                  &correction, rgb));
    const double cx = (w - 1) * .5, cy = (h - 1) * .5;
    const double scale = correction.display_scale();
    for (const auto &pixel : {m_lane::ImagePoint{20, 20}, {150, 100}, {300, 220}})
    {
        auto source = m_lane::ImagePoint{cx + (pixel.x - cx) / scale,
                                        cy + (pixel.y - cy) / scale};
        assert(camera.distort(source, w, h));
        const auto at = (size_t)((int)pixel.y * w + (int)pixel.x) * 3;
        assert(rgb[at] == 20);
        assert(std::abs(rgb[at + 1] - source.y / 2) <= 1.1);
        assert(std::abs(rgb[at + 2] - source.x / 2) <= 1.1);
    }
    assert(mosaic == before);
    assert(!traffic::copy_input_rgb(mosaic.data(), w, h, traffic::InputView(), &correction, rgb));
    assert(traffic::copy_input_rgb(mosaic.data(), mw, mh,
                                  traffic::InputView(320, 240, traffic::PixelOrder::BGR),
                                  nullptr, rgb));
    assert(rgb[0] == 5 && rgb[1] == 0 && rgb[2] == 250);
    std::puts("PASS perception geometry: BEV paint, stable point identity, black-map barriers, corrected traffic coordinates and RGB");
}
