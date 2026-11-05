#include "lane_curve_geometry.h"
#include "front_radial_preview.h"
#include <cassert>
#include <cstdio>

int main(int argc, char **argv)
{
    constexpr int width = 320, height = 240;
    const auto camera = m_lane::FrontRadial::from_text("-0.188218");
    assert(camera.valid() && camera.enabled());
    for (const auto source : {m_lane::ImagePoint{0, 0}, {159.5, 119.5}, {319, 239}, {41, 185}})
    {
        auto point = source;
        assert(camera.undistort(point, width, height));
        assert(camera.distort(point, width, height));
        assert(std::hypot(point.x - source.x, point.y - source.y) < 1e-9);
    }

    // Subpixel line centers in the approved nine-grid capture, source 60199.
    const m_lane::ImagePoint stripes[][3] = {
        {{43., 36.14}, {163., 30.46}, {289., 43.58}},
        {{33., 202.74}, {163., 218.24}, {295., 209.53}}};
    for (const auto &line : stripes)
    {
        auto left = line[0], middle = line[1], right = line[2];
        auto deviation = [&] {
            return std::abs(middle.y - left.y - (middle.x - left.x) *
                            (right.y - left.y) / (right.x - left.x));
        };
        assert(deviation() > 8.);
        assert(camera.undistort(left, width, height) && camera.undistort(middle, width, height) &&
               camera.undistort(right, width, height));
        assert(deviation() < 1.2);
    }
    assert(!m_lane::FrontRadial::for_camera("0", "unused").enabled());
    assert(!m_lane::FrontRadial::for_camera("nan", "unused").valid());
    if (argc == 2)
    {
        const auto from_file = m_lane::FrontRadial::for_camera(nullptr, argv[1]);
        assert(from_file.valid() && from_file.coefficient() == camera.coefficient());
    }

    // Exercise the actual RGB remapper, with sentinels in the other three tiles.
    constexpr int mosaic_width = width * 2, mosaic_height = height * 2;
    std::vector<uint8_t> mosaic(mosaic_width * mosaic_height * 3, 231);
    auto pixel = [&](int x, int y) { return mosaic.data() + (y * mosaic_width + x) * 3; };
    for (int y = 0; y < height; ++y)
        for (int x = 0; x < width; ++x)
        {
            auto p = pixel(x, y);
            p[0] = (uint8_t)y;
            p[1] = (uint8_t)(x / 2);
            p[2] = 199;
        }
    auto identity = mosaic;
    m_lane::FrontRadialPreview disabled(mosaic_width, mosaic_height, m_lane::FrontRadial());
    disabled.apply(mosaic.data());
    assert(mosaic == identity);
    m_lane::FrontRadialPreview preview(mosaic_width, mosaic_height, camera);
    assert(preview.enabled() && preview.display_scale() > .70 && preview.display_scale() < .72);
    preview.apply(mosaic.data());
    for (int y = 0; y < mosaic_height; ++y)
        for (int x = 0; x < mosaic_width; ++x)
            if (x >= width || y >= height)
                assert(pixel(x, y)[0] == 231 && pixel(x, y)[1] == 231 && pixel(x, y)[2] == 231);
    for (auto point : {m_lane::ImagePoint{159., 119.}, {90., 96.}, {227., 179.}, {319., 239.}, {0., 0.}})
    {
        const auto p = pixel((int)point.x, (int)point.y);
        point.x = 159.5 + (point.x - 159.5) / preview.display_scale();
        point.y = 119.5 + (point.y - 119.5) / preview.display_scale();
        assert(camera.distort(point, width, height));
        assert(std::abs(p[0] - point.y) < .51 && std::abs(p[1] - point.x / 2.) < 1.51 && p[2] == 199);
    }
    assert(pixel(159, 0)[0] == 0 && pixel(159, 0)[1] == 0 && pixel(159, 0)[2] == 0);

    YOLOLineStyleResult road{};
    road.width = width;
    road.height = height;
    road.road_area = 6000;
    road.object_count = 2;
    road.left.first_y = road.right.first_y = 140;
    road.left.last_y = road.right.last_y = 220;
    road.left.fit_a = 69.5;
    road.right.fit_a = 249.5;
    road.left.fit_b = -85.;
    road.right.fit_b = 85.;
    const auto map = [&](m_lane::ImagePoint &point) {
        return camera.undistort(point, width, height);
    };
    const auto straight = m_lane::estimate_ego_curve_mapped(road, map);
    assert(straight.confidence >= 60 && straight.type == 0);
    for (int direction : {-1, 1})
    {
        auto bend = road;
        // Image y grows toward the camera; the requested heading points forward.
        bend.left.fit_b -= 96. * direction;
        bend.right.fit_b -= 96. * direction;
        bend.left.fit_c = bend.right.fit_c = -18. * direction;
        const auto shallow = m_lane::estimate_ego_curve_mapped(bend, map);
        assert(shallow.confidence >= 60 && shallow.type == 0);
        bend.left.fit_c = bend.right.fit_c = -45. * direction;
        const auto turn = m_lane::estimate_ego_curve_mapped(bend, map);
        assert(turn.confidence >= 60 && turn.type == (direction < 0 ? 1u : 2u));
    }
    road.left.last_y = 0;
    assert(!m_lane::estimate_ego_curve_mapped(road, map).confidence);
    std::puts("PASS front radial: grid straightness, round trip, RGB preview, other tiles, straight/left/right and missing edge");
}
