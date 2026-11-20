#include "dashboard_ui.h"
#include "../../../include/wheel_speed.h"
#include <cassert>
#include <fstream>

static unsigned warning_pixels(const uint32_t* frame) {
    unsigned count = 0;
    for (int y = 69; y < 69 + perception_ui::SPEED_WARNING_HEIGHT; ++y)
        for (int x = 889; x < 889 + perception_ui::SPEED_WARNING_WIDTH; ++x)
            count += frame[y * perception_ui::WIDTH + x] == 0xFFFF526D;
    return count;
}

int main(int argc, char** argv) {
    using namespace perception_ui;
    Dashboard dashboard;
    ViewStatus status;
    status.gear_known = status.gear_live = true;
    const auto render = [&]() {
        return dashboard.compose(nullptr, 0, nullptr, 0, true, -1, "OVERVIEW", status);
    };
    status.wheel_known = status.wheel_metric = true;
    for (unsigned gear = 1; gear <= 3; ++gear) {
        status.gear = gear;
        status.wheel_value = 99;
        assert(warning_pixels(render()) == 0);
        status.wheel_value = 100;
        assert(warning_pixels(render()) > 300);
        status.wheel_value = 101;
        assert(warning_pixels(render()) > 300);
    }
    status.gear_known = status.gear_live = false;
    assert(warning_pixels(render()) > 300);
    if (argc == 2) {
        std::ofstream file(argv[1], std::ios::binary);
        file << "P6\n" << WIDTH << " " << HEIGHT << "\n255\n";
        for (int i = 0; i < WIDTH * HEIGHT; ++i) {
            const uint32_t p = dashboard.pixels()[i];
            const char rgb[] = {char(p >> 16), char(p >> 8), char(p)};
            file.write(rgb, 3);
        }
    }
    status.wheel_known = false;
    assert(warning_pixels(render()) == 0);
    status.wheel_known = true;
    status.wheel_metric = false;
    assert(warning_pixels(render()) == 0); // RPM is not a calibrated km/h reading.
    status.wheel_metric = true;
    assert(warning_pixels(render()) > 300);
    status.wheel_value = wheel_speed_tenths(691, 691, WHEEL_DEFAULT_PULSES_PER_TURN);
    assert(status.wheel_value == 360);
    const auto *high_speed=render();
    assert(warning_pixels(high_speed) > 300);
    unsigned digits=0;
    for (int y=56;y<96;++y)
        for (int x=988;x<1092;++x)
            digits+=high_speed[y*WIDTH+x]==0xFFC1F7FC;
    assert(digits>0);
    status.wheel_value = 99; // Clear below the limit, including fast composition.
    assert(warning_pixels(render()) == 0);
    std::puts("SPEED WARNING: 9.9/10.0 km/h boundary, gear independence, unknown and RPM PASS");
}
