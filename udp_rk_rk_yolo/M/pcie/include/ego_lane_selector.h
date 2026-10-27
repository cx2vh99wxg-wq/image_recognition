#ifndef EGO_LANE_SELECTOR_H
#define EGO_LANE_SELECTOR_H
#include "line_style.h"
#include "line_style_trace.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <vector>
namespace m_lane {
/* Read-only lane ownership layer. Road segmentation supplies support, not a
 * lane ID. A bounded set of line objects is built before a corridor is chosen.
 * No image overlay, legacy curve decision or control register is changed here. */
class EgoLaneSelector {
    double fixed_anchor_;
    struct Point { double x,t; int y,width; bool nn; };
    struct Fit {
        double a=0,b=0,c=0;
        double at(double t) const { return a+b*t+c*t*t; }
    };
    struct Track {
        Fit fit;
        std::vector<Point> points;
        int first=0,last=0;
        unsigned nn=0;
        double length=0,error=0;
    };
    struct Road {
        std::vector<uint8_t> mask;
        std::vector<int> left,right;
        int first=0,last=0;
        unsigned area=0;
        double reference=-1,reference_y=0;
        bool centered=false;
    };
    static double coordinate(int y,int h) { return (y-h*.95)/(h*.52); }
    static bool solve(const std::vector<Point>& p,Fit& f,bool linear=false) {
        if(p.size()<8)return false;
        const int size=linear?2:3;
        double m[3][4]={};
        for(const auto& point:p) {
            double v[3]={1,point.t,point.t*point.t};
            for(int r=0;r<size;++r){for(int c=0;c<size;++c)m[r][c]+=v[r]*v[c];m[r][size]+=v[r]*point.x;}
        }
        for(int c=0;c<size;++c) {
            int pivot=c;for(int r=c+1;r<size;++r)if(std::abs(m[r][c])>std::abs(m[pivot][c]))pivot=r;
            if(std::abs(m[pivot][c])<1e-8)return false;
            for(int k=c;k<=size;++k)std::swap(m[pivot][k],m[c][k]);
            double d=m[c][c];for(int k=c;k<=size;++k)m[c][k]/=d;
            for(int r=0;r<size;++r)if(r!=c){d=m[r][c];for(int k=c;k<=size;++k)m[r][k]-=d*m[c][k];}
        }
        f.a=m[0][size];f.b=m[1][size];f.c=linear?0:m[2][size];
        return std::isfinite(f.a)&&std::isfinite(f.b)&&std::isfinite(f.c);
    }
    static double arc(const Fit& f,int first,int last,int h) {
        double length=0;
        for(int y=first;y<last;++y)length+=std::hypot(1.,f.at(coordinate(y+1,h))-f.at(coordinate(y,h)));
        return length;
    }
    static bool fit_geometry(const std::vector<Point>& p,Fit& f,int w,int h) {
        Fit linear;if(!solve(p,linear,true))return false;f=linear;
        if(p.back().y-p.front().y<h*.10)return true;
        Fit quadratic;if(!solve(p,quadratic,false))return true;
        double a=0,b=0;
        for(const auto& point:p){a+=std::abs(point.x-linear.at(point.t));b+=std::abs(point.x-quadratic.at(point.t));}
        double span=p.back().t-p.front().t;
        if((a-b)/p.size()>.35&&std::abs(quadratic.c)*span*span<w*.25)f=quadratic;
        return true;
    }
    static double extended(const Track& t,int y,int h) {
        int endpoint=std::max(t.first,std::min(t.last,y));
        double v=coordinate(endpoint,h);
        return t.fit.at(v)+(y-endpoint)*(t.fit.b+2*t.fit.c*v)/(h*.52);
    }
    static int light(const uint8_t* p) { return (p[0]+2*p[1]+p[2])/4; }
    static bool color(const uint8_t* p) {
        int hi=std::max(p[0],std::max(p[1],p[2])),lo=std::min(p[0],std::min(p[1],p[2]));
        return (lo>=75&&hi-lo<85)||(p[1]>=95&&std::max(p[0],p[2])>=105&&std::min(p[0],p[2])<p[1]*.8);
    }
    static int ridge(const uint8_t* rgb,int w,int h,int x,int y,int search) {
        if(!rgb||y<0||y>=h)return -1;
        const int margin=std::max(2,w/40);
        double best=0;int selected=-1;
        for(int px=std::max(margin,x-search);px<=std::min(w-1-margin,x+search);++px) {
            const uint8_t* p=rgb+((size_t)y*w+px)*3;if(!color(p))continue;
            int background=0,sides=0;
            for(int sign:{-1,1}) {
                int sum=0,n=0;
                for(int d=6;d<=10;d+=2){int sx=px+sign*d;if(sx>=margin&&sx<w-margin){sum+=light(rgb+((size_t)y*w+sx)*3);++n;}}
                if(n){background=std::max(background,sum/n);++sides;}
            }
            if(!sides)continue;
            int contrast=light(p)-background;
            if(contrast<=std::max(12,background/8))continue;
            double score=contrast-.65*std::abs(px-x);
            if(score>best){best=score;selected=px;}
        }
        return selected;
    }
    static bool black_row(const uint8_t* rgb,int w,int h,int x,int y) {
        if(y<0||y>=h)return false;
        int first=std::max(w/40,x-8),last=std::min(w-1-w/40,x+8);if(last-first<6)return false;
        for(int px=first;px<=last;++px){const uint8_t* p=rgb+((size_t)y*w+px)*3;if(p[0]||p[1]||p[2])return false;}
        return true;
    }
    static Road road_region(const uint8_t* green,int w,int h) {
        Road road;road.mask.assign((size_t)w*h,0);road.left.assign(h,w);road.right.assign(h,-1);
        if(!green)return road;
        const int lo=h*12/100,hi=h*96/100;
        std::vector<uint8_t> visited((size_t)w*h,0);std::vector<int> queue,best;
        for(int y=lo;y<hi;++y)for(int x=0;x<w;++x) {
            int start=y*w+x;if(!green[start]||visited[start])continue;
            queue.clear();queue.push_back(start);visited[start]=1;
            for(size_t i=0;i<queue.size();++i) {
                int p=queue[i],px=p%w,py=p/w;
                const int dx[4]={-1,1,0,0},dy[4]={0,0,-1,1};
                for(int d=0;d<4;++d) {
                    int nx=px+dx[d],ny=py+dy[d];if(nx<0||nx>=w||ny<lo||ny>=hi)continue;
                    int q=ny*w+nx;if(green[q]&&!visited[q]){visited[q]=1;queue.push_back(q);}
                }
            }
            if(queue.size()>best.size())best=queue;
        }
        if(best.size()<(size_t)(w*h/160))return road;
        road.first=h;road.last=0;road.area=(unsigned)best.size();
        for(int p:best){int y=p/w,x=p%w;road.mask[p]=1;road.first=std::min(road.first,y);road.last=std::max(road.last,y);road.left[y]=std::min(road.left[y],x);road.right[y]=std::max(road.right[y],x);}
        if(road.last-road.first<h*.12){road.area=0;return road;}
        // Fixed forward-camera input uses the bottom optical axis. For inset
        // screen recordings the road may end above that point; use its far
        // entry axis instead, with a corridor confidence check below.
        int anchor_y=h*90/100,anchor_x=w/2;
        road.reference_y=road.first;
        if(road.area>=(unsigned)(w*h/4)&&road.last>=anchor_y&&road.mask[anchor_y*w+anchor_x]){road.reference=anchor_x;road.centered=true;}
        else {
            std::vector<double> centers;int band=std::max(5,(road.last-road.first)/8);
            for(int y=road.first;y<=std::min(road.last,road.first+band);++y)
                if(road.right[y]-road.left[y]>=2)centers.push_back((road.left[y]+road.right[y])*.5);
            if(centers.size()>=3){std::sort(centers.begin(),centers.end());road.reference=centers[centers.size()/2];}
        }
        return road;
    }
    static bool near_road(const Road& road,int x,int y,int w,int h) {
        if(y<0||y>=h)return false;
        for(int dx=-5;dx<=5;dx+=2){int px=x+dx;if(px>=0&&px<w&&road.mask[y*w+px])return true;}
        return false;
    }
    static std::vector<Track> objects(const float* mask,const uint8_t* rgb,Road& road,int w,int h) {
        std::vector<std::vector<Point>> rows;
        for(int y=h*12/100;y<h*96/100;++y) {
            std::vector<Point> row;
            for(int x=0;x<w;) {
                if(!(mask[y*w+x]>.3f)){++x;continue;}int begin=x;
                while(x<w&&mask[y*w+x]>.3f)++x;
                if(x-begin<=w/10)row.push_back({(begin+x-1)*.5,coordinate(y,h),y,x-begin,true});
            }
            if(row.size()>12){std::stable_sort(row.begin(),row.end(),[](const Point& a,const Point& b){return a.nn>b.nn;});row.resize(12);}
            rows.push_back(row);
        }
        if(!road.centered) {
            std::vector<double> centers;std::vector<int> center_rows;
            for(const auto& row:rows) {
                if(row.empty()||row[0].y<road.first-h*.15||row[0].y>road.first+h*.1)continue;
                int y=std::max(road.first,std::min(road.last,row[0].y));
                std::vector<double> xs;
                for(const auto& p:row)if(p.x>=road.left[y]-w*.1&&p.x<=road.right[y]+w*.1)xs.push_back(p.x);
                if(xs.size()<2)continue;
                std::sort(xs.begin(),xs.end());double width=xs.back()-xs.front();
                if(width<w*.015||width>w*.25)continue;
                centers.push_back((xs.front()+xs.back())*.5);center_rows.push_back(row[0].y);if(centers.size()>=8)break;
            }
            if(centers.size()>=3){std::sort(centers.begin(),centers.end());road.reference=centers[centers.size()/2];road.reference_y=center_rows[center_rows.size()/2];}
        }
        std::vector<Track> tracks;
        const double tolerance=std::max(3.,w*.022);
        for(int object=0;object<6;++object) {
            std::vector<Point> anchors;
            for(size_t r=0;r<rows.size();r+=4)for(const auto& p:rows[r])anchors.push_back(p);
            if(anchors.size()<4)break;
            double best_score=0;Fit selected;bool branch_left=false;
            std::vector<std::pair<size_t,size_t>> pairs;
            if(anchors.size()<=32){for(size_t i=0;i<anchors.size();++i)for(size_t j=i+1;j<anchors.size();++j)pairs.push_back({i,j});}
            else {uint32_t random=0x51a7u+object;for(int k=0;k<512;++k){random=random*1664525u+1013904223u;size_t i=(random>>8)%anchors.size();random=random*1664525u+1013904223u;size_t j=(random>>8)%anchors.size();if(i>j)std::swap(i,j);if(i!=j)pairs.push_back({i,j});}}
            for(const auto& pair:pairs) {
                const auto& a=anchors[pair.first];const auto& b=anchors[pair.second];
                if(b.y<=a.y||std::hypot(b.x-a.x,(double)b.y-a.y)<h*.06)continue;
                double slope=(b.x-a.x)/(b.y-a.y);if(std::abs(slope)>3.5*w/h)continue;
                Fit f;f.b=slope*h*.52;f.a=a.x-slope*(a.y-h*.95);
                int hits=0,first=-1,last=-1;double error=0,first_x=0,last_x=0;
                for(size_t r=0;r<rows.size();r+=2) {
                    const Point* match=nullptr;double distance=tolerance;
                    for(const auto& p:rows[r]){double d=std::abs(p.x-f.at(p.t));if(d<distance){distance=d;match=&p;}}
                    if(match){++hits;error+=distance;if(first<0){first=match->y;first_x=match->x;}last=match->y;last_x=match->x;}
                }
                if(hits<4||std::hypot(last_x-first_x,(double)last-first)<h*.08)continue;
                if(std::min(first_x,last_x)<road.reference-w*.05&&std::max(first_x,last_x)>road.reference+w*.05)continue;
                double score=(std::hypot(last_x-first_x,(double)last-first)+hits*1.5)/(1+error/hits/tolerance);
                if(score>best_score){best_score=score;selected=f;branch_left=last_x<road.reference;}
            }
            if(!best_score)break;
            Track track;track.fit=selected;
            for(int pass=0;pass<4;++pass) {
                track.points.clear();
                for(const auto& row:rows) {
                    const Point* match=nullptr;double distance=tolerance*(pass?1:1.5);
                    for(const auto& p:row){if(branch_left?p.x>road.reference+w*.05:p.x<road.reference-w*.05)continue;double d=std::abs(p.x-track.fit.at(p.t));if(d<distance){distance=d;match=&p;}}
                    if(match)track.points.push_back(*match);
                }
                if(track.points.size()<8)break;
                if(!fit_geometry(track.points,track.fit,w,h)){track.points.clear();break;}
            }
            if(track.points.size()<8)break;
            track.first=track.points.front().y;track.last=track.points.back().y;
            // Refine an NN corridor to the observed paint ridge. This does not
            // fill virtual dash gaps; only actual RGB evidence enters refit.
            std::vector<Point> paint_points;
            for(const auto& p:track.points){int x=(int)std::lround(track.fit.at(p.t));int peak=ridge(rgb,w,h,x,p.y,5);if(peak>=0)paint_points.push_back({(double)peak,p.t,p.y,p.width,p.nn});}
            if(paint_points.size()>=8&&paint_points.back().y-paint_points.front().y>=(track.last-track.first)*.5) {
                Fit refined;
                if(fit_geometry(paint_points,refined,w,h)) {
                    double movement=0;for(const auto& p:track.points)movement+=std::abs(refined.at(p.t)-track.fit.at(p.t));
                    if(movement/track.points.size()<5)track.fit=refined;
                }
            }
            for(const auto& p:track.points){track.nn+=p.nn;track.error+=std::abs(p.x-track.fit.at(p.t));}
            track.error/=track.points.size();
            // Retain only RGB-confirmed extension, bounded to a few rows.
            // This recovers the paint at the ends of a short NN segment;
            // unseen/extrapolated pixels never count as solid evidence.
            for(int direction:{-1,1}) {
                int endpoint=direction<0?track.first:track.last,last_good=endpoint,missing=0;
                int upper_limit=std::min(track.first,road.first);
                for(int step=1;step<=h/25;++step) {
                    int y=endpoint+direction*step;if(y<upper_limit||y<h*12/100||y>=h*96/100)break;
                    int x=(int)std::lround(track.fit.at(coordinate(y,h)));if(x<0||x>=w)break;
                    if(ridge(rgb,w,h,x,y,3)>=0){last_good=y;missing=0;}else if(++missing>1)break;
                }
                if(direction<0)track.first=last_good;else track.last=last_good;
            }
            track.length=arc(track.fit,track.first,track.last,h);
            // Peel this object, so a longer adjacent line cannot consume every
            // candidate slot and hide a short boundary near the image edge.
            for(auto& row:rows)row.erase(std::remove_if(row.begin(),row.end(),[&](const Point& p){return p.y>road.first+h*.06&&std::abs(p.x-track.fit.at(p.t))<tolerance;}),row.end());
            double u0=coordinate(track.first,h),u1=coordinate(track.last,h);
            double angle0=std::atan((track.fit.b+2*track.fit.c*u0)/(h*.52));
            double angle1=std::atan((track.fit.b+2*track.fit.c*u1)/(h*.52));
            if(track.error>w*.025||track.length<h*.08||std::abs(angle1-angle0)>1.0472)continue;
            bool duplicate=false;
            for(const auto& old:tracks){int lo=std::max(track.first,old.first),hi=std::min(track.last,old.last);if(hi<lo)continue;double d=0;for(int y=lo;y<=hi;y+=3)d+=std::abs(track.fit.at(coordinate(y,h))-old.fit.at(coordinate(y,h)));d/=((hi-lo)/3+1);if(d<tolerance){duplicate=true;break;}}
            if(!duplicate)tracks.push_back(track);
        }
        // A foreground-only road segmentation can start below the real road
        // entry. Use the mutually converging NN objects to correct that entry,
        // rather than letting a green patch at the bottom choose a neighbour.
        auto correct_reference=[&]() {
            std::vector<int> entries;
            for(const auto& t:tracks)if(t.nn>=8&&t.first>=road.first-h*.15&&t.first<road.first-h*.025)entries.push_back(t.first);
            if(!road.centered&&entries.size()>=2&&road.area<(unsigned)(w*h/4)) {
                std::sort(entries.begin(),entries.end());int y=entries[0];std::vector<double> xs;
                for(const auto& t:tracks)if(t.nn>=8&&t.first<=y+h*.05&&t.last>=y)xs.push_back(extended(t,y,h));
                if(xs.size()>=2) {
                    std::sort(xs.begin(),xs.end());
                    if(xs.back()-xs.front()<w*.2&&xs.front()>=0&&xs.back()<w){road.reference=xs[xs.size()/2];road.reference_y=y;}
                }
            }
        };
        correct_reference();
        // Recover a clipped outer marking from actual RGB near the lateral
        // road border. Keep this separate from NN RANSAC: mixing the bottom
        // of a photographed screen into its point pool creates hood/bezel
        // curves and can erase the real short boundary during peeling.
        for(int side=0;side<2;++side) {
            std::vector<Point> paint;
            for(int y=road.first;y<=road.last;++y) {
                int source=std::max(road.first,y),edge=side?road.right[source]:road.left[source];if(edge<0||edge>=w)continue;
                int peak=ridge(rgb,w,h,edge,y,std::max(5,w/32));
                if(peak<0||(side?peak<=road.reference+2:peak>=road.reference-2))continue;
                paint.push_back({(double)peak,coordinate(y,h),y,3,false});
            }
            if(paint.size()<8||paint.front().y>road.first+h*.1)continue;
            Fit f;if(!solve(paint,f,true))continue;
            for(int pass=0;pass<4;++pass) {
                std::vector<Point> selected;
                for(const auto& p:paint)if(std::abs(p.x-f.at(p.t))<std::max(4.,w*.018))selected.push_back(p);
                if(selected.size()<8)break;
                paint=selected;if(!solve(paint,f,paint.back().y-paint.front().y<h*.18))break;
            }
            if(paint.size()<8||paint.back().y-paint.front().y<h*.025)continue;
            double start=f.at(paint.front().t);
            if(std::abs(start-road.reference)>w*.12)continue;
            double end=f.at(paint.back().t);
            if(side?end<start-5:end>start+5)continue;
            Track recovered;recovered.fit=f;recovered.points=paint;
            recovered.first=paint.front().y;recovered.last=paint.back().y;
            recovered.length=arc(f,recovered.first,recovered.last,h);
            for(const auto& p:paint)recovered.error+=std::abs(p.x-f.at(p.t));
            recovered.error/=paint.size();if(recovered.error>w*.018||recovered.length<h*.08)continue;
            bool duplicate=false;
            for(auto& old:tracks) {
                int lo=std::max(recovered.first,old.first),hi=std::min(recovered.last,old.last);if(hi<lo)continue;
                double distance=0;for(int y=lo;y<=hi;++y)distance+=std::abs(f.at(coordinate(y,h))-old.fit.at(coordinate(y,h)));
                if(distance/(hi-lo+1)<tolerance) {
                    duplicate=true;
                    if(recovered.length>=old.length*.8&&recovered.error<old.error){recovered.nn=old.nn;old=recovered;}
                    break;
                }
            }
            if(!duplicate)tracks.push_back(recovered);
        }
        correct_reference();
        for(auto& track:tracks) {
            for(int direction:{-1,1}) {
                int endpoint=direction<0?track.first:track.last,last_good=endpoint,missing=0;
                int upper_limit=std::min(track.first,road.first);
                for(int step=1;step<=h/25;++step) {
                    int y=endpoint+direction*step;if(y<upper_limit||y<h*12/100||y>=h*96/100)break;
                    int x=(int)std::lround(track.fit.at(coordinate(y,h)));if(x<0||x>=w)break;
                    if(ridge(rgb,w,h,x,y,3)>=0){last_good=y;missing=0;}else if(++missing>1)break;
                }
                if(direction<0)track.first=last_good;else track.last=last_good;
            }
            track.length=arc(track.fit,track.first,track.last,h);
        }
        return tracks;
    }
    static YOLOLineBoundary measure(const Track& track,const uint8_t* rgb,int w,int h,int road_first,int road_height,double lane_width,std::vector<PaintSample>* trace) {
        YOLOLineBoundary out={};out.fit_a=track.fit.a;out.fit_b=track.fit.b;out.fit_c=track.fit.c;
        out.first_y=track.first;out.last_y=track.last;out.x_bottom=track.fit.at(0);out.x_top=track.fit.at(coordinate(track.first,h));
        out.coverage=std::min(100,(int)(track.length*100/(h*.52)));
        std::vector<int> values;int valid=0;
        for(int y=track.first;y<=track.last;++y) {
            int x=(int)std::lround(track.fit.at(coordinate(y,h)));
            int value=-1;
            if(x>=w/40&&x<w-w/40) {
                // Exact, isolated all-zero acquisition rows are unavailable
                // evidence, not a dark gap in the road marking.
                bool missing=black_row(rgb,w,h,x,y)&&!black_row(rgb,w,h,x,y-1)&&!black_row(rgb,w,h,x,y+1);
                if(!missing){value=ridge(rgb,w,h,x,y,3)>=0?1:0;++valid;}
            }
            values.push_back(value);if(trace)trace->push_back({y,x,value,value});
        }
        if(valid<8){out.reason=LINE_SHORT_TRACK;return out;}
        auto cleaned=values;
        for(size_t i=1;i+1<values.size();++i)if(values[i]==1&&values[i-1]==0&&values[i+1]==0)cleaned[i]=0;
        int painted=0,spans=0,gaps=0,longest=0,longest_paint=0,run=0,previous=-1;
        for(size_t i=0;i<=cleaned.size();++i) {
            int value=i<cleaned.size()?cleaned[i]:-1;
            if(i<cleaned.size()&&value<0)continue;
            if(i<cleaned.size()){painted+=value==1;if(trace)(*trace)[i].filtered=value;}
            if(value!=previous) {
                if(previous==1&&run>=3){++spans;longest_paint=std::max(longest_paint,run);}
                if(previous==0&&run>=3&&i>(size_t)run&&i<cleaned.size()&&cleaned[i-run-1]==1&&value==1){++gaps;longest=std::max(longest,run);}
                run=0;previous=value;
            }
            ++run;
        }
        out.duty=100*painted/valid;out.spans=spans;out.gaps=gaps;
        double first_x=track.fit.at(coordinate(track.first,h)),last_x=track.fit.at(coordinate(track.last,h));
        bool clipped=std::min(first_x,last_x)<w*.04||std::max(first_x,last_x)>w*.96;
        double required_length=h*.22;
        if(clipped&&track.first<=road_first+h*.1) {
            // A small inset road must not be judged against the height of the
            // entire camera tile. Keep a minimum physical observation length
            // and reduce it only for a line clipped at the side of that road.
            required_length=std::max(24.,std::min(h*.20,std::min(road_height*.35,lane_width*.3)));
        }
        bool near_continuous=out.duty>=82||(out.duty>=80&&longest_paint>=valid*.6);
        // On a short, side-clipped observation a single gap may already be
        // part of a dash period. Require nearly uninterrupted paint there;
        // do not use the wear tolerance intended for a long visible line.
        if(valid<h*.18)near_continuous=out.duty>=95&&gaps==0;
        if(near_continuous&&gaps<=1&&longest<=std::max(4,h*3/100)&&track.length>=required_length) {
            out.type=LINE_SOLID;out.quality=std::min(95u,68u+(out.duty-80)/2+out.coverage/10);
        } else if(out.duty>=15&&out.duty<=72&&spans>=2&&gaps>=1&&longest>=3) {
            out.type=LINE_DASHED;out.quality=std::min(94u,60u+std::min(3,spans)*5u+out.coverage/10);
        }
        out.raw_type=out.type;out.reason=out.type?LINE_OK:out.duty<15?LINE_NO_PAINT:track.length<required_length?LINE_SHORT_TRACK:LINE_AMBIGUOUS;
        return out;
    }
public:
    explicit EgoLaneSelector(double normalized_anchor=-1):fixed_anchor_(normalized_anchor) {}
    YOLOLineStyleResult analyze(const float* mask,const uint8_t* green,const uint8_t* rgb,int w,int h,LineStyleTrace* trace=nullptr) const {
        YOLOLineStyleResult result={};result.width=w;result.height=h;result.left.reason=result.right.reason=LINE_NO_MASK;
        if(!mask||!green||!rgb||w<80||h<60)return result;
        Road road=road_region(green,w,h);if(!road.area||road.reference<0)return result;
        if(fixed_anchor_>=0&&fixed_anchor_<=1){road.reference=fixed_anchor_*w;road.centered=true;}
        auto tracks=objects(mask,rgb,road,w,h);
        result.road_area=road.area;result.object_count=(uint32_t)tracks.size();result.reference_x=road.reference;result.reference_y=road.reference_y;
        int near_y=(int)(road.first+.72*(road.last-road.first));
        std::sort(tracks.begin(),tracks.end(),[&](const Track& a,const Track& b){return extended(a,near_y,h)<extended(b,near_y,h);});
        if(trace){trace->reference_x=road.reference;trace->near_y=near_y;trace->road_area=road.area;for(const auto& t:tracks)trace->objects.push_back({t.fit.a,t.fit.b,t.fit.c,t.first,t.last,t.nn});}
        double best=-1;int selected=-1;
        for(size_t i=0;i+1<tracks.size();++i) {
            const Track& left=tracks[i];const Track& right=tracks[i+1];
            if(left.nn+right.nn<8)continue;
            double lx=extended(left,near_y,h),rx=extended(right,near_y,h),width=rx-lx;
            if(width<w*.08||width>w*1.8||lx>=road.reference-w*.012||rx<=road.reference+w*.012)continue;
            int far=std::max(left.first,right.first);
            double far_width=extended(right,far,h)-extended(left,far,h);
            if(far_width<-w*.025||far_width>width*.9+w*.03)continue;
            int supported=0,total=0,axis_rows=0;
            for(int y=road.first;y<=road.last;y+=2) {
                int a=std::max(road.left[y],std::max(0,(int)std::ceil(extended(left,y,h)))),b=std::min(road.right[y],std::min(w-1,(int)std::floor(extended(right,y,h))));
                if(a>b)continue;
                int row_count=0;for(int x=a;x<=b;++x){++total;if(road.mask[y*w+x]){++supported;++row_count;}}
                axis_rows+=row_count>=3;
            }
            if(total<32||(unsigned)supported<road.area/30||axis_rows<8)continue;
            double coverage=(double)supported/total;
            if(coverage<.3)continue;
            // Area is a tie breaker. It cannot select a different corridor
            // that excludes the vehicle/reference axis.
            double balance=std::min(road.reference-lx,rx-road.reference)/width;
            double score=coverage+.25*balance+.1*std::min(1.,(double)supported/road.area);
            if(score>best){best=score;selected=(int)i;}
        }
        if(selected<0){result.left.reason=result.right.reason=LINE_NO_TRACK;return result;}
        if(trace){trace->selected_left=selected;trace->selected_right=selected+1;}
        double visible_width=std::min((double)w,extended(tracks[selected+1],near_y,h))-std::max(0.,extended(tracks[selected],near_y,h));
        result.left=measure(tracks[selected],rgb,w,h,road.first,road.last-road.first,visible_width,trace?&trace->left:nullptr);
        result.right=measure(tracks[selected+1],rgb,w,h,road.first,road.last-road.first,visible_width,trace?&trace->right:nullptr);
        return result;
    }
};
}
#endif
