#include "traffic_service.h"
#include "traffic_detector.h"
#include "traffic_light_ipc.h"
#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <thread>
#include <unistd.h>
namespace traffic {
namespace {
struct Frame {std::vector<uint8_t> pixels;uint64_t id=0,captured=0;};
struct Observation {Box box;Frame reference;bool valid=false,ambiguous=false;};
}
struct Service::Impl {
    Detector detector;Admission admission;bool ready=false,enabled=true;
    const InputView view;
    std::atomic<unsigned> deferred{0};std::atomic<bool> model_failed{false};
    std::mutex mutex;std::condition_variable wake;Frame latest;uint64_t pending_id=0;bool stopping=false;
    std::shared_ptr<const Observation> observation;std::thread cpu;
    TrafficLightIPC ipc={-1,nullptr};uint32_t session=0;
    uint64_t report_ns=0,calls=0,passes=0,skipped_curve=0,skipped_budget=0,total_ns=0,max_ns=0;
    explicit Impl(const char* path,InputView input):view(input){
        const char* option=std::getenv("VISION_TRAFFIC");enabled=!option||std::atoi(option)!=0;
        session=(uint32_t)(traffic_clock_ns()^((uint64_t)getpid()<<19))|1u;
        traffic_ipc_open(&ipc,1);TrafficLightData initial={};traffic_ipc_write(&ipc,&initial);
        if(enabled){std::string error;ready=detector.init(path,&error);if(!ready)std::fprintf(stderr,"[TRAFFIC] optional detector unavailable: %s; lane pipeline continues\n",error.c_str());}
        std::printf("[TRAFFIC] enabled=%d ready=%d input=%d max_locator_rate=1Hz max_NPU_passes=3 color_rate=10Hz single_NPU_owner=lane_worker control_output=none\n",enabled,ready,detector.input_size());
        std::printf("[TRAFFIC INPUT] source_rect=%d,%d,320,240 source_order=%s detector_order=RGB\n",view.x,view.y,view.order==PixelOrder::BGR?"BGR":"RGB");
        cpu=std::thread(&Impl::color_loop,this);
    }
    ~Impl(){
        {std::lock_guard<std::mutex> lock(mutex);stopping=true;}wake.notify_one();if(cpu.joinable())cpu.join();
        TrafficLightData empty={};traffic_ipc_write(&ipc,&empty);traffic_ipc_close(&ipc);
    }
    void submit(const uint8_t* rgb,int w,int h,uint64_t id,uint64_t captured){
        if(!rgb||w<320||h<240||!captured)return;
        {std::lock_guard<std::mutex> lock(mutex);if(stopping||(latest.captured&&(captured<=latest.captured||captured-latest.captured<100000000ULL))||id==latest.id)return;
            if(!copy_camera_rgb(rgb,w,h,w*3,view,latest.pixels))return;
            latest.id=id;latest.captured=captured;pending_id=id;}
        wake.notify_one();
    }
    void npu(const LaneBudget& lane){
        uint64_t now=traffic_clock_ns();unsigned reason=0;
        if(lane.curve_active){++skipped_curve;reason=TRAFFIC_LANE_PRIORITY;}
        if(!admission.allow(lane,now)){++skipped_budget;reason|=TRAFFIC_BUDGET;}
        deferred.store(reason);
        if(!ready||!enabled||model_failed.load()||reason)return;
        Frame frame;
        {std::lock_guard<std::mutex> lock(mutex);if(stopping||latest.pixels.empty())return;frame=latest;}
        if(now<frame.captured||now-frame.captured>200000000ULL)return;
        uint64_t start=traffic_clock_ns();std::vector<Box> boxes;std::string error;
        bool okay=detector.detect(frame.pixels.data(),320,240,320*3,boxes,&error);
        auto result=std::make_shared<Observation>();result->reference.id=frame.id;result->reference.captured=frame.captured;
        if(!okay){model_failed.store(true);std::fprintf(stderr,"[TRAFFIC] optional detector disabled after error: %s\n",error.c_str());}
        else if(!boxes.empty()){
            float reference=std::max(0.f,std::min(320.f,lane.reference_x));
            auto rank=[&](const Box& b){float dx=std::abs(b.x+b.w/2-reference)/320.f;return b.score/(1+2*dx);};
            std::sort(boxes.begin(),boxes.end(),[&](const Box& a,const Box& b){return rank(a)>rank(b);});
            result->box=boxes[0];result->valid=true;result->reference.pixels=std::move(frame.pixels);
            Color first=classify_color(result->reference.pixels.data(),320,240,320*3,boxes[0]);
            for(size_t i=1;i<boxes.size();++i)if(rank(boxes[i])>=rank(boxes[0])*.8f&&overlap(boxes[i],boxes[0])<.2f){Color other=classify_color(result->reference.pixels.data(),320,240,320*3,boxes[i]);if(first.state&&other.state&&first.state!=other.state)result->ambiguous=true;}
        }
        std::shared_ptr<const Observation> immutable=result;std::atomic_store(&observation,immutable);
        // Include CPU candidate arbitration in the lane-gap budget as well.
        uint64_t end=traffic_clock_ns(),elapsed=end-start;admission.completed(end,elapsed);++calls;passes+=detector.last_passes();total_ns+=elapsed;max_ns=std::max(max_ns,elapsed);
        if(!report_ns||end-report_ns>=5000000000ULL){double seconds=report_ns?(end-report_ns)/1e9:1;
            std::printf("[TRAFFIC PERF] locator_hz=%.2f npu_hz=%.2f calls=%llu mean_ms=%.2f max_ms=%.2f skip_curve=%llu skip_budget=%llu boxes=%zu budget_backoff=%u\n",calls/seconds,passes/seconds,(unsigned long long)calls,calls?total_ns/1e6/calls:0.,max_ns/1e6,(unsigned long long)skipped_curve,(unsigned long long)skipped_budget,boxes.size(),admission.violations());
            report_ns=end;calls=passes=skipped_curve=skipped_budget=total_ns=max_ns=0;}
    }
    void color_loop(){
        HousingTracker tracker;ColorConfirmation confirmation;Box tracked;uint64_t detector_id=0,last_id=0,last_captured=0,last_log=0;bool have_track=false;
        try {
            for(;;){Frame frame;
                {std::unique_lock<std::mutex> lock(mutex);wake.wait(lock,[&]{return stopping||pending_id!=last_id;});if(stopping)return;frame=latest;last_id=frame.id;}
                uint64_t now=traffic_clock_ns();if(frame.captured<=last_captured||now<frame.captured||now-frame.captured>250000000ULL)continue;last_captured=frame.captured;
                TrafficLightData data={};data.session=session;data.source_id=(uint32_t)frame.id;data.reason=enabled?TRAFFIC_NO_BOX:TRAFFIC_DEFERRED;
                data.flags=(ready?TRAFFIC_MODEL_READY:0)|deferred.load()|(!enabled?TRAFFIC_DISABLED:0);traffic_set_deadline(&data,frame.captured+TRAFFIC_TTL_NS);
                auto box=std::atomic_load(&observation);
                if(!ready||model_failed.load())data.reason=enabled?TRAFFIC_MODEL_ERROR:TRAFFIC_DEFERRED;
                else if(box&&frame.captured>=box->reference.captured){
                    uint64_t age=frame.captured-box->reference.captured;data.detector_id=(uint32_t)box->reference.id;data.detection_age_ms=(uint32_t)std::min<uint64_t>(60000,age/1000000);
                    if(box->valid&&age<=1800000000ULL){
                        if(box->reference.id!=detector_id){bool same=have_track&&overlap(tracked,box->box)>.3f;tracker.start(box->reference.pixels.data(),320,240,320*3,box->box);tracked=box->box;have_track=true;detector_id=box->reference.id;if(!same)confirmation.reset();}
                        have_track=have_track&&tracker.update(frame.pixels.data(),320,240,320*3,box->box,tracked);
                        if(have_track&&!box->ambiguous){
                            Color color=classify_color(frame.pixels.data(),320,240,320*3,tracked);data.raw_state=color.state;data.color_quality=color.quality;data.reason=color.reason;
                            data.state=confirmation.observe(color.state,frame.id,frame.captured);if(color.state&&!data.state)data.reason=TRAFFIC_CONFIRMING;
                            data.flags|=TRAFFIC_BOX|TRAFFIC_TRACKED;if(data.state)data.flags|=TRAFFIC_CONFIRMED;
                            data.detector_quality=(uint32_t)std::min(100.f,box->box.score*100);data.x=(uint32_t)tracked.x;data.y=(uint32_t)tracked.y;data.width=(uint32_t)tracked.w;data.height=(uint32_t)tracked.h;
                        }else{data.reason=box->ambiguous?TRAFFIC_AMBIGUOUS:TRAFFIC_TRACK_LOST;confirmation.reset();}
                    }else{data.reason=box->valid?TRAFFIC_STALE:TRAFFIC_NO_BOX;have_track=false;tracker.reset();confirmation.reset();}
                }
                traffic_ipc_open(&ipc,1);traffic_ipc_write(&ipc,&data);
                if(!last_log||now-last_log>=2000000000ULL){std::printf("[TRAFFIC] source=%u detector=%u state=%s raw=%s color_q=%u detector_q=%u age_ms=%u reason=%u flags=%u box=%u,%u,%u,%u\n",data.source_id,data.detector_id,traffic_state_name(data.state),traffic_state_name(data.raw_state),data.color_quality,data.detector_quality,data.detection_age_ms,data.reason,data.flags,data.x,data.y,data.width,data.height);last_log=now;}
            }
        }catch(...){model_failed.store(true);TrafficLightData empty={};traffic_ipc_write(&ipc,&empty);std::fprintf(stderr,"[TRAFFIC] color worker stopped; lane worker remains independent\n");}
    }
};
Service::Service(const char* path,InputView view):impl_(new Impl(path,view)){}
Service::~Service()=default;
void Service::submit_raw(const uint8_t* p,int w,int h,uint64_t id,uint64_t captured){impl_->submit(p,w,h,id,captured);}
void Service::after_lane(const LaneBudget& b){try{impl_->npu(b);}catch(...){impl_->model_failed.store(true);}}
}
