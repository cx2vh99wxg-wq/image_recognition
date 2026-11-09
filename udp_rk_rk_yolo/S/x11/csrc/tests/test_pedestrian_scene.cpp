#include "pedestrian_scene.h"
#include "vehicle_artwork.h"
#include "../../../../common/perception/include/display_people_wire.h"
#include "../../../pcie/include/person_scene_snapshot.h"
#include <cassert>
#include <cstdio>
#include <chrono>
int main(){
 using namespace display_people;
 constexpr uint64_t now=1000000000ULL;
 Snapshot s={};s.session=12;s.source=44;s.ready=1;expires(s,now+500000000ULL);
 auto p=box(Front,140,110,30,60,.9f);p.flags=Ground|InRoad;p.gx=500;p.gy=600;add(s,p);
 add(s,p);assert(s.count==1);p.x=30;add(s,p);assert(s.count==2);
 uint8_t wire[WIRE_SIZE];assert(encode(wire,s,now));Receiver rx;Snapshot received={};
 assert(rx.ingest(wire,sizeof(wire),now,now,received)&&received.count==2);
 assert(rx.ingest(wire,sizeof(wire),now+100000000ULL,now+100000000ULL,received));
 assert(deadline(received)==now+500000000ULL); // replay cannot refresh a person
 wire[40]^=1;assert(!rx.ingest(wire,sizeof(wire),now,now,received));wire[40]^=1;
 auto old=s;old.source--;assert(encode(wire,old,now));assert(!rx.ingest(wire,sizeof(wire),now,now,received));
 assert(!fresh(s,now+500000000ULL));s.count=13;assert(!valid(s));s.count=2;
 auto left=box(Front,10,120,20,50,.9f),right=left;right.x=240;
 assert(pedestrian_scene::locate(left,0).x<pedestrian_scene::locate(right,0).x);
 right=left;right.y=165;assert(pedestrian_scene::locate(left,0).y<pedestrian_scene::locate(right,0).y);
 auto rear=box(Rear,120,100,40,100,.8f),lr=rear,rr=rear;lr.view=LeftRear;rr.view=RightRear;
 assert(pedestrian_scene::locate(lr,0).x<80&&pedestrian_scene::locate(rr,0).x>240);
 assert(pedestrian_scene::locate(rear,0).y>440);
 detect_result_group_t detections={};detections.count=4;
 for(int i=0;i<4;++i){auto&r=detections.results[i];strcpy(r.name,"person");r.prop=.9f;r.box.left=(i%2)*320+80;r.box.right=r.box.left+30;r.box.top=(i/2)*240+80;r.box.bottom=r.box.top+100;}
 auto scene=s_vision::person_scene(detections,true,now,23,8);assert(scene.count==3);unsigned views=0;
 for(unsigned i=0;i<scene.count;++i){views|=1u<<scene.people[i].view;assert(scene.people[i].y==60);}
 assert(views==14); // duplicated rear center merged, three cameras retained
 auto only_duplicate=detections;only_duplicate.count=1;
 assert(s_vision::person_scene(only_duplicate,true,now,23,9).count==1);
 assert(!s_vision::person_scene(detections,false,now,23,8).ready);
 pedestrian_scene::Actors actors;actors.append(scene);s.count=1;actors.append(s);assert(actors.count==4);
 constexpr int stride=336,guard=16;std::vector<uint32_t> canvas(stride*480+2*guard,0x11223344);
 auto clear=[&]{for(int y=0;y<480;++y)std::fill_n(canvas.data()+guard+y*stride,320,0xff080b20);};
 clear();pedestrian_scene::draw(canvas.data()+guard,stride,320,480,actors,15);
 unsigned white=0,shade=0;for(int y=0;y<480;++y)for(int x=0;x<320;++x){auto c=canvas[guard+y*stride+x];white+=((c>>16)&255)>205;shade+=c!=0xff080b20;}
 assert(white>20&&shade>150);
 for(int i=0;i<guard;++i)assert(canvas[i]==0x11223344&&canvas[canvas.size()-1-i]==0x11223344);
 for(int y=0;y<480;++y)for(int x=320;x<stride;++x)assert(canvas[guard+y*stride+x]==0x11223344);
 clear();vehicle_artwork::person_glow(canvas.data()+guard,stride,0,374,true);auto bright=canvas;
 clear();vehicle_artwork::person_glow(canvas.data()+guard,stride,0,374,false);
 assert(((bright[guard+374*stride]>>16)&255)>((canvas[guard+374*stride]>>16)&255));
 assert(canvas[guard+140*stride]==0xff080b20); // warning moved down, same radial glow
 const auto begin=std::chrono::steady_clock::now();for(int i=0;i<200;++i){clear();pedestrian_scene::draw(canvas.data()+guard,stride,320,480,actors,15);}
 printf("PEDESTRIAN SCENE PASS: four directions, bbox foot positioning, calibrated front corridor, duplicate removal, bounded wire/TTL/order, 3D shading and guards; four_people_ms=%.3f\n",std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-begin).count()/200);
}
