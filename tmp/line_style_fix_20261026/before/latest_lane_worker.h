#ifndef M_LATEST_LANE_WORKER_H
#define M_LATEST_LANE_WORKER_H

#include "yolo_integration.h"
#include "line_style_analyzer.h"
#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>
#include <time.h>

namespace m_lane {

inline uint64_t clock_ns() {
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) return 0;
    return (uint64_t)now.tv_sec * 1000000000ULL + now.tv_nsec;
}

constexpr uint64_t MAX_RESULT_AGE_NS = 500000000ULL;

struct Snapshot {
    uint64_t source_id = 0, captured_ns = 0, completed_ns = 0;
    bool valid = false;
    int width = 0, height = 0;
    YOLOCurveResult curve = {};
    YOLOLineStyleResult line_style = {};
    std::vector<float> lane, drivable;

    bool fresh(uint64_t now) const {
        return valid && captured_ns && now >= captured_ns &&
               now - captured_ns <= MAX_RESULT_AGE_NS;
    }
    yolopv2_result_t masks() const {
        // The legacy C view uses mutable pointers, but the pure overlay API
        // only reads them. The snapshot itself is published as const.
        yolopv2_result_t view;
        view.lane_line_data = const_cast<float*>(lane.data());
        view.drivable_area_data = const_cast<float*>(drivable.data());
        view.width = width;
        view.height = height;
        return view;
    }
};

struct Totals {
    uint64_t calls, successful, fresh_results, stale_results, replaced;
    uint64_t inference_ns, analysis_ns;
};

/* One processing frame plus one replaceable pending frame. Neither capture
 * nor X11 waits for inference. RKNN context/buffers are exclusively owned by
 * this worker until stop() joins it; the caller may then clean up RKNN. */
class LatestWorker {
public:
    LatestWorker(YOLOEngine* engine, int width, int height)
        : engine_(engine), width_(width), height_(height),
          pending_pixels_((size_t)width * height * 3),
          working_pixels_((size_t)width * height * 3) {
        thread_ = std::thread(&LatestWorker::run, this);
    }
    ~LatestWorker() { stop(); }
    LatestWorker(const LatestWorker&) = delete;
    LatestWorker& operator=(const LatestWorker&) = delete;

    void submit(const uint8_t* image, uint64_t id, uint64_t captured_ns) {
        if (!image || failed_.load()) return;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stopping_) return;
            if (pending_) ++replaced_;
            memcpy(pending_pixels_.data(), image, pending_pixels_.size());
            pending_id_ = id;
            pending_capture_ns_ = captured_ns;
            pending_ = true;
        }
        ready_.notify_one();
    }

    std::shared_ptr<const Snapshot> latest() const {
        return std::atomic_load(&latest_);
    }
    bool failed() const { return failed_.load(); }
    Totals totals() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return {calls_.load(), successful_.load(), fresh_.load(), stale_.load(),
                replaced_.load(), inference_ns_.load(), analysis_ns_.load()};
    }
    void stop() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopping_ = true;
            pending_ = false;
        }
        ready_.notify_one();
        if (thread_.joinable()) thread_.join();
    }

