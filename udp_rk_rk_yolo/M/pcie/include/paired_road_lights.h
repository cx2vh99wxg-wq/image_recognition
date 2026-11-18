#ifndef M_PAIRED_ROAD_LIGHTS_H
#define M_PAIRED_ROAD_LIGHTS_H
#include "traffic_vision.h"

namespace traffic {
// Distant horizontal signal heads may each occupy only a few scan lines.
// Require a matching second illuminated housing on the same overhead bar.
struct RoadLight {
    Box box;
    unsigned state=0;
    int x=0,y=0,partner_x=-1,partner_y=-1;
    double radius=0;
    int chroma=0;
};

inline unsigned lamp_chroma(const uint8_t *p)
{
    const int r=p[0], g=p[1], b=p[2];
    if(r>=90 && r>=g+25 && r>=b+9) return TRAFFIC_RED;
    if(g>=112 && g>=r+35 && b>=r+19 && g*100>=b*94) return TRAFFIC_GREEN;
    return 0;
}

inline bool same_road_head(const RoadLight &a,const RoadLight &b)
{
    const auto close=[&](int x,int y){
        return std::abs(x-b.x)<=std::max(5.,3.*a.radius) && std::abs(y-b.y)<=std::max(4.,2.5*a.radius);
    };
    return a.state && a.state==b.state && (close(a.x,a.y) || (a.partner_x>=0 && close(a.partner_x,a.partner_y))) &&
           std::max(a.radius,b.radius)<=2.*std::min(a.radius,b.radius);
}

inline RoadLight paired_road_light(const uint8_t *rgb,int w,int h,int stride,
                                   const RoadLight *anchor=nullptr,bool *saw_pair=nullptr)
{
    RoadLight result;
    if(saw_pair)*saw_pair=false;
    if(!rgb || (w!=320 && w!=640) || h*4!=w*3 || stride<w*3) return result;
    const int bottom=h*63/100;
    std::vector<uint8_t> mask(w*bottom);
    for(int y=2;y<bottom;++y)
        for(int x=2;x<w-2;++x) mask[y*w+x]=uint8_t(lamp_chroma(rgb+y*stride+x*3));
    std::vector<RoadLight> heads;
    std::vector<int> component;
    auto lum=[&](int x,int y){return luminance(rgb+y*stride+x*3);};
    for(int at=0;at<w*bottom;++at) {
        const unsigned state=mask[at];
        if(!state) continue;
        mask[at]=0;component.assign(1,at);
        int xmin=at%w,xmax=xmin,ymin=at/w,ymax=ymin,sx=0,sy=0;
        for(size_t k=0;k<component.size();++k) {
            const int a=component[k],x=a%w,y=a/w;
            xmin=std::min(xmin,x);xmax=std::max(xmax,x);
            ymin=std::min(ymin,y);ymax=std::max(ymax,y);sx+=x;sy+=y;
            for(int n:{a-1,a+1,a-w,a+w}) if(n>=0 && n<w*bottom && mask[n]==state) {
                mask[n]=0;component.push_back(n);
            }
        }
        const int bw=xmax-xmin+1,bh=ymax-ymin+1;
        if(bw>14*w/320 || bh>14*w/320 || std::max(bw,bh)>3*std::min(bw,bh)) continue;
        int chroma=0,peak=0;
        for (const int pixel : component) {
            const uint8_t *p=rgb+(pixel/w)*stride+(pixel%w)*3;
            const int c=state==TRAFFIC_RED ? p[0]-std::max(p[1],p[2]) : std::min(p[1],p[2])-p[0];
            if (c>chroma) { chroma=c; peak=state==TRAFFIC_RED?p[0]:p[1]; }
        }
        // Pale cyan window reflections can have the correct hue and housing
        // geometry. Require an emitting core in each candidate green head.
        if (state==TRAFFIC_GREEN && (chroma<58 || peak<164)) continue;
        if (state==TRAFFIC_RED && chroma<30) continue;
        const bool vivid=chroma>=92 && peak>=178;
        const int x=int(std::lround(double(sx)/component.size())), y=int(std::lround(double(sy)/component.size()));
        const double radius=std::max(.85,(bw+bh)*.25);
        const int side=state==TRAFFIC_RED?1:-1;
        // Sample both unlit lamp positions and the space between them. Compare
        // the dark shell with its surroundings above AND below, not the lamp hue.
        for(double factor:{1.,1.45,2.0,2.8,.7}) {
            if (factor<1 && !vivid) continue;
            const double r=radius*factor;
            const int dy=std::max(2,int(std::lround(r*2.25)));
            const int end=x+side*int(std::lround(5.7*r));
            if(end<1 || end>=w-1 || y-dy<1 || y+dy>=bottom) continue;
            int contrast=0, dark=0,inside_sum=0;
            for(double dx:{1.9,2.7,3.6,4.5,5.2}) {
                const int xx=x+side*int(std::lround(dx*r));
                const int center=lum(xx,y);
                inside_sum+=center;
                const int surround=std::min(lum(xx,y-dy),lum(xx,y+dy));
                contrast+=surround-center;
                dark+=center<105 && surround>=center+14;
            }
            if(dark<4 || contrast<138) {
                // Against trees a dark housing may have little external
                // contrast. Verify that both unlit positions are much darker
                // than a vivid core in its own emitting channel.
                if (!vivid) continue;
                int unlit=0;
                for (double dx : {2.5,3.5,4.5,5.2}) {
                    const int xx=x+side*int(std::lround(dx*r));
                    const int channel=rgb[y*stride+3*xx+(state==TRAFFIC_GREEN?1:0)];
                    unlit+=channel+65<=peak && lum(xx,y)<118;
                }
                if (unlit<3) continue;
            }
            // Both ends of a head terminate against the support or background.
            // A strip of tinted window glass on a building has no such end caps.
            const int outside_lamp=x-side*int(std::ceil(r*2.6));
            const int outside_shell=x+side*int(std::ceil(r*8.1));
            if(std::min(outside_lamp,outside_shell)<1 || std::max(outside_lamp,outside_shell)>=w-1)continue;
            if(std::min(lum(outside_lamp,y),lum(outside_shell,y))*5<inside_sum+75)continue;
            RoadLight head;head.x=x;head.y=y;head.radius=r;head.state=state;head.chroma=chroma;
            head.box=clipped(Box(float(std::min(x,end)-r),float(y-1.4*r),
                                 float(std::abs(x-end)+2*r+1),float(2.8*r+1),.70f),w,h);
            if(heads.size()>=128) return {}; // Dense repeating textures cannot establish a unique bar.
            heads.push_back(head);break;
        }
    }
    double best=0;bool conflict=false;
    for(size_t i=0;i<heads.size();++i)for(size_t j=0;j<i;++j) {
        const auto &a=heads[i],&b=heads[j];
        const double r=std::max(a.radius,b.radius), dx=std::abs(a.x-b.x),dy=std::abs(a.y-b.y);
        // An oblique approach tilts both housings and their connecting bar.
        // Require two saturated cores before searching the wider slope range.
        const bool bright_pair = std::min(a.chroma,b.chroma)>=92;
        const double tilt = bright_pair ? .18 : .06;
        if(std::min(a.radius,b.radius)<.62*r || dx<9*r || dx>31*r ||
           dy>std::max(1.5,std::min(r*(bright_pair?3.:1.5),dx*tilt))) continue;
        // An overhead bar has a continuous horizontal edge BETWEEN the two
        // housings. Repeating windows, leaves and colored car trim also produce
        // pairs; agreement on hue and height alone is insufficient.
        const auto &left=a.x<b.x?a:b, &right=a.x<b.x?b:a;
        const double gap_left=left.box.x+left.box.w+1, gap_right=right.box.x-1;
        if(gap_right-gap_left<4)continue;
        const int offset=std::max(2,int(std::ceil(r)));
        bool bar=false;
        for(int shift=-offset;shift<=offset && !bar;++shift){
            int bright=0,shadow=0;
            for(int k=0;k<9;++k){
                const int x=int(std::lround(gap_left+(gap_right-gap_left)*k/8.));
                const double fraction=double(x-left.x)/(right.x-left.x);
                const int y=int(std::lround(left.y+(right.y-left.y)*fraction))+shift;
                if(y-offset<0 || y+offset>=bottom)continue;
                const int delta=2*lum(x,y)-lum(x,y-offset)-lum(x,y+offset);
                bright+=delta>=25;shadow+=delta<=-25;
            }
            bar=std::max(bright,shadow)>=7;
        }
        if(!bar)continue;
        if(a.state!=b.state){conflict=true;continue;}
        const double merit=std::min(a.chroma,b.chroma)*std::sqrt(r)/(1.+dy/r);
        if(result.state && result.state!=a.state)conflict=true;
        if(merit>best){
            const bool choose_a=std::abs(a.x-w/2)<std::abs(b.x-w/2);
            result=choose_a?a:b;
            const auto &other=choose_a?b:a;
            result.partner_x=other.x; result.partner_y=other.y;
            best=merit;
        }
    }
    if(conflict)return {};
    if(result.state){if(saw_pair)*saw_pair=true;return result;}
    if(anchor)for(const auto &head:heads)if(same_road_head(*anchor,head))return head;
    return {};
}

class PairedRoadLightTracker {
    RoadLight previous_;
    uint64_t pair_ns_=0;
    bool confirmed_=false;
public:
    RoadLight observe(const uint8_t *rgb,uint64_t stamp,int width=320) {
        const bool fresh=pair_ns_ && stamp>pair_ns_ && stamp-pair_ns_<=800000000ULL;
        bool pair=false;
        auto head=paired_road_light(rgb,width,width*3/4,width*3,fresh&&confirmed_?&previous_:nullptr,&pair);
        if(pair) {
            confirmed_=fresh && same_road_head(previous_,head);
            previous_=head;pair_ns_=stamp;
        } else if(!fresh) confirmed_=false;
        // A single visible lamp can bridge LED flicker only after two paired
        // observations. It must show the same shell and fresh color now.
        if(!confirmed_) return {};
        const float scale=320.f/width;
        head.box.x*=scale;head.box.y*=scale;head.box.w*=scale;head.box.h*=scale;
        return head;
    }
};
}
#endif
