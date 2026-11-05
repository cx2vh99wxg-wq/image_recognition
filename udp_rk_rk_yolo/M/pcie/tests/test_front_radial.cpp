#include "lane_curve_geometry.h"
#include "front_camera_layout.h"
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
    const size_t half_frame = (size_t)mosaic_width * height;
    std::vector<uint16_t> raw(half_frame * 2);
    for (int y = 0; y < mosaic_height; ++y)
        for (int x = 0; x < mosaic_width; ++x)
            raw[y * mosaic_width + x] = ((y < height) == (x < width)) ? 0x7e0 : x < width ? 0x1f : 0xf800;
    const auto normal_frame = raw;
    m_lane::FrontCameraLayout layout;
    assert(layout.observe(raw.data(), mosaic_width, mosaic_height) && !layout.rows_swapped());
    std::rotate(raw.begin(), raw.begin() + half_frame, raw.end());
    m_lane::FrontCameraLayout first_frame_swapped;
    assert(first_frame_swapped.observe(raw.data(), mosaic_width, mosaic_height) && first_frame_swapped.rows_swapped());
    assert(!layout.observe(raw.data(), mosaic_width, mosaic_height));
    assert(!layout.observe(raw.data(), mosaic_width, mosaic_height));
    assert(layout.observe(raw.data(), mosaic_width, mosaic_height) && layout.rows_swapped());
    std::vector<uint8_t> normalized(3 * raw.size()), expected(normalized.size());
    layout.unpack(raw.data(), normalized.data(), mosaic_width, mosaic_height);
    frame_unpack_rgb565(normal_frame.data(), expected.data(), normal_frame.size());
    assert(normalized == expected);
    std::vector<uint16_t> ambiguous(raw.size(), 0xffff);
    assert(!layout.observe(ambiguous.data(), mosaic_width, mosaic_height) && layout.rows_swapped());

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
    assert(preview.enabled() && preview.display_scale() > .894 && preview.display_scale() < .896);
    m_lane::FrontRadialPreview model_input(mosaic_width, mosaic_height, camera, nullptr, true);
    const size_t tile_size = (size_t)width * height;
    const size_t frame_size = 4 * tile_size;
    std::vector<float> lane(tile_size), drivable(2 * tile_size);
    std::vector<float> raw_lane(frame_size), raw_drivable(2 * frame_size);
    for (size_t i = 0; i < tile_size; ++i)
    {
        lane[i] = (float)(i + 1);
        drivable[i] = -lane[i];
        drivable[tile_size + i] = 2 * lane[i];
    }
    assert(model_input.enabled());
    model_input.restore_front_masks(lane.data(), drivable.data(), raw_lane.data(), raw_drivable.data());
    assert(raw_lane[0] == 0.f);
    const int raw_x = 91, raw_y = 160;
    m_lane::ImagePoint corrected{(double)raw_x, (double)raw_y};
    assert(camera.undistort(corrected, width, height));
    const int cx = (int)std::lround(159.5 + (corrected.x - 159.5) * model_input.display_scale());
    const int cy = (int)std::lround(119.5 + (corrected.y - 119.5) * model_input.display_scale());
    const size_t dst = (size_t)raw_y * mosaic_width + raw_x;
    const size_t src = (size_t)cy * width + cx;
    assert(raw_lane[dst] == lane[src] && raw_drivable[dst] == drivable[src]);
    assert(raw_drivable[frame_size + dst] == drivable[tile_size + src]);
    for (size_t i = 0; i < frame_size; ++i)
        if (i % mosaic_width >= width || i / mosaic_width >= height)
            assert(raw_lane[i] == 0.f && raw_drivable[i] == 0.f && raw_drivable[frame_size + i] == 0.f);
    m_lane::FrontRadialPreview identity_input(mosaic_width, mosaic_height, m_lane::FrontRadial(), nullptr, true);
    assert(identity_input.enabled());
    identity_input.restore_front_masks(lane.data(), drivable.data(), raw_lane.data(), raw_drivable.data());
    assert(raw_lane[dst] == lane[(size_t)raw_y * width + raw_x]);
    // The top midpoint bounds the full, same-aspect crop.
    m_lane::ImagePoint beyond_y{159.5, 119.5 - 119.5 / (preview.display_scale() - .001)};
    assert(camera.distort(beyond_y, width, height) && beyond_y.y < 0.);
    preview.apply(mosaic.data());
    std::vector<uint16_t> udp_frame(raw.size());
    frame_pack_rgb565(mosaic.data(), udp_frame.data(), udp_frame.size());
    for (int y = 0; y < mosaic_height; ++y)
        for (int x = 0; x < mosaic_width; ++x)
        {
            if ((x < width) != (y < height))
                assert(pixel(x, y)[0] == 231 && pixel(x, y)[1] == 231 && pixel(x, y)[2] == 231);
            else
                assert(pixel(x, y)[2] == 199);
            if (x >= width && y >= height)
            {
                assert(std::memcmp(pixel(x, y), pixel(x - width, y - height), 3) == 0);
                assert(udp_frame[y * mosaic_width + x] == udp_frame[(y - height) * mosaic_width + x - width]);
            }
        }
    for (auto point : {m_lane::ImagePoint{159., 119.}, {90., 96.}, {227., 179.}, {319., 239.}, {0., 0.}})
    {
        const auto p = pixel((int)point.x, (int)point.y);
        point.x = 159.5 + (point.x - 159.5) / preview.display_scale();
        point.y = 119.5 + (point.y - 119.5) / preview.display_scale();
        assert(camera.distort(point, width, height));
        assert(std::abs(p[0] - point.y) < .51 && std::abs(p[1] - point.x / 2.) < 1.51 && p[2] == 199);
    }
    m_lane::FrontRadialPreview pincushion(mosaic_width, mosaic_height, m_lane::FrontRadial(.15));
    assert(pincushion.enabled() && pincushion.display_scale() > 1.);
    std::fill(mosaic.begin(), mosaic.end(), 173);
    pincushion.apply(mosaic.data());
    assert(std::all_of(mosaic.begin(), mosaic.end(), [](uint8_t p) { return p == 173; }));

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
    std::puts("PASS front radial: DMA row order, centered 4:3 crop, both M center tiles, S front center, and steering geometry");
}
