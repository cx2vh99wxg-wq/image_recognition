#ifndef S_PEDESTRIAN_SCENE_H
#define S_PEDESTRIAN_SCENE_H
#include "lane_angle_cue.h"
#include "../../../../common/perception/include/display_people.h"
#include <array>
#include "pedestrian_motion.h"
#include "pedestrian_gait.h"
namespace pedestrian_scene {
using display_people::Person;
struct Actors {
 unsigned count=0;Person people[24]={};Motion motion[24]={};
 // Confidence selects tracks before this point, but is not a drawn pixel.
 // Keep it in the snapshot; do not invalidate the image cache for score jitter.
 bool operator==(const Actors&o)const{
  if(count!=o.count)return false;
  for(unsigned i=0;i<count;++i){const auto&a=people[i];const auto&b=o.people[i];
   if(a.view!=b.view||a.x!=b.x||a.y!=b.y||a.w!=b.w||a.h!=b.h||a.flags!=b.flags||a.gx!=b.gx||a.gy!=b.gy)return false;
   if(motion[i].walking!=o.motion[i].walking||motion[i].direction!=o.motion[i].direction||motion[i].phase!=o.motion[i].phase)return false;
  }
  return true;
 }
 void append(const display_people::Snapshot&s){for(unsigned i=0;i<s.count&&count<24;++i)people[count++]=s.people[i];}
};
struct Placement {double x,y,height;bool calibrated;};
inline Placement locate(const Person&p,int /*angle*/,int width=320,int height=480){
 const auto r=lane_cue::picture_rect(width,height);
 const double nx=(p.x+p.w*.5)/320.,ny=std::min(1.,(p.y+p.h)/240.);
 Placement out={0,0,0,false};
 if(p.view==display_people::Front){
  const double top=r.y+r.height*lane_cue::TOP,seam=r.y+r.height*lane_cue::SEAM,bottom=r.y+r.height*lane_cue::BOTTOM;
  const bool ground=(p.flags&display_people::Ground)!=0;
  const bool ground_x=ground&&!(p.flags&display_people::RoadAligned); // older senders reused gx
  const double proximity=std::max(0.,std::min(1.,ground?(p.gy/1000.+.8)/2.:(ny-.60)/.40));
  // A fixed virtual camera behind the ego car compresses distant ground.
  // Only ground position drives the actors; the steering cue is a separate overlay.
  const double depth=proximity/(1.+.65*(1.-proximity));
  out.y=top+12+(seam-top-16)*depth;
  const double road_depth=(out.y-top)/(bottom-top),half=r.width*(.12+.25*road_depth);
  const double lateral=ground_x?2*(p.gx/1000.-.5):2*(nx-.5);
  const double excess=std::max(0.,std::abs(lateral)-1.);
  const double shown=std::copysign(std::min(1.,std::abs(lateral))+excess/(1.+.65*excess),lateral);
  out.x=width*.5+half*shown;out.height=19+15*depth;out.calibrated=ground;
 }else if(p.view==display_people::Rear){
  out.x=width*.5+(nx-.5)*r.width*.6;out.y=r.y+r.height*(.935+.045*(1-ny));out.height=28+8*ny;
 }else{
  out.x=r.x+r.width*(p.view==display_people::LeftRear?.035+.14*nx:.825+.14*nx);
  out.y=r.y+r.height*(.77+.14*(1-ny));out.height=28+9*ny;
 }
 out.height*=1.35;
 out.x=std::max(out.height*.26,std::min(width-1-out.height*.26,out.x));
 out.y=std::max(out.height+2,std::min(height-3.,out.y));return out;
}
// Cached software 3D mannequin: lit ellipsoids with depth compositing, not
// a stick icon or a copy of the user's low-resolution reference picture.
struct Sprite {
 static constexpr int W=64,H=128;
 std::array<uint32_t,W*H> pixels;
 Sprite(){
  struct Volume{double x,y,rx,ry,z,rz;};
  const Volume body[]={
   {0,.90,.070,.084,0,.067},{0,.80,.035,.047,0,.034},
   {0,.64,.135,.181,0,.085},{0,.445,.113,.077,0,.070},
   {-.170,.676,.047,.112,0,.047},{.170,.676,.047,.112,0,.047},
   {-.193,.507,.038,.106,.010,.039},{.193,.507,.038,.106,.010,.039},
   {-.195,.405,.039,.043,.014,.034},{.195,.405,.039,.043,.014,.034},
   {-.061,.314,.055,.128,0,.052},{.061,.314,.055,.128,0,.052},
   {-.066,.130,.040,.119,.008,.041},{.066,.130,.040,.119,.008,.041},
   {-.068,.020,.049,.026,.025,.060},{.068,.020,.049,.026,.025,.060}};
  for(int y=0;y<H;++y)for(int x=0;x<W;++x){
   unsigned total=0,hits=0;
   for(int sy=0;sy<2;++sy)for(int sx=0;sx<2;++sx){
    const double px=((x+(sx+.5)/2)/W-.5)*.56,py=1-(y+(sy+.5)/2)/H;
    double nearest=-1;unsigned shade=0;
    for(const auto&b:body){const double dx=(px-b.x)/b.rx,dy=(py-b.y)/b.ry,q=dx*dx+dy*dy;
     if(q>1)continue;
     const double z=std::sqrt(1-q),depth=b.z+b.rz*z;
     if(depth<nearest)continue;
     nearest=depth;
     double nx=dx/b.rx,ny=dy/b.ry,nz=z/b.rz,n=std::sqrt(nx*nx+ny*ny+nz*nz);
     const double light=std::max(0.,(-.38*nx+.35*ny+.86*nz)/n);
     shade=(unsigned)(145+109*light);
    }
    if(nearest>=0){++hits;total+=shade;}
   }
   if(!hits){pixels[y*W+x]=0;continue;}
   const unsigned c=total/hits,a=hits*255/4;pixels[y*W+x]=(a<<24)|(c<<16)|(c<<8)|c;
  }
 }
};
inline void project_foot(const Person&p,int angle,double&x,double&y){const auto q=locate(p,angle);x=q.x;y=q.y;}
inline void draw_person(uint32_t*pixels,int stride,int width,int height,const Placement&p,const Motion&motion=Motion{}){
 static const Sprite sprite;
 const uint32_t*source=motion.walking?gait_atlas().frame(motion.direction,motion.phase):sprite.pixels.data();
 const int h=(int)std::lround(p.height),w=(int)std::lround(h*.5),left=(int)std::lround(p.x-w*.5),top=(int)std::lround(p.y-h);
 for(int y=0;y<h;++y)for(int x=0;x<w;++x){
  if(left+x<0||left+x>=width||top+y<0||top+y>=height)continue;
  const auto c=source[(y*Sprite::H/h)*Sprite::W+x*Sprite::W/w];const unsigned a=c>>24;if(!a)continue;
  auto&dst=pixels[(top+y)*stride+left+x];uint32_t out=0xff000000u;
  for(unsigned shift:{0u,8u,16u})out|=((unsigned((dst>>shift)&255)*(255-a)+unsigned((c>>shift)&255)*a+127)/255)<<shift;
  dst=out;
 }
}
inline void draw(uint32_t*pixels,int stride,int width,int height,const Actors&actors,int angle){
 if(!pixels||stride<width||width<8||height<8)return;
 std::array<Placement,24> where;std::array<unsigned,24> order;
 for(unsigned i=0;i<actors.count;++i){where[i]=locate(actors.people[i],angle,width,height);order[i]=i;}
 std::sort(order.begin(),order.begin()+actors.count,[&](unsigned a,unsigned b){return where[a].y<where[b].y;});
 for(unsigned i=0;i<actors.count;++i)draw_person(pixels,stride,width,height,where[order[i]],actors.motion[order[i]]);
}
}
#endif
