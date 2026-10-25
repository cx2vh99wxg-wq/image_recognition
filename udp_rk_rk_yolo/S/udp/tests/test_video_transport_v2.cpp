#include "video_transport_v2.h"
#include <cassert>
#include <random>
#include <numeric>
#include <iostream>
using namespace video_v2;
using Bytes=std::vector<uint8_t>;
Bytes image(FRAME_BYTES), packet(sizeof(WireHeader)+LINE_BYTES), output(FRAME_BYTES);
static Metadata meta(uint32_t id) { Metadata m; m.frame_id=id;m.checksum=checksum(image.data(),image.size());m.curve_type=2;m.curve_confidence=87;m.curve_angle=12;return m;}
static void row(Assembler& a,const Metadata&m,unsigned line,uint64_t now=1000000,uint64_t source=1,bool corrupt=false){
 WireHeader h=make_header(m,line,checksum(image.data()+line*LINE_BYTES,LINE_BYTES));
 memcpy(packet.data(),&h,sizeof h);memcpy(packet.data()+sizeof h,image.data()+line*LINE_BYTES,LINE_BYTES);
 if(corrupt)packet.back()^=1;
 a.ingest(packet.data(),packet.size(),source,now);
}
static void all(Assembler&a,uint32_t id,uint64_t now=1000000,uint64_t source=1) {
 auto m=meta(id);for(unsigned n=0;n<HEIGHT;++n)row(a,m,n,now+n,source);
}
int main(){
 for(size_t i=0;i<image.size();++i)image[i]=(uint8_t)(i*13+i/LINE_BYTES);
 Metadata out;
 {Assembler a;auto m=meta(1);std::vector<unsigned> order(HEIGHT);std::iota(order.begin(),order.end(),0);std::mt19937 rng(5);std::shuffle(order.begin(),order.end(),rng);
  for(auto n:order){row(a,m,n);if(n%7==0)row(a,m,n);}
  assert(a.take_latest(output.data(),out)&&out.frame_id==1&&output==image&&out.curve_confidence==87);
  assert(a.statistics().complete==1&&a.statistics().duplicates>0);
  assert(!a.take_latest(output.data(),out));
 }
 {Assembler a;auto m=meta(10);for(unsigned n=1;n<HEIGHT;++n)row(a,m,n);assert(!a.take_latest(output.data(),out));
  all(a,11);all(a,12);assert(a.take_latest(output.data(),out)&&out.frame_id==12&&output==image);assert(a.statistics().incomplete==1);
 }
 {Assembler a;auto m=meta(20);for(unsigned n=0;n<HEIGHT;++n)row(a,m,n,1000000,1,n==4);
  assert(!a.take_latest(output.data(),out)&&a.statistics().corrupt==1);row(a,m,4);
  assert(a.take_latest(output.data(),out)&&output==image);
 }
 {Assembler a;all(a,30);all(a,31);all(a,32);assert(a.take_latest(output.data(),out)&&out.frame_id==32);assert(a.statistics().skipped_complete==2);
  all(a,31);assert(!a.take_latest(output.data(),out));
 }
 {Assembler a;all(a,0xfffffffeu);assert(a.take_latest(output.data(),out));all(a,0xffffffffu);assert(a.take_latest(output.data(),out));all(a,0u);assert(a.take_latest(output.data(),out)&&out.frame_id==0);}
 {Assembler a;all(a,80,1000000,1);assert(a.take_latest(output.data(),out));all(a,1,2000000,2);assert(a.take_latest(output.data(),out)&&out.frame_id==1);
  all(a,81,FRAME_TIMEOUT_NS+3000000,1);assert(!a.take_latest(output.data(),out));
 }
 {Assembler a;row(a,meta(90),0,1);row(a,meta(90),1,FRAME_TIMEOUT_NS+2);assert(a.statistics().incomplete==1&&!a.take_latest(output.data(),out));
  auto m=meta(90);for(unsigned n=0;n<HEIGHT;++n)row(a,m,n,FRAME_TIMEOUT_NS+3);assert(a.take_latest(output.data(),out));
 }
 {Assembler a;auto m=meta(100);row(a,m,0);m.checksum++;row(a,m,1);assert(a.statistics().corrupt==1&&!a.take_latest(output.data(),out));}
 // Capture-only restart changes session and can reset both source counters.
 {Assembler a;auto m=meta(90000);m.session_id=10;m.source_frame_id=90000;m.curve_frame_id=80000;
  for(unsigned n=0;n<HEIGHT;++n){row(a,m,n);}assert(a.take_latest(output.data(),out));
  auto next=meta(90001);next.session_id=11;next.source_frame_id=1;next.curve_frame_id=1;
  for(unsigned n=0;n<HEIGHT;++n){row(a,next,n);}assert(a.take_latest(output.data(),out)&&out.source_frame_id==1);
  // A delayed datagram from retired producer session cannot flip the receiver back.
  m.frame_id=90002;for(unsigned n=0;n<HEIGHT;++n){row(a,m,n);}assert(!a.take_latest(output.data(),out));
 }
 // V2 remains a video-only fallback with curve confidence explicitly invalidated.
 {Assembler a;auto m=meta(3);for(unsigned n=0;n<HEIGHT;++n){
   auto h=make_header(m,n,checksum(image.data()+n*LINE_BYTES,LINE_BYTES));h.version=htonl(2);
   Bytes old(sizeof(WireHeaderV2)+LINE_BYTES);memcpy(old.data(),&h,sizeof(WireHeaderV2));
   memcpy(old.data()+sizeof(WireHeaderV2),image.data()+n*LINE_BYTES,LINE_BYTES);
   a.ingest(old.data(),old.size(),1,1000000+n);
  }assert(a.take_latest(output.data(),out)&&out.version==2&&out.curve_confidence==0&&output==image);
 }
 {CurveSenderState sender;Metadata m;
  sender.observe(7,1,80,20,1000000000ULL,1100000000ULL);sender.fill(m,1200000000ULL);
  assert(m.curve_frame_id==7&&m.curve_age_ms==200&&m.curve_confidence==80);
  // Raw frames and repeated reads cannot refresh the original capture age.
  sender.observe(7,1,80,20,1000000000ULL,1400000000ULL);sender.fill(m,1500000000ULL);
  assert(m.curve_confidence==0&&m.curve_age_ms==500);
  sender.observe(6,2,90,30,1550000000ULL,1600000000ULL);sender.fill(m,1600000000ULL);assert(m.curve_frame_id==7&&m.curve_confidence==0);
  sender.observe(8,2,90,30,1550000000ULL,1600000000ULL);sender.fill(m,1600000000ULL);assert(m.curve_frame_id==8&&m.curve_confidence==90);
  sender.observe(8,2,0,30,1550000000ULL,1610000000ULL);sender.fill(m,1610000000ULL);assert(m.curve_confidence==0);
  sender.observe(8,2,90,30,1550000000ULL,1620000000ULL);sender.fill(m,1620000000ULL);assert(m.curve_confidence==0);
  sender.reset();sender.observe(1,1,70,10,0,2000000000ULL);sender.fill(m,2100000000ULL);
  assert(m.curve_age_ms==(AGE_OBSERVED|100u));sender.observe(1,1,70,10,0,2200000000ULL);sender.fill(m,2500000000ULL);assert(!m.curve_confidence);
 }
 {CurveReceiverState receiver;CurveEvent event;auto m=meta(100);m.session_id=1;m.source=20;m.curve_frame_id=7;
  m.curve_age_ms=200;m.first_packet_ns=1000000000ULL;
  assert(receiver.observe(m,1100000000ULL,event)&&event.confidence==87); // 300ms input age
  m.frame_id++;m.first_packet_ns=1200000000ULL;m.curve_age_ms=400;
  assert(!receiver.observe(m,1250000000ULL,event));
  assert(receiver.expire(1300000000ULL,event)&&!event.confidence); // first deadline was not renewed
  assert(!receiver.expire(1400000000ULL,event));
  m.curve_age_ms=1;m.first_packet_ns=1400000000ULL;
  assert(!receiver.observe(m,1400000000ULL,event)); // same NN cannot revive
  m.curve_frame_id=8;assert(receiver.observe(m,1400000000ULL,event)&&event.confidence==87);
  m.curve_frame_id=7;assert(!receiver.observe(m,1400000000ULL,event)); // old NN result
  m.session_id=2;m.curve_frame_id=1;assert(receiver.observe(m,1400000000ULL,event)&&event.frame_id==1);
  m.curve_confidence=0;assert(receiver.observe(m,1410000000ULL,event)&&!event.confidence);
  assert(!receiver.observe(m,1420000000ULL,event));
  m.session_id=3;m.curve_confidence=80;m.curve_age_ms=490;m.first_packet_ns=1400000000ULL;
  assert(receiver.observe(m,1420000000ULL,event)&&!event.confidence); // reassembly/queue age included
  m.version=2;assert(!receiver.observe(m,1430000000ULL,event));
 }
 std::cout<<"PASS V3/V2: row integrity/reorder/drop/latest, source restart/retired session, NN identity/TTL/expiry/age\n";
}
