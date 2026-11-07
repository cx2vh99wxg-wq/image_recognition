#include "ego_lane_selector.h"
#include "front_radial_preview.h"
#include "lane_curve_geometry.h"

#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <fstream>

namespace
{
constexpr int width = 320, height = 240;
constexpr size_t pixels = width * height;

struct Scene
{
    std::vector<float> lane = std::vector<float>(pixels);
    std::vector<uint8_t> road = std::vector<uint8_t>(pixels);
    std::vector<uint8_t> rgb = std::vector<uint8_t>(3 * pixels, 68);
};

Scene make_scene(int direction, bool dashed, bool extra)
{
    Scene scene;
    for (int y = 40; y < height; ++y)
    {
        const double near = (y - 40) / 199.;
        const double center = 160 + direction * 138 * (1. - near) * (1. - near);
        const double half = 9 + 82 * near;
        const int left = (int)std::lround(center - half), right = (int)std::lround(center + half);
        for (int x = std::max(0, left); x <= std::min(width - 1, right); ++x)
            scene.road[y * width + x] = 1;
        for (int x : {left, right, extra ? right + 30 : width + 3})
            if (!dashed || x != right || (y - 40) % 19 < 12)
                for (int px = std::max(0, x - 2); px <= std::min(width - 1, x + 2); ++px)
                {
                    scene.lane[y * width + px] = .9f;
                    for (int c = 0; c < 3; ++c)
                        scene.rgb[(y * width + px) * 3 + c] = 220;
                }
    }
    return scene;
}

struct Result
{
    m_lane::EgoCurve curve;
    bool pair;
};

Result compare(const char *name, const Scene &scene, bool bottom_up, const m_lane::FrontRadial *camera = nullptr)
{
    const m_lane::EgoLaneSelector selector(-1., bottom_up);
    m_lane::LineStyleTrace trace;
    const auto r = selector.analyze(scene.lane.data(), scene.road.data(), scene.rgb.data(), width, height, &trace);
    const auto curve = camera ? m_lane::estimate_ego_curve_mapped(r, [&](m_lane::ImagePoint &p) {
                           return camera->undistort(p, width, height);
                       }) : m_lane::estimate_ego_curve(r);
    using Clock = std::chrono::steady_clock;
    const auto start = Clock::now();
    unsigned guard = 0;
    for (int i = 0; i < 10; ++i)
        guard += selector.analyze(scene.lane.data(), scene.road.data(), scene.rgb.data(), width, height).object_count;
    const double ms = std::chrono::duration<double, std::milli>(Clock::now() - start).count() / 10.;
    std::printf("%s %s tracks=%u pair=%d/%d ref=%.1f L=%u..%u R=%u..%u span=%.1f dir=%u conf=%u reason=%u ms=%.3f guard=%u\n",
                name, bottom_up ? "bottom-up" : "seed-fit", r.object_count, trace.selected_left,
                trace.selected_right, r.reference_x, r.left.first_y, r.left.last_y,
                r.right.first_y, r.right.last_y, curve.span_px, curve.type, curve.confidence,
                curve.reason, ms, guard);
    if (bottom_up && std::strncmp(name, "saved-", 6) == 0)
        for (const auto &object : trace.objects)
        {
            const auto x = [&](int y) {
                const double t = (y - height * .95) / (height * .52);
                return object.a + t * (object.b + t * object.c);
            };
            std::printf("%s candidate: y=%d..%d fit_x=%.1f..%.1f bottom_x=%.1f ref_y=%.1f support=%u\n",
                        name, object.first, object.last, x(object.first), x(object.last),
                        object.observed_bottom_x, trace.near_y, object.nn_points);
        }
    return {curve, trace.selected_left >= 0 && trace.selected_right >= 0};
}

bool check_sampling()
{
    Scene scene;
    const auto put = [&](int y, int x, float score = .9f) { scene.lane[y * width + x] = score; };
    const m_lane::EgoLaneSelector selector(.25, true);
    const auto analyze = [&]() {
        m_lane::LineStyleTrace trace;
        selector.analyze(scene.lane.data(), nullptr, scene.rgb.data(), width, height, &trace);
        return trace;
    };
    for (int y = 120; y <= 183; ++y)
    {
        for (int x : {80, 130, 131, 200, 235, 160})
            put(y, x);
        put(y, 170, .3f);
    }
    // A nearer, non-sampled track must never enter the candidate set.
    for (int y = 122; y < 183; y += 10)
        put(y, 155);
    auto trace = analyze();
    assert(trace.reference_x == 160 && trace.near_y == 183);
    assert(trace.objects.size() == 6);
    for (const auto &object : trace.objects)
        assert(object.first == 123 && object.last == 183 && object.nn_points == 7);
    assert(trace.selected_left >= 0 && trace.selected_right >= 0);
    assert(trace.objects[trace.selected_left].observed_bottom_x == 131);
    assert(trace.objects[trace.selected_right].observed_bottom_x == 200);

    std::fill(scene.lane.begin(), scene.lane.end(), 0.f);
    for (int y = 131; y <= 180; ++y)
        put(y, 100);
    trace = analyze();
    assert(trace.objects.empty() && trace.selected_left < 0 && trace.selected_right < 0);
    put(130, 100);
    trace = analyze();
    assert(trace.objects.size() == 1 && trace.selected_left == 0 && trace.selected_right < 0);
    assert(trace.objects.front().first == 130 && trace.objects.front().last == 180);
    // Check the cutoff for an individual chain even when the frame range is larger.
    put(120, 280);
    for (int y = 140; y <= 180; ++y)
        put(y, 210);
    trace = analyze();
    assert(trace.objects.size() == 1 && trace.selected_left == 0 && trace.selected_right < 0);
    std::puts("SAMPLE_STEP_ENDPOINT_AND_INCLUSIVE_SPAN_PASS");
    return true;
}

Scene saved_scene(const char *path)
{
    Scene scene;
    std::vector<uint8_t> lane(pixels);
    std::ifstream file(path, std::ios::binary);
    assert(file.read((char *)scene.rgb.data(), scene.rgb.size()));
    assert(file.read((char *)lane.data(), lane.size()));
    assert(file.read((char *)scene.road.data(), scene.road.size()));
    assert(file.peek() == EOF);
    for (size_t i = 0; i < pixels; ++i)
        scene.lane[i] = lane[i] ? 1.f : 0.f;
    return scene;
}

Scene restore(const Scene &corrected, const char *path, const m_lane::FrontRadial &camera)
{
    Scene scene;
    std::ifstream raw(path, std::ios::binary);
    assert(raw.read((char *)scene.rgb.data(), scene.rgb.size()));
    std::vector<float> corrected_road(2 * pixels), raw_lane(4 * pixels), raw_road(8 * pixels);
    for (size_t i = 0; i < pixels; ++i)
    {
        corrected_road[i] = !corrected.road[i];
        corrected_road[pixels + i] = !!corrected.road[i];
    }
    m_lane::FrontRadialPreview preview(2 * width, 2 * height, camera, nullptr, true);
    assert(preview.enabled());
    preview.restore_front_masks(corrected.lane.data(), corrected_road.data(), raw_lane.data(), raw_road.data());
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
        {
            const size_t target = y * width + x, source = y * 2 * width + x;
            scene.lane[target] = raw_lane[source];
            scene.road[target] = raw_road[4 * pixels + source] > raw_road[source];
        }
    return scene;
}
}

