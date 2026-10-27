#ifndef LINE_STYLE_ANALYZER_H
#define LINE_STYLE_ANALYZER_H
#include "line_style.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>
namespace m_lane {
/* Separate read-only path. Never changes the legacy curve mask or image. */
class LineStyleAnalyzer {
    struct Point { double x,t; int y,w; };
    struct Fit {
        double a=0,b=0,c=0;
        double at(double t) const { return a+b*t+c*t*t; }
    };
    struct Confirm { uint32_t candidate=0,count=0; } confirm_[2];
    uint64_t last_id_=0,last_capture_=0;
    static bool fit(const std::vector<Point>& points, Fit& f) {
        if(points.size()<8) return false;
        double m[3][4]={};
        for(const auto& p:points) {
            double v[3]={1,p.t,p.t*p.t};
            for(int i=0;i<3;++i) { for(int j=0;j<3;++j)m[i][j]+=v[i]*v[j]; m[i][3]+=v[i]*p.x; }
        }
        for(int k=0;k<3;++k) {
            int best=k; for(int r=k+1;r<3;++r)if(std::abs(m[r][k])>std::abs(m[best][k]))best=r;
            if(std::abs(m[best][k])<1e-7)return false;
            for(int j=k;j<4;++j)std::swap(m[k][j],m[best][j]);
            double d=m[k][k];for(int j=k;j<4;++j)m[k][j]/=d;
            for(int r=0;r<3;++r)if(r!=k){d=m[r][k];for(int j=k;j<4;++j)m[r][j]-=d*m[k][j];}
        }
        f.a=m[0][3];f.b=m[1][3];f.c=m[2][3];return true;
    }
    static int light(const uint8_t* p) { return (p[0]+2*p[1]+p[2])/4; }
    static bool paint(const uint8_t* image,int iw,int ih,int mw,int mh,int x,int y,int radius) {
        if(!image)return false;
        int py=std::min(ih-1,y*ih/mh), peak=0,side=0,n=0;
        for(int dx=-radius;dx<=radius;++dx) {
            int px=std::max(0,std::min(iw-1,(x+dx)*iw/mw));
            const uint8_t* p=image+((size_t)py*iw+px)*3;
            int hi=std::max(p[0],std::max(p[1],p[2])),lo=std::min(p[0],std::min(p[1],p[2]));
            bool white=lo>=75 && hi-lo<85;
            // Accept RGB/BGR white and yellow paint without relying on channel order.
            bool yellow=p[1]>=95 && std::max(p[0],p[2])>=105 && std::min(p[0],p[2])<p[1]*.8;
            if(white||yellow)peak=std::max(peak,light(p));
        }
        for(int sign:{-1,1})for(int d=radius+4;d<=radius+8;d+=2) {
            int px=(x+sign*d)*iw/mw;if(px<0||px>=iw)continue;
            side+=light(image+((size_t)py*iw+px)*3);++n;
        }
        return n && peak>side/n+18;
    }
    static YOLOLineBoundary boundary(const float* mask,const uint8_t* image,int iw,int ih,int w,int h,
                                     const std::vector<std::vector<Point>>& rows,int side) {
        YOLOLineBoundary out={};
        const double horizon=h*.30, bottom=h*.95, center=w*.5;
        std::vector<double> hist(w,0);
        // Project bottom/middle observations onto the bottom row to seed the
        // nearest ego-lane boundary, rather than selecting two global peaks.
        for(const auto& row:rows)for(const auto& p:row) {
            double xb=center+(p.x-center)*(bottom-horizon)/(p.y-horizon);
            int b=(int)std::lround(xb);
            if(b<8||b>=w-8)continue;
            if((side==0&&b>=center-w*.07)||(side==1&&b<=center+w*.07))continue;
            for(int dx=-8;dx<=8;++dx)if(b+dx>=0&&b+dx<w)hist[b+dx]+=1.0-std::abs(dx)/10.0;
        }
        int seed=-1;
        for(int x=8;x<w-8;++x) {
            if((side==0&&x>=center-w*.07)||(side==1&&x<=center+w*.07)||hist[x]<6)continue;
            // Penalize far-away adjacent lanes, but require strong support.
            double value=hist[x]/(1+std::abs(x-center)/w);
            if(seed<0||value>hist[seed]/(1+std::abs(seed-center)/w))seed=x;
        }
        if(seed<0)return out;
        Fit f;f.a=seed;f.b=(seed-center)*(h*.52)/(bottom-horizon);
        std::vector<Point> selected;
        for(int pass=0;pass<4;++pass) {
            selected.clear();
            for(const auto& row:rows) {
                const Point* best=nullptr; double distance=(pass? w*.035:w*.09);
                for(const auto& p:row) {
                    double d=std::abs(p.x-f.at(p.t));
                    if(d<distance){distance=d;best=&p;}
                }
                if(best)selected.push_back(*best);
            }
            if(!fit(selected,f))return out;
        }
        if(selected.size()<12)return out;
        int first=selected.front().y,last=selected.back().y;
        if(last-first<h*.28)return out;
        double error=0;std::vector<int> widths;
        for(const auto& p:selected){error+=std::abs(p.x-f.at(p.t));widths.push_back(p.w);}
        error/=selected.size();if(error>w*.018)return out;
        std::sort(widths.begin(),widths.end());int radius=std::max(3,std::min(12,widths[widths.size()/2]/2+3));
        out.x_bottom=f.at(0);out.x_top=f.at((first-bottom)/(h*.52));
        std::vector<int> samples;
        int valid=0,painted=0,geom=0;
        for(int y=first;y<=last;y+=3) {
            int x=std::lround(f.at((y-bottom)/(h*.52)));
            if(x-radius-8<0||x+radius+8>=w){samples.push_back(-1);continue;}
            bool lane=false;for(int dx=-radius;dx<=radius;++dx)if(mask[y*w+x+dx]>.3f){lane=true;break;}
            bool rgb=paint(image,iw,ih,w,h,x,y,radius);
            // Geometry may fill dash gaps. RGB evidence retains actual paint.
            // A mask with no usable RGB is insufficient for safe dash inference.
            int value=image?(rgb?1:0):(lane?1:0);
            samples.push_back(value);++valid;painted+=value;geom+=lane;
        }
        if(valid<25)return out;
        // Fill only isolated single-sample holes; never close long gaps.
        auto cleaned=samples;
        for(size_t i=1;i+1<samples.size();++i)if(samples[i]>=0&&samples[i-1]==samples[i+1]&&samples[i-1]>=0)cleaned[i]=samples[i-1];
        painted=0;for(int v:cleaned)painted+=v==1;
        int spans=0,gaps=0,longest_gap=0,run=0,prev=-1;
        for(size_t i=0;i<=cleaned.size();++i) {
            int v=i<cleaned.size()?cleaned[i]:-1;
            if(v!=prev) {
                if(prev==1&&run>=3)++spans;
                if(prev==0&&run>=3) {longest_gap=std::max(longest_gap,run);if(i>(size_t)run&&i<cleaned.size())++gaps;}
                run=0;prev=v;
            }
            ++run;
        }
        out.duty=100*painted/valid;out.spans=spans;out.gaps=gaps;
        out.coverage=std::min(100,(last-first)*100/(int)(h*.52));
        if(geom<12)return out;
        if(out.duty>=82&&longest_gap<=4) {out.type=LINE_SOLID;out.quality=std::min(95u,65u+(out.duty-82)+out.coverage/8);}
        else if(image&&out.duty>=15&&out.duty<=72&&spans>=2&&gaps>=1&&longest_gap>=3) {
            out.type=LINE_DASHED;out.quality=std::min(94u,60u+std::min(3,spans)*5u+out.coverage/10);
        }
        return out;
    }
public:
    YOLOLineStyleResult analyze_front_camera(const float* mask,int w,int h,const uint8_t* image,int iw,int ih) const {
        // M supplies a 2x2 camera mosaic. Legacy curve analysis uses camera 1
        // (top-left 320x240); do not merge unrelated boundaries from four views.
        if(w<600||h<400)return analyze(mask,w,h,image,iw,ih);
        int cw=w/2,ch=h/2,ciw=iw/2,cih=ih/2;
        std::vector<float> cropped((size_t)cw*ch);
        std::vector<uint8_t> rgb((size_t)ciw*cih*3);
        for(int y=0;y<ch;++y)std::copy(mask+y*w,mask+y*w+cw,cropped.data()+y*cw);
        if(image)for(int y=0;y<cih;++y)std::copy(image+y*iw*3,image+y*iw*3+ciw*3,rgb.data()+y*ciw*3);
        return analyze(cropped.data(),cw,ch,image?rgb.data():nullptr,ciw,cih);
    }
    YOLOLineStyleResult analyze(const float* mask,int w,int h,const uint8_t* image,int iw,int ih) const {
        YOLOLineStyleResult result={};if(!mask||w<80||h<60||iw<1||ih<1)return result;
        std::vector<std::vector<Point>> rows;
        for(int y=h*43/100;y<h*96/100;y+=3) {
            std::vector<Point> row;
            for(int x=0;x<w;) {
                if(!(mask[y*w+x]>.3f)){++x;continue;}int start=x;
                while(x<w&&mask[y*w+x]>.3f)++x;
                if(x-start<=w/18)row.push_back({(start+x-1)*.5,(y-h*.95)/(h*.52),y,x-start});
            }
            rows.push_back(row);
        }
        result.left=boundary(mask,image,iw,ih,w,h,rows,0);
        result.right=boundary(mask,image,iw,ih,w,h,rows,1);
        if(result.left.type&&result.right.type&&result.left.x_bottom>=result.right.x_bottom-w*.12)result={};
        return result;
    }
    YOLOLineStyleResult confirm(YOLOLineStyleResult raw,uint64_t id,uint64_t captured) {
        if(!captured||captured<=last_capture_||id==last_id_)return {};
        if(last_capture_&&captured-last_capture_>500000000ULL){confirm_[0]={};confirm_[1]={};}
        last_id_=id;last_capture_=captured;
        YOLOLineBoundary* lines[2]={&raw.left,&raw.right};
        for(int i=0;i<2;++i) {
            auto& c=confirm_[i];auto& line=*lines[i];
            if(!line.type){c={};continue;}
            if(c.candidate!=line.type){c.candidate=line.type;c.count=0;}
            ++c.count;
            // A contradictory observation withdraws permission immediately;
            // every classified transition requires three unique NN inputs.
            if(c.count<3){line.type=LINE_UNKNOWN;line.quality=0;}
        }
        return raw;
    }
};
}
#endif
