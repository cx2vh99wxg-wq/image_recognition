#include "yolo_integration.h"
#include "overlay_blend_lut.h"
#include <cassert>
#include <cstdio>
#include <vector>
int main(){
    for(unsigned value=0;value<256;++value){
        assert(overlay_darker[value]==(uint8_t)(value*.6+0*.4));
        assert(overlay_greener[value]==(uint8_t)(value*.6+255*.4));
    }
    constexpr int w=640,h=480;const size_t n=w*h;
    std::vector<float> lane(n,0.f),road(2*n,0.f);
    std::vector<uint8_t> image(3*n);unsigned random=9;
    auto next=[&](){random=random*1664525u+1013904223u;return random;};
    for(int frame=0;frame<12;++frame){
        for(auto &b:image)b=(uint8_t)next();
        for(int y=0;y<h/2;++y)for(int x=0;x<w/2;++x){const size_t i=y*w+x;
            lane[i]=(next()%1000)/999.f;road[i]=(next()%2000)/999.f;road[n+i]=(next()%2000)/999.f;}
        auto expected=image,full=image,roi=image;yolopv2_result_t mask={lane.data(),road.data(),w,h};
        int count=0;
        for(size_t i=0;i<n;++i){auto *p=expected.data()+3*i;
            if(road[i]<road[n+i]){p[0]=(uint8_t)(p[0]*.6+0*.4);p[1]=(uint8_t)(p[1]*.6+255*.4);p[2]=(uint8_t)(p[2]*.6+0*.4);}
            if(lane[i]>.3){p[0]=p[1]=0;p[2]=255;++count;}}
        const int a=yolo_overlay_lane_and_drivable(full.data(),w,h,&mask);
        const int b=yolo_overlay_lane_and_drivable_roi(roi.data(),w,h,&mask,w/2,h/2);
        assert(a==b&&a==count&&full==roi&&roi==expected);
    }
    puts("OVERLAY ROI PASS: 12 randomized frames, identical pixel bytes/lane count, all other camera tiles preserved");
}
