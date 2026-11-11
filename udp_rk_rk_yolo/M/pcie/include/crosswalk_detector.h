#ifndef M_CROSSWALK_DETECTOR_H
#define M_CROSSWALK_DETECTOR_H
#include "../../../common/perception/include/crosswalk_display.h"
#include <array>
#include <cmath>
#include <vector>
namespace crosswalk {
struct Stripe {
 double x,y,length,width,ax,ay,fill;
 int first_y,last_y;
 bool bounded;
 crosswalk_display::Quad quad;
};
// Fixed-size CPU evidence: neutral bright components, elongation, shared band,
// comparable widths/lengths and regularly spaced repetition. No model/controls.
class Detector {
 std::vector<int> integral_,paint_integral_,queue_;
 std::vector<uint8_t> mask_,seen_,luma_,filtered_;
 public:
 crosswalk_display::Geometry detect(const uint8_t*bgr,const uint8_t*coverage,int w=320,int h=240){
  crosswalk_display::Geometry empty;if(!bgr||!coverage||w!=320||h!=240)return empty;
  integral_.assign((w+1)*(h+1),0);luma_.resize(w*h);
  for(int y=0;y<h;++y){int row=0;for(int x=0;x<w;++x){const auto*p=bgr+(y*w+x)*3;luma_[y*w+x]=(29*p[0]+150*p[1]+77*p[2]+128)>>8;row+=luma_[y*w+x];integral_[(y+1)*(w+1)+x+1]=integral_[y*(w+1)+x+1]+row;}}
  auto result=detect_band(bgr,coverage,w,h,h*55/100);
  if(!result.count)result=detect_band(bgr,coverage,w,h,h*65/100);
  if(!result.count)result=detect_rows(bgr,coverage,w,h);
  return result;
 }
 private:
 // A bright run on one row is only a seed. The stripe must remain visible
 // above and below that row along a single axis.
 struct RowSupport {bool stripe=false,through=false,shared_band=false;};
 RowSupport row_support(double x,int y,const uint8_t*coverage,int w,int h) const {
  RowSupport support;
  int best_support=0;
  for(double slope:{0.,-.25,.25,-.5,.5,-.75,.75}){
   const auto sample=[&](int dy){
    const int row=y+dy,col=int(std::lround(x+slope*dy));
    if(row<0||row>=h||col<1||col>=w-1||!coverage[row*w+col])return -1;
    const int at=row*w+col;
    return int(mask_[at-1]||mask_[at]||mask_[at+1]);
   };
   int above=0,below=0,far_above=0,far_below=0;
   for(int d:{4,8,12}){above+=sample(-d)==1;below+=sample(d)==1;}
   if(!above||!below||above+below<4)continue;
   const bool same_band=sample(-4)==1&&sample(4)==1;
   support.shared_band|=same_band;
   const int local_support=2*(above+below)+same_band;
   if(local_support<=best_support)continue;
   best_support=local_support;
   int clear_above=0,clear_below=0;
   for(int d:{22,28,34}){
    const int a=sample(-d),b=sample(d);
    far_above+=a==1;far_below+=b==1;
    clear_above+=a==0;clear_below+=b==0;
   }
   // Only extend the best supported local axis. A different slope can
   // hit a neighbor or an obstruction and look like a through-going line.
   support.through=far_above>=2&&far_below>=2;
   support.stripe=clear_above&&clear_below&&clear_above+far_above>=2&&clear_below+far_below>=2;
  }
  return support;
 }

