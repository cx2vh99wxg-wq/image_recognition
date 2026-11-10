#include "traffic_service.h"
#include "traffic_signal_geometry.h"
#include "traffic_detector.h"
#include "traffic_frame_input.h"
#include "traffic_light_ipc.h"
#include "front_person_ipc.h"
#include "person_road_association.h"
#include "traffic_structure.h"
#include "../../../common/perception/include/display_people.h"
#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <thread>
#include <unistd.h>
namespace traffic
{
namespace
{
struct Frame
{
    std::vector<uint8_t> pixels;
    uint64_t id = 0, captured = 0;
};
struct PreparedDetection {
    Frame frame;
    std::vector<Box> boxes;
    PersonResult people;
    bool matched=false,okay=false;
    std::string error;
    DetectorTiming timing;
    DetectorSchedule checkpoint;
    unsigned passes=0;
    uint64_t compute_ns=0;
    bool scouted=false;
    uint64_t next_scout=0;
    unsigned next_cursor=0;
    unsigned next_seen=0;
    std::array<float,SIGNAL_WINDOWS> merits={};
};
struct Observation
{
    Box box;
    bool regional=false;
    Frame reference;
    bool valid = false, ambiguous = false, structural = false;
};
}
struct Service::Impl
{
    Detector detector;
    PersonRoadAssociation road_association;
    Admission admission{configured_budget_ms(std::getenv("TRAFFIC_NPU_BUDGET_MS")),
                        configured_locator_interval_ms(std::getenv("TRAFFIC_NPU_INTERVAL_MS"))};
    bool ready = false, enabled = true;
    const InputView view;
    const m_lane::FrontRadialPreview *correction;
    std::atomic<unsigned> deferred{0};
    std::atomic<bool> model_failed{false};
    std::mutex mutex;
    std::condition_variable wake;
    Frame latest;
    uint64_t pending_id = 0;
    bool stopping = false;
    std::shared_ptr<const Observation> observation;
    std::thread cpu;
    TrafficLightIPC ipc = {-1, nullptr};
    FrontPersonIPC person_ipc = {-1, nullptr};
    display_people::IPC display_ipc;
    const m_lane::GroundProjection display_ground=m_lane::GroundProjection::for_camera(nullptr,"front_ipm_test.conf");
    uint32_t session = 0;
    uint64_t report_ns = 0, calls = 0, passes = 0, skipped_curve = 0, skipped_budget = 0,
             total_ns = 0, max_ns = 0;
    DetectorTiming detector_totals;
    std::unique_ptr<PreparedDetection> prepared;
    uint64_t prepared_used=0,prepared_discarded=0;
    uint64_t scout_after=0;
    unsigned scout_cursor=0;
    unsigned scout_seen=0;
    std::array<float,SIGNAL_WINDOWS> scout_merits={};
    explicit Impl(const char *path, InputView input, const m_lane::FrontRadialPreview *front)
        : view(input), correction(front)
    {
        const char *option = std::getenv("VISION_TRAFFIC");
        enabled = !option || std::atoi(option) != 0;
        session = (uint32_t)(traffic_clock_ns() ^ ((uint64_t)getpid() << 19)) | 1u;
        traffic_ipc_open(&ipc, 1);
        TrafficLightData initial = {};
        traffic_ipc_write(&ipc, &initial);
        front_person_ipc_open(&person_ipc, 1);
        FrontPersonData empty_person = {}; front_person_ipc_write(&person_ipc, &empty_person);
        if (enabled)
        {
            std::string error;
            ready = detector.init(path, &error);
            if (!ready)
                std::fprintf(
                    stderr,
                    "[TRAFFIC] optional detector unavailable: %s; lane pipeline continues\n",
                    error.c_str());
        }
        std::printf(
                "[TRAFFIC] enabled=%d ready=%d input=%d locator_interval_ms=%u max_NPU_passes=2 extra_signal_ROI=conditional color_rate=10Hz budget_ms=%u single_NPU_owner=lane_worker classes=person:0,traffic_light:9 view=full-front control_output=metadata\n",
            enabled, ready, detector.input_size(), admission.interval_ms(), admission.budget_ms());
        std::printf(
            "[TRAFFIC INPUT] geometry=%s source_rect=%d,%d,320,240 source_order=%s detector_order=RGB\n",
            correction ? "radial-corrected" : "raw", view.x, view.y,
            view.order == PixelOrder::BGR ? "BGR" : "RGB");
        cpu = std::thread(&Impl::color_loop, this);
    }
    ~Impl()
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            stopping = true;
        }
        wake.notify_one();
        if (cpu.joinable())
            cpu.join();
        TrafficLightData empty = {};
        traffic_ipc_write(&ipc, &empty);
        traffic_ipc_close(&ipc);
        FrontPersonData empty_person = {}; front_person_ipc_write(&person_ipc, &empty_person);
        front_person_ipc_close(&person_ipc);
    }
    void submit(const uint8_t *rgb, int w, int h, uint64_t id, uint64_t captured)
    {
        if (!rgb || w < 320 || h < 240 || !captured)
            return;
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (stopping ||
                (latest.captured &&
                 (captured <= latest.captured || captured - latest.captured < 100000000ULL)) ||
                id == latest.id)
                return;
            if (!copy_input_rgb(rgb, w, h, view, correction, latest.pixels))
                return;
            latest.id = id;
            latest.captured = captured;
            pending_id = id;
        }
        wake.notify_one();
    }
    void cancel() noexcept {
        if(prepared){detector.restore_schedule(prepared->checkpoint);prepared.reset();++prepared_discarded;}
    }
    std::unique_ptr<PreparedDetection> compute(const LaneBudget &lane,uint64_t now){
        std::unique_ptr<PreparedDetection> d(new PreparedDetection);
        auto &frame=d->frame;d->matched=lane.front.matches(lane.source,lane.captured);
        if(d->matched){
            if(!copy_camera_rgb(lane.front.pixels,320,240,320*3,InputView(0,0,view.order),frame.pixels))return nullptr;
            frame.id=lane.source;frame.captured=lane.captured;
        }else{
            std::lock_guard<std::mutex> lock(mutex);
            if(stopping||latest.pixels.empty())return nullptr;
            frame=latest;
        }
        if(now<frame.captured || now-frame.captured>300000000ULL)return nullptr;
        d->checkpoint=detector.schedule();
        const uint64_t start=traffic_clock_ns();
        std::vector<Box> hints;
        try{
            d->okay=detector.detect(frame.pixels.data(),320,240,320*3,d->boxes,&d->error,admission.budget_ms(),&d->people,false,&hints);
        }catch(...){detector.restore_schedule(d->checkpoint);throw;}
        d->timing=detector.last_timing();d->passes=detector.last_passes();
        d->next_scout=scout_after;d->next_cursor=scout_cursor;d->next_seen=scout_seen;d->merits=scout_merits;
        float strongest=0;bool conflict=false;std::vector<unsigned> colors;
        for(const auto&b:d->boxes){auto c=classify_signal_color(frame.pixels.data(),320,240,960,b);colors.push_back(c.state);if(c.state)strongest=std::max(strongest,b.score);}
        for(size_t i=0;i<d->boxes.size();++i)for(size_t j=0;j<i;++j)
            if(colors[i]&&colors[j]&&colors[i]!=colors[j]&&same_signal_bar(d->boxes[i],d->boxes[j]))conflict=true;
        // Preserve the full-frame person pass. A bounded extra signal-only
        // crop is allowed for weak/missing heads. An initial empty sweep can
        // try the next view on the next job; steady searches remain at 600 ms.
        const auto prior=std::atomic_load(&observation);
        // A verified large three-lamp diagram already has the strict CPU
        // locator below. Skip redundant tiny NN crops; commit still locates
        // and classifies the CURRENT frame, never reuses its old color.
        const bool structure_lock=prior&&prior->valid&&prior->structural&&frame.captured>=prior->reference.captured&&frame.captured-prior->reference.captured<=1800000000ULL;
        bool regional_missing=prior&&prior->valid&&prior->regional&&frame.captured>=prior->reference.captured&&frame.captured-prior->reference.captured<=1500000000ULL;
        if(regional_missing)for(const auto&b:d->boxes)if(overlap(prior->box,b)>.2f)regional_missing=false;
        if(d->okay&&!structure_lock&&now>=scout_after&&(strongest<.30f||conflict||regional_missing)&&!hints.empty()){
            unsigned index=scout_cursor%SIGNAL_WINDOWS;
            if(!scout_seen){
                float best=0;
                for(const auto&hint:hints){
                    bool explained=false;for(const auto&b:d->boxes)if(overlap(hint,b)>.3f)explained=true;
                    if(!explained&&hint.score>best){best=hint.score;float x=hint.x+hint.w*.5f;index=x<120?0:x<200?1:2;}
                }
            }
            index=next_signal_window(scout_seen,scout_cursor,scout_merits,index);
            const auto window=signal_window(index);const int x0=window.x,y0=window.y,size=window.size;std::vector<uint8_t> crop(size*size*3);
            for(int y=0;y<size;++y)std::memcpy(crop.data()+y*size*3,frame.pixels.data()+(y+y0)*960+x0*3,size*3);
            std::vector<Box> local;std::string crop_error;
            const bool okay=detector.detect(crop.data(),size,size,size*3,local,&crop_error,admission.budget_ms(),nullptr,true);
            const auto t=detector.last_timing();d->timing.prepare+=t.prepare;d->timing.inputs+=t.inputs;d->timing.run+=t.run;d->timing.outputs+=t.outputs;d->timing.decode+=t.decode;d->timing.release+=t.release;d->timing.filter+=t.filter;d->passes+=detector.last_passes();
            if(okay){
                d->scouted=true;d->next_cursor=scout_cursor+1;d->next_seen=scout_seen|(1u<<index);d->merits[index]=0;
                for(auto b:local){
                    if(b.x<3||b.y<2||b.x+b.w>size-3||b.y+b.h>size-2)continue;
                    b.x+=x0;b.y+=y0;
                    if(classify_signal_color(frame.pixels.data(),320,240,960,b).state)
                        d->merits[index]=std::max(d->merits[index],signal_relevance(b,lane.reference_x));
                    bool duplicate=false;
                    for(auto&old:d->boxes)if(overlap(old,b)>.45f){if(b.score>old.score)old=b;duplicate=true;break;}
                    if(!duplicate)d->boxes.push_back(b);
                }
                if(*std::max_element(scout_merits.begin(),scout_merits.end())>0&&*std::max_element(d->merits.begin(),d->merits.end())<=0){d->next_seen=1u<<index;if(index==1)d->next_cursor=3;}
                d->next_scout=now+signal_scout_delay(d->merits,d->next_seen);
                std::fprintf(stderr,"[TRAFFIC SCOUT] source=%llu window=%u seen=%u boxes=%zu merit=%.3f/%.3f/%.3f/%.3f\n",(unsigned long long)frame.id,index,d->next_seen,local.size(),d->merits[0],d->merits[1],d->merits[2],d->merits[3]);
            }
        }
        d->compute_ns=traffic_clock_ns()-start;
        return d;
    }
    void prepare(const LaneBudget &lane){
        cancel();
        const uint64_t now=traffic_clock_ns();
        // Admission here is only a compute hint. The final, post-analysis
        // budget and freshness guard below remains authoritative. A rejected
        // prepared result restores crop/track scheduling and publishes nothing.
        if(!ready||!enabled||model_failed.load()||!lane.front.matches(lane.source,lane.captured)||!admission.allow(lane,now))return;
        prepared=compute(lane,now);
    }
    void npu(const LaneBudget &lane)
    {
        uint64_t now = traffic_clock_ns();
        unsigned reason = 0;
        if (!admission.allow(lane, now))
        {
            ++skipped_budget;
            reason |= TRAFFIC_BUDGET;
        }
        deferred.store(reason);
        if (!ready || !enabled || model_failed.load() || reason){cancel();return;}
        std::unique_ptr<PreparedDetection> detection;
        if(prepared && prepared->frame.id==lane.source && prepared->frame.captured==lane.captured){
            detection=std::move(prepared);++prepared_used;
        }else{
            cancel();detection=compute(lane,now);
        }
        if(!detection)return;
        scout_after=detection->next_scout;scout_cursor=detection->next_cursor;scout_seen=detection->next_seen;scout_merits=detection->merits;
        auto &frame=detection->frame;auto &boxes=detection->boxes;auto &people=detection->people;
        const bool matched=detection->matched,okay=detection->okay;
        const auto &error=detection->error;
        const uint64_t commit_started=traffic_clock_ns();
        const auto dt=detection->timing;
        detector_totals.prepare+=dt.prepare;detector_totals.inputs+=dt.inputs;
        detector_totals.run+=dt.run;detector_totals.outputs+=dt.outputs;
        detector_totals.decode+=dt.decode;detector_totals.release+=dt.release;detector_totals.filter+=dt.filter;
        FrontPersonData person = {};
        person.session = session; person.source_id = (uint32_t)frame.id;
        person.flags = ready ? FRONT_PERSON_READY : 0;
        if (okay && traffic_clock_ns()-frame.captured<=500000000ULL)
            person.flags |= FRONT_PERSON_VALID;
        road_association.annotate(people,matched ? lane.front : FrontRoadFrame{},
                                  frame.id,frame.captured,person);
        front_person_set_deadline(&person, frame.captured + FRONT_PERSON_TTL_NS);
        front_person_ipc_open(&person_ipc, 1); front_person_ipc_write(&person_ipc, &person);
        // Independent, bounded visual metadata. Brake/control records above
        // remain unchanged, including all threshold-passing anchor statistics.
        display_people::Snapshot scene={};scene.session=session;scene.source=(uint32_t)frame.id;
        scene.ready=(person.flags&FRONT_PERSON_VALID)?1:0;
        display_people::expires(scene,frame.captured+display_people::TTL);
        if(scene.ready)for(const auto&b:people.candidates){
            if(!std::isfinite(b.x)||!std::isfinite(b.y)||!std::isfinite(b.w)||!std::isfinite(b.h)||b.w<=0||b.h<=0)continue;
            auto glyph=display_people::box(display_people::Front,(int)b.x,(int)b.y,(int)std::ceil(b.w),(int)std::ceil(b.h),b.score);
            const auto relation=road_association.classify(b);
            if(relation==FootRelation::Inside)glyph.flags|=display_people::InRoad;
            else if(relation==FootRelation::Outside)glyph.flags|=display_people::OffRoad;
            const double scale=correction?correction->display_scale():1.;
            m_lane::ImagePoint foot={159.5+(b.x+b.w*.5-159.5)/scale,119.5+(b.y+b.h-119.5)/scale};
            if(display_ground.ground_at(foot,320,240)){
                glyph.flags|=display_people::Ground;
                glyph.gx=(int32_t)std::lround((foot.x/160.+.5)*1000.);
                glyph.gy=(int32_t)std::lround(foot.y/(160.*display_ground.aspect())*1000.);
            }
            display_people::add(scene,glyph);
        }
        if(display_ipc.open(display_people::FRONT_KEY,true))display_ipc.write(scene);
        // Keep the existing person/brake publication above ahead of any
        // optional CPU signal search. This path does not alter its candidates.
        const bool structural=okay&&boxes.empty();
        if(structural)boxes=locate_three_lamp_housings(frame.pixels.data(),320,240,320*3);
        auto result = std::make_shared<Observation>();
        result->regional=detection->scouted;
        result->structural=structural&&!boxes.empty();
        result->reference.id = frame.id;
        result->reference.captured = frame.captured;
        if (!okay)
        {
            model_failed.store(true);
            std::fprintf(stderr, "[TRAFFIC] optional detector disabled after error: %s\n",
                         error.c_str());
        }
        else if (!boxes.empty())
        {
            float reference = std::max(0.f, std::min(320.f, lane.reference_x));
            auto rank = [&](const Box &b)
            {
                return signal_relevance(b,reference);
            };
            std::sort(boxes.begin(), boxes.end(),
                      [&](const Box &a, const Box &b) { return rank(a) > rank(b); });
            result->box = boxes[0];
            result->valid = true;
            result->reference.pixels = std::move(frame.pixels);
            Color first =
                classify_signal_color(result->reference.pixels.data(), 320, 240, 320 * 3, boxes[0],result->structural);
            for (size_t i = 1; i < boxes.size(); ++i)
                if (signal_competing_head(boxes[0],boxes[i],rank(boxes[0]),rank(boxes[i])) && overlap(boxes[i], boxes[0]) < .2f)
                {
                    Color other = classify_signal_color(result->reference.pixels.data(), 320, 240, 320 * 3,
                                                 boxes[i],result->structural);
                    if (first.state && other.state && first.state != other.state)
                        result->ambiguous = true;
                }
        }
        std::shared_ptr<const Observation> immutable = result;
        const auto previous=std::atomic_load(&observation);
        // A full-frame miss must not erase a more confident regional head in
        // between scouts. Keep its ORIGINAL NN frame/time; do not renew its age.
        // The color thread still rechecks/tracks the current camera pixels.
        const uint8_t*current=result->reference.pixels.empty()?frame.pixels.data():result->reference.pixels.data();
        if(!result->ambiguous&&previous&&previous->regional&&previous->valid&&
           frame.captured>=previous->reference.captured&&frame.captured-previous->reference.captured<=1500000000ULL&&
           (!result->valid||(overlap(previous->box,result->box)<.2f&&signal_relevance(previous->box,lane.reference_x)>signal_relevance(result->box,lane.reference_x)*1.1f))&&
           signal_frame_housing(current,320,240,960,previous->box)&&
           classify_signal_color(current,320,240,960,previous->box,previous->structural).state)immutable=previous;
        std::atomic_store(&observation, immutable);
        // CPU 候选仲裁的时间也计入车道间隙预算。
        uint64_t end = traffic_clock_ns(), elapsed = detection->compute_ns + end - commit_started;
        admission.completed(end, elapsed);
        ++calls;
        passes += detection->passes;
        total_ns += elapsed;
        max_ns = std::max(max_ns, elapsed);
        if (!report_ns || end - report_ns >= 5000000000ULL)
        {
            double seconds = report_ns ? (end - report_ns) / 1e9 : 1;
            const double divisor=1e6*std::max<uint64_t>(1,calls);
            std::printf("[PERF TRAFFIC] prepare_ms=%.3f inputs_ms=%.3f run_ms=%.3f outputs_ms=%.3f decode_ms=%.3f release_ms=%.3f filter_ms=%.3f\n",
                detector_totals.prepare/divisor,detector_totals.inputs/divisor,detector_totals.run/divisor,detector_totals.outputs/divisor,
                detector_totals.decode/divisor,detector_totals.release/divisor,detector_totals.filter/divisor);
            detector_totals={};
            std::printf(
                "[TRAFFIC PERF] locator_hz=%.2f npu_hz=%.2f calls=%llu mean_ms=%.2f max_ms=%.2f person_candidates=%u person_valid=%d skip_curve=%llu skip_budget=%llu boxes=%zu budget_backoff=%u road_source=%u person_source=%u road_pixels=%u in_path=%u outside=%u uncertain=%u matched=%d prepared_used=%llu prepared_discarded=%llu\n",
                calls / seconds, passes / seconds, (unsigned long long)calls,
                calls ? total_ns / 1e6 / calls : 0., max_ns / 1e6,
                people.count, okay, (unsigned long long)skipped_curve, (unsigned long long)skipped_budget, boxes.size(),
                admission.violations(),person.road_source_id,person.source_id,person.road_pixels,
                person.risk_count,person.outside_count,person.unknown_count,matched,
                (unsigned long long)prepared_used,(unsigned long long)prepared_discarded);
            report_ns = end;prepared_used=prepared_discarded=0;
            calls = passes = skipped_curve = skipped_budget = total_ns = max_ns = 0;
        }
    }
    void color_loop()
    {
        HousingTracker tracker;
        ColorConfirmation confirmation;
        Box tracked;
        uint64_t detector_id = 0, last_id = 0, last_captured = 0, last_log = 0;
        bool have_track = false;
        try
        {
            for (;;)
            {
                Frame frame;
                {
                    std::unique_lock<std::mutex> lock(mutex);
                    wake.wait(lock, [&] { return stopping || pending_id != last_id; });
                    if (stopping)
                        return;
                    frame = latest;
                    last_id = frame.id;
                }
                uint64_t now = traffic_clock_ns();
                if (frame.captured <= last_captured || now < frame.captured ||
                    now - frame.captured > 250000000ULL)
                    continue;
                last_captured = frame.captured;
                TrafficLightData data = {};
                data.session = session;
                data.source_id = (uint32_t)frame.id;
                data.reason = enabled ? TRAFFIC_NO_BOX : TRAFFIC_DEFERRED;
                data.flags = (ready ? TRAFFIC_MODEL_READY : 0) | deferred.load() |
                             (!enabled ? TRAFFIC_DISABLED : 0);
                traffic_set_deadline(&data, frame.captured + TRAFFIC_TTL_NS);
                auto box = std::atomic_load(&observation);
                if (!ready || model_failed.load())
                    data.reason = enabled ? TRAFFIC_MODEL_ERROR : TRAFFIC_DEFERRED;
                else if (box && frame.captured >= box->reference.captured)
                {
                    uint64_t age = frame.captured - box->reference.captured;
                    data.detector_id = (uint32_t)box->reference.id;
                    data.detection_age_ms = (uint32_t)std::min<uint64_t>(60000, age / 1000000);
                    if (box->valid && age <= 1800000000ULL)
                    {
                        if (box->reference.id != detector_id)
                        {
                            bool same = have_track && overlap(tracked, box->box) > .3f;
                            tracker.start(box->reference.pixels.data(), 320, 240, 320 * 3,
                                          box->box);
                            tracked = box->box;
                            have_track = true;
                            detector_id = box->reference.id;
                            if (!same)
                                confirmation.reset();
                        }
                        have_track = have_track && tracker.update(frame.pixels.data(), 320, 240,
                                                                  320 * 3, box->box, tracked);
                        const bool housing = have_track &&
                            signal_frame_housing(frame.pixels.data(), 320, 240, 320 * 3, tracked);
                        if (housing && !box->ambiguous)
                        {
                            Color color =
                                classify_signal_color(frame.pixels.data(), 320, 240, 320 * 3, tracked,box->structural);
                            data.raw_state = color.state;
                            data.color_quality = color.quality;
                            data.reason = color.reason;
                            data.state =
                                confirmation.observe(color.state, frame.id, frame.captured);
                            if (color.state && !data.state)
                                data.reason = TRAFFIC_CONFIRMING;
                            data.flags |= TRAFFIC_BOX | TRAFFIC_TRACKED;
                            if (data.state)
                                data.flags |= TRAFFIC_CONFIRMED;
                            data.detector_quality = (uint32_t)std::min(100.f, box->box.score * 100);
                            data.x = (uint32_t)tracked.x;
                            data.y = (uint32_t)tracked.y;
                            data.width = (uint32_t)tracked.w;
                            data.height = (uint32_t)tracked.h;
                        }
                        else
                        {
                            data.reason = box->ambiguous ? TRAFFIC_AMBIGUOUS :
                                          have_track ? TRAFFIC_NO_BOX : TRAFFIC_TRACK_LOST;
                            confirmation.reset();
                        }
                    }
                    else
                    {
                        data.reason = box->valid ? TRAFFIC_STALE : TRAFFIC_NO_BOX;
                        have_track = false;
                        tracker.reset();
                        confirmation.reset();
                    }
                }
                traffic_ipc_open(&ipc, 1);
                traffic_ipc_write(&ipc, &data);
                if (!last_log || now - last_log >= 2000000000ULL)
                {
                    std::printf(
                        "[TRAFFIC] source=%u detector=%u state=%s raw=%s color_q=%u detector_q=%u age_ms=%u reason=%u flags=%u box=%u,%u,%u,%u locator=%s\n",
                        data.source_id, data.detector_id, traffic_state_name(data.state),
                        traffic_state_name(data.raw_state), data.color_quality,
                        data.detector_quality, data.detection_age_ms, data.reason, data.flags,
                        data.x, data.y, data.width, data.height,box&&box->structural?"three-lamp-structure":"YOLOv5n");
                    last_log = now;
                }
            }
        }
        catch (...)
        {
            model_failed.store(true);
            TrafficLightData empty = {};
            traffic_ipc_write(&ipc, &empty);
            std::fprintf(stderr,
                         "[TRAFFIC] color worker stopped; lane worker remains independent\n");
        }
    }
};
Service::Service(const char *path, InputView view, const m_lane::FrontRadialPreview *correction)
    : impl_(new Impl(path, view, correction))
{
}
Service::~Service() = default;
void Service::submit_raw(const uint8_t *p, int w, int h, uint64_t id, uint64_t captured)
{
    impl_->submit(p, w, h, id, captured);
}
void Service::prepare_lane(const LaneBudget &b)
{
    try{impl_->prepare(b);}catch(...){impl_->cancel();impl_->model_failed.store(true);}
}
void Service::cancel_prepared() noexcept { impl_->cancel(); }
void Service::after_lane(const LaneBudget &b)
{
    try
    {
        impl_->npu(b);
    }
    catch (...)
    {
        impl_->cancel();impl_->model_failed.store(true);
    }
}
}
