#ifndef PERCEPTION_VIDEO_INPUT_FPS_H
#define PERCEPTION_VIDEO_INPUT_FPS_H

#include <cstdint>

namespace perception_ui
{

// 统计源端发布次数，不统计 GUI 重绘或 OV5640 硬件 VSYNC。
// 有意只使用简单存储对象，因为 RenderEngine 由 memset 整体清零。
struct FrameIdFps
{
    uint64_t window_start_ns;
    uint64_t pending_frames;
    uint32_t previous_id;
    float frames_per_second;
    bool initialized;

    void reset()
    {
        window_start_ns = pending_frames = 0;
        previous_id = 0;
        frames_per_second = 0;
        initialized = false;
    }

    // 每轮输入轮询均调用，且只使用上一次已通过校验的
    // 帧号；相同 ID 记零，跳增 N 就记 N 次发布。
    // 显示帧率需要更新时返回 true。
    bool update(uint32_t frame_id, uint64_t now_ns, bool source_live)
    {
        if (!source_live)
        {
            const bool changed = frames_per_second != 0;
            reset();
            return changed;
        }
        const uint32_t delta = frame_id - previous_id;
        if (!initialized || now_ns < window_start_ns || delta >= 0x80000000u)
        {
            const bool changed = frames_per_second != 0;
            reset();
            initialized = true;
            previous_id = frame_id;
            window_start_ns = now_ns;
            return changed;
        }
        pending_frames += delta; // 使用 uint32 减法，同时兼容正常计数回绕。
        previous_id = frame_id;
        const uint64_t elapsed_ns = now_ns - window_start_ns;
        if (elapsed_ns < 1000000000ULL)
            return false;
        frames_per_second = static_cast<float>(static_cast<double>(pending_frames) * 1e9 /
                                               static_cast<double>(elapsed_ns));
        pending_frames = 0;
        window_start_ns = now_ns;
        return true;
    }
};

} // perception_ui：本节对应的标识或作用域。
#endif