 crosswalk_display::Geometry detect_rows(const uint8_t*bgr,const uint8_t*coverage,int w,int h){
  crosswalk_display::Geometry out;mask_.assign(w*h,0);paint_integral_.assign((w+1)*(h+1),0);
  for(int y=0;y<h;++y){int row=0;for(int x=0;x<w;++x){const auto*p=bgr+(y*w+x)*3;int hi=std::max(p[0],std::max(p[1],p[2])),lo=std::min(p[0],std::min(p[1],p[2]));
   int x0=std::max(0,x-9),x1=std::min(w,x+10),y0=std::max(0,y-9),y1=std::min(h,y+10);
   int sum=integral_[y1*(w+1)+x1]-integral_[y0*(w+1)+x1]-integral_[y1*(w+1)+x0]+integral_[y0*(w+1)+x0];
   mask_[y*w+x]=coverage[y*w+x]&&luma_[y*w+x]>=55&&(hi-lo)*100<=hi*45&&(int(luma_[y*w+x])-8)*(x1-x0)*(y1-y0)>sum;
   row+=mask_[y*w+x];paint_integral_[(y+1)*(w+1)+x+1]=paint_integral_[y*(w+1)+x+1]+row;
  }}
  filtered_=mask_;
  for(int y=6;y<h-6;++y)for(int x=1;x<w-1;++x)if(mask_[y*w+x]){
   int sum=paint_integral_[(y+7)*(w+1)+x+2]-paint_integral_[(y-6)*(w+1)+x+2]-paint_integral_[(y+7)*(w+1)+x-1]+paint_integral_[(y-6)*(w+1)+x-1];filtered_[y*w+x]=sum>=8;
  }
  struct Run {int l,r;double center()const{return (l+r)*.5;}int width()const{return r-l;}};
  Run best[32];unsigned best_count=0;int best_y=0;
  for(int y=h*54/100;y<h*88/100;y+=2){Run runs[32];unsigned run_count=0;bool overflow=false;
   for(int x=0;x<w;){if(!filtered_[y*w+x]){++x;continue;}int l=x;while(x<w&&filtered_[y*w+x])++x;if(x-l>=2&&x-l<=24){if(run_count==32){overflow=true;break;}runs[run_count++]={l,x};}}
   if(overflow)continue;
   auto mean=[&](int l,int r){return double(integral_[(y+1)*(w+1)+r]-integral_[y*(w+1)+r]-integral_[(y+1)*(w+1)+l]+integral_[y*(w+1)+l])/std::max(1,r-l);};
   for(unsigned a=0;a<run_count;++a)for(unsigned b=a+1;b<run_count;++b){
    double step=runs[b].center()-runs[a].center();if(step<7||step>45)continue;
    if(runs[b].l-runs[a].r<2||mean(runs[a].l,runs[a].r)-mean(runs[a].r,runs[b].l)<7)continue;
    int minw=std::min(runs[a].width(),runs[b].width()),maxw=std::max(runs[a].width(),runs[b].width());if(maxw>minw*2.8)continue;
    Run chain[32];chain[0]=runs[a];chain[1]=runs[b];unsigned chain_count=2;
    for(unsigned c=b+1;c<run_count;++c){const auto&last=chain[chain_count-1];const auto&r=runs[c];
     if(std::abs(r.center()-last.center()-step)>step*.4||r.l-last.r<2)continue;
     int lo=std::min(minw,r.width()),hi=std::max(maxw,r.width());if(hi>lo*2.8||mean(last.l,last.r)-mean(last.r,r.l)<7)continue;
     chain[chain_count++]=r;minw=lo;maxw=hi;
    }
    if(chain_count<6||chain_count<=best_count)continue;
    const double span=chain[chain_count-1].center()-chain[0].center();
    const double center=(chain[chain_count-1].center()+chain[0].center())*.5;
    // The weak row fallback only represents crossings in front of the car.
    // Fence posts and curb stripes can repeat regularly along the outer edge.
    if(span<45 || center<w*.25 || center>w*.75)continue;
    int paint_width=0;
    for(unsigned i=0;i<chain_count;++i)paint_width+=chain[i].width();
    if(paint_width<.22*(span+maxw))continue;
    unsigned supported=0,through=0,shared_band=0;
    for(unsigned i=0;i<chain_count;++i){
     const auto support=row_support((chain[i].l+chain[i].r-1)*.5,y,coverage,w,h);
     supported+=support.stripe&&!support.through;
     through+=support.through;
     shared_band+=support.shared_band;
    }
    if(supported*3<chain_count*2||through*3>=chain_count||shared_band*3<=chain_count*2)continue;
    std::copy_n(chain,chain_count,best);best_count=chain_count;best_y=y;
   }
  }
  if(best_count>=6){out.count=std::min<unsigned>(best_count,crosswalk_display::CAPACITY);out.quality=std::min(90u,55u+out.count*4);
   for(unsigned i=0;i<out.count;++i){const auto&r=best[i];crosswalk_display::Quad q={{unsigned(r.l),unsigned(best_y-6),unsigned(r.r-1),unsigned(best_y-6),unsigned(r.r-1),unsigned(best_y+6),unsigned(r.l),unsigned(best_y+6)}};out.stripes[i]=q;}}
  return out;
 }
 crosswalk_display::Geometry detect_band(const uint8_t*bgr,const uint8_t*coverage,int w,int h,int first_y){
  crosswalk_display::Geometry out;if(!bgr||!coverage||w!=320||h!=240)return out;
  const unsigned n=w*h;mask_.assign(n,0);seen_.assign(n,0);queue_.reserve(n);
  for(int y=first_y;y<h-2;++y)for(int x=2;x<w-2;++x){int at=y*w+x;if(!coverage[at])continue;const auto*p=bgr+at*3;
   int hi=std::max(p[0],std::max(p[1],p[2])),lo=std::min(p[0],std::min(p[1],p[2])),lum=(29*p[0]+150*p[1]+77*p[2]+128)>>8;
   if(lum<75||(hi-lo)*100>hi*40)continue;
   const int x0=std::max(0,x-9),x1=std::min(w,x+10),y0=std::max(0,y-9),y1=std::min(h,y+10);
   int sum=integral_[y1*(w+1)+x1]-integral_[y0*(w+1)+x1]-integral_[y1*(w+1)+x0]+integral_[y0*(w+1)+x0];
   mask_[at]=(lum-12)*(x1-x0)*(y1-y0)>sum;
  }
  // A transverse stop bar or image border must not join otherwise separate
  // paint stripes into one huge component. Retain only the stripe evidence.
  for(int y=first_y;y<h;++y)for(int x=0;x<w;){if(!mask_[y*w+x]){++x;continue;}int begin=x;while(x<w&&mask_[y*w+x])++x;if(x-begin>40)std::fill(mask_.begin()+y*w+begin,mask_.begin()+y*w+x,0);}
  std::vector<Stripe> stripes;
  for(unsigned start=0;start<n;++start){if(!mask_[start]||seen_[start])continue;queue_.clear();queue_.push_back(start);seen_[start]=1;
   double sx=0,sy=0,sxx=0,sxy=0,syy=0;
   for(size_t k=0;k<queue_.size();++k){int at=queue_[k],x=at%w,y=at/w;sx+=x;sy+=y;sxx+=x*x;sxy+=x*y;syy+=y*y;
    for(int dy=-1;dy<=1;++dy)for(int dx=-1;dx<=1;++dx){int xx=x+dx,yy=y+dy;if(xx<0||xx>=w||yy<0||yy>=h)continue;int j=yy*w+xx;if(mask_[j]&&!seen_[j]){seen_[j]=1;queue_.push_back(j);}}}
   const double count=queue_.size();if(count<12||count>1800)continue;
   int min_y=h,max_y=0;for(int at:queue_){min_y=std::min(min_y,at/w);max_y=std::max(max_y,at/w);}
   if(min_y<=first_y+1&&max_y>=h-4)continue;
   Stripe s={};s.x=sx/count;s.y=sy/count;double xx=sxx/count-s.x*s.x,xy=sxy/count-s.x*s.y,yy=syy/count-s.y*s.y;
   double theta=.5*std::atan2(2*xy,xx-yy);s.ax=std::cos(theta);s.ay=std::sin(theta);if(s.ay<0){s.ax=-s.ax;s.ay=-s.ay;}
   double a0=1e9,a1=-1e9,b0=1e9,b1=-1e9;
   for(int at:queue_){double x=at%w-s.x,y=at/w-s.y,a=x*s.ax+y*s.ay,b=-x*s.ay+y*s.ax;a0=std::min(a0,a);a1=std::max(a1,a);b0=std::min(b0,b);b1=std::max(b1,b);}
   s.length=a1-a0+1;s.width=b1-b0+1;s.fill=count/(s.length*s.width);
   if(s.length<14||s.length>105||s.width<1.8||s.width>22||s.length<s.width*2.2||s.fill<.38||s.ay<.75)continue;
   s.first_y=min_y;s.last_y=max_y;
   const auto covered=[&](double a){
    const int x=int(std::lround(s.x+s.ax*a)),y=int(std::lround(s.y+s.ay*a));
    return x>=2&&x<w-2&&y>=0&&y<h&&coverage[y*w+x];
   };
   s.bounded=min_y>first_y+1&&max_y<h-4&&covered(a0-4)&&covered(a1+4);
   const double aa[]={a0,a1,a1,a0},bb[]={b0,b0,b1,b1};
   for(unsigned j=0;j<4;++j){s.quad.xy[j*2]=std::max(0,std::min(319,int(std::lround(s.x+s.ax*aa[j]-s.ay*bb[j]))));s.quad.xy[j*2+1]=std::max(0,std::min(239,int(std::lround(s.y+s.ay*aa[j]+s.ax*bb[j]))));}
   stripes.push_back(s);
  }
  std::sort(stripes.begin(),stripes.end(),[](const Stripe&a,const Stripe&b){return a.fill*a.length>b.fill*b.length;});if(stripes.size()>32)stripes.resize(32);
  std::sort(stripes.begin(),stripes.end(),[](const Stripe&a,const Stripe&b){return a.x<b.x;});
  std::vector<unsigned> best;
  for(unsigned a=0;a<stripes.size();++a)for(unsigned b=a+1;b<stripes.size();++b){
   auto&A=stripes[a];auto&B=stripes[b];double step=B.x-A.x;if(step<6||step>45)continue;
   std::vector<unsigned> chain={a,b};double spacing=step;
   for(unsigned c=b+1;c<stripes.size();++c){auto&S=stripes[c];auto&last=stripes[chain.back()];double dx=S.x-last.x;
    if(dx<spacing*.55||dx>spacing*1.65||dx<(S.width+last.width)*.60)continue;
    if(std::abs(S.y-A.y)>std::max(7.,A.length*.22)||std::max(S.length,A.length)>1.8*std::min(S.length,A.length)||std::max(S.width,A.width)>2.2*std::min(S.width,A.width)||S.ax*A.ax+S.ay*A.ay<.70)continue;
    chain.push_back(c);spacing=(S.x-A.x)/(chain.size()-1);
   }
   if(chain.size()<4||stripes[chain.back()].x-A.x<36)continue;
   // Require comparable endpoints and a shared vertical band. Several lane
   // markings at different depths must not form a crossing.
   bool good=true;double minlen=1e9,maxlen=0,minw=1e9,maxw=0,total_width=0;
   int first_top=h,last_top=0,first_bottom=h,last_bottom=0;
   unsigned bounded=0;
   for(unsigned c:chain){
    const auto&S=stripes[c];
    minlen=std::min(minlen,S.length);maxlen=std::max(maxlen,S.length);
    minw=std::min(minw,S.width);maxw=std::max(maxw,S.width);total_width+=S.width;
    first_top=std::min(first_top,S.first_y);last_top=std::max(last_top,S.first_y);
    first_bottom=std::min(first_bottom,S.last_y);last_bottom=std::max(last_bottom,S.last_y);
    bounded+=S.bounded;
    if(S.ax*A.ax+S.ay*A.ay<.70)good=false;
   }
   const double span=stripes[chain.back()].x-A.x,end_spread=std::max(10.,minlen*.40);
   double min_pitch=1e9,max_pitch=0;
   for(size_t i=1;i<chain.size();++i){
    const double pitch=stripes[chain[i]].x-stripes[chain[i-1]].x;
    min_pitch=std::min(min_pitch,pitch);max_pitch=std::max(max_pitch,pitch);
   }
   if(!good||maxlen>minlen*1.8||maxw>minw*2.2||bounded*4<chain.size()*3||
      last_top-first_top>end_spread||last_bottom-first_bottom>end_spread||
      first_bottom-last_top<std::max(8.,minlen*.38)||max_pitch>min_pitch*1.85||
      total_width<.24*(span+maxw))continue;
   if(chain.size()>best.size())best=chain;
  }
  if(best.size()>=4){out.count=std::min<unsigned>(best.size(),crosswalk_display::CAPACITY);out.quality=std::min(95u,55u+out.count*5);
   for(unsigned i=0;i<out.count;++i)out.stripes[i]=stripes[best[i]].quad;}
  return out;
 }
};

// Require repeated independent results at the same location. Six-stripe
// proposals had the most false positives, so require one additional result.
class Confirmation {
 struct Extent {unsigned left=320,right=0,near=0,far=240;};
 Extent previous_;
 uint64_t last_source_=0,last_capture_=0;
 unsigned hits_=0;

