#include "lane_angle_cue.h"
#include "dashboard_ui.h"
#include <cassert>
#include <cstdio>
#include <vector>
#include <fstream>

int main(int argc, char **argv)
{
    assert(lane_cue::signed_degrees(1,5)==-5);
    assert(lane_cue::signed_degrees(2,5)==5);
    assert(lane_cue::signed_degrees(0,0)==0);
    assert(lane_cue::signed_degrees(1,curve_encode_image_heading(45))==-45);
    assert(lane_cue::valid(0,85,0) && lane_cue::valid(2,60,5));
    assert(!lane_cue::valid(0,0,0) && !lane_cue::valid(3,85,5));
    assert(!lane_cue::valid(0,85,5) && !lane_cue::valid(2,85,0));
    assert(!lane_cue::valid(1,85,999));
    constexpr int w=320,h=480,stride=336,guard=32;
    std::vector<uint32_t> solid(stride*h+guard*2,0xA55AA55A), dashed=solid;
    auto clear=[](std::vector<uint32_t> &p){
        for(int y=0;y<h;++y) std::fill_n(p.data()+guard+y*stride,w,0xff080b20);
    };
    for(int a:{-90,-45,-5,0,5,45,90})
    {
        clear(solid);
        lane_cue::draw(solid.data()+guard,stride,w,h,a,1,1);
        for(int i=0;i<guard;++i)assert(solid[i]==0xA55AA55A && solid[solid.size()-1-i]==0xA55AA55A);
        for(int y=0;y<h;++y)for(int x=w;x<stride;++x)
            assert(solid[guard+y*stride+x]==0xA55AA55A);
        if(a==5 || a==-5)
        {
            double xsum=0;unsigned count=0;
            for(int y=20;y<70;++y)for(int x=0;x<w;++x)
                if(solid[guard+y*stride+x]==0xff00ff00){xsum+=x;++count;}
            assert(count && (a>0 ? xsum/count>160 : xsum/count<160));
        }
    }
    clear(solid);clear(dashed);
    lane_cue::draw(solid.data()+guard,stride,w,h,0,1,1);
    lane_cue::draw(dashed.data()+guard,stride,w,h,0,2,1);
    unsigned sl=0,dl=0;
    for(int y=0;y<h;++y)for(int x=0;x<w;++x)
    {
        if(x<160){sl+=solid[guard+y*stride+x]==0xff00ff00;dl+=dashed[guard+y*stride+x]==0xff00ff00;}
        else assert(solid[guard+y*stride+x]==dashed[guard+y*stride+x]);
    }
    assert(dl>sl/3 && dl<sl*4/5);
    lane_cue::draw(dashed.data()+guard,stride,w,h,0,1,2);
    perception_ui::Dashboard dashboard;
    perception_ui::ViewStatus status;
    status.lane_angle_valid=true;status.lane_angle_deg=-5;
    auto frame=dashboard.compose(nullptr,0,nullptr,0,true,-1,"TEST",status);
    std::vector<uint32_t> before(frame,frame+1600*900);
    status.lane_angle_deg=5;
    frame=dashboard.compose(nullptr,0,nullptr,0,true,-1,"TEST",status);
    assert(!std::equal(before.begin()+234*1600,before.begin()+268*1600,frame+234*1600));
    if(argc==2)
    {
        std::ofstream out(argv[1],std::ios::binary);out<<"P6\n1600 900\n255\n";
        for(int i=0;i<1600*900;++i){const char p[]={char(frame[i]>>16),char(frame[i]>>8),char(frame[i])};out.write(p,3);}
    }
    status.lane_angle_valid=false;
    frame=dashboard.compose(nullptr,0,nullptr,0,true,-1,"TEST",status);
    std::puts("LANE ANGLE CUE PASS: signed angles, variable curves, independent dash styles, clipping guards and angle label updates");
}
