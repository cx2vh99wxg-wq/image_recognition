#include "pedestrian_scene.h"
#include <cassert>
#include <cstdio>
#include <chrono>
using namespace display_people;
static Snapshot sample(unsigned source,unsigned x,uint64_t time){
 Snapshot s={};s.session=10;s.source=source;s.ready=1;s.count=1;s.people[0]=box(Front,x,100,24,70,.9f);expires(s,time+TTL);return s;
}
int main(){
 constexpr uint64_t start=1000000000ULL,dt=200000000ULL;
 pedestrian_scene::MotionTracker tracker;pedestrian_scene::Motion m[24]={};Person shown[24]={};
 auto observe=[&](const Snapshot* s,uint64_t now){return tracker.collect(s,now,0,shown,m,0,24);};
 auto a=sample(1,80,start);assert(observe(&a,start)==1&&!m[0].walking);
 auto b=sample(2,88,start+dt);observe(&b,start+dt);assert(!m[0].walking); // exactly 40 px/s
 auto c=sample(3,97,start+2*dt);observe(&c,start+2*dt);assert(!m[0].walking); // initial standing holds 1s
 for(unsigned i=3;i<=5;++i){c=sample(i+1,97+(i-2)*9,start+i*dt);observe(&c,start+i*dt);}
 assert(m[0].walking&&m[0].direction==2);
 const auto phase=m[0].phase;observe(&c,start+5*dt+130000000ULL);assert(m[0].walking&&m[0].phase!=phase);
 for(unsigned i=6;i<=10;++i){c=sample(i+1,124-(i-5)*12,start+i*dt);observe(&c,start+i*dt);if(i<10)assert(m[0].direction==2);}
 assert(m[0].walking&&m[0].direction==6);
 auto missing=c;missing.source++;missing.count=0;expires(missing,start+11*dt+TTL);
 assert(observe(&missing,start+11*dt)==1&&m[0].direction==6);
 assert(observe(nullptr,start+10*dt+999999999ULL)==1);
 assert(observe(nullptr,start+10*dt+1000000000ULL)==0); // no duplicate renewal
 auto restart=c;restart.session++;
 expires(restart,start+4*1000000000ULL+TTL);observe(&restart,start+4*1000000000ULL);assert(!m[0].walking);
 // NMS ordering/camera identity must not create extra people.
 a=sample(10,60,start);a.count=2;a.people[1]=box(Rear,180,100,24,70,.9f);
 pedestrian_scene::MotionTracker order;assert(order.collect(&a,start,0,shown,m,0,24)==2);
 b=a;b.source++;b.people[0]=a.people[1];b.people[1]=a.people[0];b.people[1].x+=12;expires(b,start+dt+TTL);
 assert(order.collect(&b,start+dt,0,shown,m,0,24)==2);
 const auto begin=std::chrono::steady_clock::now();const auto&atlas=pedestrian_scene::gait_atlas();
 const double warm=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-begin).count();
 assert(std::memcmp(atlas.frame(2,1),atlas.frame(2,5),64*128*4));
 assert(std::memcmp(atlas.frame(2,1),atlas.frame(6,1),64*128*4));
 constexpr int W=320,H=480,stride=336,guard=32;const uint32_t marker=0xff102030;
 std::vector<uint32_t>canvas(guard+stride*H+guard,marker);pedestrian_scene::Actors actors;
 for(unsigned i=0;i<4;++i){actors.people[i]=box(i,140,100,30,80,.9f);actors.motion[i]={1,i*2,0};}actors.count=4;
 const auto draw_begin=std::chrono::steady_clock::now();
 for(unsigned frame=0;frame<240;++frame){for(unsigned i=0;i<4;++i)actors.motion[i].phase=frame%8;pedestrian_scene::draw(canvas.data()+guard,stride,W,H,actors,15);}
 const double ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-draw_begin).count()/240;
 for(unsigned i=0;i<guard;++i)assert(canvas[i]==marker&&canvas[canvas.size()-1-i]==marker);
 for(int y=0;y<H;++y)for(int x=W;x<stride;++x)assert(canvas[guard+y*stride+x]==marker);
 printf("PEDESTRIAN_MOTION_PASS threshold_strict_gt40=1 duplicate_speed_ignored=1 session_order_ttl=1 directions=8 phases=8 four_walking_ms=%.4f atlas_startup_ms=%.2f bytes=%zu\n",ms,warm,atlas.pixels.size()*4);
}
