#ifndef PERCEPTION_VIDEO_INPUT_FPS_H
#define PERCEPTION_VIDEO_INPUT_FPS_H

#include <cstdint>

namespace perception_ui {

// Measures source publications, not GUI redraws or OV5640 hardware VSYNC.
// Trivial storage is intentional: RenderEngine is zero-initialized with memset.
struct FrameIdFps {
    uint64_t window_start_ns;
    uint64_t pending_frames;
    uint32_t previous_id;
    float frames_per_second;
    bool initialized;

    void reset() {
        window_start_ns = pending_frames = 0;
        previous_id = 0;
        frames_per_second = 0;
        initialized = false;
    }

    // Call on every input-poll iteration, using only the last checksum-verified
    // frame ID. Repeated IDs count zero; a jump of N counts N publications.
    // Returns true when the displayed rate needs updating.
    bool update(uint32_t frame_id, uint64_t now_ns, bool source_live) {
        if (!source_live) {
            const bool changed = frames_per_second != 0;
            reset();
            return changed;
        }
        const uint32_t delta = frame_id - previous_id;
        if (!initialized || now_ns < window_start_ns || delta >= 0x80000000u) {
            const bool changed = frames_per_second != 0;
            reset();
            initialized = true;
            previous_id = frame_id;
            window_start_ns = now_ns;
            return changed;
        }
        pending_frames += delta; // uint32 subtraction also handles normal wrap.
        previous_id = frame_id;
        const uint64_t elapsed_ns = now_ns - window_start_ns;
        if (elapsed_ns < 1000000000ULL) return false;
        frames_per_second = static_cast<float>(
            static_cast<double>(pending_frames) * 1e9 / static_cast<double>(elapsed_ns));
        pending_frames = 0;
        window_start_ns = now_ns;
        return true;
    }
};

} // namespace perception_ui
#endif