 static Extent extent(const crosswalk_display::Geometry &geometry) {
  Extent bounds;
  for(unsigned i=0;i<geometry.count;++i)
   for(unsigned j=0;j<8;j+=2) {
    const auto &q=geometry.stripes[i].xy;
    bounds.left=std::min(bounds.left,q[j]);bounds.right=std::max(bounds.right,q[j]);
    bounds.far=std::min(bounds.far,q[j+1]);bounds.near=std::max(bounds.near,q[j+1]);
   }
  return bounds;
 }
public:
 crosswalk_display::Geometry update(const crosswalk_display::Geometry &raw,uint64_t source,uint64_t captured) {
  if(!captured || captured<=last_capture_ || source==last_source_)return {};
  const bool timely=last_capture_ && captured-last_capture_<=550000000ULL;
  last_capture_=captured;
  last_source_=source;
  if(!raw.count){hits_=0;return {};}
  const auto current=extent(raw);
  const int overlap=int(std::min(current.right,previous_.right))-int(std::max(current.left,previous_.left));
  const int span=std::min(int(current.right)-int(current.left),int(previous_.right)-int(previous_.left));
  const bool coherent=hits_ && timely && span>0 && overlap*4>=span*3 &&
      std::abs(int(current.left+current.right)-int(previous_.left+previous_.right))<=42 &&
      std::abs(int(current.far+current.near)-int(previous_.far+previous_.near))<=26;
  previous_=current;
  hits_=coherent?std::min(hits_+1,3u):1u;
  return hits_>=(raw.count==6?3u:2u)?raw:crosswalk_display::Geometry{};
 }
};
}
#endif
