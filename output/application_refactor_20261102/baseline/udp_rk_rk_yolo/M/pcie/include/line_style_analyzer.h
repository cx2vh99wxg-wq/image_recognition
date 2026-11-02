#ifndef LINE_STYLE_ANALYZER_H
#define LINE_STYLE_ANALYZER_H
#include "line_style.h"
#include "line_style_trace.h"
#include "ego_lane_selector.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <vector>
namespace m_lane {
/* Separate read-only path. Never changes the legacy curve mask or image. */
class LineStyleAnalyzer {
    struct Point { double x,t; int y,w; };
    struct Fit {
        double a=0,b=0,c=0;
        double at(double t) const { return a+b*t+c*t*t; }
    };
    struct Confirm {
        uint32_t candidate=0,count=0;
        YOLOLineBoundary stable={};
        uint32_t width=0,height=0,road_area=0;
        float reference_x=0,reference_y=0;
    } confirm_[2];
    uint64_t last_id_=0,last_capture_=0;
    double ego_anchor_=-1;
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
        int py=std::min(ih-1,y*ih/mh), peak=0;
        // The NN corridor is much wider than the actual paint. Searching its
        // full width picks caption characters beside the line as false paint.
        int core=std::max(2,std::min(3,radius/2));
        for(int dx=-core;dx<=core;++dx) {
            int px=std::max(0,std::min(iw-1,(x+dx)*iw/mw));
            const uint8_t* p=image+((size_t)py*iw+px)*3;
            int hi=std::max(p[0],std::max(p[1],p[2])),lo=std::min(p[0],std::min(p[1],p[2]));
            bool white=lo>=75 && hi-lo<85;
            // Accept RGB/BGR white and yellow paint without relying on channel order.
            bool yellow=p[1]>=95 && std::max(p[0],p[2])>=105 && std::min(p[0],p[2])<p[1]*.8;
            if(white||yellow)peak=std::max(peak,light(p));
        }
        int background=0,valid_sides=0;
        for(int sign:{-1,1}) {
            int sum=0,n=0;
            for(int d=radius+4;d<=radius+8;d+=2) {
                int px=(x+sign*d)*iw/mw;if(px<0||px>=iw)continue;
                sum+=light(image+((size_t)py*iw+px)*3);++n;
            }
            if(n){background=std::max(background,sum/n);++valid_sides;}
        }
        // Compare to the brighter shoulder. Averaging a dark phone caption
        // with normal asphalt makes unpainted road look like a continuous line.
        return valid_sides==2 && peak>background+std::max(12,background/8);
    }
    static YOLOLineBoundary boundary(const float* mask,const uint8_t* image,int iw,int ih,int w,int h,
                                     const std::vector<std::vector<Point>>& rows,int side,std::vector<PaintSample>* trace) {
        YOLOLineBoundary out={};
        const double bottom=h*.95, center=w*.5;
        // Fit in observed image coordinates. Phone/video framing moves the
        // vanishing point; a lane may leave the image well above the bottom.
        // Hypotheses come from separated observed rows, never a fixed horizon.
        std::vector<Point> anchors;
        for(size_t row=0;row<rows.size();row+=4)
            for(const auto& p:rows[row])anchors.push_back(p);
        if(anchors.size()<4){out.reason=LINE_NO_MASK;return out;}
        Fit f;double best_score=0;std::vector<Point> selected;
        const double tolerance=std::max(3.0,w*.025);
        std::vector<std::pair<size_t,size_t>> pairs;
        if(anchors.size()<=40) {
            for(size_t i=0;i<anchors.size();++i)for(size_t j=i+1;j<anchors.size();++j)pairs.push_back({i,j});
        } else {
            uint32_t random=0x51a7u;
            for(unsigned trial=0;trial<768;++trial) {
                random=random*1664525u+1013904223u;size_t i=(random>>8)%anchors.size();
                random=random*1664525u+1013904223u;size_t j=(random>>8)%anchors.size();
                if(i>j)std::swap(i,j);
                if(i!=j)pairs.push_back({i,j});
            }
        }
        for(const auto& pair:pairs) {
            size_t i=pair.first,j=pair.second;
            const auto& a=anchors[i];const auto& b=anchors[j];
            if(b.y-a.y<h*.08)continue;
            double slope=(b.x-a.x)/(b.y-a.y);
            if(std::abs(slope)>3.5*w/h)continue;
            Fit hypothesis;hypothesis.b=slope*h*.52;hypothesis.a=a.x-slope*(a.y-bottom);
            double sum_error=0;int hits=0,first_y=-1,last_y=-1;double last_x=0;
            for(size_t row=0;row<rows.size();row+=2) {
                const Point* match=nullptr;double residual=tolerance;
                for(const auto& p:rows[row]) {
                    double distance=std::abs(p.x-hypothesis.at(p.t));
                    if(distance<residual){residual=distance;match=&p;}
                }
                if(match){++hits;sum_error+=residual;if(first_y<0)first_y=match->y;last_y=match->y;last_x=match->x;}
            }
            if(hits<5||last_y-first_y<h*.08)continue;
            if((side==0&&last_x>=center-w*.025)||(side==1&&last_x<=center+w*.025))continue;
            double span=last_y-first_y;
            double density=std::min(1.0,hits*4.0/(span+1));
            double score=(span+hits*2)*(.55+.45*density)/(1+sum_error/hits/tolerance);
            score/=1+.25*std::abs(hypothesis.at(0)-center)/w;
            if(score>best_score){best_score=score;f=hypothesis;}
        }
        if(best_score==0){out.reason=LINE_NO_TRACK;return out;}
        for(int pass=0;pass<4;++pass) {
            selected.clear();
            for(const auto& row:rows) {
                const Point* match=nullptr;double residual=pass?tolerance:1.5*tolerance;
                for(const auto& p:row){double d=std::abs(p.x-f.at(p.t));if(d<residual){residual=d;match=&p;}}
                if(match)selected.push_back(*match);
            }
            if(!fit(selected,f)){out.reason=LINE_BAD_FIT;return out;}
        }
        if(selected.size()<8){out.reason=LINE_SHORT_TRACK;return out;}
        int first=selected.front().y,last=selected.back().y;
        if(last-first<h*.12){out.reason=LINE_SHORT_TRACK;return out;}
        double error=0;std::vector<int> widths;
        for(const auto& p:selected){error+=std::abs(p.x-f.at(p.t));widths.push_back(p.w);}
        error/=selected.size();if(error>w*.018){out.reason=LINE_BAD_FIT;return out;}
        std::sort(widths.begin(),widths.end());int radius=std::max(3,std::min(12,widths[widths.size()/2]/2+3));
        out.x_bottom=f.at(0);out.x_top=f.at((first-bottom)/(h*.52));
        out.fit_a=f.a;out.fit_b=f.b;out.fit_c=f.c;out.first_y=first;out.last_y=last;
        std::vector<int> samples;
        int valid=0,painted=0,geom=0;
        for(int y=first;y<=last;y+=2) {
            int x=std::lround(f.at((y-bottom)/(h*.52)));
            if(x-radius-8<0||x+radius+8>=w){samples.push_back(-1);if(trace)trace->push_back({y,x,-1,-1});continue;}
            bool lane=false;for(int dx=-radius;dx<=radius;++dx)if(mask[y*w+x+dx]>.3f){lane=true;break;}
            bool rgb=paint(image,iw,ih,w,h,x,y,radius);
            // Geometry may fill dash gaps. RGB evidence retains actual paint.
            // A mask with no usable RGB is insufficient for safe dash inference.
            int value=image?(rgb?1:0):(lane?1:0);
            samples.push_back(value);++valid;painted+=value;geom+=lane;
            if(trace)trace->push_back({y,x,value,value});
        }
        if(valid<12){out.reason=LINE_SHORT_TRACK;return out;}
        // Suppress isolated bright impulses. Never fill dark samples: at the
        // deployed 320x240 camera scale a genuine dash gap may be only 2px.
        auto cleaned=samples;
        for(size_t i=1;i+1<samples.size();++i)if(samples[i]==1&&samples[i-1]==0&&samples[i+1]==0)cleaned[i]=0;
        if(trace)for(size_t i=0;i<cleaned.size();++i)(*trace)[i].filtered=cleaned[i];
        painted=0;for(int v:cleaned)painted+=v==1;
        int spans=0,gaps=0,longest_gap=0,run=0,prev=-1;
        for(size_t i=0;i<=cleaned.size();++i) {
            int v=i<cleaned.size()?cleaned[i]:-1;
            if(v!=prev) {
                if(prev==1&&run>=2)++spans;
                if(prev==0&&run>=2) {longest_gap=std::max(longest_gap,run);if(i>(size_t)run&&i<cleaned.size())++gaps;}
                run=0;prev=v;
            }
            ++run;
        }
        out.duty=100*painted/valid;out.spans=spans;out.gaps=gaps;
        out.coverage=std::min(100,(last-first)*100/(int)(h*.52));
        if(geom<6){out.reason=LINE_NO_MASK;return out;}
        if(image&&out.duty>=82&&longest_gap<=4&&last-first>=h*.22) {out.type=LINE_SOLID;out.quality=std::min(95u,65u+(out.duty-82)+out.coverage/8);}
        else if(image&&out.duty>=15&&out.duty<=72&&spans>=2&&gaps>=1&&longest_gap>=3) {
            out.type=LINE_DASHED;out.quality=std::min(94u,60u+std::min(3,spans)*5u+out.coverage/10);
        }
        out.reason=out.type?LINE_OK:out.duty<15?LINE_NO_PAINT:LINE_AMBIGUOUS;
        out.raw_type=out.type;
        return out;
    }
public:
    LineStyleAnalyzer() {
        const char* value=std::getenv("LANE_EGO_ANCHOR_X");
        if(value&&*value){char* end=nullptr;double x=std::strtod(value,&end);if(end&&!*end&&std::isfinite(x)&&x>=.05&&x<=.95)ego_anchor_=x;}
    }
    YOLOLineStyleResult analyze_front_camera(const float* mask,int w,int h,const uint8_t* image,int iw,int ih,LineStyleTrace* trace=nullptr,const float* drivable=nullptr) const {
        if(!mask||w<80||h<60||iw<1||ih<1)return {};
        // M supplies a 2x2 camera mosaic. Legacy curve analysis uses camera 1
        // (top-left 320x240); do not merge unrelated boundaries from four views.
        if(w<600||h<400) {
            std::vector<uint8_t> green;
            if(drivable){green.resize((size_t)w*h);for(size_t p=0;p<green.size();++p)green[p]=drivable[(size_t)w*h+p]>drivable[p];}
            return analyze(mask,w,h,image,iw,ih,trace,green.empty()?nullptr:green.data());
        }
        int cw=w/2,ch=h/2,ciw=iw/2,cih=ih/2;
        std::vector<float> cropped((size_t)cw*ch);
        std::vector<uint8_t> rgb((size_t)ciw*cih*3);
        std::vector<uint8_t> green;
        for(int y=0;y<ch;++y)std::copy(mask+y*w,mask+y*w+cw,cropped.data()+y*cw);
        if(image)for(int y=0;y<cih;++y)std::copy(image+y*iw*3,image+y*iw*3+ciw*3,rgb.data()+y*ciw*3);
        if(drivable){green.resize((size_t)cw*ch);for(int y=0;y<ch;++y)for(int x=0;x<cw;++x)green[y*cw+x]=drivable[(size_t)w*h+y*w+x]>drivable[y*w+x];}
        return analyze(cropped.data(),cw,ch,image?rgb.data():nullptr,ciw,cih,trace,green.empty()?nullptr:green.data());
    }
    YOLOLineStyleResult analyze(const float* mask,int w,int h,const uint8_t* image,int iw,int ih,LineStyleTrace* trace=nullptr,const uint8_t* green=nullptr) const {
        if(trace)*trace=LineStyleTrace{};
        YOLOLineStyleResult result={};if(!mask||w<80||h<60||iw<1||ih<1)return result;
        if(green) {
            std::vector<uint8_t> resized;
            const uint8_t* rgb=image;
            if(image&&(iw!=w||ih!=h)) {
                resized.resize((size_t)w*h*3);
                for(int y=0;y<h;++y)for(int x=0;x<w;++x)for(int c=0;c<3;++c)resized[((size_t)y*w+x)*3+c]=image[((size_t)(y*ih/h)*iw+x*iw/w)*3+c];
                rgb=resized.data();
            }
            return EgoLaneSelector(ego_anchor_).analyze(mask,green,rgb,w,h,trace);
        }
        std::vector<std::vector<Point>> rows;
        // A phone/video can put the road horizon near the top of this camera
        // tile. Starting at 32% discards real dash gaps and truncates a solid
        // boundary that leaves the image early. Include the upper band as
        // track evidence; the fitted per-line extent and RGB tests below
        // still decide which samples are usable.
        for(int y=h*12/100;y<h*96/100;y+=2) {
            std::vector<Point> row;
            for(int x=0;x<w;) {
                if(!(mask[y*w+x]>.3f)){++x;continue;}int start=x;
                while(x<w&&mask[y*w+x]>.3f)++x;
                if(x-start<=w/10)row.push_back({(start+x-1)*.5,(y-h*.95)/(h*.52),y,x-start});
            }
            // A noisy segmentation must not turn hypothesis scoring quadratic
            // in every foreground pixel or stall the asynchronous NN worker.
            if(row.size()>12) {
                std::stable_sort(row.begin(),row.end(),[](const Point& a,const Point& b){return a.w>b.w;});
                row.resize(12);
            }
            rows.push_back(row);
        }
        result.left=boundary(mask,image,iw,ih,w,h,rows,0,trace?&trace->left:nullptr);
        result.right=boundary(mask,image,iw,ih,w,h,rows,1,trace?&trace->right:nullptr);
        if(result.left.type&&result.right.type) {
            // Compare where both tracks were observed. Extrapolating a short
            // curved phone/video lane to the unseen image bottom is unreliable.
            double y=std::min(result.left.last_y,result.right.last_y),t=(y-h*.95)/(h*.52);
            double lx=result.left.fit_a+result.left.fit_b*t+result.left.fit_c*t*t;
            double rx=result.right.fit_a+result.right.fit_b*t+result.right.fit_c*t*t;
            if(lx>=rx-w*.06){result={};result.left.reason=result.right.reason=LINE_BAD_FIT;}
        }
        return result;
    }
    YOLOLineStyleResult confirm(YOLOLineStyleResult raw,uint64_t id,uint64_t captured) {
        if(!captured||captured<=last_capture_||id==last_id_)return {};
        if(last_capture_&&captured-last_capture_>500000000ULL){confirm_[0]={};confirm_[1]={};}
        last_id_=id;last_capture_=captured;
        YOLOLineBoundary* lines[2]={&raw.left,&raw.right};
        for(int i=0;i<2;++i) {
            auto& c=confirm_[i];auto& line=*lines[i];
            if(!raw.width||!raw.height) {
                // Legacy callers have no lane-ownership context to track.
                if(!line.type){c={};continue;}
                if(c.candidate!=line.type){c.candidate=line.type;c.count=0;}
                if(c.count<3)++c.count;
                if(c.count<3){line.type=LINE_UNKNOWN;line.quality=0;line.reason=LINE_CONFIRMING;}
                else line.evidence_ns=captured;
                continue;
            }
            bool context=c.stable.type&&raw.width&&raw.width==c.width&&raw.height==c.height&&raw.road_area&&raw.object_count&&
                std::abs(raw.reference_x-c.reference_x)<=raw.width*.08&&raw.road_area>=c.road_area/2&&raw.road_area<=c.road_area*2;
            bool geometry=true,have_geometry=line.last_y>line.first_y&&c.stable.last_y>c.stable.first_y;
            if(context&&have_geometry) {
                double lo=std::max(line.first_y-raw.reference_y,c.stable.first_y-c.reference_y),hi=std::min(line.last_y-raw.reference_y,c.stable.last_y-c.reference_y);
                geometry=hi>=lo+3;
                if(geometry){double depth=(lo+hi)*.5,t=(raw.reference_y+depth-raw.height*.95)/(raw.height*.52),old_t=(c.reference_y+depth-raw.height*.95)/(raw.height*.52);double a=line.fit_a+line.fit_b*t+line.fit_c*t*t-raw.reference_x,b=c.stable.fit_a+c.stable.fit_b*old_t+c.stable.fit_c*old_t*old_t-c.reference_x;geometry=std::abs(a-b)<=raw.width*.1;}
            }
            uint32_t observed=line.type;
            bool held=context&&geometry&&c.stable.evidence_ns&&captured>=c.stable.evidence_ns&&
                captured-c.stable.evidence_ns<=(have_geometry?700000000ULL:400000000ULL);
            if(!line.type) {
                if(held){line=c.stable;line.reason=LINE_TRACK_HELD;line.raw_type=LINE_UNKNOWN;c.candidate=c.stable.type;c.count=3;}
                else {c={};continue;}
            } else {
            if(c.stable.type&&(!context||!geometry)){c={};held=false;}
            if(c.candidate!=line.type){c.candidate=line.type;c.count=0;}
            if(c.count<3)++c.count;
            // A transition requires three unique NN inputs. The previous
            // boundary can bridge a brief disagreement only within its
            // original, bounded evidence lifetime.
            if(c.count<3) {
                if(held){line=c.stable;line.reason=LINE_TRACK_HELD;line.raw_type=observed;}
                else {line.type=LINE_UNKNOWN;line.quality=0;line.reason=LINE_CONFIRMING;line.evidence_ns=0;}
            } else {
                line.evidence_ns=captured;c.stable=line;c.width=raw.width;c.height=raw.height;c.road_area=raw.road_area;c.reference_x=raw.reference_x;c.reference_y=raw.reference_y;
            }
            }
            if(line.reason==LINE_TRACK_HELD){unsigned decay=(unsigned)((captured-line.evidence_ns)/100000000ULL);line.quality=std::max(65u,line.quality>decay?line.quality-decay:65u);}
        }
        return raw;
    }
};
}
#endif
