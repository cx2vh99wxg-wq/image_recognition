#include "front_person_wire.h"
#include "front_person_guard.h"
#include <cassert>
#include <cstdio>
int main(){
 uint64_t now=10000000000ULL;FrontPersonData d={};d.session=123;d.source_id=1;d.flags=7;d.count=2;d.quality=90;d.x=80;d.y=30;d.width=90;d.height=200;
 front_person_set_deadline(&d,now+FRONT_PERSON_TTL_NS);uint8_t wire[front_person_wire::SIZE];assert(front_person_wire::encode(wire,d,now));
 front_person_wire::Receiver receiver;FrontPersonData q={};assert(receiver.ingest(wire,sizeof wire,now,now,q));
 uint64_t expiry=front_person_deadline(&q);assert(receiver.ingest(wire,sizeof wire,now+100000000,now+100000000,q));assert(front_person_deadline(&q)==expiry);
 wire[28]^=1;assert(!receiver.ingest(wire,sizeof wire,now,now,q));wire[28]^=1;
 assert(!receiver.ingest(wire,sizeof wire-1,now,now,q));assert(!receiver.ingest(wire,sizeof wire,now,now+FRONT_PERSON_TTL_NS,q));
 FrontPersonGuard p={};
 assert(front_person_guard_observe(&p,&d,now,1)==FRONT_STOP_REQUEST);
 d.flags=3;d.count=0;
 for(unsigned i=0;i<=6;++i){
  uint64_t t=now+1000000000ULL+i*500000000ULL;
  ++d.source_id;front_person_set_deadline(&d,t+FRONT_PERSON_TTL_NS);
  assert(front_person_guard_observe(&p,&d,t,1)==(i==6?FRONT_RESUME_ALLOWED:0));
  assert(front_person_guard_observe(&p,&d,t+100000000ULL,1)==(i==6?FRONT_RESUME_ALLOWED:0));
 }
 now+=4000000000ULL;
 assert(front_person_guard_observe(&p,&d,now,0)==FRONT_STOP_REQUEST);
 assert(!front_person_guard_observe(&p,&d,now,1)); // link failure invalidates the interval
 ++d.source_id;front_person_set_deadline(&d,now+FRONT_PERSON_TTL_NS);
 assert(!front_person_guard_observe(&p,&d,now,1));
 assert(front_person_guard_observe(&p,&d,now+FRONT_PERSON_TTL_NS,1)==FRONT_STOP_REQUEST);
 // A newer sample after the previous deadline starts another full interval.
 now+=FRONT_PERSON_TTL_NS;
 ++d.source_id;front_person_set_deadline(&d,now+FRONT_PERSON_TTL_NS);
 assert(!front_person_guard_observe(&p,&d,now,1));
 d.session++;
 front_person_set_deadline(&d,now+FRONT_PERSON_TTL_NS);
 assert(!front_person_guard_observe(&p,&d,now,1)&&p.clear_since==now);
 // Version 2 carries all-person road association without changing 64-byte IPC/72-byte UDP.
 static_assert(sizeof(FrontPersonData)==64,"IPC ABI");
 d=FrontPersonData{};d.session=777;d.source_id=1;d.flags=3|4|8|64;d.count=d.outside_count=1;
 d.x=20;d.y=20;d.width=20;d.height=80;d.road_pixels=100;d.road_source_id=1;
 front_person_set_deadline(&d,now+FRONT_PERSON_TTL_NS);
 assert(front_person_wire::encode(wire,d,now));front_person_wire::Receiver r2;
 assert(r2.ingest(wire,sizeof wire,now,now,q) && q.outside_count==1 && !front_person_hazard(&q));
 p=FrontPersonGuard{};
 assert(!front_person_guard_observe(&p,&d,now,1)); // person off road: no new STOP
 d.outside_count=0;d.risk_count=1;d.flags|=FRONT_PERSON_IN_PATH;
 assert(front_person_guard_observe(&p,&d,now,1)==FRONT_STOP_REQUEST);
 d.risk_count=0;d.unknown_count=1;d.flags=3|4|32|64;d.road_source_id=d.road_pixels=0;
 assert(front_person_hazard(&d) && front_person_wire::valid(d));
 assert(front_person_guard_observe(&p,&d,now,1)==FRONT_STOP_REQUEST);
 d.flags=3|4|8|64;d.unknown_count=0;d.outside_count=1;d.road_pixels=100;
 for(unsigned i=0;i<=7;++i){
  uint64_t t=now+i*500000000ULL;
  ++d.source_id;d.road_source_id=d.source_id;
  front_person_set_deadline(&d,t+FRONT_PERSON_TTL_NS);
  assert(!front_person_guard_observe(&p,&d,t,1)&&!p.clear_since); // recovery requires no person
 }
 front_person_set_deadline(&d,now+FRONT_PERSON_TTL_NS);
 d.road_source_id++;assert(!front_person_wire::valid(d));d.road_source_id=d.source_id;
 d.unknown_count=1;assert(!front_person_wire::valid(d));d.unknown_count=0;
 assert(front_person_wire::encode(wire,d,now));wire[52]^=1;
 front_person_wire::put(wire+68,front_person_wire::checksum(wire,68));
 assert(!r2.ingest(wire,sizeof wire,now,now,q)); // counts mismatch even with correct CRC
 // Legacy packets during staged upgrade retain any-person protection.
 d.flags=7;d.count=1;d.outside_count=d.road_pixels=d.road_source_id=0;
 assert(front_person_wire::encode(wire,d,now));front_person_wire::put(wire+4,0x00010048u);
 front_person_wire::put(wire+68,front_person_wire::checksum(wire,68));
 front_person_wire::Receiver old;assert(old.ingest(wire,sizeof wire,now,now,q) && front_person_hazard(&q));
 puts("FRONT PERSON POLICY/WIRE PASS: v1/v2/CRC, in-road/uncertain stop, full three-second no-person interval, duplicates, expiry and link loss");
}
