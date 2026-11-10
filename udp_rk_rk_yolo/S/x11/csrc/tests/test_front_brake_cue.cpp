#include "front_brake_cue.h"
#include "lane_angle_cue.h"
#include <cassert>
#include <vector>
#include <cstdio>
int main(){
 constexpr uint64_t now=1000000000ULL;
 FrontGuardData status={};status.auto_enabled=1;status.raw_request=1;status.person=1;status.valid=1;front_guard_set_deadline(&status,now+400000000ULL);
 const auto before=status;assert(lane_cue::front_stop_tint(status,now));assert(!memcmp(&status,&before,sizeof(status)));
 status.auto_enabled=0;assert(!lane_cue::front_stop_tint(status,now));status.auto_enabled=1;
 status.raw_request=0;assert(!lane_cue::front_stop_tint(status,now));status.raw_request=1;
 status.raw_request=0;status.blocked=1;assert(lane_cue::front_stop_tint(status,now));
 status.auto_enabled=0;assert(!lane_cue::front_stop_tint(status,now));status.auto_enabled=1;
 status.valid=0;assert(!lane_cue::front_stop_tint(status,now));status.valid=1;status.raw_request=1;
 assert(!lane_cue::front_stop_tint(status,now+400000000ULL));
 // Honor the existing hold until the controller clears it, without making a
 // second "three clear frames" or color decision in the renderer.
 status.person=0;assert(lane_cue::front_stop_tint(status,now));
 constexpr int W=320,H=480;const uint32_t bg=0xff102030;
 std::vector<uint32_t> green(W*H,bg),red=green;
 lane_cue::draw(green.data(),W,W,H,15,1,2,1,true,true,false);
 lane_cue::draw(red.data(),W,W,H,15,1,2,1,true,true,true);
 unsigned changed=0;const auto r=lane_cue::picture_rect(W,H);const int seam=(int)std::ceil(r.y+r.height*lane_cue::SEAM);
 for(int y=0;y<H;++y)for(int x=0;x<W;++x){auto a=green[y*W+x],b=red[y*W+x];
  if(a!=b){++changed;assert(y<seam);assert(((b>>16)&255)>((a>>16)&255));assert(((b>>8)&255)<((a>>8)&255));}
  if(a==lane_cue::WHITE||a==lane_cue::WARNING_RED)assert(a==b);
  if(y>=seam)assert(a==b);
 }
 assert(changed>3000);
 std::fill(green.begin(),green.end(),bg);red=green;
 lane_cue::draw(green.data(),W,W,H,15,1,2,0,false,false,false);
 lane_cue::draw(red.data(),W,W,H,15,1,2,0,false,false,true);assert(green!=red);
 for(int y=seam;y<H;++y)for(int x=0;x<W;++x)assert(green[y*W+x]==red[y*W+x]);
 puts("FRONT BRAKE CUE PASS: controller request/active person GO hold, Auto gate/TTL, unknown-angle red protection, unchanged lower road and controls");
}
