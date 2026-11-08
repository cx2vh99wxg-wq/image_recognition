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
 FrontPersonGuard p={};ControlSnapshot c={};c.contacts=31;c.gates=GATE_AUTO;
 assert(front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_GREEN));assert(!p.raw_request);
 assert(front_person_guard_permission(&p,1,TRAFFIC_GO_CLEAR,1)==TRAFFIC_GO_SET); // green cannot release person
 for(unsigned steering:{0u,(unsigned)CONTACT_LEFT_N,(unsigned)CONTACT_RIGHT_N}){
  p=FrontPersonGuard{};c.contacts=31 & ~CONTACT_GO_N & ~steering;
  assert(front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_GREEN) && p.raw_request); // forward with either turn
 }
 p=FrontPersonGuard{};c.contacts=31 & ~CONTACT_BACK_N;assert(front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_GREEN) && !p.raw_request);
 p.held_go=1;p.raw_request=1;p.recovery_pending=1;d.flags=3;d.count=0;
 for(int i=0;i<3;++i){uint64_t t=now+i*200000000ULL;d.source_id++;front_person_set_deadline(&d,t+FRONT_PERSON_TTL_NS);
  unsigned need=front_person_guard_observe(&p,&d,t,&c,1,TRAFFIC_GREEN);assert(need==(i<2));
  unsigned count=p.clear_count;front_person_guard_observe(&p,&d,t,&c,1,TRAFFIC_GREEN);assert(p.clear_count==count);}
 assert(!p.raw_request);assert(front_person_guard_permission(&p,0,TRAFFIC_GO_SET,1)==TRAFFIC_GO_SET); // person clear cannot release red
 assert(front_person_guard_permission(&p,0,0,0)==TRAFFIC_GO_CLEAR);
 front_person_set_deadline(&d,now);assert(front_person_guard_observe(&p,&d,now,&c,0,TRAFFIC_GREEN) && p.raw_request); // AUTO link failure never means clear
 p=FrontPersonGuard{};c.gates=GATE_AUTO;c.contacts=31 & ~CONTACT_GO_N;d.flags=3;d.count=0;
 front_person_set_deadline(&d,now+FRONT_PERSON_TTL_NS);
 assert(front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_RED));
 assert(!p.raw_request); // red light permission alone must not create a new person STOP
 for(int i=0;i<2;++i){++d.source_id;front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_GREEN);}
 assert(p.clear_count==2);front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_UNKNOWN);
 assert(p.clear_count==0);
 p=FrontPersonGuard{};c.gates=0;c.contacts=31;front_person_set_deadline(&d,now+FRONT_PERSON_TTL_NS);
 assert(front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_GREEN));assert(front_person_guard_permission(&p,1,0,0)==0); // preserves FPGA AUTO switch
 p=FrontPersonGuard{};c.gates=GATE_AUTO;c.contacts=31;d.flags=3;d.count=0;
 for(int i=0;i<3;++i){++d.source_id;front_person_set_deadline(&d,now+FRONT_PERSON_TTL_NS);
   front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_UNKNOWN);}
 assert(p.clear_count==3 && !front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_UNKNOWN));
 p.recovery_pending=1;p.clear_count=0;
 for(int i=0;i<3;++i){++d.source_id;front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_GREEN);}
 assert(!p.recovery_pending);++d.source_id;
 assert(!front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_UNKNOWN)); // green leaves camera after release
 // Version 2 carries all-person road association without changing 64-byte IPC/72-byte UDP.
 static_assert(sizeof(FrontPersonData)==64,"IPC ABI");
 d=FrontPersonData{};d.session=777;d.source_id=1;d.flags=3|4|8|64;d.count=d.outside_count=1;
 d.x=20;d.y=20;d.width=20;d.height=80;d.road_pixels=100;d.road_source_id=1;
 front_person_set_deadline(&d,now+FRONT_PERSON_TTL_NS);
 assert(front_person_wire::encode(wire,d,now));front_person_wire::Receiver r2;
 assert(r2.ingest(wire,sizeof wire,now,now,q) && q.outside_count==1 && !front_person_hazard(&q));
 p=FrontPersonGuard{};c.gates=GATE_AUTO;c.contacts=31 & ~CONTACT_GO_N;
 for(int i=0;i<3;++i){++d.source_id;d.road_source_id=d.source_id;front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_UNKNOWN);}
 assert(!p.raw_request && p.clear_count==3); // person off road doesn't stop forward/turn
 d.outside_count=0;d.risk_count=1;d.flags|=FRONT_PERSON_IN_PATH;
 assert(front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_GREEN) && p.raw_request);
 d.risk_count=0;d.unknown_count=1;d.flags=3|4|32|64;d.road_source_id=d.road_pixels=0;
 assert(front_person_hazard(&d) && front_person_wire::valid(d));
 assert(front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_GREEN) && p.raw_request);
 // Three distinct off-road observations restore permission, not the FPGA STOP latch.
 d.flags=3|4|8|64;d.unknown_count=0;d.outside_count=1;d.road_pixels=100;
 for(int i=0;i<3;++i){++d.source_id;d.road_source_id=d.source_id;assert(front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_GREEN)==(i<2));}
 assert(!p.raw_request);
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
 // HMI OFF removes the front GO hold even when a person remains visible.
 p=FrontPersonGuard{};p.held_go=1;c.gates=0;c.contacts=31 & ~CONTACT_GO_N;
 front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_GREEN);
 assert(!p.auto_enabled && front_person_guard_permission(&p,1,TRAFFIC_GO_CLEAR,1)==TRAFFIC_GO_CLEAR);
 assert(front_person_guard_permission(&p,1,TRAFFIC_GO_SET,1)==TRAFFIC_GO_SET); // red remains independent
 p.held_go=0; // simulate the successful OFF-era GO_CLEAR write
 // A person seen only while OFF must not become a stale STOP upon ON later.
 d.flags=3;d.count=0;
 for(int i=0;i<3;++i){++d.source_id;front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_UNKNOWN);}
 assert(!p.raw_request && !p.recovery_pending);
 c.gates=GATE_AUTO;
 assert(!front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_UNKNOWN));
 assert(!front_person_guard_permission(&p,0,0,0));
 d.flags=7;d.count=1;
 c.gates=GATE_AUTO;front_person_guard_observe(&p,&d,now,&c,1,TRAFFIC_GREEN);
 assert(front_person_guard_permission(&p,1,TRAFFIC_GO_CLEAR,1)==TRAFFIC_GO_SET);
 puts("FRONT PERSON POLICY/WIRE PASS: v1/v2/CRC/frame/count validation, outside clear, inside/uncertain stop, green+3 distinct no-risk recovery, duplicate/expiry, independent red/AUTO");
}
