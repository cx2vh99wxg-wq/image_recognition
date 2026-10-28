#ifndef TRAFFIC_VISION_H
#define TRAFFIC_VISION_H
#include "traffic_light.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <vector>
namespace traffic {
enum class PixelOrder { RGB, BGR };
struct InputView {
    int x=0,y=0;PixelOrder order=PixelOrder::RGB;
    InputView()=default;
    InputView(int left,int top,PixelOrder format):x(left),y(top),order(format){}
};
// The established lane/video buffers keep their historic byte order. Only
// this private traffic copy is normalized, matching S X11's BGR565 decoder.
inline bool copy_camera_rgb(const uint8_t* source,int width,int height,int stride,
                            const InputView& view,std::vector<uint8_t>& out){
    if(!source||view.x<0||view.y<0||width<320||height<240||
       view.x>width-320||view.y>height-240||stride<width*3)return false;
    out.resize(320*240*3);
    for(int y=0;y<240;++y){
        const uint8_t* row=source+(y+view.y)*stride+view.x*3;
        uint8_t* dest=out.data()+y*320*3;
        if(view.order==PixelOrder::RGB)std::memcpy(dest,row,320*3);
        else for(int x=0;x<320;++x){dest[x*3]=row[x*3+2];dest[x*3+1]=row[x*3+1];dest[x*3+2]=row[x*3];}
    }
    return true;
}
struct Box {float x=0,y=0,w=0,h=0,score=0;Box()=default;Box(float a,float b,float c,float d,float s):x(a),y(b),w(c),h(d),score(s){}};
struct Color {unsigned state=0,quality=0,reason=TRAFFIC_NO_COLOR,pixels=0;};
inline float overlap(const Box& a,const Box& b){float x=std::max(0.f,std::min(a.x+a.w,b.x+b.w)-std::max(a.x,b.x)),y=std::max(0.f,std::min(a.y+a.h,b.y+b.h)-std::max(a.y,b.y));float i=x*y;return i/std::max(1.f,a.w*a.h+b.w*b.h-i);}
inline Box clipped(Box b,int w,int h){float r=std::max(0.f,std::min(float(w),b.x+b.w)),d=std::max(0.f,std::min(float(h),b.y+b.h));b.x=std::max(0.f,std::min(float(w),b.x));b.y=std::max(0.f,std::min(float(h),b.y));b.w=r-b.x;b.h=d-b.y;return b;}
inline int luminance(const uint8_t* p){return (p[0]+2*p[1]+p[2])/4;}
/* RGB only: masks, dashboard pixels and already annotated video are invalid
 * inputs. Hue is in degrees here, not OpenCV's half-degree convention. */
inline Color classify_color(const uint8_t* rgb,int w,int h,int stride,Box box){
    Color result;box=clipped(box,w,h);int x0=(int)std::floor(box.x),y0=(int)std::floor(box.y),bw=(int)box.w,bh=(int)box.h;
    if(!rgb||bw<4||bh<5){result.reason=TRAFFIC_SMALL;return result;}
    // The stitched M input has a narrow invalid seam at each tile edge.
    // Ignore edge-only fragments, including when a broad NN box contains
    // them. Do not alter the source image or the lane/video pipeline.
    const int edge=w>=320?8:0;
    if(box.x+box.w*.5f<edge||box.x+box.w*.5f>w-edge){result.reason=TRAFFIC_SMALL;return result;}
    std::array<unsigned,256> histogram={};int peak=0;
    for(int y=0;y<bh;++y)for(int x=0;x<bw;++x){const uint8_t* p=rgb+(y+y0)*stride+(x+x0)*3;int v=std::max(p[0],std::max(p[1],p[2]));++histogram[v];peak=std::max(peak,v);}
    int dark=0,total=bw*bh,acc=0;for(;dark<255;++dark){acc+=histogram[dark];if(acc>=std::max(1,total/10))break;}
    const int minimum=std::max(90,(int)(peak*.55f));
    std::vector<uint8_t> mask((size_t)bw*bh,0),seen(mask.size(),0);
    for(int y=0;y<bh;++y)for(int x=0;x<bw;++x){
        if(x+x0<edge||x+x0>=w-edge)continue;
        const uint8_t* p=rgb+(y+y0)*stride+(x+x0)*3;int hi=std::max(p[0],std::max(p[1],p[2])),lo=std::min(p[0],std::min(p[1],p[2])),delta=hi-lo;
        if(hi<minimum||delta<hi*.35f||hi-dark<25)continue;
        float hue=hi==p[0]?60.f*(int(p[1])-p[2])/delta:hi==p[1]?120.f+60.f*(int(p[2])-p[0])/delta:240.f+60.f*(int(p[0])-p[1])/delta;
        if(hue<0)hue+=360;
        unsigned state=hue<=25||hue>=335?TRAFFIC_RED:hue>=32&&hue<75?TRAFFIC_YELLOW:hue>=75&&hue<=190?TRAFFIC_GREEN:TRAFFIC_UNKNOWN;
        mask[y*bw+x]=(uint8_t)state;
    }
    double scores[4]={};unsigned areas[4]={};std::vector<int> queue;queue.reserve(mask.size());
    for(int sy=0;sy<bh;++sy)for(int sx=0;sx<bw;++sx){
        int start=sy*bw+sx;unsigned state=mask[start];if(!state||seen[start])continue;
        queue.clear();queue.push_back(start);seen[start]=1;double strength=0,cx=0,cy=0,value=0;int minx=sx,maxx=sx,miny=sy,maxy=sy;
        for(size_t k=0;k<queue.size();++k){int at=queue[k],x=at%bw,y=at/bw;const uint8_t* p=rgb+(y+y0)*stride+(x+x0)*3;int hi=std::max(p[0],std::max(p[1],p[2])),lo=std::min(p[0],std::min(p[1],p[2]));strength+=(hi-lo)/255.;cx+=x;cy+=y;value+=hi;minx=std::min(minx,x);maxx=std::max(maxx,x);miny=std::min(miny,y);maxy=std::max(maxy,y);
            const int dx[]={-1,1,0,0},dy[]={0,0,-1,1};for(int j=0;j<4;++j){int nx=x+dx[j],ny=y+dy[j];if(nx>=0&&nx<bw&&ny>=0&&ny<bh){int next=ny*bw+nx;if(!seen[next]&&mask[next]==state){seen[next]=1;queue.push_back(next);}}}}
        if(queue.size()<(size_t)std::max(3,total/400)||queue.size()>total*.88)continue;
        // A bright colored patch on a bright blue sign is not a lit lamp.
        // Require local dark housing around the component. HSV value is
        // used here because a blue panel can have low luminance yet be bright.
        int radius=std::max(2,(std::max(maxx-minx+1,maxy-miny+1)+3)/4);
        // Allow reflective gray housings while demanding at least 25%
        // contrast to a lit component; bright colored panels still fail.
        int ring=0,dark_ring=0,dark_limit=std::max(70,(int)(value/queue.size()*.75));
        for(int y=std::max(0,miny-radius);y<=std::min(bh-1,maxy+radius);++y)
            for(int x=std::max(0,minx-radius);x<=std::min(bw-1,maxx+radius);++x){
                if(x>=minx&&x<=maxx&&y>=miny&&y<=maxy)continue;
                const uint8_t* p=rgb+(y+y0)*stride+(x+x0)*3;
                ++ring;if(std::max(p[0],std::max(p[1],p[2]))<dark_limit)++dark_ring;
            }
        if(ring<3||dark_ring*100<ring*30)continue;
        cx/=queue.size()*bw;cy/=queue.size()*bh;
        // Apply lamp order only when the housing clearly has that shape.
        if(bh>bw*1.5&&((state==TRAFFIC_RED&&cy>.72)||(state==TRAFFIC_GREEN&&cy<.28)))continue;
        if(bw>bh*1.8&&((state==TRAFFIC_RED&&cx>.72)||(state==TRAFFIC_GREEN&&cx<.28)))continue;
        if(strength>scores[state]){scores[state]=strength;areas[state]=(unsigned)queue.size();}
    }
    unsigned best=0;double second=0;for(unsigned k=1;k<=3;++k)if(scores[k]>scores[best])best=k;
    if(!best||scores[best]<=0)return result;
    for(unsigned k=1;k<=3;++k)if(k!=best)second=std::max(second,scores[k]);
    if(second>scores[best]*.45){result.reason=TRAFFIC_AMBIGUOUS;return result;}
    result.state=best;result.pixels=areas[best];result.reason=TRAFFIC_OK;
    result.quality=std::min(95u,65u+(unsigned)(20*(1-second/scores[best]))+std::min(10u,areas[best]/4));return result;
}
/* Track the housing's perimeter. The luminous center is deliberately omitted
 * so a red->green transition is not interpreted as a different object. */
class HousingTracker {
    static const int NX=12,NY=20;
    Box box_={};std::array<float,NX*NY> template_={};bool valid_=false;
    static std::array<float,NX*NY> sample(const uint8_t* rgb,int stride,Box b){
        std::array<float,NX*NY> a={};for(int y=0;y<NY;++y)for(int x=0;x<NX;++x){int px=(int)(b.x+(x+.5)*b.w/NX),py=(int)(b.y+(y+.5)*b.h/NY);a[y*NX+x]=(float)luminance(rgb+py*stride+px*3);}return a;
    }
    static float difference(const std::array<float,NX*NY>& a,const std::array<float,NX*NY>& b){
        float ma=0,mb=0;unsigned n=0;for(int y=0;y<NY;++y)for(int x=0;x<NX;++x)if(x<3||x>=NX-3||y<3||y>=NY-3){ma+=a[y*NX+x];mb+=b[y*NX+x];++n;}
        ma/=n;mb/=n;float error=0;for(int y=0;y<NY;++y)for(int x=0;x<NX;++x)if(x<3||x>=NX-3||y<3||y>=NY-3)error+=std::abs((a[y*NX+x]-ma)-(b[y*NX+x]-mb));return error/n+std::abs(ma-mb)*.2f;
    }
public:
    void reset(){valid_=false;}
    void start(const uint8_t* rgb,int w,int h,int stride,Box b){box_=clipped(b,w,h);valid_=rgb&&box_.w>=4&&box_.h>=5;if(valid_)template_=sample(rgb,stride,box_);}
    bool update(const uint8_t* rgb,int w,int h,int stride,Box anchor,Box& out){
        if(!valid_||!rgb)return false;
        int radius=std::max(2,std::min(10,(int)(box_.w/4)));radius=radius/2*2;
        float best=1e9;Box selected=box_;std::array<float,NX*NY> chosen={};
        for(int dy=-radius;dy<=radius;dy+=2)for(int dx=-radius;dx<=radius;dx+=2){Box b=box_;b.x+=dx;b.y+=dy;if(b.x<0||b.y<0||b.x+b.w>w||b.y+b.h>h)continue;
            if(std::abs(b.x-anchor.x)>std::max(12.f,anchor.w*.35f)||std::abs(b.y-anchor.y)>std::max(12.f,anchor.h*.25f))continue;
            auto a=sample(rgb,stride,b);float score=difference(a,template_)+.15f*(std::abs(dx)+std::abs(dy));if(score<best){best=score;selected=b;chosen=a;}}
        if(best>28){valid_=false;return false;}box_=selected;for(size_t i=0;i<template_.size();++i)template_[i]=.9f*template_[i]+.1f*chosen[i];out=box_;return true;
    }
};
class ColorConfirmation {
    uint64_t last_id_=0,last_capture_=0;unsigned candidate_=0,count_=0;
public:
    void reset(){last_id_=last_capture_=0;candidate_=count_=0;}
    unsigned observe(unsigned state,uint64_t id,uint64_t captured){
        if(!captured||captured<=last_capture_||id==last_id_)return TRAFFIC_UNKNOWN;
        if(last_capture_&&captured-last_capture_>350000000ULL)candidate_=count_=0;
        last_id_=id;last_capture_=captured;if(!state){candidate_=count_=0;return 0;}
        if(candidate_!=state){candidate_=state;count_=0;}if(count_<3)++count_;return count_>=3?candidate_:unsigned(TRAFFIC_UNKNOWN);
    }
};
/* Every NPU job is submitted by the lane worker. This policy decides whether
 * the optional detector may use the gap after a published lane result. */
struct LaneBudget {uint64_t source=0,captured=0,completed=0,duration=0;bool fresh=false,curve_active=false;float reference_x=160;};
class NpuTask {public:virtual ~NpuTask(){};virtual void after_lane(const LaneBudget& lane)=0;};
class Admission {
    uint64_t due_=0,estimate_=30000000ULL;unsigned violations_=0;
public:
    bool allow(const LaneBudget& lane,uint64_t now){return violations_<3&&lane.fresh&&!lane.curve_active&&lane.captured&&now>=lane.captured&&now-lane.captured<=300000000ULL&&lane.duration<=350000000ULL&&now>=due_&&estimate_<=60000000ULL;}
    void completed(uint64_t now,uint64_t duration){estimate_=std::max(duration,(estimate_*3+duration)/4);due_=now+1000000000ULL;if(duration>60000000ULL){++violations_;due_=now+10000000000ULL;estimate_=30000000ULL;}else violations_=0;}
    unsigned violations()const{return violations_;}
};
}
#endif
