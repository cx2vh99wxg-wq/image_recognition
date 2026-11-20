#ifndef FRONT_VEHICLE_TRACKING_H
#define FRONT_VEHICLE_TRACKING_H
#include "traffic_detector.h"
#include "../../../common/perception/include/display_vehicles.h"
#include <array>

namespace traffic {
struct RoadsideTruckWindow {
    int x;
    static constexpr int y=114, width=106, height=55, output_width=320;
    static constexpr int output_height=height*output_width/width;
};

inline RoadsideTruckWindow roadside_truck_window(bool right)
{
    return {right ? 145 : 320 - 145 - RoadsideTruckWindow::width};
}

inline bool roadside_truck_detection(const VehicleDetection &vehicle)
{
    const auto &box=vehicle.box;
    return vehicle.coco_class==display_vehicles::Truck &&
           box.w>=11 && box.h>=13 && box.w<=3.f*box.h;
}

// Bridge intermittent detector misses with a fresh image match. Only the
// detector reinitializes the template or extends its lifetime.
class FrontVehicleTracking {
    static constexpr int grid=8;
    using Patch=std::array<int,grid*grid>;
    struct Track {
        VehicleDetection detection;
        Patch reference;
        Box previous;
        uint64_t verified;
    };
    std::vector<Track> tracks_;
    static bool cycle(unsigned k) {return k==display_vehicles::Bicycle || k==display_vehicles::Motorcycle;}
    static bool observable(const Box &b) {
        return b.w>=11 && b.h>=13 && b.w<145 && b.h<120 && b.x>=0 && b.y>=0 && b.x+b.w<=320 && b.y+b.h<=240;
    }
    static void sample(const uint8_t *rgb,const Box &b,Patch &out) {
        for(int j=0;j<grid;++j) {
            const int row=int(b.y+b.h*(j+.5f)/grid);
            for(int i=0;i<grid;++i) {
                const auto *p=rgb+3*(row*320+int(b.x+b.w*(i+.5f)/grid));
                out[j*grid+i]=(int(p[0])+2*p[1]+p[2])/4;
            }
        }
    }
    static int difference(const Patch &a,const Patch &b) {
        int sum=0;for(size_t i=0;i<a.size();++i)sum+=std::abs(a[i]-b[i]);return sum;
    }
    static bool follow(const uint8_t *rgb,const Track &track,Box &out) {
        const auto &p=track.previous;
        int best=grid*grid*27;
        const int span=std::min(32,std::max(12,int(.9f*p.w)));
        auto inspect=[&](float cx,float cy,float w,float h) {
            Box b(cx-.5f*w,cy-.5f*h,w,h,p.score);
            if(!observable(b))return;
            Patch patch;sample(rgb,b,patch);
            const int error=difference(track.reference,patch);
            if(error<best){best=error;out=b;}
        };
        const float cx=p.x+.5f*p.w,cy=p.y+.5f*p.h;
        for(float scale:{.89f,1.f,1.13f}) for(int y=-9;y<=9;y+=3)
            for(int x=-span;x<=span;x+=3)inspect(cx+x,cy+y,p.w*scale,p.h*scale);
        if(!out.w)return false;
        const float x=out.x+.5f*out.w,y=out.y+.5f*out.h,w=out.w,h=out.h;
        for(int j=-2;j<=2;++j)for(int i=-2;i<=2;++i)inspect(x+i,y+j,w,h);
        return best<=grid*grid*22;
    }
public:
    void update(const uint8_t *pixels,uint64_t now,std::vector<VehicleDetection> &objects) {
        std::vector<Track> next;
        for(const auto &obj:objects) {
            if((!cycle(obj.coco_class) && obj.coco_class!=display_vehicles::Truck) || !observable(obj.box))continue;
            Patch patch;sample(pixels,obj.box,patch);
            const auto limits=std::minmax_element(patch.begin(),patch.end());
            if(*limits.second-*limits.first>=64)next.push_back({obj,patch,obj.box,now});
        }
        for(auto track:tracks_) {
            if(now<=track.verified || now-track.verified>950000000ULL || objects.size()>=display_vehicles::CAPACITY)continue;
            if(std::any_of(objects.begin(),objects.end(),[&](const VehicleDetection &v) {
                return cycle(v.coco_class)==cycle(track.detection.coco_class) && overlap(v.box,track.previous)>.62f;
            }))continue;
            Box matched={};
            if(!follow(pixels,track,matched)) {
                if(next.size()<display_vehicles::CAPACITY)next.push_back(track);
                continue;
            }
            if(std::any_of(objects.begin(),objects.end(),[&](const VehicleDetection &v){
                return cycle(v.coco_class)==cycle(track.detection.coco_class) && overlap(v.box,matched)>.35f;
            }))continue;
            track.previous=matched;objects.push_back({track.detection.coco_class,matched});next.push_back(track);
        }
        tracks_.swap(next);
    }
};
}
#endif
