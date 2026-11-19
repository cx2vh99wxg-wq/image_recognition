#include "front_radial_preview.h"
#include "lane_point_selector.h"
#include "yolo_integration.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char **argv)
{
    if (argc != 4) return 2;
    constexpr int w = 320, h = 240, mw = 2 * w, mh = 2 * h;
    const auto calibration = m_lane::GroundProjection::for_camera(nullptr, argv[3]);
    const auto ground = m_lane::GroundProjection::for_video(calibration, 0, 30, w, 180);
    m_lane::FrontRadialPreview front(mw, mh, m_lane::FrontRadial(), nullptr, true);
    m_lane::FrontRadialPreview ipm(mw, mh, m_lane::FrontRadial(), &ground);
    if (!ipm.enabled() || !front.enabled()) return 3;
    const auto mask_restore = front.mask_restore();
    const auto mask_pull = ipm.compose_masks_from(*mask_restore);
    if (!mask_pull.enabled()) return 4;
    YOLOEngine engine = {};
    if (yolo_engine_init(&engine, argv[1], nullptr) != 0) return 5;

    std::ifstream list(argv[2]);
    std::string source, destination;
    std::vector<uint8_t> mosaic(mw * mh * 3), tile(w * h * 3), green(w * h);
    std::vector<uint16_t> udp(mw * mh);
    std::vector<float> lane(w * h), raw_lane(mw * mh), raw_road(2 * mw * mh);
    while (list >> source >> destination)
    {
        std::ifstream input(source, std::ios::binary);
        if (!input.read(reinterpret_cast<char *>(tile.data()), tile.size())) return 6;
        std::fill(mosaic.begin(), mosaic.end(), 0);
        for (int y = 0; y < h; ++y)
            std::copy_n(tile.data() + y * w * 3, w * 3, mosaic.data() + y * mw * 3);
        front.render(mosaic.data(), tile.data());
        yolopv2_result_t result = {};
        if (yolo_engine_detect(&engine, tile.data(), w, h, &result) != 0 ||
            !mask_pull.remap(result.lane_line_data, result.drivable_area_data, result.width,
                             result.height, lane.data(), green.data())) return 7;
        const auto rows = m_lane::lane_mask_rows(lane.data(), w, h, 0, h);
        const auto points = m_lane::select_point_lanes(lane.data(), w, h, &rows);
        if (!points.sampled) continue;
        const auto curve = m_lane::estimate_ego_curve(points, true);

        mask_restore->restore(result.lane_line_data, result.drivable_area_data,
                              raw_lane.data(), raw_road.data());
        yolopv2_result_t restored{raw_lane.data(), raw_road.data(), mw, mh};
        if (yolo_overlay_lane_and_drivable_roi(mosaic.data(), mw, mh, &restored, w, h) < 0) return 8;
        ipm.apply(mosaic.data(), udp.data(), nullptr, &points);
        for (int y = 0; y < h; ++y)
            frame_unpack_rgb565(udp.data() + (h + y) * mw + w, tile.data() + y * w * 3, w);
        std::ofstream output(destination, std::ios::binary);
        output << "P6\n" << w << " " << h << "\n255\n";
        if (!output.write(reinterpret_cast<const char *>(tile.data()), tile.size())) return 9;

        std::cout << "FIVE " << destination << ' ' << unsigned(curve.type) << ' '
                  << unsigned(curve.confidence) << ' ' << unsigned(curve.angle) << ' '
                  << curve.curvature;
        for (size_t i = 0; i < points.left_x.size(); ++i)
            std::cout << ' ' << std::lround((points.left_x[i] + points.right_x[i]) * .5)
                      << ',' << std::lround(points.first + (points.last - points.first) * .25 * i);
        std::cout << std::endl;
    }
    yolo_engine_cleanup(&engine);
    return 0;
}
