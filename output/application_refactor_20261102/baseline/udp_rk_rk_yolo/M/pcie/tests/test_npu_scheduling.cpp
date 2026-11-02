#include "latest_lane_worker.h"
#include <cassert>
#include <chrono>
#include <cstdio>
#include <functional>
#include <stdexcept>
#include <thread>
static std::atomic<unsigned> in_lane{0};
extern "C" int yolo_engine_detect(YOLOEngine*,const uint8_t*,int,int,yolopv2_result_t* r){
    assert(++in_lane==1);std::this_thread::sleep_for(std::chrono::milliseconds(4));
    static float lane[192]={},road[384]={};r->lane_line_data=lane;r->drivable_area_data=road;r->width=16;r->height=12;--in_lane;return 0;
}
extern "C" int yolo_overlay_lane_and_drivable(uint8_t*,int,int,const yolopv2_result_t*){return 0;}
extern "C" int yolo_analyze_annotated_curve(const uint8_t*,int,int,int,YOLOCurveResult* r){r->curve_type=1;r->confidence=60;return 0;}
struct Optional:traffic::NpuTask {
    m_lane::LatestWorker* owner=nullptr;std::atomic<unsigned> calls{0};std::atomic<bool> entered{false};bool throw_error=false;
    void after_lane(const traffic::LaneBudget& b)override {
        assert(in_lane==0&&owner);auto published=owner->latest();
        assert(published&&published->source_id==b.source&&published->valid);
        assert(b.fresh&&b.curve_active);++calls;entered=true;
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        assert(in_lane==0);if(throw_error)throw std::runtime_error("optional failure");
    }
};
static void until(std::function<bool()> test){for(int i=0;i<2000&&!test();++i)std::this_thread::sleep_for(std::chrono::milliseconds(1));assert(test());}
int main(){
    YOLOEngine engine={};engine.model_width=16;engine.model_height=12;
    std::vector<uint8_t> rgb(16*12*3);
    {
        Optional task;m_lane::LatestWorker w(&engine,16,12,&task);task.owner=&w;
        w.submit(rgb.data(),1,m_lane::clock_ns());until([&]{return task.entered.load();});
        for(unsigned id=2;id<=20;++id)w.submit(rgb.data(),id,m_lane::clock_ns());
        until([&]{auto p=w.latest();return p&&p->source_id==20;});w.stop();
        assert(!w.failed()&&task.calls==2&&w.totals().replaced>=18);
    }
    {
        Optional task;task.throw_error=true;m_lane::LatestWorker w(&engine,16,12,&task);task.owner=&w;
        w.submit(rgb.data(),1,m_lane::clock_ns());until([&]{return task.entered.load();});
        w.submit(rgb.data(),2,m_lane::clock_ns());until([&]{auto p=w.latest();return p&&p->source_id==2;});
        w.stop();assert(!w.failed()&&task.calls==1&&w.latest()->valid);
    }
    puts("NPU SCHEDULING PASS: lane result published first, single owner, latest-only pending frame, optional failure does not stop lane");
}
