#ifndef TRAFFIC_SIGNAL_GEOMETRY_H
#define TRAFFIC_SIGNAL_GEOMETRY_H
#include "traffic_vision.h"
namespace traffic {
// Strong semantic proposals can have a washed-out shell. Retain a contrast
// check on BOTH sides and on an unlit end, instead of accepting color alone.
inline bool exposed_signal_housing(const uint8_t*rgb,int w,int h,int stride,Box b){
    if(!rgb||stride<w*3||!std::isfinite(b.x+b.y+b.w+b.h+b.score)||b.score<.30f)return false;
    b=clipped(b,w,h);int x0=(int)b.x,y0=(int)b.y,bw=(int)b.w,bh=(int)b.h;
    if(bw<5||bh<9||bh<bw*1.5||bh>bw*5.5||x0<2||x0+bw>w-2||y0<2||y0+bh>h-2)return false;
    unsigned peak=0,core=0;for(int y=0;y<bh;++y)for(int x=0;x<bw;++x){const auto*p=rgb+(y+y0)*stride+(x+x0)*3;unsigned hi=std::max(p[0],std::max(p[1],p[2])),lo=std::min(p[0],std::min(p[1],p[2]));peak=std::max(peak,hi);core+=hi>=235&&lo*100>=hi*72;}
    if(core<3)return false;
    const unsigned limit=std::min(180u,peak*80/100);
    const int inset=bw/5,cap=std::max(2,bh/4);
    unsigned dark=0,sides[2]={},ends[2]={};
    for(int y=0;y<bh;++y)for(int x=0;x<bw;++x){const auto*p=rgb+(y+y0)*stride+(x+x0)*3;bool dim=std::max(p[0],std::max(p[1],p[2]))<(int)limit;dark+=dim;if(x==(int)inset)sides[0]+=dim;if(x==bw-1-(int)inset)sides[1]+=dim;if(y<cap)ends[0]+=dim;if(y>=bh-cap)ends[1]+=dim;}
    return dark*100>=(unsigned)bw*bh*20&&sides[0]*100>=(unsigned)bh*10&&sides[1]*100>=(unsigned)bh*10&&std::max(ends[0],ends[1])*100>=(unsigned)bw*cap*35;
}
inline Box signal_variant(Box b,float sx,float sy,float dx){return Box(b.x+b.w*(1-sx)*.5f+b.w*dx,b.y+b.h*(1-sy)*.5f,b.w*sx,b.h*sy,b.score);}
// Detector class/score admission precedes this bounded alignment. Never search
// the whole picture for colored objects, or move a proposal to a different lamp.
inline bool refine_signal_box(const uint8_t*rgb,int w,int h,int stride,Box b,Box&out){
    if(signal_housing(rgb,w,h,stride,b)){out=b;return true;}
    float best=1e9;bool found=false;
    for(float sx:{.8f,1.f,1.2f})for(float sy:{1.f,1.25f})for(float dx:{-.15f,0.f,.15f}){
        float cost=std::abs(sx-1)+std::abs(sy-1)+std::abs(dx);if(cost>.401f||cost>=best)continue;
        Box q=clipped(signal_variant(b,sx,sy,dx),w,h);
        if(signal_housing(rgb,w,h,stride,q)){out=q;best=cost;found=true;}
    }
    if(found)return true;
    if(exposed_signal_housing(rgb,w,h,stride,b)){out=b;return true;}
    return false;
}
inline bool signal_frame_housing(const uint8_t*rgb,int w,int h,int stride,const Box&b){return signal_housing(rgb,w,h,stride,b)||exposed_signal_housing(rgb,w,h,stride,b);}
inline Color classify_signal_color(const uint8_t*rgb,int w,int h,int stride,Box b,bool structural=false){
    Color c=classify_color(rgb,w,h,stride,b,structural);
    if(c.state||c.reason==TRAFFIC_AMBIGUOUS||structural||!exposed_signal_housing(rgb,w,h,stride,b))return c;
    // Padding changes the local context, not HSV/area/contrast thresholds. The
    // unpadded semantic box above must independently pass the shell evidence.
    float best=1e9;Color chosen=c;
    for(float sx:{1.f,1.2f,1.5f})for(float sy:{1.f,1.25f,1.5f})for(float dx:{-.15f,0.f,.15f}){
        float cost=std::abs(sx-1)+std::abs(sy-1)+std::abs(dx);if(cost>=best)continue;
        Color q=classify_color(rgb,w,h,stride,clipped(signal_variant(b,sx,sy,dx),w,h));
        if(q.state){best=cost;chosen=q;}
    }
    return chosen;
}
inline bool same_signal_bar(const Box&a,const Box&b){
    return a.h>=a.w*1.4f&&b.h>=b.w*1.4f&&std::max(a.h,b.h)<=2*std::min(a.h,b.h)&&
        std::abs(a.y+a.h*.5f-b.y-b.h*.5f)<=std::max(a.h,b.h)*.45f&&
        std::abs(a.x+a.w*.5f-b.x-b.w*.5f)>std::max(a.w,b.w);
}
inline float signal_relevance(const Box&b,float reference){
    const float dx=std::abs(b.x+b.w*.5f-reference)/320.f;
    const float vertical=std::max(0.f,std::min(1.f,(b.y+b.h*.5f)/240.f));
    return b.score/(1+2*dx+2*vertical);
}
inline unsigned next_signal_window(unsigned seen,unsigned cursor,const std::array<float,3>&merits,unsigned preferred){
    if(!seen)return preferred%3;
    if(seen!=7){for(unsigned n=0;n<3;++n){unsigned k=(cursor+n)%3;if(!(seen&(1u<<k)))return k;}}
    unsigned index=(unsigned)(std::max_element(merits.begin(),merits.end())-merits.begin());
    if(merits[index]<=0)return cursor%3;
    return cursor%9==8?(index+1+(cursor/9)%2)%3:index;
}
inline uint64_t signal_scout_delay(const std::array<float,3>&merits,unsigned seen){return seen!=7&&*std::max_element(merits.begin(),merits.end())<=0?100000000ULL:600000000ULL;}
}
#endif
