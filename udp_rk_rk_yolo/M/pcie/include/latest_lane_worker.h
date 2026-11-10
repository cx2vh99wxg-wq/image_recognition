#ifndef M_LATEST_LANE_WORKER_H
#define M_LATEST_LANE_WORKER_H

#include "yolo_integration.h"
#include "line_style_analyzer.h"
#include "lane_marking_profile.h"
#include "line_style_snapshot.h"
#include "front_radial_preview.h"
#include "lane_curve_geometry.h"
#include "ground_projection.h"
#include "../../../common/perception/include/curve_heading.h"
#include "traffic_vision.h"
#include "lane_cpu_stage.h"
#include "crosswalk_detector.h"
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

namespace m_lane
{

inline uint64_t clock_ns()
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
        return 0;
    return (uint64_t)now.tv_sec * 1000000000ULL + now.tv_nsec;
}

constexpr uint64_t MAX_RESULT_AGE_NS = 500000000ULL;

struct Snapshot
{
    uint64_t source_id = 0, captured_ns = 0, completed_ns = 0;
    bool valid = false;
    int width = 0, height = 0;
    YOLOCurveResult curve = {};
    YOLOLineStyleResult line_style = {};
    YOLOLineStyleResult ipm_pair = {};
    PointLanePair ipm_points;
    std::vector<float> lane, drivable;
    std::shared_ptr<const FrontMaskRestore> restore_map;
    mutable std::once_flag restored;
    mutable std::vector<float> restored_lane, restored_road;

    bool fresh(uint64_t now) const
    {
        return valid && captured_ns && now >= captured_ns && now - captured_ns <= MAX_RESULT_AGE_NS;
    }
    yolopv2_result_t masks() const
    {
        // 旧 C 视图虽然暴露可写指针，纯叠加接口实际上
        // 只读取数据；发布出去的快照本身仍为 const。
        yolopv2_result_t view;
        if(restore_map){
            std::call_once(restored,[&]{
                const size_t n=(size_t)width*height;
                restored_lane.assign(n,0.f);restored_road.assign(2*n,0.f);
                restore_map->restore(lane.data(),drivable.data(),restored_lane.data(),restored_road.data());
            });
        }
        view.lane_line_data = const_cast<float *>(restore_map?restored_lane.data():lane.data());
        view.drivable_area_data = const_cast<float *>(restore_map?restored_road.data():drivable.data());
        view.width = width;
        view.height = height;
        return view;
    }
};

struct Totals
{
    uint64_t calls, successful, fresh_results, stale_results, replaced;
    uint64_t inference_ns, analysis_ns;
    uint64_t front_ns;
    YOLOTiming timing;
};

/* 工作线程独占 RKNN 上下文和缓冲区，
 * 使用一个处理帧加一个可覆盖的待处理帧；采集及 X11 不等待推理。
 * stop() 完成 join 后，调用者方可清理 RKNN。 */