int main(int argc, char **argv)
{
    bool pass = check_sampling();
    for (int d : {-1, 0, 1})
        for (bool dashed : {false, true})
        {
            const auto scene = make_scene(d, dashed, true);
            const char *name = d < 0 ? (dashed ? "left-dashed" : "left")
                                   : d > 0 ? (dashed ? "right-dashed" : "right")
                                           : (dashed ? "straight-dashed" : "straight");
            compare(name, scene, false);
            const auto candidate = compare(name, scene, true);
            pass &= candidate.pair && candidate.curve.confidence && candidate.curve.type == (d < 0 ? 1u : d > 0 ? 2u : 0u);
        }
    auto empty = make_scene(0, false, false);
    std::fill(empty.lane.begin(), empty.lane.end(), 0.f);
    std::fill(empty.rgb.begin(), empty.rgb.end(), 68);
    pass &= !compare("no-lane", empty, true).pair;
    if (argc == 4)
    {
        const auto radial = m_lane::FrontRadial::from_text("-0.188218");
        const auto legacy = saved_scene(argv[1]);
        const auto current = restore(saved_scene(argv[2]), argv[3], radial);
        compare("saved-left-old-mask", legacy, false, &radial);
        compare("saved-left-320-mask", current, false, &radial);
        for (const auto &fixture : {std::make_pair("saved-left-old-mask", &legacy),
                                    std::make_pair("saved-left-320-mask", &current)})
        {
            compare(fixture.first, *fixture.second, true, &radial);
        }
    }
    std::puts(pass ? "BOTTOM_UP_REGRESSION_PASS" : "BOTTOM_UP_REGRESSION_FAIL");
    return pass ? 0 : 1;
}
