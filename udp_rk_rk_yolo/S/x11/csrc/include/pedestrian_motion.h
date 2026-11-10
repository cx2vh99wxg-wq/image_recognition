#ifndef S_PEDESTRIAN_MOTION_H
#define S_PEDESTRIAN_MOTION_H
#include "../../../../common/perception/include/display_people.h"
#include <array>
namespace pedestrian_scene {
struct Motion {unsigned walking=0,direction=0,phase=0;};
// Per-camera 320x240 foot coordinates, pixels/second, not physical speed.
// Private display state: never publishes hazard, STOP or steering decisions.
class MotionTracker {
 display_people::Snapshot previous_={};
 struct Velocity {double x=0,y=0;bool moving=false;};
 std::array<Velocity,display_people::CAPACITY> velocities_{};
public:
 void append(const display_people::Snapshot&s,uint64_t now,int angle,
             Motion*out,unsigned offset,unsigned capacity,
             void (*project)(const display_people::Person&,int,double&,double&)=nullptr){
  if(!display_people::valid(s)||!display_people::fresh(s,now))return;
  const bool session=previous_.session==s.session;
  const bool newer=!session||int32_t(s.source-previous_.source)>0;
  if(newer){
   std::array<Velocity,display_people::CAPACITY> next{};
   const uint64_t a=display_people::deadline(previous_),b=display_people::deadline(s);
   const double dt=b>a?(b-a)*1.e-9:0.;
   if(session&&dt>=.05&&dt<=.75){
    bool used[display_people::CAPACITY]={},assigned[display_people::CAPACITY]={};
    for(unsigned pass=0;pass<std::min(s.count,previous_.count);++pass){
     double best=1.e20;unsigned ci=s.count,pi=previous_.count;
     for(unsigned i=0;i<s.count;++i)if(!assigned[i])for(unsigned j=0;j<previous_.count;++j)if(!used[j]){
      const auto&p=s.people[i];const auto&q=previous_.people[j];if(p.view!=q.view)continue;
      if(p.h>q.h*2||q.h>p.h*2||p.w>q.w*2||q.w>p.w*2)continue;
      const double dx=(p.x+p.w*.5)-(q.x+q.w*.5),dy=double(p.y+p.h)-double(q.y+q.h);
      const double d=dx*dx+dy*dy,gate=std::max(24.,std::min(90.,dt*250.+std::max(p.h,q.h)*.25));
      if(d>gate*gate)continue;
      const double cost=d+std::pow(double(p.h)-q.h,2)*.25;
      if(cost<best){best=cost;ci=i;pi=j;}
     }
     if(ci==s.count)break;
     assigned[ci]=used[pi]=true;const auto&p=s.people[ci];const auto&q=previous_.people[pi];
     auto&v=next[ci];v.x=((p.x+p.w*.5)-(q.x+q.w*.5))/dt;v.y=(double(p.y+p.h)-double(q.y+q.h))/dt;
     v.moving=v.x*v.x+v.y*v.y>1600.+1.e-6;
     if(project){double px,py,qx,qy;project(p,angle,px,py);project(q,angle,qx,qy);
      if(std::hypot(px-qx,py-qy)>.01){v.x=px-qx;v.y=py-qy;}}
    }
   }
   previous_=s;velocities_=next;
  }
  if(previous_.session!=s.session||previous_.source!=s.source)return;
  for(unsigned i=0;i<s.count&&offset+i<capacity;++i){
   auto&d=out[offset+i];const auto&v=velocities_[i];d={};if(!v.moving)continue;
   // Same display projection and same road angle for both observations;
   // changing the road cue alone cannot create measured pedestrian motion.
   const double yaw=std::atan2(v.x,-v.y);
   int direction=int(std::lround(yaw/.7853981633974483));direction=(direction%8+8)%8;
   d.walking=1;d.direction=unsigned(direction);d.phase=unsigned(now/125000000ULL)%8;
  }
 }
};
}
#endif
