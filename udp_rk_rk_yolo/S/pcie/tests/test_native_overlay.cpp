#include "yolo_integration.h"
#include "person_alert_effects.h"
#include "camera_orientation.h"
#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <vector>
#include <sys/time.h>
extern "C" int person_alert_cache_wait(unsigned);
extern "C" int test_gettimeofday(struct timeval *t,void*){t->tv_sec=1000;t->tv_usec=0;return 0;}
int main(){
 constexpr int W=640,H=480,N=W*H,G=32;const uint16_t marker=0x9a71;
 std::vector<uint16_t>source(N+2*G,marker),native=source,expected=source;
 std::vector<uint8_t>rgb(N*3),scratch(rgb.size()),check(rgb.size());
 for(int i=0;i<N;++i)source[G+i]=uint16_t(i*173+911);
 const auto untouched=source;
 for(bool repair:{false,true}){
  unpack_rgb565_tiles_180(source.data()+G,rgb.data(),W,H,320,240,repair);
  frame_pack_rgb565(rgb.data(),expected.data()+G,N);
  orient_rgb565_tiles_180(source.data()+G,native.data()+G,W,H,320,240,repair);
  assert(native==expected&&source==untouched);
  frame_unpack_rgb565(native.data()+G,check.data(),N);assert(check==rgb);
 }
 const auto oriented=native;
 for(int scene=0;scene<20;++scene){
  detect_result_group_t raw={};raw.count=scene%7;if(scene==17)raw.count=64;if(scene==18)raw.count=10;if(scene==19)raw.count=12;
  for(int i=0;i<raw.count;++i){auto&r=raw.results[i];strcpy(r.name,i%3==2?"car":"person");r.prop=.6f;
   r.box={i*47%620-10,i*47%620+20+scene*3,i*31%460-10,i*31%460+30+scene*5};}
  auto retained=raw;if(!retained.count){retained.count=1;strcpy(retained.results[0].name,"person");retained.results[0].box={330,410,120,220};}
  const auto before=raw;
  for(int phase=0;phase<2;++phase){
   frame_unpack_rgb565(oriented.data()+G,rgb.data(),N);g_alert_state={1000000,phase,1};
   draw_detection_alerts(rgb.data(),W,H,&raw);assert(person_alert_cache_wait(5000));
   frame_unpack_rgb565(oriented.data()+G,rgb.data(),N);draw_detection_alerts(rgb.data(),W,H,&raw);draw_detection_boxes(rgb.data(),W,H,&retained);
   expected=oriented;frame_pack_rgb565(rgb.data(),expected.data()+G,N);
   native=oriented;assert(!draw_detection_alerts565(native.data()+G,W,H,&raw,scratch.data()));draw_detection_boxes565(native.data()+G,W,H,&retained);
   assert(native==expected&&source==untouched&&!memcmp(&raw,&before,sizeof(raw)));
   for(int i=0;i<G;++i)assert(native[i]==marker&&native[N+G+i]==marker);
  }
 }
 detect_result_group_t raw={};raw.count=4;
 for(int i=0;i<4;++i){strcpy(raw.results[i].name,"person");raw.results[i].box={i%2*320+40,i%2*320+200,i/2*240+20,i/2*240+220};raw.results[i].prop=.8f;}
 draw_detection_alerts(rgb.data(),W,H,&raw);assert(person_alert_cache_wait(5000));
 auto bench=[&](bool fast){auto start=std::chrono::steady_clock::now();for(int i=0;i<60;++i){
  if(fast){orient_rgb565_tiles_180(source.data()+G,native.data()+G,W,H,320,240,true);if(i%3==0)frame_unpack_rgb565(native.data()+G,rgb.data(),N);
   assert(!draw_detection_alerts565(native.data()+G,W,H,&raw,scratch.data()));draw_detection_boxes565(native.data()+G,W,H,&raw);
  }else {unpack_rgb565_tiles_180(source.data()+G,rgb.data(),W,H,320,240,true);draw_detection_alerts(rgb.data(),W,H,&raw);draw_detection_boxes(rgb.data(),W,H,&raw);frame_pack_rgb565(rgb.data(),expected.data()+G,N);}
 }return std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count()/60;};
 const double old=bench(false),fast=bench(true);
 printf("NATIVE_OVERLAY_PASS cases=40 pixels_identical=1 source_and_guards_unchanged=1 inference_input_bytes_identical=1 dense_legacy_fallback=1 legacy_ms=%.4f native_ms=%.4f\n",old,fast);
}
