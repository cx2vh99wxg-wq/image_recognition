#include "latest_lane_worker.h"
#include <cassert>
#include <chrono>
#include <cstdio>
#include <thread>
int main(){
    constexpr int w=320,h=240,mw=640,mh=480;const size_t n=w*h,full=mw*mh;
    std::vector<float> lane(n),road(2*n),legacy_lane(full,0),legacy_road(2*full,0),a(n),b(n);
    std::vector<uint8_t> ga(n),gb(n);
    unsigned random=7;auto value=[&](){random=random*1664525u+1013904223u;return ((random>>8)%65536)/32768.f-1.f;};
    const m_lane::GroundProjection ground(".30,.40,.70,.40,.90,.95,.10,.95,2");
    for(double lambda:{-.12,0.,.06}){
        m_lane::FrontRadialPreview front(mw,mh,m_lane::FrontRadial(lambda),nullptr,true);
        m_lane::FrontRadialPreview bird(mw,mh,m_lane::FrontRadial(lambda),&ground);
        auto restore=front.mask_restore();assert(restore);
        auto pull=bird.compose_masks_from(*restore);assert(pull.enabled());
        for(int frame=0;frame<4;++frame){
            for(auto &v:lane)v=value();
            for(auto &v:road)v=value();
            front.restore_front_masks(lane.data(),road.data(),legacy_lane.data(),legacy_road.data());
            assert(bird.remap_masks(legacy_lane.data(),legacy_road.data(),mw,mh,a.data(),ga.data()));
            assert(pull.remap(lane.data(),road.data(),w,h,b.data(),gb.data()));
            assert(!memcmp(a.data(),b.data(),n*sizeof(float)) && ga==gb);
            m_lane::Snapshot snapshot;snapshot.width=mw;snapshot.height=mh;
            snapshot.lane=lane;snapshot.drivable=road;snapshot.restore_map=restore;
            yolopv2_result_t p={},q={};
            std::thread first([&]{p=snapshot.masks();}),second([&]{q=snapshot.masks();});first.join();second.join();
            assert(p.width==mw&&p.height==mh&&p.lane_line_data==q.lane_line_data);
            assert(!memcmp(p.lane_line_data,legacy_lane.data(),full*sizeof(float)));
            assert(!memcmp(p.drivable_area_data,legacy_road.data(),2*full*sizeof(float)));
            std::fill(lane.begin(),lane.end(),9.f);
            assert(!memcmp(p.lane_line_data,legacy_lane.data(),full*sizeof(float))); // owns NN output copy
        }
    }
    puts("DIRECT MASK IPM PASS: 12 randomized cases / 3 calibrations, bitwise bilinear values/green decisions, exact mosaic overlay, concurrent lazy restore and ownership");
}
