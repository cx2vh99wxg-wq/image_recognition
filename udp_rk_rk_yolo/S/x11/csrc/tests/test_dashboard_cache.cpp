#include "dashboard_ui.h"
#include <cstdio>
#include <cstdint>
#include <vector>
int main()
{
    perception_ui::Dashboard dash;
    perception_ui::ViewStatus s;
    std::vector<uint32_t> video(1280 * 480, 0xff386297), car(320 * 480, 0xff102030);
    for (int i = 0; i < 20; ++i)
    {
        s.lane_angle_valid = i % 3; s.lane_angle_deg = i - 10;
        s.line_left = i % 3; s.line_right = (i + 1) % 3;
        s.line_left_quality = i * 4; s.line_right_quality = 90 - i;
        s.gear_known = i % 4; s.gear_live = i % 3; s.gear = 1 + i % 3;
        s.person = i % 2; s.go_blocked = i % 3 == 0;
        s.traffic = i % 4; s.traffic_live = i % 3;
        s.pcie_live = i % 3; s.udp_live = i % 4;
        s.pcie_fps = 30 - i * .5; s.udp_fps = 28 + i * .1;
        s.lane_blocked = i % 4;
        const bool overview = i % 4 < 2;
        const auto *p = dash.compose(i == 10 ? nullptr : video.data(), 1280,
                                    car.data(), 320, overview, i % 4 == 0 ? .4 : -1,
                                    i % 2 ? "CAMERA FRONT" : "OVERVIEW", s);
        uint64_t hash = 1469598103934665603ULL;
        for (int j = 0; j < 1600 * 900; ++j) { hash ^= p[j]; hash *= 1099511628211ULL; }
        std::printf("FRAME %d %016llx\n", i, (unsigned long long)hash);
    }
}
