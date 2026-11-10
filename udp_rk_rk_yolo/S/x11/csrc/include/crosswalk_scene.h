#ifndef S_CROSSWALK_SCENE_H
#define S_CROSSWALK_SCENE_H
#include "lane_angle_cue.h"
#include "../../../../common/perception/include/crosswalk_display.h"
namespace crosswalk_scene {
inline double median(double *v,unsigned n){
 for(unsigned i=1;i<n;++i){double x=v[i];unsigned j=i;while(j&&v[j-1]>x){v[j]=v[j-1];--j;}v[j]=x;}
 return n&1?v[n/2]:(v[n/2-1]+v[n/2])*.5;
}
// Full crossing row: longitudinal placement comes from detected paint, while
// horizontal span always covers the road between the two displayed boundaries.
inline void draw(uint32_t*pixels,int stride,int width,int height,const crosswalk_display::Geometry&g,int angle){
 if(!pixels||stride<width||width<8||height<8||g.count<4||g.count>crosswalk_display::CAPACITY)return;
 double centers[crosswalk_display::CAPACITY];
 for(unsigned i=0;i<g.count;++i){double lo=239,hi=0;
  for(unsigned j=0;j<4;++j){const auto&q=g.stripes[i];if(q.xy[j*2]>=320||q.xy[j*2+1]>=240)return;
   lo=std::min(lo,double(q.xy[j*2+1]));hi=std::max(hi,double(q.xy[j*2+1]));}
  centers[i]=(lo+hi)*.5;
 }
 const auto r=lane_cue::picture_rect(width,height);const double top=r.y+r.height*lane_cue::TOP,seam=r.y+r.height*lane_cue::SEAM,bottom=r.y+r.height*lane_cue::BOTTOM;
 // Fixed medium-long extent: 42 px on the 320x480 vehicle panel. Only
 // the center follows evidence. Clamp the whole band at road end points.
 const int band=std::max(1,int(std::lround(height*42./480.)));
 const double center_y=std::max(top+band*.5,std::min(seam-4-band*.5,
     top+20+(seam-top-24)*median(centers,g.count)/239.));
 const double theta=std::max(-85,std::min(85,angle))*.017453292519943295,bend=std::max(-r.width*.28,std::min(r.width*.28,(seam-top)*.5*std::tan(theta)));
 const int first=std::max(0,int(std::lround(center_y-band*.5))),last=std::min(height-1,first+band-1);
 for(int y=first;y<=last;++y){
  const double t=(seam-y)/(seam-top),road=(y-top)/(bottom-top),half=r.width*(.12+.25*road);
  const double left=width*.5+bend*t*t-half+8,right=width*.5+bend*t*t+half-8;if(right<=left)continue;
  for(int x=std::max(0,int(std::ceil(left)));x<=std::min(width-1,int(std::floor(right)));++x){
   const double stripe=(x-left)/(right-left)*9.;if(stripe-std::floor(stripe)>.66)continue;
   uint32_t&c=pixels[y*stride+x],out=0xff000000u;
   for(unsigned s:{0u,8u,16u})out|=(((c>>s&255)*55+255*200+127)/255)<<s;
   c=out;
  }
 }
}
}
#endif