class LatestWorker
{
  public:
    LatestWorker(YOLOEngine *engine, int width, int height,
                 traffic::NpuTask *optional_task = nullptr,
                 const FrontRadialPreview *ipm_test = nullptr,
                 const FrontRadialPreview *front_input = nullptr)
        : engine_(engine), optional_task_(optional_task), width_(width), height_(height),
          pending_pixels_((size_t)width * height * 3), working_pixels_((size_t)width * height * 3),
          ipm_test_(ipm_test), front_input_(front_input)
    {
        const char *curve_mode = std::getenv("LANE_CURVE_MODE");
        ego_curve_ = !curve_mode || std::strcmp(curve_mode, "ego-boundaries") == 0;
        std::fprintf(stderr, "[CURVE_MODE] %s\n",
                     ego_curve_ ? "ego-boundaries candidate" : "legacy projection");
        std::fprintf(stderr, "[LANE_CANDIDATES] %s\n",
                     use_point_lanes() ? "point-tracks" : line_analyzer_.bottom_up() ? "bottom-up" : "seed-fit");
        if (use_ipm_heading())
            std::fprintf(stderr, "[CURVE_GEOMETRY] ipm point-tracks\n");
        if (radial_.enabled())
            std::fprintf(stderr, "[FRONT_GEOMETRY] radial calibration %s lambda=%.6f\n",
                         radial_.valid() ? "enabled" : "invalid; heading suppressed",
                         radial_.coefficient());
        if (ground_.enabled())
            std::fprintf(stderr, "[FRONT_GEOMETRY] ground calibration %s length/width=%.6f\n",
                         ground_.valid() ? "enabled" : "invalid; heading suppressed",
                         ground_.aspect());
        if (ipm_test_)
        {
            const size_t tile_pixels = (size_t)(width / 2) * (height / 2);
            ipm_rgb_.resize(3 * tile_pixels);
            ipm_lane_.resize(tile_pixels);
            ipm_green_.resize(tile_pixels);
            ipm_coverage_.resize(tile_pixels);
            ipm_test_->coverage_mask(ipm_coverage_.data());
        }
        if (front_input_)
        {
            const size_t pixels = (size_t)width * height;
            if(ipm_test_ && ego_curve_ && use_ipm_heading() &&
               (!std::getenv("VISION_DIRECT_MASK_IPM") || std::strcmp(std::getenv("VISION_DIRECT_MASK_IPM"),"0"))) {
                restore_map_=front_input_->mask_restore();
                if(restore_map_)mask_pull_=ipm_test_->compose_masks_from(*restore_map_);
            }
            if(!mask_pull_.enabled()) {raw_lane_.resize(pixels);raw_road_.resize(2*pixels);}
        }
        const char *overlap=std::getenv("VISION_LANE_NPU_OVERLAP");
        if(optional_task_ && optional_task_->overlap_capable() && (!overlap||std::strcmp(overlap,"0")))
            cpu_stage_.reset(new LaneCpuStage);
        std::fprintf(stderr,"[LANE PIPELINE] CPU/NPU overlap=%d single_NPU_owner=1 owned_same_frame=1\n",cpu_stage_?1:0);
        thread_ = std::thread(&LatestWorker::run, this);
    }
    ~LatestWorker()
    {
        stop();
    }
    LatestWorker(const LatestWorker &) = delete;
    LatestWorker &operator=(const LatestWorker &) = delete;

