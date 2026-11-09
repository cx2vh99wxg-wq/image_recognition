#ifndef DISPLAY_PEOPLE_WIRE_H
#define DISPLAY_PEOPLE_WIRE_H
#include "display_people.h"
#include <arpa/inet.h>
namespace display_people {
constexpr size_t WIRE_SIZE=32+CAPACITY*36+4;
inline uint32_t hash(const uint8_t*p,size_t n){uint32_t v=2166136261u;for(size_t i=0;i<n;++i)v=(v^p[i])*16777619u;return v;}
inline void put(uint8_t*p,uint32_t v){v=htonl(v);memcpy(p,&v,4);}
inline uint32_t get(const uint8_t*p){uint32_t v;memcpy(&v,p,4);return ntohl(v);}
inline bool packet(const uint8_t*p,size_t n){return n>=4&&get(p)==0x50475031u;}
inline bool encode(uint8_t*p,const Snapshot&s,uint64_t now){
 if(!fresh(s,now)||!valid(s))return false;
 memset(p,0,WIRE_SIZE);put(p,0x50475031u);put(p+4,1);put(p+8,s.session);put(p+12,s.source);put(p+16,s.count);
 put(p+20,uint32_t((deadline(s)-now)/1000000ULL));put(p+24,s.ready);
 for(unsigned i=0;i<s.count;++i){uint32_t words[9];memcpy(words,&s.people[i],36);for(unsigned j=0;j<9;++j)put(p+32+i*36+j*4,words[j]);}
 put(p+WIRE_SIZE-4,hash(p,WIRE_SIZE-4));return true;
}
class Receiver {
 uint32_t session_=0,source_=0,retired_[16]={};unsigned retired_count_=0;uint64_t until_=0;
 public:
 bool ingest(const uint8_t*p,size_t n,uint64_t received,uint64_t now,Snapshot&out){
  if(n!=WIRE_SIZE||!packet(p,n)||get(p+4)!=1||get(p+WIRE_SIZE-4)!=hash(p,WIRE_SIZE-4)||received>now)return false;
  Snapshot s={};s.session=get(p+8);s.source=get(p+12);s.count=get(p+16);s.ready=get(p+24);
  uint32_t remain=get(p+20);if(!remain||remain>750||s.count>CAPACITY)return false;
  for(unsigned i=0;i<s.count;++i){uint32_t words[9];for(unsigned j=0;j<9;++j)words[j]=get(p+32+i*36+j*4);memcpy(&s.people[i],words,36);if(s.people[i].view!=Front)return false;}
  if(!valid(s))return false;
  uint64_t end=received+uint64_t(remain)*1000000ULL;if(end<=now)return false;
  if(session_&&session_!=s.session){for(unsigned i=0;i<std::min(retired_count_,16u);++i)if(retired_[i]==s.session)return false;retired_[retired_count_++%16]=session_;}
  else if(session_&&int32_t(s.source-source_)<0)return false;
  else if(session_&&s.source==source_)end=std::min(end,until_);
  if(end<=now)return false;
  session_=s.session;source_=s.source;until_=end;expires(s,end);out=s;return true;
 }
};
}
#endif
