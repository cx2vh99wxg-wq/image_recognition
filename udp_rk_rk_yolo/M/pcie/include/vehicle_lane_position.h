#ifndef M_VEHICLE_LANE_POSITION_H
#define M_VEHICLE_LANE_POSITION_H
#include "lane_point_selector.h"

namespace m_lane {
// Perspective fallback for separated dashes. Votes describe a longitudinal
// marking by slope and intercept at a near road row. Only mask observations
// from the same detector frame contribute. The regular point tracks take
// precedence wherever both were actually measured.
struct MarkingFit {
    double x=0, slope=0;
    int top=240, bottom=0, support=0;
    double at(int y) const { return x+slope*(y-195); }
    bool covers(int y) const { return support>=7 && y>=top-10 && y<=bottom+16; }
};

inline std::array<MarkingFit,2> fit_vehicle_markings(const float *mask)
{
    std::array<MarkingFit,2> best{};
    if(!mask) return best;
    const int first=106, end=225;
    const auto rows=lane_mask_rows(mask,320,240,first,end);
    for(int side=0;side<2;++side) {
        double best_score=0.;
        for(int slope_index=1;slope_index<=34;++slope_index) {
            const double slope=(side?1:-1)*slope_index*.10;
            std::array<unsigned,80> votes{};
            for(int y=first;y<end;y+=3) {
                int previous=-1;
                for(const auto &p:rows[y-first]) {
                    if(p.width>15) continue;
                    const int bin=int(std::floor((p.x-slope*(y-195)+80.)/6.));
                    if(bin<0 || bin>=80 || bin==previous) continue;
                    ++votes[bin];previous=bin;
                }
            }
            for(int bin=1;bin<79;++bin) {
                if(votes[bin]<5 || votes[bin]<votes[bin-1] || votes[bin]<votes[bin+1])continue;
                const double near_x=bin*6.-77.;
                const double offset=(near_x-159.5)*(side?1:-1);
                if(offset<10 || offset>195)continue;
                MarkingFit fit;fit.slope=slope;fit.x=near_x;
                double intercept_sum=0.;
                for(int y=first;y<end;y+=3) {
                    const double expected=fit.at(y);
                    const LaneRowPoint *closest=nullptr;
                    for(const auto &point:rows[y-first]) {
                        if(point.width<=15 && std::abs(point.x-expected)<=4. &&
                           (!closest || std::abs(point.x-expected)<std::abs(closest->x-expected)))
                            closest=&point;
                    }
                    if(!closest)continue;
                    ++fit.support;fit.top=std::min(fit.top,y);fit.bottom=y;
                    intercept_sum+=closest->x-slope*(y-195);
                }
                if(fit.support<7 || fit.bottom-fit.top<24 || fit.bottom<157)continue;
                fit.x=intercept_sum/fit.support;
                const double score=(fit.support+.08*(fit.bottom-fit.top))/(1.+offset/100.);
                if(score>best_score){best_score=score;best[side]=fit;}
            }
        }
    }
    return best;
}
inline bool fit_at_foot(const std::array<MarkingFit,2> &fit,int y,double &left,double &right)
{
    if(!fit[0].covers(y) || !fit[1].covers(y))return false;
    const double width=fit[1].x-fit[0].x, opening=fit[1].slope-fit[0].slope;
    if(width<36 || opening<.25)return false;
    const double horizon=195-width/opening, vanish_x=fit[0].x+fit[0].slope*(horizon-195);
    if(horizon<55 || horizon>168 || vanish_x<65 || vanish_x>255)return false;
    left=fit[0].at(y);right=fit[1].at(y);return right-left>=18;
}
}
#endif
