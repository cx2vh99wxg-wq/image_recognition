#include "latest_lane_worker.h"
#include <cassert>
#include <chrono>
#include <cstdio>
#include <stdexcept>
#include <sched.h>
static std::thread::id npu_owner,cpu_owner;
static std::atomic<bool> cpu_started{false},raise_cpu{false};
static std::vector<float> engine_lane,engine_road;
extern "C" int yolo_engine_detect(YOLOEngine*e,const uint8_t*,int,int,yolopv2_result_t*r){
    npu_owner=std::this_thread::get_id();
    engine_lane.assign(e->model_width*e->model_height,0.f);engine_road.assign(engine_lane.size()*2,7.f);
    *r={engine_lane.data(),engine_road.data(),e->model_width,e->model_height};return 0;
}
extern "C" int yolo_overlay_lane_and_drivable(uint8_t*,int,int,const yolopv2_result_t*){return 0;}
extern "C" int yolo_analyze_annotated_curve(const uint8_t*,int,int,int,YOLOCurveResult*r){
    cpu_owner=std::this_thread::get_id();cpu_started=true;
    std::this_thread::sleep_for(std::chrono::milliseconds(25));
    if(raise_cpu)throw std::runtime_error("CPU failure");
    r->curve_type=1;r->confidence=60;return 0;
}
template<class F> static void until(F f){for(int i=0;i<2000&&!f();++i)std::this_thread::sleep_for(std::chrono::milliseconds(1));assert(f());}
struct Task:traffic::NpuTask {
    m_lane::LatestWorker*owner=nullptr;
    std::atomic<unsigned> prepared{0},committed{0},cancelled{0};
    bool overlap=true,throw_prepare=false,direct=false;unsigned pause=0;
    bool overlap_capable()const override{return overlap;}
    void prepare_lane(const traffic::LaneBudget&b)override{
        assert(std::this_thread::get_id()==npu_owner);++prepared;
        assert(b.front.matches(b.source,b.captured)&&b.front.road[0]==7.f&&b.front.pixels[0]==77);
        if(!direct){until([]{return cpu_started.load();});assert(cpu_owner!=npu_owner);}
        // Model-owned output reuse must not change either consumer's snapshot.
        std::fill(engine_road.begin(),engine_road.end(),99.f);
        assert(b.front.road[0]==7.f);
        if(throw_prepare)throw std::runtime_error("optional prepare failure");
        if(pause)std::this_thread::sleep_for(std::chrono::milliseconds(pause));
    }
    void cancel_prepared()noexcept override{++cancelled;}
    void after_lane(const traffic::LaneBudget&b)override{
        assert(std::this_thread::get_id()==npu_owner);
        const auto p=owner->latest();assert(p&&p->valid&&p->source_id==b.source);
        assert(p->masks().drivable_area_data[0]==7.f&&b.front.road[0]==7.f);
        if(direct){assert(p->lane.size()==320*240&&p->drivable.size()==2*320*240);
            assert(b.front.road==p->drivable.data());} // no second mask copy
        assert(b.front.matches(b.source,b.captured));++committed;
    }
};
int main(){
    // Direct IPM analysis also publishes display metadata. Tests must never
    // replace the live board's crosswalk record or consume live requests.
    if(unshare(CLONE_NEWIPC)){perror("private test IPC namespace");return 1;}
    assert(!setenv("LANE_CURVE_MODE","legacy",1));
    YOLOEngine engine={};engine.model_width=320;engine.model_height=240;
    std::vector<uint8_t> rgb(640*480*3,77);
    m_lane::FrontRadialPreview front(640,480,m_lane::FrontRadial(0),nullptr,true);
    for(bool parallel:{false,true}){
        assert(!setenv("VISION_LANE_NPU_OVERLAP",parallel?"1":"0",1));cpu_started=false;
        Task t;m_lane::LatestWorker w(&engine,640,480,&t,nullptr,&front);t.owner=&w;
        w.submit(rgb.data(),17,m_lane::clock_ns());until([&]{return t.committed.load()==1;});w.stop();
        assert(!w.failed()&&t.prepared==unsigned(parallel));
        assert((cpu_owner!=npu_owner)==parallel);
    }
    assert(!setenv("VISION_LANE_NPU_OVERLAP","1",1));
    {assert(!setenv("LANE_CURVE_MODE","ego-boundaries",1));
     m_lane::GroundProjection ground("0,0,1,0,1,1,0,1,1");
     m_lane::FrontRadialPreview ipm(640,480,m_lane::FrontRadial(0),&ground);
     Task t;t.direct=true;m_lane::LatestWorker w(&engine,640,480,&t,&ipm,&front);t.owner=&w;
     w.submit(rgb.data(),21,m_lane::clock_ns());until([&]{return t.committed.load()==1;});w.stop();
     assert(!w.failed()&&w.latest()->restore_map);
     assert(!setenv("LANE_CURVE_MODE","legacy",1));}
    {cpu_started=false;Task t;t.throw_prepare=true;m_lane::LatestWorker w(&engine,640,480,&t,nullptr,&front);t.owner=&w;
     w.submit(rgb.data(),18,m_lane::clock_ns());until([&]{return bool(w.latest());});w.stop();assert(!w.failed()&&w.latest()->valid&&!t.committed&&t.cancelled);}
    {cpu_started=false;raise_cpu=true;Task t;m_lane::LatestWorker w(&engine,640,480,&t,nullptr,&front);t.owner=&w;
     w.submit(rgb.data(),19,m_lane::clock_ns());until([&]{return w.failed();});w.stop();assert(!t.committed&&t.cancelled);raise_cpu=false;}
    {cpu_started=false;Task t;t.pause=70;m_lane::LatestWorker w(&engine,640,480,&t,nullptr,&front);t.owner=&w;
     w.submit(rgb.data(),20,m_lane::clock_ns());until([&]{return t.prepared.load()!=0;});w.stop();assert(!t.committed&&t.cancelled);}
    puts("LANE OVERLAP PASS: actual concurrent CPU stage, one NPU owner, immutable matched snapshots, publish-before-commit, synchronous equivalence, CPU failure and stop cancellation");
}
