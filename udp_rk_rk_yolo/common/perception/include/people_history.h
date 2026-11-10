#ifndef PERCEPTION_PEOPLE_HISTORY_H
#define PERCEPTION_PEOPLE_HISTORY_H
#include "display_people.h"
#include <array>
namespace display_people {
// Display only. Original frame identity and detector freshness are untouched.
class History {
 uint32_t session_=0,source_=0,generation_=0;
 static Person smooth(const Person&a,const Person&b){
  Person p=b;p.x=(a.x+b.x+1)/2;p.y=(a.y+b.y+1)/2;p.w=(a.w+b.w+1)/2;p.h=(a.h+b.h+1)/2;
  p.w=std::min(p.w,320-p.x);p.h=std::min(p.h,240-p.y);
  if((a.flags&Ground)&&(b.flags&Ground))p.gy=(a.gy+b.gy)/2;
  if(((a.flags&Ground)&&(b.flags&Ground))||((a.flags&RoadAligned)&&(b.flags&RoadAligned)))p.gx=(a.gx+b.gx)/2;
  return p;
 }
public:
 static constexpr uint64_t HOLD=1000000000ULL;
 struct Track {Person raw={},previous={},shown={};uint64_t seen=0,captured=0,previous_capture=0;uint32_t generation=0,observations=0;bool active=false;};
 std::array<Track,CAPACITY> tracks{};
 void update(const Snapshot*s,uint64_t now){
  for(auto&t:tracks)if(t.active&&(now<t.seen||now-t.seen>=HOLD))t.active=false;
  if(!s||!valid(*s)||!fresh(*s,now))return;
  if(s->session!=session_){for(auto&t:tracks)t.active=false;session_=s->session;source_=s->source-1;}
  if(int32_t(s->source-source_)<=0)return;
  source_=s->source;
  bool used[CAPACITY]={},assigned[CAPACITY]={};
  const uint64_t capture=deadline(*s)>=TTL?deadline(*s)-TTL:0;
  for(unsigned pass=0;pass<CAPACITY;++pass){double best=1.e20;unsigned i_best=CAPACITY,j_best=CAPACITY;
   for(unsigned i=0;i<s->count;++i)if(!assigned[i])for(unsigned j=0;j<CAPACITY;++j)if(tracks[j].active&&!used[j]){
    const auto&p=s->people[i];const auto&q=tracks[j].raw;if(p.view!=q.view)continue;
    if(p.h>q.h*2||q.h>p.h*2||p.w>q.w*2||q.w>p.w*2)continue;
    const double dx=(p.x+p.w*.5)-(q.x+q.w*.5),dy=double(p.y+p.h)-double(q.y+q.h);
    const double dt=now>=tracks[j].seen?(now-tracks[j].seen)*1.e-9:0.;
    const double gate=std::max(24.,std::min(90.,dt*250.+std::max(p.h,q.h)*.25));
    if(dx*dx+dy*dy>gate*gate)continue;
    const double cost=dx*dx+dy*dy+.25*std::pow(double(p.h)-q.h,2);
    if(cost<best){best=cost;i_best=i;j_best=j;}
   }
   if(i_best==CAPACITY)break;
   auto&t=tracks[j_best];t.previous=t.raw;t.previous_capture=t.captured;t.raw=s->people[i_best];t.shown=smooth(t.shown,t.raw);
   t.captured=capture;t.seen=now;++t.observations;used[j_best]=assigned[i_best]=true;
  }
  for(unsigned i=0;i<s->count;++i)if(!assigned[i]){
   unsigned slot=CAPACITY;
   for(unsigned j=0;j<CAPACITY;++j)if(!tracks[j].active){slot=j;break;}
   if(slot==CAPACITY)continue;
   auto&t=tracks[slot];t=Track{};t.raw=t.shown=s->people[i];t.seen=now;t.captured=capture;t.active=true;t.generation=++generation_;t.observations=1;
  }
 }
};
}
#endif
