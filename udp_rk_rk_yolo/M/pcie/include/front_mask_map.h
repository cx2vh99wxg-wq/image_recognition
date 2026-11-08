#ifndef M_FRONT_MASK_MAP_H
#define M_FRONT_MASK_MAP_H
#include <array>
#include <cstdint>
#include <vector>
namespace m_lane {
struct FrontMaskRestore {
    int tile_width=0,tile_height=0,mosaic_width=0,mosaic_height=0;
    std::vector<uint32_t> corrected_at_raw;
    void restore(const float *lane,const float *road,float *dst_lane,float *dst_road) const {
        const size_t tile_size=(size_t)tile_width*tile_height;
        const size_t image_size=(size_t)mosaic_width*mosaic_height;
        for(size_t i=0;i<corrected_at_raw.size();++i){
            const uint32_t at=corrected_at_raw[i];
            const size_t dst=(i/tile_width)*mosaic_width+i%tile_width;
            dst_lane[dst]=at==UINT32_MAX?0.f:lane[at];
            dst_road[dst]=at==UINT32_MAX?0.f:road[at];
            dst_road[image_size+dst]=at==UINT32_MAX?0.f:road[tile_size+at];
        }
    }
};
class FrontMaskPull {
    friend class FrontRadialPreview;
    struct Sample {std::array<uint32_t,4> at;float fx,fy;};
    int width_=0,height_=0;
    std::vector<Sample> samples_;
    static float sample(const float *p,const Sample &s){
        const float p0=s.at[0]==UINT32_MAX?0.f:p[s.at[0]],p1=s.at[1]==UINT32_MAX?0.f:p[s.at[1]],
                    p2=s.at[2]==UINT32_MAX?0.f:p[s.at[2]],p3=s.at[3]==UINT32_MAX?0.f:p[s.at[3]];
        // Keep the original two-stage operation order; composing weights into
        // one dot product would change rounding near the classification gate.
        const float a=p0+(p1-p0)*s.fx,b=p2+(p3-p2)*s.fx;
        return a+(b-a)*s.fy;
    }
  public:
    bool enabled() const {return !samples_.empty();}
    bool remap(const float *lane,const float *road,int w,int h,float *dst_lane,uint8_t *green) const {
        if(!enabled()||!lane||!road||w!=width_||h!=height_)return false;
        const size_t n=(size_t)w*h;
        for(size_t i=0;i<samples_.size();++i){const auto &s=samples_[i];
            dst_lane[i]=sample(lane,s);green[i]=sample(road+n,s)>sample(road,s);}
        return true;
    }
};
}
#endif
