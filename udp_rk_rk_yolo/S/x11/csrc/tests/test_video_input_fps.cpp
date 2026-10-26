#include "video_input_fps.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <type_traits>

static void near(float actual, float expected) {
    assert(std::fabs(actual - expected) < .001f);
}

int main() {
    using perception_ui::FrameIdFps;
    static_assert(std::is_trivial<FrameIdFps>::value, "RenderEngine uses memset");
    FrameIdFps s = {}, m = {};
    // GUI samples twelve times while producers publish 18 and 30 frames/s.
    // Existing high IDs at attachment are a baseline, not new frames.
    s.update(100000, 0, true); m.update(900000, 0, true);
    for (uint64_t tick = 1; tick <= 12; ++tick) {
        const uint64_t now = tick * 1000000000ULL / 12;
        s.update(100000 + uint32_t(tick * 18 / 12), now, true);
        m.update(900000 + uint32_t(tick * 30 / 12), now, true);
    }
    near(s.frames_per_second, 18); near(m.frames_per_second, 30);
    // Repeated frames and missing/stopped sources must not retain a stale FPS.
    m.update(900030, 2000000000ULL, true); near(m.frames_per_second, 0);
    s.update(100018, 2000000000ULL, false); near(s.frames_per_second, 0);
    assert(!s.initialized);
    // A reconnect starts from its current ID, even if the old process ran long.
    s.update(8000000, 3000000000ULL, true); near(s.frames_per_second, 0);
    s.update(8000016, 4000000000ULL, true); near(s.frames_per_second, 16);
    // An ID rollback is a reset; it cannot become a multi-billion FPS spike.
    s.update(1, 4100000000ULL, true); near(s.frames_per_second, 0);
    s.update(21, 5100000000ULL, true); near(s.frames_per_second, 20);
    // Normal 32-bit rollover retains all new frames.
    s.reset(); s.update(0xfffffff0u, 0, true);
    s.update(14, 1000000000ULL, true); near(s.frames_per_second, 30);
    // Divide by actual monotonic duration, not an assumed one-second tick.
    s.update(44, 2500000000ULL, true); near(s.frames_per_second, 20);
    s.update(45, 100, true); near(s.frames_per_second, 0); // clock reset
    s.update(70, 1000000100ULL, true); near(s.frames_per_second, 25);
    FrameIdFps zeroed;
    std::memset(&zeroed, 0, sizeof(zeroed));
    zeroed.update(50000, 10, true); near(zeroed.frames_per_second, 0);
    zeroed.update(50015, 1000000010ULL, true); near(zeroed.frames_per_second, 15);
    puts("PASS input FPS: renderer-independent deltas, skipped IDs, idle, reconnect, reset, wrap, monotonic duration");
}
