#include "ego_lane_selector.h"
#include "front_radial_preview.h"
#include "lane_curve_geometry.h"
#include <cassert>
#include <cstdio>

int main(int argc, char **argv)
{
    constexpr int w = 320, h = 240, mw = 2 * w, mh = 2 * h;
    const size_t pixels = (size_t)mw * mh;
    const m_lane::FrontRadial lens(-.188218);
    const auto ground = m_lane::GroundProjection::for_camera(
        argc == 2 ? nullptr : ".36,.68,.64,.69,.79,1.02,.22,1.00,1.36",
        argc == 2 ? argv[1] : "unused");
    m_lane::FrontRadialPreview preview(mw, mh, lens, &ground);
    assert(preview.enabled());
    std::vector<uint8_t> rgb(3 * pixels, 51), corrected(3 * w * h), green(w * h);
    std::vector<float> lane(pixels), road(2 * pixels), corrected_lane(w * h);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
        {
            const size_t i = y * mw + x;
            rgb[3 * i] = (uint8_t)(x / 2);
            lane[i] = (float)rgb[3 * i];
            road[i + pixels] = 1;
        }
    preview.render(rgb.data(), corrected.data());
    assert(!preview.remap_masks(lane.data(), road.data(), w, h, corrected_lane.data(), green.data()));
    assert(preview.remap_masks(lane.data(), road.data(), mw, mh, corrected_lane.data(), green.data()));
    size_t valid = 0;
    for (size_t i = 0; i < green.size(); ++i)
    {
        valid += green[i];
        assert(std::abs(corrected[3 * i] - corrected_lane[i]) <= .5f);
        if (!green[i])
            assert(corrected[3 * i + 1] == 0 && corrected_lane[i] == 0);
    }
    assert(valid > green.size() / 2 && valid < green.size());
    preview.apply(rgb.data());
    for (int y = 0; y < h; ++y)
    {
        assert(std::memcmp(rgb.data() + 3 * y * mw,
                           rgb.data() + 3 * ((y + h) * mw + w), w * 3) == 0);
        for (int x = 0; x < w; ++x)
            assert(rgb[3 * (y * mw + x + w)] == 51 && rgb[3 * ((y + h) * mw + x)] == 51);
    }
    assert(rgb[3 * (4 * mw + 4)] == 0 && rgb[3 * (6 * mw + 9)] == 255);

    std::fill(corrected_lane.begin(), corrected_lane.end(), 0.f);
    std::fill(corrected.begin(), corrected.end(), 48);
    std::fill(green.begin(), green.end(), 0);
    for (int y = 80; y <= 220; ++y)
    {
        const int offset = (190 - y) / 5;
        for (int x = 99 + offset; x <= 221 + offset; ++x)
            green[y * w + x] = 1;
        for (int x = 98 + offset; x <= 100 + offset && y <= 178; ++x)
            corrected_lane[y * w + x] = 1;
        for (int x = 220 + offset; x <= 222 + offset && y <= 205; ++x)
            corrected_lane[y * w + x] = 1;
    }
    m_lane::LineStyleTrace trace;
    const auto pair = m_lane::EgoLaneSelector(.5).analyze(corrected_lane.data(), green.data(),
                                                         corrected.data(), w, h, &trace, true);
    assert(trace.selected_left >= 0 && trace.selected_right >= 0);
    assert(trace.near_y == 178 && pair.left.last_y == 178 && pair.right.last_y == 205);
    const auto turn = m_lane::estimate_ego_curve(pair, true);
    assert(turn.confidence && turn.type == 2 && turn.span_px == 98);
    std::fill(rgb.begin(), rgb.end(), 51);
    std::vector<uint16_t> wire(pixels);
    frame_pack_rgb565(rgb.data(), wire.data(), pixels);
    const uint16_t before = wire[0];
    preview.apply(rgb.data(), wire.data(), &pair);
    const int x = (int)std::lround(m_lane::curve_x(pair.left, 130, h));
    const auto *left_edge = rgb.data() + 3 * (130 * mw + x);
    assert(left_edge[0] == 255 && left_edge[1] == 240 && left_edge[2] == 48);
    uint16_t packed = 0;
    frame_pack_rgb565(left_edge, &packed, 1);
    assert(wire[(h + 130) * mw + w + x] == packed && wire[130 * mw + x] == before);
    assert(rgb[3 * ((h + 130) * mw + w + x)] == 51);
    std::puts("PASS IPM: RGB/mask co-registration, both front tiles, measured near endpoint, heading");
}
