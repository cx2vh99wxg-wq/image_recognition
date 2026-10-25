#include "latest_lane_worker.h"
#include <cassert>
#include <chrono>
#include <cstdio>
#include <stdexcept>
#include <thread>
static std::atomic<int> mode{0}, active{0}, overlays{0}, curves{0};
struct Active {
    Active() { assert(++active == 1); }
    ~Active() { --active; }
};
extern "C" int yolo_engine_detect(YOLOEngine*, const uint8_t* p, int, int, yolopv2_result_t* r) {
    Active guard;
    int m=mode.load();
    std::this_thread::sleep_for(std::chrono::milliseconds(m==6?60:10));
    if(m==7) throw std::runtime_error("mock error");
    if(m==5) return -1;
    static float lane[192],seg[384];
    std::fill(lane,lane+192,(float)p[0]);
    std::fill(seg,seg+384,(float)p[0]);
    r->lane_line_data=m==1?nullptr:lane;
    r->drivable_area_data=m==8?nullptr:seg;
    r->width=m==2?0:(m==4?17:16);
    r->height=m==3?0:12;
    return 0;
}
extern "C" int yolo_overlay_lane_and_drivable(uint8_t* p,int,int,const yolopv2_result_t* r) {
    assert(r->width==16&&r->height==12&&r->lane_line_data&&r->drivable_area_data);
    ++overlays; p[0]=255; return 150;
}
extern "C" int yolo_analyze_annotated_curve(const uint8_t* p,int,int,int count,YOLOCurveResult* r) {
    assert(p[0]==255&&count==150);
    ++curves; r->curve_type=1;r->confidence=60;r->angle=25;return 0;
}
static void nap(int ms=1) {std::this_thread::sleep_for(std::chrono::milliseconds(ms));}
static std::shared_ptr<const m_lane::Snapshot> wait_id(m_lane::LatestWorker& w,unsigned id) {
    for(int n=0;n<2000;++n) {auto s=w.latest(); if(s&&s->source_id==id)return s;nap();}
    assert(false);return {};
}
int main() {
    YOLOEngine engine={}; engine.model_width=16;engine.model_height=12;
    std::vector<uint8_t> image(16*12*3);
    {
        m_lane::LatestWorker worker(&engine,16,12);
        std::shared_ptr<const m_lane::Snapshot> held;
        for(unsigned id=1;id<=200;++id) {
            if((id-1)%3==0) {std::fill(image.begin(),image.end(),id);worker.submit(image.data(),id,m_lane::clock_ns());}
            auto s=worker.latest();
            if(s&&s->valid) {
                assert(s->source_id<=id&&s->lane.size()==192&&s->drivable.size()==384);
                assert(s->lane[0]==(float)(uint8_t)s->source_id);
                if(!held)held=s;
            }
            if(held)assert(held->lane[0]==(float)(uint8_t)held->source_id);
            nap();
        }
        auto last=wait_id(worker,199);assert(last->valid);
        auto total=worker.totals();
        assert(total.replaced>0&&total.calls<67&&total.successful==total.calls);
        assert(total.fresh_results==total.successful&&total.stale_results==0);
        worker.stop();assert(active==0);
        std::printf("bounded slot PASS calls=%llu replaced=%llu latest=%llu\n",(unsigned long long)total.calls,(unsigned long long)total.replaced,(unsigned long long)last->source_id);
    }
    for(int bad: {1,2,3,4,5,8}) {
        mode=bad;int before=overlays;
        m_lane::LatestWorker worker(&engine,16,12);
        worker.submit(image.data(),1,m_lane::clock_ns());auto result=wait_id(worker,1);
        assert(!result->valid&&!worker.failed()&&overlays==before);
        worker.stop();assert(active==0);
    }
    mode=0;
    for(int future=0;future<2;++future) {
        m_lane::LatestWorker worker(&engine,16,12);int before=overlays;
        auto now=m_lane::clock_ns();
        worker.submit(image.data(),1,future?now+1000000000ULL:now-501000000ULL);
        auto result=wait_id(worker,1);assert(!result->valid&&overlays==before);
        auto t=worker.totals();assert(t.successful==1&&t.stale_results==1);
    }
    mode=6;
    {
        m_lane::LatestWorker worker(&engine,16,12);worker.submit(image.data(),1,m_lane::clock_ns());
        for(int n=0;active==0&&n<100;++n) { nap(); }
        assert(active==1);
        worker.stop();assert(active==0);
    }
    mode=7;
    {
        m_lane::LatestWorker worker(&engine,16,12);worker.submit(image.data(),1,m_lane::clock_ns());
        for(int n=0;!worker.failed()&&n<1000;++n) { nap(); }
        assert(worker.failed());
        worker.stop();assert(active==0);
    }
    m_lane::Snapshot sample;sample.valid=true;sample.captured_ns=100;
    assert(sample.fresh(500000100)&&!sample.fresh(500000101)&&!sample.fresh(99));
    assert(overlays==curves);
    std::puts("invalid outputs/stale/future/exceptions/join/immutable masks PASS");
}
