#ifndef PERSON_ROAD_ASSOCIATION_H
#define PERSON_ROAD_ASSOCIATION_H
#include "traffic_detector.h"
#include "front_person.h"
namespace traffic {
enum class FootRelation { Outside, Inside, Unknown };
// Classify feet in corrected camera coordinates, never the colored display
// or the bird's-eye mask. Work is one 320x240 mask scan plus tiny foot patches.
class PersonRoadAssociation {
    std::vector<uint8_t> road_;
    unsigned pixels_ = 0;
    bool valid_ = false;
    unsigned patch(int x0, int y0, int x1, int y1) const {
        unsigned n = 0;
        for (int y=y0; y<=y1; ++y)
            for (int x=x0; x<=x1; ++x) n += road_[y*320+x];
        return n;
    }
  public:
    bool prepare(const FrontRoadFrame &f, uint64_t id, uint64_t captured) {
        valid_=false; pixels_=0;
        if (!f.matches(id,captured)) return false;
        road_.resize(320*240);
        for (unsigned i=0; i<road_.size(); ++i) {
            const float a=f.road[i], b=f.road[road_.size()+i];
            if (!std::isfinite(a) || !std::isfinite(b)) { pixels_=0; return false; }
            road_[i]=b>a; pixels_+=road_[i];
        }
        valid_=pixels_>=64;
        return valid_;
    }
    unsigned pixels() const { return pixels_; }
    FootRelation classify(const Box &b) const {
        if (!valid_ || !std::isfinite(b.x) || !std::isfinite(b.y) ||
            !std::isfinite(b.w) || !std::isfinite(b.h) || b.w<3 || b.h<8 ||
            b.x<0 || b.y<0 || b.x+b.w>320 || b.y+b.h>240)
            return FootRelation::Unknown;
        const int x=(int)std::floor(b.x+b.w*.5f);
        // The body occludes the road: inspect the ground immediately below feet.
        const int y=(int)std::ceil(b.y+b.h);
        const int half=std::max(2,std::min(8,(int)std::lround(b.w*.15f)));
        // Truncated boxes / incomplete ground support cannot prove OUTSIDE.
        if (x-half<0 || x+half>=320 || y<0 || y+5>=240)
            return FootRelation::Unknown;
        const unsigned hits=patch(x-half,y,x+half,y+5);
        const unsigned area=(2*half+1)*6;
        if (hits*5>=area) return FootRelation::Inside; // >=20% ground support
        const unsigned margin=patch(std::max(0,x-half-3),std::max(0,y-2),
                                    std::min(319,x+half+3),std::min(239,y+8));
        return margin ? FootRelation::Unknown : FootRelation::Outside;
    }
    void annotate(const PersonResult &people, const FrontRoadFrame &f,
                  uint64_t id, uint64_t captured, FrontPersonData &d) {
        const bool road_ok=prepare(f,id,captured);
        d.flags |= FRONT_PERSON_ROI_MODE;
        d.road_source_id=road_ok ? (uint32_t)f.source : 0;
        d.road_pixels=pixels_;
        if (road_ok) d.flags |= FRONT_PERSON_ROAD_VALID;
        if (!(d.flags & FRONT_PERSON_VALID)) return;
        Box chosen; int priority=-1;
        for (const auto &b:people.candidates) {
            const auto relation=classify(b);
            const int rank=relation==FootRelation::Inside ? 2 : relation==FootRelation::Unknown ? 1 : 0;
            if (rank==2) ++d.risk_count;
            else if (rank==1) ++d.unknown_count;
            else ++d.outside_count;
            if (rank>priority || (rank==priority && b.score>chosen.score)) { chosen=b; priority=rank; }
        }
        d.count=d.risk_count+d.outside_count+d.unknown_count;
        if (d.count) {
            d.flags |= FRONT_PERSON_FOUND;
            d.quality=(uint32_t)std::min(100.f,chosen.score*100.f);
            d.x=(uint32_t)chosen.x; d.y=(uint32_t)chosen.y;
            d.width=std::min(320-d.x,std::max(1u,(uint32_t)std::ceil(chosen.w)));
            d.height=std::min(240-d.y,std::max(1u,(uint32_t)std::ceil(chosen.h)));
        }
        if (d.risk_count) d.flags |= FRONT_PERSON_IN_PATH;
        if (d.unknown_count) d.flags |= FRONT_PERSON_PATH_UNKNOWN;
    }
};
}
#endif
