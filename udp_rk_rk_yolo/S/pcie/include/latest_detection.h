#ifndef S_LATEST_DETECTION_H
#define S_LATEST_DETECTION_H

#include "postprocess.h"
#include <chrono>
#include <condition_variable>
#include <cstring>
#include <cstdlib>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace s_vision
{
inline unsigned configured_submit_interval(const char*text,unsigned fallback=3)
{
    if(!text||!*text)return fallback;
    char*end=nullptr;unsigned long value=std::strtoul(text,&end,10);
    return end&&!*end&&value>=1&&value<=5?(unsigned)value:fallback;
}
struct DetectionSnapshot
{
    detect_result_group_t results = {};
    uint64_t captured = 0, elapsed = 0;
    bool valid = false;

    bool fresh(uint64_t now) const
    {
        return valid && captured && now >= captured && now - captured <= 350000000ULL;
    }
};

// One inference owner. New input replaces the waiting image, never the image
// being inferred. The capture loop can keep publishing video independently.
class LatestDetection
{
    using Infer = std::function<int(const uint8_t *, detect_result_group_t *)>;
    Infer infer_;
    std::vector<uint8_t> pending_, processing_;
    mutable std::mutex mutex_;
    std::condition_variable wake_;
    DetectionSnapshot latest_;
    uint64_t submitted_ = 0;
    bool waiting_ = false, stopping_ = false;
    std::thread worker_;

    void run()
    {
        for (;;)
        {
            DetectionSnapshot next;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                wake_.wait(lock, [&] { return waiting_ || stopping_; });
                if (stopping_)
                    return;
                pending_.swap(processing_);
                next.captured = submitted_;
                waiting_ = false;
            }
            const auto started = std::chrono::steady_clock::now();
            try
            {
                next.valid = infer_(processing_.data(), &next.results) == 0;
            }
            catch (...)
            {
                next.valid = false;
            }
            next.elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now() - started).count();
            std::lock_guard<std::mutex> lock(mutex_);
            if (stopping_)
                return;
            latest_ = next;
        }
    }

  public:
    LatestDetection(size_t image_bytes, const Infer &infer)
        : infer_(infer), pending_(image_bytes), processing_(image_bytes),
          worker_(&LatestDetection::run, this)
    {
    }
    ~LatestDetection() { stop(); }

    void submit(const uint8_t *pixels, uint64_t captured)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stopping_ || captured <= submitted_)
                return;
            std::memcpy(pending_.data(), pixels, pending_.size());
            submitted_ = captured;
            waiting_ = true;
        }
        wake_.notify_one();
    }

    DetectionSnapshot latest() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return latest_;
    }

    void stop()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopping_ = true;
            latest_ = {};
        }
        wake_.notify_one();
        if (worker_.joinable())
            worker_.join();
    }
};
}
#endif