private:
    void run() {
        try {
            for (;;) {
                uint64_t source, captured;
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    ready_.wait(lock, [this] { return stopping_ || pending_; });
                    if (stopping_) return;
                    pending_pixels_.swap(working_pixels_);
                    source = pending_id_;
                    captured = pending_capture_ns_;
                    pending_ = false;
                }
                std::shared_ptr<Snapshot> result = std::make_shared<Snapshot>();
                result->source_id = source;
                result->captured_ns = captured;
                yolopv2_result_t raw = {};
                uint64_t start = clock_ns();
                int status = yolo_engine_detect(engine_, working_pixels_.data(), width_, height_, &raw);
                uint64_t inference_time = clock_ns() - start;
                uint64_t analysis_time = 0;
                if (status == 0 && (!engine_ || !raw.lane_line_data || !raw.drivable_area_data ||
                    raw.width <= 0 || raw.height <= 0 || raw.width != engine_->model_width ||
                    raw.height != engine_->model_height)) status = -1;
                if (status == 0) {
                    uint64_t now = clock_ns();
                    if (captured && now >= captured && now - captured <= MAX_RESULT_AGE_NS) {
                        start = now;
                        YOLOLineStyleResult observed = line_analyzer_.analyze_front_camera(raw.lane_line_data,
                            raw.width, raw.height, working_pixels_.data(), width_, height_);
                        result->line_style = line_analyzer_.confirm(observed,source,captured);
                        if(!line_log_ns_ || now-line_log_ns_>=2000000000ULL) {
                            float lo=raw.lane_line_data[0],hi=lo;
                            for(int k=0;k<raw.width*raw.height;++k){lo=std::min(lo,raw.lane_line_data[k]);hi=std::max(hi,raw.lane_line_data[k]);}
                            printf("[LINE_STYLE] source=%llu camera=1 left=%s q=%u duty=%u spans=%u right=%s q=%u duty=%u spans=%u mask_range=%.3f..%.3f\n",
                                (unsigned long long)source,line_style_name(result->line_style.left.type),result->line_style.left.quality,observed.left.duty,observed.left.spans,
                                line_style_name(result->line_style.right.type),result->line_style.right.quality,observed.right.duty,observed.right.spans,lo,hi);
                            line_log_ns_=now;
                        }
                        // Optional bounded diagnostic pairs; no normal-runtime disk I/O.
                        const char* dump=getenv("LANE_STYLE_DUMP_DIR");
                        if(dump && *dump && dump_count_<6 && (!dump_ns_||now-dump_ns_>=1000000000ULL)) {
                            char path[512];snprintf(path,sizeof(path),"%s/source_%llu.ppm",dump,(unsigned long long)source);
                            FILE* fp=fopen(path,"wb");if(fp){fprintf(fp,"P6\n%d %d\n255\n",width_,height_);fwrite(working_pixels_.data(),1,working_pixels_.size(),fp);fclose(fp);}
                            snprintf(path,sizeof(path),"%s/source_%llu_mask.pgm",dump,(unsigned long long)source);
                            fp=fopen(path,"wb");if(fp){fprintf(fp,"P5\n%d %d\n255\n",raw.width,raw.height);for(int k=0;k<raw.width*raw.height;++k)fputc(raw.lane_line_data[k]>.3f?255:0,fp);fclose(fp);}
                            ++dump_count_;dump_ns_=now;
                        }
                        int pixels = yolo_overlay_lane_and_drivable(working_pixels_.data(), width_, height_, &raw);
                        if (yolo_analyze_annotated_curve(working_pixels_.data(), width_, height_,
                                                        pixels, &result->curve) == 0) {
                            result->width = raw.width;
                            result->height = raw.height;
                            size_t count = (size_t)raw.width * raw.height;
                            result->lane.assign(raw.lane_line_data, raw.lane_line_data + count);
                            result->drivable.assign(raw.drivable_area_data, raw.drivable_area_data + count * 2);
                            result->valid = true;
                        }
                        analysis_time = clock_ns() - start;
                    }
                    result->completed_ns = clock_ns();
                    if (!result->fresh(result->completed_ns)) result->valid = false;
                } else result->completed_ns = clock_ns();
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    if (stopping_) return;
                    ++calls_;
                    inference_ns_ += inference_time;
                    analysis_ns_ += analysis_time;
                    if (status == 0) {
                        ++successful_;
                        if (result->valid) ++fresh_;
                        else ++stale_;
                    }
                }
                // Both mask vectors own copies, never engine's next-frame buffers.
                std::shared_ptr<const Snapshot> immutable = result;
                std::atomic_store(&latest_, immutable);
            }
        } catch (...) {
            // The capture thread will invalidate curve confidence and keep
            // publishing raw video; no thread frees an in-use RKNN context.
            failed_ = true;
        }
    }

    YOLOEngine* engine_;
    LineStyleAnalyzer line_analyzer_;
    uint64_t line_log_ns_=0,dump_ns_=0;
    unsigned dump_count_=0;
    int width_, height_;
    mutable std::mutex mutex_;
    std::condition_variable ready_;
    std::thread thread_;
    bool stopping_ = false, pending_ = false;
    uint64_t pending_id_ = 0, pending_capture_ns_ = 0;
    std::vector<uint8_t> pending_pixels_, working_pixels_;
    std::shared_ptr<const Snapshot> latest_;
    std::atomic<bool> failed_{false};
    std::atomic<uint64_t> calls_{0}, successful_{0}, fresh_{0}, stale_{0}, replaced_{0};
    std::atomic<uint64_t> inference_ns_{0}, analysis_ns_{0};
};
}
#endif
