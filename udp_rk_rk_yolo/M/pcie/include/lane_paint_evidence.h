#ifndef M_LANE_PAINT_EVIDENCE_H
#define M_LANE_PAINT_EVIDENCE_H
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace m_lane {
enum LanePaintObservation { PAINT_WEAK=-2, PAINT_MISSING=-1, PAINT_GAP=0, PAINT_PRESENT=1 };
// Style evidence only. This never moves the steering-selected observations.
// Input is the unannotated BGR bird's-eye image and its geometric coverage.
class LanePaintEvidence {
    const uint8_t *image_, *coverage_;
    int width_, height_;
    struct Pixel { int b, g, r, value; };
    bool read(double x, double y, Pixel &p) const {
        const int ix=(int)std::lround(x), iy=(int)std::lround(y);
        if(ix<0 || ix>=width_ || iy<0 || iy>=height_ ||
           (coverage_ && !coverage_[iy*width_+ix])) return false;
        const auto *v=image_+((size_t)iy*width_+ix)*3;
        p={v[0],v[1],v[2],(v[0]+2*v[1]+v[2])/4};
        return true;
    }
    static bool yellow(const Pixel &p) {
        return p.g>=12 && p.r>=p.g*65/100 && p.b*4<std::min(p.r,p.g)*3;
    }
    static bool paint_color(const Pixel &p) {
        const int hi=std::max(p.b,std::max(p.g,p.r));
        const int lo=std::min(p.b,std::min(p.g,p.r));
        // Relative chroma tolerates a white line in shadow. Cyan/blue objects
        // are not yellow paint: the byte order is explicitly BGR here.
        const bool white=lo>=10 && hi-lo<=std::max(18,hi*45/100);
        return white || yellow(p);
    }
    static int median(int a,int b,int c) {
        return std::max(std::min(a,b),std::min(std::max(a,b),c));
    }
  public:
    LanePaintEvidence(const uint8_t *image,int width,int height,const uint8_t *coverage)
        :image_(image),coverage_(coverage),width_(width),height_(height){}

    // Two-sided ridge strength for checking long, coherent troughs in the
    // paint. A negative value is unmapped, never dark road.
    int ridge_strength(double x, double y, double slope) const
    {
        if(!image_ || width_<1 || height_<1)return -1;
        const double nx=1./std::sqrt(1.+slope*slope),ny=-slope*nx;
        int strength=0,supported=0;
        for(int offset=-4;offset<=4;++offset) {
            const double px=x+offset;
            Pixel peak,side[2][3];
            if(!read(px,y,peak))continue;
            bool visible=true;
            for(int sign=0;sign<2;++sign)for(int i=0;i<3;++i) {
                const int d=(sign?1:-1)*(6+2*i);
                visible &= read(px+d*nx,y+d*ny,side[sign][i]);
            }
            if(!visible)continue;
            ++supported;
            if(!paint_color(peak))continue;
            const int left=median(side[0][0].value,side[0][1].value,side[0][2].value);
            const int right=median(side[1][0].value,side[1][1].value,side[1][2].value);
            const int background=std::max(left,right);
            const int contrast=std::max(0,peak.value-background);
            strength=std::max(strength,std::min(255,std::max(contrast,48*contrast/(background+8))));
        }
        return supported>=5?strength:-1;
    }

    // Weak contrast is separate from missing geometric image coverage. Only
    // the former may be resolved by neighboring, directly observed gap rows.
    // Search a cross-line band anchored to this row; sample its background
    // normal to the polyline. Borrowing paint from neighboring rows would
    // incorrectly fill short genuine dash gaps on diagonal boundaries.
    int sample(double x,double y,double slope,int mask_width) const {
        if(!image_ || width_<1 || height_<1) return -1;
        Pixel center;
        if(!read(x,y,center)) return -1;
        const double nx=1./std::sqrt(1.+slope*slope),ny=-slope*nx;
        const double tx=-ny,ty=nx;
        const int search=std::max(4,std::min(6,mask_width/2+3));
        const int distance=std::max(6,std::min(12,mask_width/2+4));
        bool weak=false;unsigned supported=0;
        for(int offset=-search;offset<=search;++offset) {
            const double px=x+offset,py=y;
            Pixel peak,side[2][3];
            if(!read(px,py,peak))continue;
            // A bright shoulder/guardrail or an illumination gradient can be
            // brighter than the paint at the distant background samples.
            // First test a narrow, two-sided local ridge. A flat bright road
            // beside a dark object fails the higher-of-both-flanks condition.
            const int near_distance=std::max(2,std::min(4,mask_width/2+1));
            Pixel near[2][3];bool near_complete=true;
            for(int sign=0;sign<2;++sign)for(int i=0;i<3;++i){
                const int d=(sign?1:-1)*(near_distance+i);
                if(!read(px+d*nx,py+d*ny,near[sign][i]))near_complete=false;
            }
            if(near_complete && paint_color(peak)){
                const int left=median(near[0][0].value,near[0][1].value,near[0][2].value);
                const int right=median(near[1][0].value,near[1][1].value,near[1][2].value);
                const int average=(left+right)/2;
                const int local_threshold=std::max(5,average/16),margin=std::max(3,std::max(left,right)/64);
                if(peak.value>=std::max(left,right)+margin && peak.value>=average+local_threshold){
                    unsigned support=1;
                    for(int sign=-1;sign<=1;sign+=2){
                        Pixel along;
                        if(read(px+sign*tx,py+sign*ty,along) && paint_color(along) &&
                           along.value>=average+local_threshold)++support;
                    }
                    if(support>=2)return 1;
                    weak=true;
                }
            }
            bool complete[2]={true,true};
            for(int sign=0;sign<2;++sign) for(int i=0;i<3;++i) {
                const int d=(sign?1:-1)*(distance+2*i);
                if(!read(px+d*nx,py+d*ny,side[sign][i]))complete[sign]=false;
            }
            if(!complete[0] && !complete[1])continue;
            const bool both=complete[0] && complete[1];
            // At an IPM coverage edge, the visible side can still establish
            // strong paint contrast. It cannot establish a dark dash gap:
            // only fully observed two-sided samples count toward `supported`.
            supported+=both;
            int background=0,yellow_background=0;
            for(int sign=0;sign<2;++sign)if(complete[sign]){
                background=std::max(background,median(side[sign][0].value,side[sign][1].value,side[sign][2].value));
                yellow_background=std::max(yellow_background,
                    median(std::min(side[sign][0].g,side[sign][0].r),std::min(side[sign][1].g,side[sign][1].r),std::min(side[sign][2].g,side[sign][2].r)));
            }
            const auto contrast_of=[&](const Pixel &p){
                return yellow(p)?std::max(p.value-background,std::min(p.g,p.r)-yellow_background):p.value-background;
            };
            const int contrast=contrast_of(peak),threshold=std::max(5,background/8)*(both?1:2);
            if(!paint_color(peak) || contrast<std::max(2,threshold/2))continue;
            unsigned support=1;
            for(int sign=-1;sign<=1;sign+=2) {
                Pixel along;
                if(read(px+sign*tx,py+sign*ty,along) && paint_color(along) &&
                   contrast_of(along)>=threshold) ++support;
            }
            if(contrast>=threshold && support>=2) return 1;
            weak=true;
        }
        if(supported<(unsigned)(search+1))return PAINT_MISSING;
        return weak?PAINT_WEAK:PAINT_GAP;
    }
};
}
#endif
