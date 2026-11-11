#ifndef S_PEDESTRIAN_MOTION_H
#define S_PEDESTRIAN_MOTION_H
#include "../../../../common/perception/include/people_history.h"
#include <array>
namespace pedestrian_scene {
struct Motion {unsigned walking=0,direction=0,phase=0;};
class MotionTracker {
 static constexpr uint64_t POSE_HOLD_NS=500000000ULL;
 display_people::History history_;
 struct State {uint32_t generation=0,observations=0;uint64_t until=0;Motion pose;};
 std::array<State,display_people::CAPACITY>states_{};
public:
 unsigned collect(const display_people::Snapshot*s,uint64_t now,int angle,
                  display_people::Person*people,Motion*out,unsigned offset,unsigned capacity,
                  void(*project)(const display_people::Person&,int,double&,double&)=nullptr){
  history_.update(s,now);unsigned count=0;
  for(unsigned i=0;i<display_people::CAPACITY&&offset+count<capacity;++i){
   const auto&t=history_.tracks[i];if(!t.active)continue;auto&state=states_[i];
   if(state.generation!=t.generation){state=State{};state.generation=t.generation;state.until=now+POSE_HOLD_NS;}
   if(state.observations!=t.observations){
    Motion desired;const double dt=t.captured>t.previous_capture?(t.captured-t.previous_capture)*1.e-9:0.;
    if(t.observations>1&&dt>=.05&&dt<=1.){
     const auto&p=t.raw;const auto&q=t.previous;
     double x=(p.x+p.w*.5)-(q.x+q.w*.5),y=double(p.y+p.h)-double(q.y+q.h);
     if((x*x+y*y)/(dt*dt)>1600.+1.e-6){
      if(project){double px,py,qx,qy;project(p,angle,px,py);project(q,angle,qx,qy);if(std::hypot(px-qx,py-qy)>.01){x=px-qx;y=py-qy;}}
      int direction=int(std::lround(std::atan2(x,-y)/.7853981633974483));
      desired.walking=1;desired.direction=unsigned((direction%8+8)%8);
     }
    }
    if(now>=state.until&&(desired.walking!=state.pose.walking||desired.direction!=state.pose.direction)){
     state.pose=desired;state.until=now+POSE_HOLD_NS;
    }
    state.observations=t.observations;
   }
   people[offset+count]=t.shown;out[offset+count]=state.pose;
   if(state.pose.walking)out[offset+count].phase=unsigned(now/125000000ULL)%8;
   ++count;
  }
  return count;
 }
};
}
#endif
