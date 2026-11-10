#ifndef M_CROSSWALK_DETECTOR_H
#define M_CROSSWALK_DETECTOR_H
#include "../../../common/perception/include/crosswalk_display.h"
#include <array>
#include <cmath>
#include <vector>
namespace crosswalk {
struct Stripe {double x,y,length,width,ax,ay,fill;crosswalk_display::Quad quad;};
// Fixed-size CPU evidence: neutral bright components, elongation, shared band,
// comparable widths/lengths and regularly spaced repetition. No model/controls.
class Detector {
 std::vector<int> integral_,queue_;
 std::vector<uint8_t> mask_,seen_;
 public:
 crosswalk_display::Geometry detect(const uint8_t*bgr,const uint8_t*coverage,int w=320,int h=240){
  auto result=detect_band(bgr,coverage,w,h,h*55/100);
  if(!result.count)result=detect_band(bgr,coverage,w,h,h*65/100);
  return result;
 }
 private:
 crosswalk_display::Geometry detect_band(const uint8_t*bgr,const uint8_t*coverage,int w,int h,int first_y){
  crosswalk_display::Geometry out;if(!bgr||!coverage||w!=320||h!=240)return out;
  const unsigned n=w*h;integral_.assign((w+1)*(h+1),0);mask_.assign(n,0);seen_.assign(n,0);queue_.reserve(n);
  for(int y=0;y<h;++y){int row=0;for(int x=0;x<w;++x){const auto*p=bgr+(y*w+x)*3;row+=(29*p[0]+150*p[1]+77*p[2]+128)>>8;integral_[(y+1)*(w+1)+x+1]=integral_[y*(w+1)+x+1]+row;}}
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
   Stripe s={};s.x=sx/count;s.y=sy/count;double xx=sxx/count-s.x*s.x,xy=sxy/count-s.x*s.y,yy=syy/count-s.y*s.y;
   double theta=.5*std::atan2(2*xy,xx-yy);s.ax=std::cos(theta);s.ay=std::sin(theta);if(s.ay<0){s.ax=-s.ax;s.ay=-s.ay;}
   double a0=1e9,a1=-1e9,b0=1e9,b1=-1e9;
   for(int at:queue_){double x=at%w-s.x,y=at/w-s.y,a=x*s.ax+y*s.ay,b=-x*s.ay+y*s.ax;a0=std::min(a0,a);a1=std::max(a1,a);b0=std::min(b0,b);b1=std::max(b1,b);}
   s.length=a1-a0+1;s.width=b1-b0+1;s.fill=count/(s.length*s.width);
   if(s.length<8||s.length>105||s.width<1.8||s.width>22||s.length<s.width*2.2||s.fill<.38||s.ay<.75)continue;
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
   // Validate the initial pair too, then reject an isolated pair of lanes.
   bool good=true;double minlen=1e9,maxlen=0,minw=1e9,maxw=0,miny=1e9,maxy=0;
   for(auto c:chain){const auto&S=stripes[c];minlen=std::min(minlen,S.length);maxlen=std::max(maxlen,S.length);minw=std::min(minw,S.width);maxw=std::max(maxw,S.width);miny=std::min(miny,S.y);maxy=std::max(maxy,S.y);if(S.ax*A.ax+S.ay*A.ay<.70)good=false;}
   if(!good||maxlen>minlen*1.8||maxw>minw*2.2||maxy-miny>std::max(7.,minlen*.3))continue;
   if(chain.size()>best.size())best=chain;
  }
  if(best.size()>=4){out.count=std::min<unsigned>(best.size(),crosswalk_display::CAPACITY);out.quality=std::min(95u,55u+out.count*5);
   for(unsigned i=0;i<out.count;++i)out.stripes[i]=stripes[best[i]].quad;}
  return out;
 }
};
}
#endif
