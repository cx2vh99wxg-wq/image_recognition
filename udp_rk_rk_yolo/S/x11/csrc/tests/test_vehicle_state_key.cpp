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
 vehicle_scene::draw(out.data(),320,320,480,s.vehicles,s.signed_angle);
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

 display_vehicles::Snapshot sent = {};
 sent.session = 4;
 sent.source = 17;
 sent.count = 4;
 const uint64_t now = 5000000000ULL;
 display_vehicles::expires(sent, now + display_vehicles::TTL);
 for (unsigned i = 0; i < sent.count; ++i)
     sent.vehicles[i] = {vehicle_scene::kinds[i], display_people::box(display_people::Front, 40+i*68, 90, 23, 50, .8f)};
 uint8_t packet[display_vehicles::WIRE_SIZE];
 assert(display_vehicles::encode(packet, sent, now));
 display_vehicles::Receiver receiver;
 display_vehicles::Snapshot received = {};
 assert(receiver.ingest(packet, sizeof(packet), now, now, received));
 assert(!receiver.ingest(packet, sizeof(packet), now+1000000, now+1000000, received));
 vehicle_scene::Tracker tracker;
 b = a;
 b.vehicles = tracker.update(&received, now);
 assert(b.vehicles.count == 4 && !(a == b));
 assert(draw(b) != reference);
 const auto verify_top_vehicles = [&] {
     using namespace display_vehicles;
     Snapshot crowded = {};
     crowded.session = 9;
     crowded.source = 1;
     expires(crowded, now + TTL);
     const Kind classes[] = {Car, Bus, Car, Bus, Car, Bus, Car, Bicycle, Motorcycle};
     const unsigned scores[] = {55, 93, 68, 78, 85, 60, 99, 25, 30};
     crowded.count = sizeof(classes) / sizeof(classes[0]);
     for (unsigned i = 0; i < crowded.count; ++i) {
         auto foot = display_people::box(display_people::Front, 8 + 32 * i, 90, 15, 40, .8f);
         foot.quality = scores[i];
         crowded.vehicles[i] = {classes[i], foot};
     }
     vehicle_scene::Tracker crowded_tracker;
     const auto selected = [](const vehicle_scene::Geometry &geometry, unsigned index) {
         return std::any_of(geometry.vehicles, geometry.vehicles + geometry.count,
                            [index](const Vehicle &vehicle) { return vehicle.footprint.x == 8 + 32 * index; });
     };
     const auto first = crowded_tracker.update(&crowded, now);
     assert(first.count == 7 && !selected(first, 0) && !selected(first, 5));
     for (unsigned i : {1u, 2u, 3u, 4u, 6u, 7u, 8u})
         assert(selected(first, i));
     crowded.vehicles[2].footprint.quality = 95;
     ++crowded.source;
     assert(crowded_tracker.update(&crowded, now + 1000000) == first);
     crowded.vehicles[0].footprint.quality = 97;
     crowded.vehicles[3].footprint.quality = 70;
     ++crowded.source;
     const auto second = crowded_tracker.update(&crowded, now + 2000000);
     assert(second.count == 7 && selected(second, 0) && !selected(second, 3));
     assert(selected(second, 7) && selected(second, 8));
 };
 verify_top_vehicles();
 constexpr int panel_width=300, panel_height=480;
 assert(std::abs(vehicle_scene::lateral_spacing(0., panel_height) - 1.80) < 1.e-9);
 assert(std::abs(vehicle_scene::lateral_spacing(239.5, panel_height) - 1.45) < 1.e-9);
 assert(std::abs(vehicle_scene::lateral_spacing(479., panel_height) - 1.10) < 1.e-9);
 auto actor=display_people::box(display_people::Front,140,80,20,100,.9f);
 actor.flags=display_people::Ground;actor.gy=800;actor.gx=500;
 const auto at_left=[](display_people::Person foot) {
     while(foot.gx>-1500 && pedestrian_scene::locate(foot,0,panel_width,panel_height).x>100.)
         --foot.gx;
     assert(std::abs(pedestrian_scene::locate(foot,0,panel_width,panel_height).x-100.)<.2);
     return foot;
 };
 auto left_actor=at_left(actor);
 auto right_actor=left_actor;right_actor.gx=1000-left_actor.gx;
 auto far_actor=actor;far_actor.gy=-500;
 const auto far_left_actor=at_left(far_actor);
 const auto visible_bounds=[&](uint32_t kind,const display_people::Person &foot) {
     vehicle_scene::Geometry geometry;
     geometry.count=1;geometry.vehicles[0]={kind,foot};
     std::vector<uint32_t> pixels(panel_width*panel_height,0);
     vehicle_scene::draw(pixels.data(),panel_width,panel_width,panel_height,geometry,0);
     std::array<int,4> bounds{{panel_width,panel_height,-1,-1}};
     for(int y=0;y<panel_height;++y)for(int x=0;x<panel_width;++x)
         if(pixels[y*panel_width+x]) {
             bounds[0]=std::min(bounds[0],x);bounds[1]=std::min(bounds[1],y);
             bounds[2]=std::max(bounds[2],x);bounds[3]=std::max(bounds[3],y);
         }
     return bounds;
 };
 for (unsigned i = 0; i < 4; ++i) {
     const auto kind=vehicle_scene::kinds[i];
     const auto &image = vehicle_scene::sprite(kind);
     assert(image.right > image.left && image.bottom > image.top);
     assert(std::abs(image.left + image.right - vehicle_scene::Sprite::SIDE) <= 1);
     for (unsigned j = 0; j < i; ++j)
         assert(image.pixels != vehicle_scene::sprite(vehicle_scene::kinds[j]).pixels);
     const auto center=visible_bounds(kind,actor);
     const auto left=visible_bounds(kind,left_actor),right=visible_bounds(kind,right_actor);
     const bool wider=kind==display_vehicles::Car || kind==display_vehicles::Bus;
     const auto position=pedestrian_scene::locate(left_actor,0,panel_width,panel_height);
     const int offset=wider ? std::lround((150.-position.x)*(1.80-.70*position.y/479.)) : 50;
     assert(left[0]==center[0]-offset && right[0]==center[0]+offset);
     assert(left[3]==center[3] && right[3]==center[3]);
     const auto far_position=pedestrian_scene::locate(far_left_actor,0,panel_width,panel_height);
     const int far_offset=wider ? std::lround((150.-far_position.x)*(1.80-.70*far_position.y/479.)) : 50;
     assert(visible_bounds(kind,far_left_actor)[0]==visible_bounds(kind,far_actor)[0]-far_offset);
     assert(wider?far_offset>offset:far_offset==offset);
     const double old_scale=kind==display_vehicles::Bus?1.18:kind==display_vehicles::Car?.98:.80;
     const auto placement=pedestrian_scene::locate(actor,0,panel_width,panel_height);
     assert(center[3]-center[1]+1>std::lround(placement.height*old_scale*1.20));
     if(wider) {
         auto edge=actor;edge.gx=-2000;
         const auto bounds=visible_bounds(kind,edge);
         assert(bounds[2]-bounds[0]==center[2]-center[0]);
     }
 }
 assert(tracker.update(&received, now+display_people::History::HOLD).count == 0);
 packet[25] ^= 1;
 display_vehicles::Receiver corrupt;
 assert(!corrupt.ingest(packet, sizeof(packet), now, now, received));
 std::puts("FRONT VEHICLES PASS: combined car/bus top five, depth spacing, edge bounds, wire integrity");
}
