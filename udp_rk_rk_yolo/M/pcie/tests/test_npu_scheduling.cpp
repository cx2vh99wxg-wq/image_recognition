#include "latest_lane_worker.h"
#include <cassert>
#include <chrono>
#include <cstdio>
#include <functional>
#include <stdexcept>
#include <thread>
static std::atomic<unsigned> in_lane{0};
static float road_value=7.f;
extern "C" int yolo_engine_detect(YOLOEngine* e,const uint8_t*,int,int,yolopv2_result_t* r){
    assert(++in_lane==1);std::this_thread::sleep_for(std::chrono::milliseconds(4));
    static std::vector<float> lane,road;lane.assign(e->model_width*e->model_height,0.f);road.assign(lane.size()*2,road_value);
    r->lane_line_data=lane.data();r->drivable_area_data=road.data();r->width=e->model_width;r->height=e->model_height;--in_lane;return 0;
}
extern "C" int yolo_overlay_lane_and_drivable(uint8_t*,int,int,const yolopv2_result_t*){return 0;}
extern "C" int yolo_analyze_annotated_curve(const uint8_t*,int,int,int,YOLOCurveResult* r){r->curve_type=1;r->confidence=60;return 0;}
struct Optional:traffic::NpuTask {
    m_lane::LatestWorker* owner=nullptr;std::atomic<unsigned> calls{0};std::atomic<bool> entered{false},release{false};bool block=false,throw_error=false,check_front=false;
    void after_lane(const traffic::LaneBudget& b)override {
        assert(in_lane==0&&owner);auto published=owner->latest();
        assert(published&&published->source_id==b.source&&published->valid);
        assert(b.fresh&&b.curve_active);++calls;entered=true;
        if(check_front) {
            assert(b.front.matches(b.source,b.captured));
            assert(b.front.pixels[0]==77 && b.front.road[0]==7.f);
            assert(!b.front.matches(b.source+1,b.captured));
        }
        if(block)while(!release.load())std::this_thread::sleep_for(std::chrono::milliseconds(1));
        else std::this_thread::sleep_for(std::chrono::milliseconds(25));
        assert(in_lane==0);if(throw_error)throw std::runtime_error("optional failure");
    }
};
static void until(std::function<bool()> test){for(int i=0;i<2000&&!test();++i)std::this_thread::sleep_for(std::chrono::milliseconds(1));assert(test());}
struct DirectOptional:traffic::NpuTask {
    m_lane::LatestWorker *owner=nullptr;std::atomic<bool> called{false};
    void after_lane(const traffic::LaneBudget &b)override{
        const auto p=owner->latest();assert(p&&p->valid&&p->restore_map);
        assert(b.front.matches(b.source,b.captured)&&b.front.road[0]==7.f);
        called=true;
    }
};
int main(){
    // The fake curve below is emitted by the legacy overlay stub only.
    if (setenv("LANE_CURVE_MODE", "legacy", 1) != 0) return 1;
    YOLOEngine engine={};engine.model_width=16;engine.model_height=12;
    std::vector<uint8_t> rgb(16*12*3);
    {
        Optional task;task.block=true;m_lane::LatestWorker w(&engine,16,12,&task);task.owner=&w;
        w.submit(rgb.data(),1,m_lane::clock_ns());until([&]{return task.entered.load();});
        for(unsigned id=2;id<=20;++id)w.submit(rgb.data(),id,m_lane::clock_ns());
        task.release=true;
        until([&]{auto p=w.latest();return p&&p->source_id==20&&task.calls==2;});w.stop();
        assert(!w.failed()&&task.calls==2&&w.totals().replaced>=18);
    }
    {
        Optional task;task.throw_error=true;m_lane::LatestWorker w(&engine,16,12,&task);task.owner=&w;
        w.submit(rgb.data(),1,m_lane::clock_ns());until([&]{return task.entered.load();});
        w.submit(rgb.data(),2,m_lane::clock_ns());until([&]{auto p=w.latest();return p&&p->source_id==2;});
        w.stop();assert(!w.failed()&&task.calls==1&&w.latest()->valid);
    }
    {
        engine.model_width=320;engine.model_height=240;rgb.assign(640*480*3,77);
        m_lane::FrontRadialPreview front(640,480,m_lane::FrontRadial(0),nullptr,true);
        assert(front.enabled());Optional task;task.check_front=true;
        m_lane::LatestWorker w(&engine,640,480,&task,nullptr,&front);task.owner=&w;
        w.submit(rgb.data(),37,m_lane::clock_ns());until([&]{return task.entered.load();});w.stop();
        assert(!w.failed()&&task.calls==1);
    }
    {
        char directory[]="build/npu-direct-XXXXXX",previous[4096];
        assert(getcwd(previous,sizeof(previous))&&mkdtemp(directory)&&!chdir(directory));
        assert(!setenv("LANE_CURVE_MODE","ego-boundaries",1));
        assert(!setenv("LANE_CANDIDATE_MODE","bottom-up",1));unsetenv("LANE_GROUND_RECT");
        std::shared_ptr<const m_lane::Snapshot> kept;
        {
            m_lane::FrontRadialPreview front(640,480,m_lane::FrontRadial(0),nullptr,true);
            m_lane::GroundProjection ground(".30,.40,.70,.40,.90,.95,.10,.95,2");
            m_lane::FrontRadialPreview bird(640,480,m_lane::FrontRadial(0),&ground);
            DirectOptional task;m_lane::LatestWorker w(&engine,640,480,&task,&bird,&front);task.owner=&w;
            w.submit(rgb.data(),48,m_lane::clock_ns());until([&]{return task.called.load();});w.stop();
            kept=w.latest();assert(!w.failed());
        }
        road_value=9; yolopv2_result_t newer={};yolo_engine_detect(&engine,rgb.data(),320,240,&newer);
        auto restored=kept->masks();assert(restored.width==640&&restored.height==480);
        assert(restored.drivable_area_data[0]==7.f&&restored.drivable_area_data[640*480]==7.f);
        assert(!chdir(previous)&&!rmdir(directory));
    }
    puts("NPU SCHEDULING PASS: direct IPM output ownership after engine/mapper advance, matched frame/mask, lane published first, single owner, latest mailbox, optional failure isolation");
}
