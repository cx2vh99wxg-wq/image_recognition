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
 assert(front_person_guard_observe(&p,&d,now,&c,1));assert(!p.raw_request);
 assert(front_person_guard_permission(&p,1,TRAFFIC_GO_CLEAR,1)==TRAFFIC_GO_SET); // green cannot release person
 for(unsigned steering:{0u,(unsigned)CONTACT_LEFT_N,(unsigned)CONTACT_RIGHT_N}){
  p=FrontPersonGuard{};c.contacts=31 & ~CONTACT_GO_N & ~steering;
  assert(front_person_guard_observe(&p,&d,now,&c,1) && p.raw_request); // forward with either turn
 }
 p=FrontPersonGuard{};c.contacts=31 & ~CONTACT_BACK_N;assert(front_person_guard_observe(&p,&d,now,&c,1) && !p.raw_request);
 p.held_go=1;p.raw_request=1;d.flags=3;d.count=0;
 for(int i=0;i<=4;++i){uint64_t t=now+i*500000000ULL;front_person_set_deadline(&d,t+FRONT_PERSON_TTL_NS);
  unsigned need=front_person_guard_observe(&p,&d,t,&c,1);assert(need==(i<4));}
 assert(!p.raw_request);assert(front_person_guard_permission(&p,0,TRAFFIC_GO_SET,1)==TRAFFIC_GO_SET); // person clear cannot release red
 assert(front_person_guard_permission(&p,0,0,0)==TRAFFIC_GO_CLEAR);
 front_person_set_deadline(&d,now);assert(front_person_guard_observe(&p,&d,now,&c,0) && p.raw_request); // AUTO link failure never means clear
 p=FrontPersonGuard{};c.gates=0;c.contacts=31;front_person_set_deadline(&d,now+FRONT_PERSON_TTL_NS);
 assert(front_person_guard_observe(&p,&d,now,&c,1));assert(front_person_guard_permission(&p,1,0,0)==0); // preserves FPGA AUTO switch
 puts("FRONT PERSON POLICY/WIRE PASS: corruption/expiry/duplicate, forward+both turns, reverse, 2s valid clear, independent red hold, AUTO and failures");
}
