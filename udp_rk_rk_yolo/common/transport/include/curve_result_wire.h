#ifndef CURVE_RESULT_WIRE_H
#define CURVE_RESULT_WIRE_H
#include "video_transport_v2.h"
namespace curve_wire {
constexpr size_t SIZE=36;
inline void put(uint8_t*p,uint32_t v){v=htonl(v);std::memcpy(p,&v,4);}
inline uint32_t get(const uint8_t*p){uint32_t v;std::memcpy(&v,p,4);return ntohl(v);}
inline uint32_t hash(const uint8_t*p,size_t n){uint32_t v=2166136261u;for(size_t i=0;i<n;++i)v=(v^p[i])*16777619u;return v;}
inline bool packet(const uint8_t*p,size_t n){return n>=4&&get(p)==0x43565031u;}
inline bool encode(uint8_t*p,const video_v2::Metadata&m){
 if(!m.session_id||m.curve_type>3||m.curve_confidence>100||m.curve_angle>360)return false;
 const uint32_t values[]={0x43565031u,0x00010024u,m.session_id,m.curve_frame_id,m.curve_type,m.curve_confidence,m.curve_angle,m.curve_age_ms};
 for(unsigned i=0;i<8;++i){put(p+i*4,values[i]);}
 put(p+32,hash(p,32));return true;
}
class Receiver {
 uint64_t peer_=0,last_=0;uint32_t session_=0,retired_[16]={};unsigned retired_count_=0;
public:
 bool ingest(const uint8_t*p,size_t n,uint64_t peer,uint64_t received,uint64_t now,video_v2::Metadata&m){
  if(n!=SIZE||!packet(p,n)||get(p+4)!=0x00010024u||get(p+32)!=hash(p,32)||received>now)return false;
  const uint32_t session=get(p+8);if(!session||get(p+16)>3||get(p+20)>100||get(p+24)>360)return false;
  if(peer_&&peer!=peer_&&now-last_<750000000ULL)return false;
  if(peer==peer_&&session!=session_){
   for(unsigned i=0;i<std::min(16u,retired_count_);++i)if(retired_[i]==session)return false;
   if(session_)retired_[retired_count_++%16]=session_;
  }else if(peer!=peer_){retired_count_=0;std::fill(retired_,retired_+16,0);}
  peer_=peer;session_=session;last_=now;
  m={};m.version=3;m.source=peer;m.session_id=session;m.curve_frame_id=get(p+12);
  m.curve_type=get(p+16);m.curve_confidence=get(p+20);m.curve_angle=get(p+24);m.curve_age_ms=get(p+28);m.first_packet_ns=received;
  return true;
 }
 // Same NN identity and expiry state serve both transports. Old video from a
 // retired session cannot undo a newly received direct result.
 bool allow_video(const video_v2::Metadata&m,uint64_t now)const{
  return !last_||now-last_>=750000000ULL||(m.source==peer_&&m.session_id==session_);
 }
};
}
#endif
