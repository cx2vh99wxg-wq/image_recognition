#include "paired_replay.hpp"
#include "latest_lane_worker.h"
#include "traffic_service.h"
#include "hdmi_front_capture.h"
#include <iostream>
#include <vector>

extern "C" int curve_detection_update(uint32_t, CurveType, uint32_t, uint32_t) { return -1; }

int main(int argc, char **argv)
{
    if (argc < 6 || !private_ipc()) return 2;
    const std::string output = argv[2];
    const std::string clip = argv[3];
    uint32_t source = std::stoul(argv[4]);
    const unsigned first_id = source;
    std::vector<unsigned> targets;
    for (int i = 5; i < argc; ++i) targets.push_back(std::stoul(argv[i]));
    std::ifstream yuv_input(argv[1], std::ios::binary);
    if (!yuv_input) return 3;
    constexpr unsigned w = 640, h = 480, front_w = 320, front_h = 240;
    const auto calibration = m_lane::GroundProjection::for_camera(nullptr, "front_hdmi_ipm.conf");
    const auto ground = m_lane::GroundProjection::for_video(calibration, 0, 30, front_w, 180);
    const bool actors = std::getenv("POSTER_ACTORS") != nullptr;
    m_lane::FrontRadialPreview front(w, h, m_lane::FrontRadial(), nullptr, true);
    m_lane::FrontRadialPreview bird(w, h, m_lane::FrontRadial(), &ground);
    YOLOEngine engine = {};
    if (!bird.enabled() || yolo_engine_init(&engine, "model/yolopv2_front_320x240_mixed_error_guided_rk3568.rknn", nullptr)) return 4;
    {
        traffic::Service service("model/traffic_yolov5n_rk3568.rknn",
            traffic::InputView(0, 0, traffic::PixelOrder::BGR), nullptr, &ground, first_id + 1);
        m_lane::LatestWorker worker(&engine, w, h, actors ? nullptr : &service, &bird, &front, true);
        TrafficLightIPC signal = {-1, nullptr};
        FrontPersonIPC person = {-1, nullptr};
        display_people::IPC people;
        display_vehicles::IPC vehicles;
        crosswalk_display::IPC crosswalk;
        hdmi_front::SignalDetail detail;
        std::vector<uint8_t> yuv(w*h*2), tile(front_w*front_h*3), mosaic(w*h*3), painted(w*h*3);
        std::vector<uint16_t> ipm(w*h), raw(front_w*front_h), ipm_tile(front_w*front_h);
        std::vector<float> tile_lane(front_w*front_h), tile_road(2*front_w*front_h);
        unsigned frame = 0;
        while (yuv_input.read(reinterpret_cast<char *>(yuv.data()), yuv.size()))
        {
            if (!hdmi_front::convert_yuyv(yuv.data(), yuv.size(), w, h, w*2, false, false, tile.data())) return 5;
            hdmi_front::apply_bgr(tile.data(), mosaic.data(), w);
            detail.capture(yuv.data(), w, h, w*2, false, false);
            const auto submit = [&](bool infer_lane) {
                const auto now = m_lane::clock_ns();
                ++source;
                service.submit_raw(mosaic.data(), w, h, source, now, &detail);
                if (infer_lane) worker.submit(mosaic.data(), source, now);
                else {
                    traffic::LaneBudget budget;
                    budget.source=source;budget.captured=budget.completed=now;budget.fresh=true;
                    budget.front.pixels=tile.data();budget.front.lane_mask=tile_lane.data();
                    budget.front.road=tile_road.data();budget.front.width=front_w;budget.front.height=front_h;
                    budget.front.source=source;budget.front.captured=now;
                    service.after_lane(budget);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(83));
            };
            const bool selected = std::binary_search(targets.begin(), targets.end(), frame);
            const uint32_t first = source + 1;
            PosterFrame record;
            std::shared_ptr<const m_lane::Snapshot> result;
            // Hold the exact source image until both independent model streams observe it.
            // Save only observations whose source IDs belong to that image.
            for (int tick = 0; tick < (selected ? 20 : 1); ++tick)
            {
                // For actor examples, let lane inference finish before running the
                // same traffic service on repeated copies of this identical image.
                if (actors && result) detail.capture(yuv.data(), w, h, w*2, false, false);
                submit(!actors || !result);
                if (!selected) continue;
                const auto now = m_lane::clock_ns();
                const auto valid = [&](uint32_t id) { return id >= first && id <= source; };
                auto snapshot = worker.latest();
                if (snapshot && snapshot->fresh(now) && valid(snapshot->source_id) && snapshot != result) {
                    result = snapshot;
                    if (actors) {
                        const auto masks = result->masks();
                        for (unsigned y=0; y<front_h; ++y) {
                            std::copy_n(masks.lane_line_data+y*w,front_w,tile_lane.data()+y*front_w);
                            std::copy_n(masks.drivable_area_data+y*w,front_w,tile_road.data()+y*front_w);
                            std::copy_n(masks.drivable_area_data+w*h+y*w,front_w,tile_road.data()+(y+front_h)*front_w);
                        }
                    }
                }
                TrafficLightData s = {};
                if (!traffic_ipc_open(&signal, 0) && !traffic_ipc_read(&signal, &s) &&
                    traffic_fresh(&s, now) && valid(s.source_id)) record.signal = s;
                FrontPersonData p = {};
                if (!front_person_ipc_open(&person, 0) && !front_person_ipc_read(&person, &p) &&
                    front_person_fresh(&p, now) && valid(p.source_id)) record.person = p;
                display_people::Snapshot pp = {};
                if (people.open(display_people::FRONT_KEY, false) && people.read(pp) &&
                    display_people::fresh(pp, now) && valid(pp.source)) record.people = pp;
                display_vehicles::Snapshot vv = {};
                if (vehicles.open(display_vehicles::KEY, false) && vehicles.read(vv) &&
                    display_vehicles::fresh(vv, now) && valid(vv.source)) record.vehicles = vv;
                crosswalk_display::Snapshot cc = {};
                if (crosswalk.open(false) && crosswalk.read(cc) &&
                    crosswalk_display::fresh(cc, now) && valid(cc.source)) record.crosswalk = cc;
            }
            if (selected && result)
            {
                record.id = first;
                record.curve_id = result->source_id;
                record.curve = result->curve.curve_type;
                record.angle = result->curve.angle;
                record.confidence = result->curve.confidence;
                record.points = result->ipm_points.sampled ? 5 : 0;
                record.lane = line_result_data(&result->line_style, first_id + 1, result->source_id, result->captured_ns);
                if (!line_fresh(&record.lane, m_lane::clock_ns())) record.lane = {};
                if (record.signal.source_id && record.signal.detector_id < first && record.signal.state &&
                    (record.signal.flags & TRAFFIC_TRACKED) == 0) record.signal = {};
                painted = mosaic;
                auto masks = result->masks();
                if (result->restore_map)
                    yolo_overlay_lane_and_drivable_roi(painted.data(), w, h, &masks, front_w, front_h);
                else
                    yolo_overlay_lane_and_drivable(painted.data(), w, h, &masks);
                bird.apply(painted.data(), ipm.data(), &result->ipm_pair, &result->ipm_points);
                frame_pack_rgb565(tile.data(), raw.data(), raw.size());
                for (unsigned y=0; y<front_h; ++y)
                    std::copy_n(ipm.data() + (y+front_h)*w+front_w, front_w, ipm_tile.data()+y*front_w);
                const auto base = output + '/' + clip + '_' + std::to_string(frame);
                if (!write_file(base+".meta", &record, 1) ||
                    !write_file(base+".raw", raw.data(), raw.size()) ||
                    !write_file(base+".ipm", ipm_tile.data(), ipm_tile.size())) return 6;
                std::cout << "SHOT " << clip << '_' << frame << ' ' << frame << ' '
                          << record.curve << ' ' << record.angle << ' ' << record.confidence << ' '
                          << record.points << ' ' << record.lane.left << ' ' << record.lane.right << ' '
                          << record.signal.state << ' ' << record.signal.raw_state << ' ' << record.signal.color_quality << ' '
                          << record.signal.x << ',' << record.signal.y << ',' << record.signal.width << ',' << record.signal.height << ' '
                          << record.people.count << ' ' << record.vehicles.count << ' ' << record.crosswalk.count;
                for (unsigned i=0;i<record.people.count;++i) {
                    const auto &p = record.people.people[i];
                    std::cout << " P" << p.x << ',' << p.y << ',' << p.w << ',' << p.h << ',' << p.quality << ',' << p.flags;
                }
                for (unsigned i=0;i<record.vehicles.count;++i) {
                    const auto &v = record.vehicles.vehicles[i];
                    const auto &p = v.footprint;
                    std::cout << " V" << v.kind << ',' << p.x << ',' << p.y << ',' << p.w << ',' << p.h << ',' << p.quality << ',' << p.flags;
                }
                std::cout << std::endl;
            }
            if (worker.failed()) return 7;
            if (++frame > targets.back()) break;
        }
        worker.stop();
        traffic_ipc_close(&signal);
        front_person_ipc_close(&person);
    }
    yolo_engine_cleanup(&engine);
}
