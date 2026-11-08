#include "dashboard_ui.h"
#include <cassert>
#include <cstdio>

int main(){
    using namespace perception_ui;
    Dashboard reference,cached;
    std::vector<uint32_t> video(1280*480),vehicle(320*480);
    ViewStatus status;status.pcie_live=status.udp_live=true;status.pcie_fps=status.udp_fps=30;
    // Independent nearest-neighbor oracle for the real six-cell placement and
    // the scaled car panel. This catches SIMD index/alpha/seam errors even
    // when both cached and full redraw would share the same fast scaler.
    for(size_t i=0;i<video.size();++i)video[i]=(i*71u)&0xffffffu;
    for(size_t i=0;i<vehicle.size();++i)vehicle[i]=(i*117u)&0xffffffu;
    Dashboard mapping;
    const auto *mapped=mapping.compose(video.data(),1280,vehicle.data(),320,true,-1,"ALL",status);
    for(int row=0;row<2;++row)for(int col=0;col<3;++col)
        for(int y=(row?1:0);y<273;++y)for(int x=(col?1:0);x<364;++x){
            const int sy=row*240+y*240/273,sx=col*320+x*320/364;
            assert(mapped[(224+row*273+y)*WIDTH+72+col*364+x]==(video[sy*1280+sx]|0xff000000u));
        }
    for(int y=20;y<420;++y)for(int x=0;x<296;++x)
        assert(mapped[(258+y)*WIDTH+1238+x]==(vehicle[(y*480/444)*320+x*320/296]|0xff000000u));
    unsigned reused=0;uint32_t random=71;
    Dashboard direct,intermediate;
    std::vector<uint16_t> pcie(640*480),udp(640*480);
    // The fused path must equal both old stages for every output pixel,
    // including six tile edges, local flips, network orientation and UI.
    for(int frame=0;frame<8;++frame){
        for(auto *plane:{&pcie,&udp})for(auto &p:*plane){random=random*1664525u+1013904223u;p=uint16_t(random>>8);}
        six_camera::overview(video.data(),pcie.data(),udp.data());
        status.person=frame&1;status.line_left=frame%3;status.lane_angle_deg=frame*3-10;
        const auto *a=intermediate.compose(video.data(),1280,vehicle.data(),320,true,-1,"ALL",status);
        const auto *b=direct.compose(video.data(),1280,vehicle.data(),320,true,-1,"ALL",status,true,pcie.data(),udp.data());
        assert(std::equal(a,a+WIDTH*HEIGHT,b));
    }
    for(int frame=0;frame<32;++frame){
        for(auto &pixel:video){random=random*1664525u+1013904223u;pixel=0xff000000u|(random&0xffffffu);}
        const bool vehicle_changed=frame%8==0;
        if(vehicle_changed)for(auto &pixel:vehicle){random=random*1664525u+1013904223u;pixel=0xff000000u|(random&0xffffffu);}
        if(frame%4==0){
            status.lane_angle_valid=frame<12;status.lane_angle_deg=frame<8?-12:8;
            status.line_left=(frame/4)%3;status.line_right=(frame/8)%3;
            status.line_left_quality=60+frame;status.line_right_reason=frame%11;
            status.motion=frame<8?MotionDirection::Left:frame<16?MotionDirection::Reverse:MotionDirection::Idle;
            status.person=frame>=8&&frame<16;status.front_person=frame>=12&&frame<16;
            status.go_blocked=frame>=16&&frame<20;status.traffic=frame<20?1:3;
            status.gear_known=status.gear_live=true;status.gear=frame<20?3:1;
            status.pcie_live=frame<24;status.udp_fps=frame<28?30.f:0.f;
        }
        const bool overview=frame<24;
        const char *label=overview?"SURROUND VIEW":"FRONT CAMERA";
        const float progress=overview?-1.f:float(frame-24)/7;
        const auto *full=reference.compose(video.data(),1280,vehicle.data(),320,overview,progress,label,status,true);
        const auto *optimized=cached.compose(video.data(),1280,vehicle.data(),320,overview,progress,label,status,vehicle_changed);
        assert(std::equal(full,full+WIDTH*HEIGHT,optimized));
        reused+=!cached.uiRedrawn();
    }
    assert(reused>=16);
    // Null artwork, placeholder and video-loss transitions must clear old UI.
    for(int frame=0;frame<4;++frame){
        const auto *v=frame&1?nullptr:video.data();const auto *car=frame<2?nullptr:vehicle.data();
        const auto *full=reference.compose(v,1280,car,320,true,-1,"ALL",status,true);
        const auto *optimized=cached.compose(v,1280,car,320,true,-1,"ALL",status,false);
        assert(std::equal(full,full+WIDTH*HEIGHT,optimized));
    }
    std::printf("DASHBOARD CACHE PASS: %u reused UI frames; every output pixel equals full redraw across video, angle, styles, alarms, gear, motion, focus and source loss\n",reused);
}
