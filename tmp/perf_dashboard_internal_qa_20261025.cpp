#include <cassert>
#include <iostream>
#include <limits>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define perception_ui baseline_ui
#include "perf_dashboard_before_internal_20261025.h"
#undef perception_ui
#undef PERCEPTION_DASHBOARD_UI_H
#include "perf_dashboard_internal_candidate_20261025.h"

int main() {
    int fw, fh, fc;
    uint8_t* font = stbi_load("../dashboard_font.png", &fw, &fh, &fc, 1);
    assert(font);
    std::vector<uint8_t> worst(512 * 612);
    for (size_t i = 0; i < worst.size(); ++i) worst[i] = (i * 73 + 29) % 256;
    std::vector<uint32_t> source(1280 * 480), car(1280 * 480);
    for (size_t i = 0; i < source.size(); ++i) {
        source[i] = i * 0x112233u;
        car[i] = i * 0x123457u;
    }
    int total = 0;
    for (int f = 0; f < 4; ++f) {
        if (f >= 2) {
            int ys[] = {0, 120, 288}, hs[] = {20, 28, 54};
            for (int j = 0; j < 3; ++j) {
                uint8_t* m = worst.data() + (ys[j] + 5 * hs[j]) * 512 + 15 * 32;
                memcpy(m, "PEUI", 4); m[4] = 1; m[5] = 0; m[6] = 8;
                m[7] = f == 2 ? hs[j] - 1 : 0; m[8] = 1;
            }
        }
        const uint8_t* atlas = f == 0 ? nullptr : f == 1 ? font : worst.data();
        baseline_ui::Dashboard old(atlas, 512, 612);
        perception_ui::Dashboard now(atlas, 512, 612);
        baseline_ui::ViewStatus bs;
        perception_ui::ViewStatus ns;
        for (int n = 0; n < 300; ++n) {
            bool overview = n % 3 != 0;
            float t = n % 9 == 0 ? -1.f : ((n % 9) - 1) / 7.f;
            if (n % 37 == 0) t = std::numeric_limits<float>::quiet_NaN();
            bs.person = ns.person = n % 2; bs.pcie_live = ns.pcie_live = n % 3;
            bs.udp_live = ns.udp_live = n % 4; bs.recorded = ns.recorded = n % 5 == 0;
            bs.fps = ns.fps = n % 3 ? 21.8f : 0;
            bs.pcie_fps = ns.pcie_fps = n % 17 == 0 ? -1e30f : 19.3f;
            bs.udp_fps = ns.udp_fps = n % 19 == 0 ? std::numeric_limits<float>::quiet_NaN() : 30.f;
            char label[64]; memset(label, 'W', sizeof(label)); label[n % 44] = 0;
            if (n % 23 == 0) label[0] = (char)0xff;
            const uint32_t* video = n % 7 == 0 ? nullptr : source.data();
            const uint32_t* vehicle = n % 5 == 0 ? nullptr : car.data();
            int vs = n % 11 == 0 ? 1279 : 1280, cs = n % 13 == 0 ? 319 : 1280;
            const uint32_t* a = old.compose(video, vs, vehicle, cs, overview, t, n % 29 ? label : nullptr, bs);
            const uint32_t* b = now.compose(video, vs, vehicle, cs, overview, t, n % 29 ? label : nullptr, ns);
            if (memcmp(a, b, 1600 * 900 * 4)) {
                size_t k = 0; while (a[k] == b[k]) ++k;
                std::cerr << "MISMATCH font=" << f << " n=" << n << " x=" << k % 1600 << " y=" << k / 1600 << '\n';
                return 2;
            }
            ++total;
        }
    }
    stbi_image_free(font);
    std::cout << "DASHBOARD_PIXEL_EQUAL cases=" << total << " fast=" << getenv("VISION_DASHBOARD_FAST") << '\n';
}
