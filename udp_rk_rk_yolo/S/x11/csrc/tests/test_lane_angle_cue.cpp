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
    const uint32_t bg=0xff080b20;
    const int seam=(int)std::ceil(lane_cue::picture_rect(w,h).y+lane_cue::picture_rect(w,h).height*lane_cue::SEAM);
    clear(solid);lane_cue::draw(solid.data()+guard,stride,w,h,0,1,1);
    const int upper=(int)((lane_cue::picture_rect(w,h).height*.17+seam)*.5);
    assert(seam>h*.51 && seam<h*.55); // blue reference is lower than old .435
    assert(((solid[guard+upper*stride+160]>>8)&255)>((bg>>8)&255));
    assert(solid[guard+(seam+20)*stride+160]==bg); // no tint below seam
    const auto straight=solid;
    for(int a:{-90,-45,-20,-5,0,5,20,45,90})
    {
        clear(solid);lane_cue::draw(solid.data()+guard,stride,w,h,a,1,1);
        for(int i=0;i<guard;++i)assert(solid[i]==0xA55AA55A&&solid[solid.size()-1-i]==0xA55AA55A);
        for(int y=0;y<h;++y)for(int x=w;x<stride;++x)assert(solid[guard+y*stride+x]==0xA55AA55A);
        // Every lower-road pixel is angle-independent, exactly as requested.
        for(int y=seam;y<h;++y)for(int x=0;x<w;++x)assert(solid[guard+y*stride+x]==straight[guard+y*stride+x]);
        double sum=0;unsigned count=0;
        for(int y=90;y<120;++y)for(int x=0;x<w;++x)if(solid[guard+y*stride+x]==lane_cue::WHITE){sum+=x;++count;}
        assert(count);
        assert(a<0?sum/count<160:a>0?sum/count>160:std::abs(sum/count-160)<.6);
    }
    clear(solid);clear(dashed);
    lane_cue::draw(solid.data()+guard,stride,w,h,0,1,1);
    lane_cue::draw(dashed.data()+guard,stride,w,h,0,2,1);
    for(int band=0;band<2;++band)
    {
        unsigned painted=0,gaps=0;
        for(int y=band?seam:90;y<(band?435:seam);++y)
        {
            unsigned sl=0,dl=0;
            for(int x=0;x<160;++x){sl+=solid[guard+y*stride+x]==lane_cue::WHITE;dl+=dashed[guard+y*stride+x]==lane_cue::WHITE;}
            if(sl){painted+=dl>0;gaps+=dl==0;}
        }
        assert(painted>0&&gaps>0); // dashed over BOTH fixed and bending parts
    }
    for(int y=0;y<h;++y)for(int x=160;x<w;++x)assert(solid[guard+y*stride+x]==dashed[guard+y*stride+x]);
    clear(dashed);lane_cue::draw(dashed.data()+guard,stride,w,h,0,1,1,1,true);
    unsigned red=0;
    for(int y=0;y<h;++y)for(int x=0;x<w;++x)
    {
        const auto base=solid[guard+y*stride+x],actual=dashed[guard+y*stride+x];
        if(x<160){if(base==lane_cue::WHITE)assert(actual==lane_cue::WARNING_RED);red+=actual==lane_cue::WARNING_RED;}
        else assert(actual==base);
    }
    assert(red>300);
    clear(dashed);lane_cue::draw(dashed.data()+guard,stride,w,h,0,1,1,1,false);
    unsigned wide=0,normal=0;
    for(int y=0;y<h;++y)for(int x=0;x<160;++x){wide+=dashed[guard+y*stride+x]==lane_cue::WHITE;normal+=solid[guard+y*stride+x]==lane_cue::WHITE;}
    assert(wide>normal*1.5); // white warning phase remains thick
    clear(dashed);lane_cue::draw(dashed.data()+guard,stride,w,h,0,1,1,0,false);
    assert(dashed==solid); // release restores normal width and color
    assert(lane_cue::warning_sides(3,0,0)==3);
    clear(dashed);lane_cue::draw(dashed.data()+guard,stride,w,h,20,0,0);
    unsigned gray=0;
    for(int y=0;y<h;++y)for(int x=0;x<w;++x)gray+=dashed[guard+y*stride+x]==lane_cue::UNKNOWN_GRAY;
    assert(gray>1000);
    clear(dashed);lane_cue::draw(dashed.data()+guard,stride,w,h,20,1,1,0,false,false);
    clear(solid);lane_cue::draw(solid.data()+guard,stride,w,h,0,0,0,0,false,false);
    assert(solid[guard+upper*stride+160]==bg);
    assert(dashed==solid); // unknown direction is always a complete straight gray pair
    (void)bg;
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
    std::puts("LANE ANGLE CUE PASS: perspective, fixed lower road, signed upper bends, full-length dashes, side-specific red/white alarm, unknowns, guards and angle label updates");
}
