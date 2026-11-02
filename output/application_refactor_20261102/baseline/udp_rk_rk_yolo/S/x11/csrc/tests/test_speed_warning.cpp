#include "dashboard_ui.h"
#include <cassert>
#include <fstream>

static unsigned warning_pixels(const uint32_t* frame) {
    unsigned count = 0;
    for (int y = 61; y < 89; ++y)
        for (int x = 1008; x < 1092; ++x)
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
    status.gear = 1;
    assert(warning_pixels(render()) == 0);
    status.gear = 2;
    assert(warning_pixels(render()) > 300);
    status.gear = 3;
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
    status.gear_live = false;
    assert(warning_pixels(render()) > 300);
    status.gear = 1;
    assert(warning_pixels(render()) == 0); // Clear after 3 -> 1, including fast composition.
    status.gear = 2;
    status.gear_known = false;
    assert(warning_pixels(render()) == 0);
    std::puts("SPEED WARNING: gear 1/2/3, stale high, unknown and warning removal PASS");
}
