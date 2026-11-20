#include "traffic_structure.h"
#include "paired_road_lights.h"
#include "traffic_detector.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <chrono>
using namespace traffic;
constexpr int W=320,H=240;
using Image=std::vector<uint8_t>;
static void rect(Image&im,int x,int y,int w,int h,unsigned r,unsigned g,unsigned b){for(int yy=y;yy<y+h;++yy)for(int xx=x;xx<x+w;++xx){auto*p=&im[(yy*W+xx)*3];p[0]=r;p[1]=g;p[2]=b;}}
static void disk(Image&im,int cx,int cy,int radius,unsigned r,unsigned g,unsigned b){for(int y=cy-radius;y<=cy+radius;++y)for(int x=cx-radius;x<=cx+radius;++x)if((x-cx)*(x-cx)+(y-cy)*(y-cy)<=radius*radius){auto*p=&im[(y*W+x)*3];p[0]=r;p[1]=g;p[2]=b;}}
static Image signal(unsigned state,bool vertical=false){
 Image im(W*H*3,160);rect(im,60,40,vertical?42:118,vertical?118:42,35,35,35);
 for(int n=0;n<3;++n){int cx=81+(vertical?0:37*n),cy=61+(vertical?37*n:0);disk(im,cx,cy,15,7,7,7);disk(im,cx,cy,13,65,65,65);
  if(n+1==(int)state)disk(im,cx,cy,13,state==1?240:state==2?240:10,state==1?10:state==2?185:235,state==3?15:0);
 }return im;
}
int main(int argc,char**argv){
 {
  const VehicleDetection car{display_vehicles::Car, Box(104,142,82,63,.73f)};
  const VehicleDetection crop_truck{display_vehicles::Truck, Box(105,140,78,64,.81f)};
  assert(!prefer_vehicle_refinement(car,crop_truck));
  auto weak_car=car;
  weak_car.box.score=.31f;
  assert(prefer_vehicle_refinement(weak_car,crop_truck));
  auto sharper_car=car;
  sharper_car.box.score=.86f;
  assert(prefer_vehicle_refinement(car,sharper_car));
 }
 {
  constexpr int width=640,height=480;
  Image road(width*height*3,180);
  auto block=[&](int x,int y,int w,int h,int r,int g,int b) {
   for(int yy=y;yy<y+h;++yy)for(int xx=x;xx<x+w;++xx) {
    auto *p=road.data()+3*(yy*width+xx);
    p[0]=r;p[1]=g;p[2]=b;
   }
  };
  auto head=[&](int x) {
   block(x,216,20,9,30,30,30);
   block(x+1,218,5,5,215,27,40);
   block(x+6,218,5,5,180,25,36);
  };
  head(285);
  block(305,220,27,1,92,92,92);
  assert(!paired_road_light(road.data(),width,height,width*3,nullptr,nullptr,true).state);
  head(332);
  bool paired=false;
  assert(paired_road_light(road.data(),width,height,width*3,nullptr,&paired,true).state==TRAFFIC_RED);
  assert(paired);
 }
 {
  Image road(W*H*3,180);
  auto red_head=[&](int x){rect(road,x-2,77,14,7,24,24,24);rect(road,x-1,79,3,3,210,32,58);};
  red_head(90);
  rect(road,102,80,18,1,110,110,110);
  PairedRoadLightTracker tracker;
  assert(!tracker.observe(road.data(),1000000000).state);
  red_head(122);
  Image disconnected=road;
  rect(disconnected,102,80,18,1,180,180,180);
  assert(!paired_road_light(disconnected.data(),W,H,W*3).state);
  assert(!tracker.observe(road.data(),1100000000).state);
  assert(tracker.observe(road.data(),1200000000).state==TRAFFIC_RED);
  rect(road,88,77,14,7,24,24,24);
  assert(tracker.observe(road.data(),1400000000).state==TRAFFIC_RED);
  assert(!tracker.observe(road.data(),2100000000).state);
  std::fill(road.begin(),road.end(),180);
  assert(!tracker.observe(road.data(),2200000000).state);
  // Both heads lie on a bar sloping down to the right in the image.
  red_head(90);
  rect(road,120,81,14,7,24,24,24);
  rect(road,121,83,3,3,210,32,58);
  for (int x=102; x<120; ++x)
      rect(road,x,80+int(std::lround((x-90)*4./32)),1,1,110,110,110);
  assert(paired_road_light(road.data(),W,H,W*3).state==TRAFFIC_RED);
 }
 {
  constexpr int width=640,height=480;
  Image wet(width*height*3,185);
  const auto paint=[&](int x,int y,int w,int h,unsigned r,unsigned g,unsigned b){
   for(int yy=y;yy<y+h;++yy)for(int xx=x;xx<x+w;++xx){
    auto *pixel=&wet[(yy*width+xx)*3];pixel[0]=r;pixel[1]=g;pixel[2]=b;
   }
  };
  for(int x:{338,378}){
   paint(x,269,16,6,55,57,59);
   paint(x+5,270,8,4,49,210,180); // Adjacent green emitters occupy most of the housing.
  }
  assert(!paired_road_light(wet.data(),width,height,width*3,nullptr,nullptr,true).state);
  paint(354,271,24,1,90,90,90);
  PairedRoadLightTracker tracker;
  assert(!tracker.observe(wet.data(),1000000000,width).state);
  assert(tracker.observe(wet.data(),1125000000,width).state==TRAFFIC_GREEN);
 }
 for(unsigned color=1;color<=3;++color)for(bool vertical:{false,true}){
  auto im=signal(color,vertical);auto boxes=locate_three_lamp_housings(im.data(),W,H,W*3);
  assert(!boxes.empty());bool found=false;for(const auto&b:boxes)found|=classify_color(im.data(),W,H,W*3,b).state==color;assert(found);
 }
 Image blank(W*H*3,160);auto im=blank;disk(im,155,61,13,5,235,10);assert(locate_three_lamp_housings(im.data(),W,H,W*3).empty());
 rect(im,60,40,118,42,35,35,35);disk(im,155,61,13,5,235,10);assert(locate_three_lamp_housings(im.data(),W,H,W*3).empty());
 im=blank;rect(im,60,40,118,42,240,5,5);assert(locate_three_lamp_housings(im.data(),W,H,W*3).empty());
 // A rotated/mirrored presentation is permitted only after independent
 // verification of all three circular lamp positions, never from a blob.
 im=signal(1,true);disk(im,81,61,13,10,235,15);
 auto rotated=locate_three_lamp_housings(im.data(),W,H,W*3);assert(!rotated.empty());
 bool green=false;for(const auto&b:rotated)green|=classify_color(im.data(),W,H,W*3,b,true).state==TRAFFIC_GREEN;assert(green);
 im=signal(1);for(int y=0;y<H;++y)for(int x=0;x<W/2;++x)for(int c=0;c<3;++c)std::swap(im[(y*W+x)*3+c],im[(y*W+W-1-x)*3+c]);
 auto mirrored=locate_three_lamp_housings(im.data(),W,H,W*3);assert(!mirrored.empty());
 bool red=false;for(const auto&b:mirrored)red|=classify_color(im.data(),W,H,W*3,b,true).state==TRAFFIC_RED;assert(red);
 im=signal(3);rect(im,60,40,42,42,35,35,35);assert(locate_three_lamp_housings(im.data(),W,H,W*3).empty());
 im=blank;for(int n=0;n<3;++n){rect(im,66+n*37,46,30,30,7,7,7);rect(im,68+n*37,48,26,26,65,65,65);}rect(im,142,48,26,26,5,235,10);assert(locate_three_lamp_housings(im.data(),W,H,W*3).empty());
 if(argc>=2){std::ifstream f(argv[1],std::ios::binary);std::string magic;int w,h,v;f>>magic>>w>>h>>v;f.get();assert(magic=="P6"&&w==W&&h==H);im.resize(W*H*3);f.read((char*)im.data(),im.size());assert(f);
  auto start=std::chrono::steady_clock::now();std::vector<Box> boxes;
  for(int i=0;i<3;++i)boxes=locate_three_lamp_housings(im.data(),W,H,W*3);
  printf("STRUCTURE_IMAGE boxes=%zu mean_ms=%.3f\n",boxes.size(),std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count()/3);
  bool expected=false;
  for(const auto&b:boxes){auto c=classify_color(im.data(),W,H,W*3,b,true);printf("STRUCTURE_BOX %.1f %.1f %.1f %.1f score=%.3f state=%u q=%u reason=%u\n",b.x,b.y,b.w,b.h,b.score,c.state,c.quality,c.reason);expected|=argc==3&&c.state==(unsigned)std::atoi(argv[2]);}
  if(argc==3)assert(expected);
 }
 puts("TRAFFIC STRUCTURE PASS: horizontal/vertical red-yellow-green, colored-object/single-lamp/square-lamp rejection");
}
