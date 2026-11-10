#include "vehicle_layer_state.h"
#include <cassert>
#include <chrono>
#include <cstdio>
#include <vector>
static std::vector<uint32_t> draw(const VehicleLayerState&s){
 std::vector<uint32_t>out(320*480,0xff182438u);
 lane_cue::draw(out.data(),320,320,480,s.signed_angle,s.left_style,s.right_style,
                s.blocked,s.warning_red,s.confidence>=60,s.front_stop);
 crosswalk_scene::draw(out.data(),320,320,480,s.crosswalk,s.signed_angle);
 pedestrian_scene::draw(out.data(),320,320,480,s.actors,s.signed_angle);
 return out;
}
int main(){
 VehicleLayerState a={};a.confidence=85;a.curve=CURVE_LEFT;a.signed_angle=-12;
 a.left_style=1;a.right_style=2;a.actors.count=4;
 for(unsigned i=0;i<a.actors.count;++i)a.actors.people[i]=display_people::box(i,130,100,25,80,.8f);
 auto original=a;const auto reference=draw(a);unsigned cases=0;
 for(unsigned confidence=60;confidence<=100;++confidence){
  auto b=a;b.confidence=confidence;b.curve=CURVE_RIGHT;
  for(unsigned j=0;j<b.actors.count;++j)b.actors.people[j].quality=confidence;
  assert(a==b);assert(draw(b)==reference);++cases;
 }
 auto b=a;b.confidence=59;assert(!(a==b));assert(draw(b)!=reference);
 b=a;b.signed_angle++;assert(!(a==b));b=a;b.front_stop=true;assert(!(a==b));
 b=a;b.left_style=2;assert(!(a==b));b=a;b.blocked=1;assert(!(a==b));
 b=a;b.warning_red=true;assert(!(a==b));b=a;b.actors.people[0].x++;assert(!(a==b));
 b=a;b.actors.people[0].gx++;assert(!(a==b));b=a;b.actors.people[0].flags^=display_people::InRoad;assert(!(a==b));
 b=a;b.actors.motion[0].walking=1;assert(!(a==b));b=a;b.actors.motion[0].direction=1;assert(!(a==b));
 b=a;b.actors.motion[0].phase=1;assert(!(a==b));assert(a.actors.people[0].quality==original.actors.people[0].quality);
 uint64_t checksum=0;unsigned reused=0;
 const auto old_begin=std::chrono::steady_clock::now();
 for(unsigned i=0;i<100;++i){b=a;b.confidence=60+i%41;for(unsigned j=0;j<4;++j)b.actors.people[j].quality=60+i%41;
  checksum+=draw(b)[i*997%(320*480)];}
 const auto begin=std::chrono::steady_clock::now();
 for(unsigned i=0;i<100;++i){b=a;b.confidence=60+i%41;for(unsigned j=0;j<4;++j)b.actors.people[j].quality=60+i%41;
  if(a==b)++reused;else checksum+=draw(b)[0];}
 const auto end=std::chrono::steady_clock::now();
 printf("VEHICLE_CACHE_KEY_PASS exact_pixels=%u invalidation_fields=12 metadata_untouched=1 redundant_score_updates_reused=%u old_redraw_ms=%.4f key_us=%.3f checksum=%llu\n",cases,reused,std::chrono::duration<double,std::milli>(begin-old_begin).count()/100,std::chrono::duration<double,std::micro>(end-begin).count()/100,(unsigned long long)checksum);
}