    void submit(const uint8_t *image, uint64_t id, uint64_t captured_ns)
    {
        if (!image || failed_.load())
            return;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (stopping_)
                return;
            if (pending_)
                ++replaced_;
            memcpy(pending_pixels_.data(), image, pending_pixels_.size());
            pending_id_ = id;
            pending_capture_ns_ = captured_ns;
            pending_ = true;
        }
        ready_.notify_one();
    }

    std::shared_ptr<const Snapshot> latest() const
    {
        return std::atomic_load(&latest_);
    }
    bool failed() const
    {
        return failed_.load();
    }
    Totals totals() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return {calls_.load(),    successful_.load(),   fresh_.load(),      stale_.load(),
                replaced_.load(), inference_ns_.load(), analysis_ns_.load(),front_ns_.load(),
                {prepare_ns_.load(),inputs_ns_.load(),run_ns_.load(),outputs_ns_.load(),copy_ns_.load(),release_ns_.load()}};
    }
    void stop()
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stopping_ = true;
            pending_ = false;
        }
        ready_.notify_one();
        if (thread_.joinable())
            thread_.join();
        if(cpu_stage_)cpu_stage_->stop();
    }

  private:
    struct AnalysisJob {
        uint64_t source=0,captured=0,inference_time=0,front_time=0,analysis_time=0;
        int status=-1,model_width=0,model_height=0;
        YOLOTiming timing={};
        std::vector<uint8_t> pixels,front_rgb;
        std::vector<float> lane,road;
        std::shared_ptr<Snapshot> result;
        traffic::LaneBudget budget;
        yolopv2_result_t raw(){return {lane.data(),road.data(),model_width,model_height};}
        traffic::FrontRoadFrame front() const {
            traffic::FrontRoadFrame f;
            if(status==0&&!front_rgb.empty()&&!road.empty()){
                f.pixels=front_rgb.data();f.road=road.data();f.width=model_width;f.height=model_height;
                f.source=source;f.captured=captured;
            }
            return f;
        }
    };
    void analyze(AnalysisJob &job)
    {
        auto result=job.result;auto raw=job.raw();
        auto &working_pixels_=job.pixels;
        const auto source=job.source,captured=job.captured;
        const auto inference_time=job.inference_time,front_time=job.front_time;
        int status=job.status;uint64_t start=clock_ns();
                uint64_t analysis_time = 0;
                if (status == 0 &&
                    (!engine_ || !raw.lane_line_data || !raw.drivable_area_data || raw.width <= 0 ||
                     raw.height <= 0 || raw.width != engine_->model_width ||
                     raw.height != engine_->model_height))
                    status = -1;
                if (status == 0)
                {
                    uint64_t now = clock_ns();
                    if (captured && now >= captured && now - captured <= MAX_RESULT_AGE_NS)
                    {
                        start = now;
                        if (front_input_)
                        {
                            if(mask_pull_.enabled()){
                                const size_t n=(size_t)raw.width*raw.height;
                                result->width=width_;result->height=height_;result->restore_map=restore_map_;
                                result->lane.assign(raw.lane_line_data,raw.lane_line_data+n);
                                result->drivable.assign(raw.drivable_area_data,raw.drivable_area_data+2*n);
                            }else{
                                front_input_->restore_front_masks(raw.lane_line_data, raw.drivable_area_data,
                                                                  raw_lane_.data(), raw_road_.data());
                                raw = {raw_lane_.data(), raw_road_.data(), width_, height_};
                            }
                        }
                        PointLanePair front_points, style_points;
                        YOLOLineStyleResult observed = {};
                        const PointLanePair *line_points = nullptr;
                        if (!use_ipm_heading())
                        {
                            observed = line_analyzer_.analyze_front_camera(
                                raw.lane_line_data, raw.width, raw.height, working_pixels_.data(),
                                width_, height_, nullptr, raw.drivable_area_data,
                                use_point_lanes() ? &front_points : nullptr);
                            if (use_point_lanes())
                                line_points = &front_points;
                        }
                        // 可选诊断只保存有限的配对样本，正常运行不执行磁盘 IO。
                        const char *dump = getenv("LANE_STYLE_DUMP_DIR");
                        if (dump && *dump && dump_count_ < 6 &&
                            (!dump_ns_ || now - dump_ns_ >= 1000000000ULL))
                        {
                            char path[512];
                            snprintf(path, sizeof(path), "%s/source_%llu.ppm", dump,
                                     (unsigned long long)source);
                            FILE *fp = fopen(path, "wb");
                            if (fp)
                            {
                                fprintf(fp, "P6\n%d %d\n255\n", width_, height_);
                                fwrite(working_pixels_.data(), 1, working_pixels_.size(), fp);
                                fclose(fp);
                            }
                            snprintf(path, sizeof(path), "%s/source_%llu_mask.pgm", dump,
                                     (unsigned long long)source);
                            fp = fopen(path, "wb");
                            if (fp)
                            {
                                const auto mask=result->restore_map?result->masks():raw;
                                fprintf(fp, "P5\n%d %d\n255\n", mask.width, mask.height);
                                for (int k = 0; k < mask.width * mask.height; ++k)
                                    fputc(mask.lane_line_data[k] > .3f ? 255 : 0, fp);
                                fclose(fp);
                            }
                            ++dump_count_;
                            dump_ns_ = now;
                        }
                        EgoCurve ipm_curve;
                        if (ipm_test_)
                        {
                            const bool log_ipm = !ipm_log_ns_ || now - ipm_log_ns_ >= 1000000000ULL;
                            if (log_ipm)
                                ipm_log_ns_ = now;
                            if (mask_pull_.enabled() ? mask_pull_.remap(raw.lane_line_data,raw.drivable_area_data,
                                                       raw.width,raw.height,ipm_lane_.data(),ipm_green_.data()) :
                                ipm_test_->remap_masks(raw.lane_line_data, raw.drivable_area_data,
                                                       raw.width, raw.height, ipm_lane_.data(),
                                                       ipm_green_.data()))
                            {
                                LineStyleTrace trace;
                                YOLOLineStyleResult bird = {};
                                ipm_test_->render(working_pixels_.data(), ipm_rgb_.data());
                                const uint64_t zebra_started=clock_ns();
                                const auto zebra=crosswalk_detector_.detect(ipm_rgb_.data(),ipm_coverage_.data());
                                crosswalk_display::Snapshot crosswalk={};
                                crosswalk.session=crosswalk_session_;crosswalk.source=(uint32_t)source;
                                crosswalk.count=zebra.count;crosswalk.quality=zebra.quality;
                                std::copy_n(zebra.stripes,zebra.count,crosswalk.stripes);
                                crosswalk_display::expires(crosswalk,captured+crosswalk_display::TTL);
                                if(crosswalk_ipc_.open(true))crosswalk_ipc_.write(crosswalk);
                                if(log_ipm)std::fprintf(stderr,"[CROSSWALK] source=%llu geometry=ipm stripes=%u quality=%u cpu_ms=%.3f display_only=1\n",(unsigned long long)source,zebra.count,zebra.quality,(clock_ns()-zebra_started)/1e6);
                                if (use_point_lanes())
                                {
                                    const auto rows=lane_mask_rows(ipm_lane_.data(),width_/2,height_/2,0,height_/2);
                                    auto points = select_point_lanes(ipm_lane_.data(), width_ / 2, height_ / 2,&rows);
                                    style_points = marking_profile(ipm_lane_.data(), points,&rows);
                                    bird = EgoLaneSelector::describe_points(style_points, ipm_rgb_.data(),
                                                                            true, ipm_coverage_.data());
                                    trace.near_y = points.last;
                                    ipm_curve = estimate_ego_curve(points, true);
                                    result->ipm_points = std::move(points);
                                    observed = bird;
                                    line_points = &style_points;
                                }
                                else
                                {
                                    bird = EgoLaneSelector(.5, line_analyzer_.bottom_up()).analyze(
                                        ipm_lane_.data(), ipm_green_.data(), ipm_rgb_.data(),
                                        width_ / 2, height_ / 2, &trace, true);
                                    ipm_curve = estimate_ego_curve(bird, true);
                                    if (ipm_curve.confidence)
                                        result->ipm_pair = bird;
                                }
                                ++ipm_frames_;
                                ipm_valid_ += ipm_curve.confidence != 0;
                                if (log_ipm)
                                    std::fprintf(stderr,
                                             "[IPM_TEST] source=%llu area=%u tracks=%u left=%d right=%d near_y=%.0f type=%u confidence=%u span=%.1f curvature_grid=%.6f reason=%u valid=%u/%u\n",
                                             (unsigned long long)source, bird.road_area, bird.object_count,
                                             trace.selected_left, trace.selected_right, trace.near_y,
                                              ipm_curve.type, ipm_curve.confidence, ipm_curve.span_px,
                                              ipm_curve.curvature, ipm_curve.reason, ipm_valid_, ipm_frames_);
                            }
                            else if (log_ipm)
                                std::fprintf(stderr, "[IPM_TEST] mask size does not match input\n");
                        }
                        result->line_style = line_analyzer_.confirm(observed, source, captured, line_points);
                        snapshot_request_.poll_lazy(now, source, captured, working_pixels_.data(),
                            width_, height_, [&]{return result->restore_map?result->masks():raw;},
                            observed, result->line_style);
                        if (!line_log_ns_ || now - line_log_ns_ >= 2000000000ULL)
                        {
                            float lo = result->restore_map?0.f:raw.lane_line_data[0], hi = lo;
                            if(result->restore_map){
                                for(const auto at:restore_map_->corrected_at_raw)if(at!=UINT32_MAX){
                                    lo=std::min(lo,raw.lane_line_data[at]);hi=std::max(hi,raw.lane_line_data[at]);}
                            }else for (int k = 0; k < raw.width * raw.height; ++k) {
                                lo=std::min(lo,raw.lane_line_data[k]);hi=std::max(hi,raw.lane_line_data[k]);}
                            printf(
                                "[LINE_STYLE] source=%llu geometry=%s camera=1 left=%s q=%u raw=%s reason=%s duty=%u spans=%u right=%s q=%u raw=%s reason=%s duty=%u spans=%u mask_range=%.3f..%.3f ego_ref=%.1f road=%u objects=%u\n",
                                (unsigned long long)source, use_ipm_heading() ? "ipm" : "front",
                                line_style_name(result->line_style.left.type),
                                result->line_style.left.quality,
                                line_style_name(observed.left.type),
                                line_reason_name(result->line_style.left.reason),
                                observed.left.duty, observed.left.spans,
                                line_style_name(result->line_style.right.type),
                                result->line_style.right.quality,
                                line_style_name(observed.right.type),
                                line_reason_name(result->line_style.right.reason),
                                observed.right.duty, observed.right.spans, lo, hi,
                                observed.reference_x, observed.road_area, observed.object_count);
                            line_log_ns_ = now;
                        }
                        // Ego steering reads masks/points, so it needs no throwaway RGB overlay.
                        int pixels = ego_curve_ ? 0 : yolo_overlay_lane_and_drivable(working_pixels_.data(), width_,
                                                                    height_, &raw);
                        int curve_status = 0;
                        if (ego_curve_)
                        {
                            const bool ipm_heading = use_ipm_heading();
                            // Reuse the same fit as the preview; an invalid IPM frame stays unknown.
                            EgoCurve observed_curve = ipm_curve;
                            if (!ipm_heading)
                            {
                                if (use_point_lanes())
                                    observed_curve = estimate_ego_curve(front_points);
                                else if (radial_.enabled() || ground_.enabled())
                                    observed_curve = estimate_ego_curve_mapped(observed, [&](ImagePoint &p) {
                                        return radial_.undistort(p, observed.width, observed.height) &&
                                               (!ground_.enabled() ||
                                                ground_.ground_at(p, observed.width, observed.height));
                                    }, ground_.enabled());
                                else
                                    observed_curve = estimate_ego_curve(observed);
                            }
                            EgoCurve curve =
                                curve_confirm_.update(observed_curve, source, captured);
                            result->curve.curve_type = curve.type;
                            result->curve.confidence = curve.confidence;
                            result->curve.angle = use_point_lanes()
                                ? curve.angle : curve_encode_image_heading(curve.angle);
                            if (!curve_log_ns_ || now - curve_log_ns_ >= 2000000000ULL)
                            {
                                std::fprintf(
                                    stderr,
                                    "[CURVE_EGO] source=%llu geometry=%s raw_type=%u type=%u confidence=%u shift_px=%.2f span_px=%.1f reason=%u heading_deg=%u\n",
                                    (unsigned long long)source, ipm_heading ? "ipm" : "front",
                                    observed_curve.type, curve.type,
                                    curve.confidence, curve.shift_px, curve.span_px, curve.reason,
                                    curve.angle);
                                curve_log_ns_ = now;
                            }
                        }
                        else
                            curve_status = yolo_analyze_annotated_curve(
                                working_pixels_.data(), width_, height_, pixels, &result->curve);
                        if (curve_status == 0)
                        {
                            if(!result->restore_map){
                                result->width = raw.width;result->height = raw.height;
                                size_t count = (size_t)raw.width * raw.height;
                                result->lane.assign(raw.lane_line_data, raw.lane_line_data + count);
                                result->drivable.assign(raw.drivable_area_data,raw.drivable_area_data + count * 2);
                            }
                            result->valid = true;
                        }
                        analysis_time = clock_ns() - start;
                    }
                    result->completed_ns = clock_ns();
                    if (!result->fresh(result->completed_ns))
                        result->valid = false;
                }
                else
                    result->completed_ns = clock_ns();
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    if (stopping_)
                        return;
                    ++calls_;
                    inference_ns_ += inference_time;
                    front_ns_ += front_time;
                    if(status==0 && engine_){const auto &t=job.timing;
                        prepare_ns_+=t.prepare_ns;inputs_ns_+=t.inputs_ns;run_ns_+=t.run_ns;
                        outputs_ns_+=t.outputs_ns;copy_ns_+=t.copy_ns;release_ns_+=t.release_ns;}
                    analysis_ns_ += analysis_time;
                    if (status == 0)
                    {
                        ++successful_;
                        if (result->valid)
                            ++fresh_;
                        else
                            ++stale_;
                    }
                }
                // 两张掩码向量各自拥有副本，不引用引擎下一帧的输出缓冲区。
                std::shared_ptr<const Snapshot> immutable = result;
                std::atomic_store(&latest_, immutable);

        job.analysis_time=analysis_time;job.budget=traffic::LaneBudget{};
        job.budget.source=source;job.budget.front=job.front();job.budget.captured=captured;
        job.budget.completed=result->completed_ns;job.budget.duration=inference_time+analysis_time;
        job.budget.fresh=result->fresh(clock_ns());
        job.budget.curve_active=result->curve.curve_type!=0&&result->curve.confidence>0;
        if(result->line_style.road_area)job.budget.reference_x=result->line_style.reference_x;
    }
    void run()
    {
        try {
            AnalysisJob job;
            for(;;){
                job.result=std::make_shared<Snapshot>();
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    ready_.wait(lock,[&]{return stopping_||pending_;});
                    if(stopping_)return;
                    pending_pixels_.swap(working_pixels_);pending_=false;
                    job.source=pending_id_;job.captured=pending_capture_ns_;
                }
                job.pixels.swap(working_pixels_);
                job.result->source_id=job.source;job.result->captured_ns=job.captured;
                uint64_t started=clock_ns();
                if(front_input_){job.front_rgb.resize((size_t)width_*height_*3/4);front_input_->render(job.pixels.data(),job.front_rgb.data());}
                job.front_time=clock_ns()-started;
                yolopv2_result_t raw={};
                job.status=yolo_engine_detect(engine_,front_input_?job.front_rgb.data():job.pixels.data(),
                    front_input_?width_/2:width_,front_input_?height_/2:height_,&raw);
                job.inference_time=clock_ns()-started;
                if(job.status==0 && (!engine_||!raw.lane_line_data||!raw.drivable_area_data||raw.width<=0||raw.height<=0||
                    raw.width!=engine_->model_width||raw.height!=engine_->model_height))job.status=-1;
                if(job.status==0){
                    job.model_width=raw.width;job.model_height=raw.height;job.timing=engine_->timing;
                    const size_t n=(size_t)raw.width*raw.height;
                    job.lane.assign(raw.lane_line_data,raw.lane_line_data+n);
                    job.road.assign(raw.drivable_area_data,raw.drivable_area_data+2*n);
                }
                const uint64_t freeze_done=clock_ns();
                if(cpu_stage_ && optional_task_ && optional_task_->overlap_capable() && job.status==0){
                    auto work=cpu_stage_->submit([&]{analyze(job);});
                    traffic::LaneBudget preliminary;
                    preliminary.source=job.source;preliminary.captured=job.captured;preliminary.front=job.front();
                    preliminary.completed=freeze_done;preliminary.duration=job.inference_time;
                    preliminary.fresh=job.captured&&freeze_done>=job.captured&&freeze_done-job.captured<=MAX_RESULT_AGE_NS;
                    try{optional_task_->prepare_lane(preliminary);}
                    catch(...){optional_task_->cancel_prepared();optional_task_=nullptr;}
                    // Always join, including the optional failure path. The next
                    // inference cannot overwrite inputs used by either consumer.
                    try{work.get();}catch(...){if(optional_task_)optional_task_->cancel_prepared();throw;}
                }else analyze(job);
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    if(stopping_){if(optional_task_)optional_task_->cancel_prepared();return;}
                }
                if(optional_task_){
                    try{optional_task_->after_lane(job.budget);}
                    catch(...){optional_task_->cancel_prepared();optional_task_=nullptr;
                        std::fprintf(stderr,"Optional traffic task stopped; lane worker continues\n");}
                }
                job.pixels.swap(working_pixels_);
                const uint64_t cycle=clock_ns()-started;
                ++pipeline_calls_;pipeline_cycle_ns_+=cycle;pipeline_freeze_ns_+=freeze_done-started-job.inference_time;
                const uint64_t now=clock_ns();
                if(!pipeline_report_ns_)pipeline_report_ns_=now;
                if(now-pipeline_report_ns_>=5000000000ULL){
                    std::fprintf(stderr,"[LANE PIPELINE] overlap=%d cycle_ms=%.3f freeze_ms=%.3f jobs=%llu\n",
                        cpu_stage_?1:0,pipeline_cycle_ns_/(1e6*pipeline_calls_),pipeline_freeze_ns_/(1e6*pipeline_calls_),
                        (unsigned long long)pipeline_calls_);
                    pipeline_calls_=pipeline_cycle_ns_=pipeline_freeze_ns_=0;pipeline_report_ns_=now;
                }
            }
        }catch(...){failed_=true;}
    }

    YOLOEngine *engine_;
    traffic::NpuTask *optional_task_;
    LineStyleAnalyzer line_analyzer_;
    bool use_point_lanes() const
    {
        return ego_curve_ && line_analyzer_.bottom_up() && !ground_.enabled();
    }
    bool use_ipm_heading() const { return ipm_test_ && use_point_lanes(); }
    EgoCurveConfirm curve_confirm_;
    const FrontRadial radial_ = FrontRadial::for_camera(std::getenv("LANE_RADIAL_LAMBDA"),
                                                       "front_radial.conf");
    const GroundProjection ground_ = GroundProjection::for_camera(std::getenv("LANE_GROUND_RECT"),
                                                                   "front_ground.conf");
    bool ego_curve_ = false;
    SnapshotRequest snapshot_request_;
    uint64_t line_log_ns_ = 0, dump_ns_ = 0, curve_log_ns_ = 0;
    unsigned dump_count_ = 0;
    int width_, height_;
    mutable std::mutex mutex_;
    std::condition_variable ready_;
    std::thread thread_;
    std::unique_ptr<LaneCpuStage> cpu_stage_;
    uint64_t pipeline_report_ns_=0,pipeline_calls_=0,pipeline_cycle_ns_=0,pipeline_freeze_ns_=0;
    bool stopping_ = false, pending_ = false;
    uint64_t pending_id_ = 0, pending_capture_ns_ = 0;
    std::vector<uint8_t> pending_pixels_, working_pixels_;
    const FrontRadialPreview *ipm_test_;
    const FrontRadialPreview *front_input_;
    std::vector<float> raw_lane_, raw_road_;
    std::shared_ptr<const FrontMaskRestore> restore_map_;
    FrontMaskPull mask_pull_;
    std::vector<uint8_t> ipm_rgb_, ipm_green_, ipm_coverage_;
    std::vector<float> ipm_lane_;
    crosswalk::Detector crosswalk_detector_;
    crosswalk_display::IPC crosswalk_ipc_;
    const uint32_t crosswalk_session_=uint32_t(clock_ns())|1u;
    uint64_t ipm_log_ns_ = 0;
    unsigned ipm_frames_ = 0, ipm_valid_ = 0;
    std::shared_ptr<const Snapshot> latest_;
    std::atomic<bool> failed_{false};
    std::atomic<uint64_t> calls_{0}, successful_{0}, fresh_{0}, stale_{0}, replaced_{0};
    std::atomic<uint64_t> inference_ns_{0}, analysis_ns_{0};
    std::atomic<uint64_t> front_ns_{0},prepare_ns_{0},inputs_ns_{0},run_ns_{0},outputs_ns_{0},copy_ns_{0},release_ns_{0};
};
}
#endif
