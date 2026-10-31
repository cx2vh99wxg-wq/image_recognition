#ifndef M_LANE_CURVE_GEOMETRY_H
#define M_LANE_CURVE_GEOMETRY_H
#include "line_style.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace m_lane {
/* Candidate image-heading estimate, not a calibrated road or wheel angle.
 * Uses the already selected ego corridor. It neither edits the segmentation
 * nor changes solid/dashed decisions. Unknown geometry has confidence zero. */
struct EgoCurve {
    uint32_t type=0,confidence=0,angle=0;
    float shift_px=0,span_px=0;
    unsigned reason=1;
};
inline double curve_x(const YOLOLineBoundary& b,double y,double height) {
    double t=(y-height*.95)/(height*.52);
    return b.fit_a+b.fit_b*t+b.fit_c*t*t;
}
inline EgoCurve estimate_ego_curve(const YOLOLineStyleResult& r) {
    EgoCurve out;
    if(r.width<80||r.height<60||!r.road_area||r.object_count<2)return out;
    const auto& l=r.left;const auto& q=r.right;
    if(l.last_y<=l.first_y||q.last_y<=q.first_y||l.last_y>=r.height||q.last_y>=r.height)return out;
    double first=std::max(l.first_y,q.first_y),last=std::min(l.last_y,q.last_y);
    double span=last-first;out.span_px=(float)span;
    if(span<std::max(24.,r.height*.10)){out.reason=2;return out;}
    double centers[5],widths[5];
    for(int i=0;i<5;++i){
        double y=first+span*i/4.,a=curve_x(l,y,r.height),b=curve_x(q,y,r.height);
        if(!std::isfinite(a)||!std::isfinite(b)||a<-.15*r.width||b>1.15*r.width||
           b-a<r.width*.025||b-a>r.width*1.1){out.reason=3;return out;}
        centers[i]=(a+b)*.5;widths[i]=b-a;
    }
    if(widths[0]>widths[4]*1.05+2){out.reason=3;return out;}
    double shift=centers[0]-centers[4];out.shift_px=(float)shift;
    double threshold=std::max(r.width*.02,span*.10);
    /* A corridor that first bends strongly one way then the other is not
     * assigned a confident single turn. No far-row/near-row track swapping. */
    double a=centers[0]-centers[2],b=centers[2]-centers[4];
    if(a*b<0&&std::min(std::abs(a),std::abs(b))>threshold){out.reason=4;return out;}
    if(std::abs(shift)>=threshold){out.type=shift<0?1:2;out.angle=25;}
    /* The complete small-heading band is straight. An extra inner half-band
     * rejected a real straight sample at 3.7px and repeatedly reset temporal
     * confirmation. Three unique frames still confirm every transition. */
    out.confidence=span>=r.height*.30?85:span>=r.height*.20?75:60;
    out.reason=0;return out;
}
class EgoCurveConfirm {
    uint64_t last_id_=0,last_capture_=0;
    unsigned candidate_=0,count_=0;
public:
    EgoCurve update(EgoCurve raw,uint64_t id,uint64_t captured) {
        if(!captured||id==last_id_||captured<=last_capture_){raw.confidence=0;raw.reason=5;return raw;}
        if(last_capture_&&captured-last_capture_>500000000ULL)count_=0;
        last_id_=id;last_capture_=captured;
        if(!raw.confidence){count_=0;return raw;}
        if(!count_||candidate_!=raw.type){candidate_=raw.type;count_=1;}
        else if(count_<3)++count_;
        if(count_<3){raw.confidence=0;raw.reason=6;}
        return raw;
    }
};
}
#endif
