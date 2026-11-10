#ifndef CROSSWALK_DISPLAY_H
#define CROSSWALK_DISPLAY_H
// Independent, display-only evidence. Never consumed by a brake/steering gate.
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <sys/shm.h>
#include <arpa/inet.h>
namespace crosswalk_display {
constexpr unsigned CAPACITY=12;
constexpr int KEY=0x12345678+18;
constexpr uint32_t MAGIC=0x43574b31u;
constexpr uint64_t TTL=750000000ULL;
struct Quad {uint32_t xy[8];}; // Clockwise 320x240 bird-image vertices.
struct Snapshot {uint32_t session,source,count,quality,deadline_lo,deadline_hi;Quad stripes[CAPACITY];};
static_assert(sizeof(Snapshot)==24+CAPACITY*32,"crosswalk display ABI");
inline uint64_t deadline(const Snapshot&s){return (uint64_t(s.deadline_hi)<<32)|s.deadline_lo;}
inline void expires(Snapshot&s,uint64_t t){s.deadline_lo=uint32_t(t);s.deadline_hi=uint32_t(t>>32);}
inline bool fresh(const Snapshot&s,uint64_t now){return deadline(s)>now&&deadline(s)-now<=TTL;}
inline bool valid(const Snapshot&s){
 if(!s.session||s.count>CAPACITY||s.quality>100||(s.count&&s.count<4))return false;
 for(unsigned i=0;i<s.count;++i){const auto&q=s.stripes[i];int64_t sign=0,area=0;
  for(unsigned j=0;j<4;++j){unsigned a=j*2,b=(j+1)%4*2,c=(j+2)%4*2;if(q.xy[a]>=320||q.xy[a+1]>=240)return false;
   int64_t cross=(int64_t(q.xy[b])-q.xy[a])*(int64_t(q.xy[c+1])-q.xy[b+1])-(int64_t(q.xy[b+1])-q.xy[a+1])*(int64_t(q.xy[c])-q.xy[b]);
   if(cross&&sign&&((cross>0)!=(sign>0)))return false;
   if(cross)sign=cross;
   area+=int64_t(q.xy[a])*q.xy[b+1]-int64_t(q.xy[b])*q.xy[a+1];}
  if(!sign||std::abs(area)<8)return false;}
 return true;
}
struct Geometry {
 unsigned count=0,quality=0;Quad stripes[CAPACITY]={};
 bool operator==(const Geometry&o)const{return count==o.count&&quality==o.quality&&!std::memcmp(stripes,o.stripes,count*sizeof(Quad));}
 void set(const Snapshot&s){count=s.count;quality=s.quality;std::copy_n(s.stripes,count,stripes);}
};
constexpr unsigned WORDS=sizeof(Snapshot)/4;
struct Segment {uint32_t sequence,magic,words[WORDS];};
struct IPC {
 int id=-1;Segment*segment=nullptr;
 ~IPC(){if(segment)shmdt(segment);}
 bool open(bool writer){int next=shmget(KEY,sizeof(Segment),0666|(writer?IPC_CREAT:0));
  if(next<0){if(segment)shmdt(segment);segment=nullptr;id=-1;return false;}
  if(segment&&id==next)return true;
  if(segment)shmdt(segment);
  void*p=shmat(next,nullptr,writer?0:SHM_RDONLY);segment=p==(void*)-1?nullptr:(Segment*)p;id=next;return segment;}
 void write(const Snapshot&s){if(!segment||!valid(s))return;uint32_t w[WORDS];memcpy(w,&s,sizeof(s));auto*q=segment;uint32_t seq=__atomic_load_n(&q->sequence,__ATOMIC_RELAXED);
  __atomic_store_n(&q->sequence,(seq+1)|1u,__ATOMIC_SEQ_CST);__atomic_store_n(&q->magic,MAGIC,__ATOMIC_RELAXED);
  for(unsigned i=0;i<WORDS;++i)__atomic_store_n(q->words+i,w[i],__ATOMIC_RELAXED);
  __atomic_store_n(&q->sequence,(seq+2)&~1u,__ATOMIC_RELEASE);}
 bool read(Snapshot&s)const{if(!segment)return false;uint32_t w[WORDS];auto*q=segment;
  for(unsigned k=0;k<4;++k){uint32_t seq=__atomic_load_n(&q->sequence,__ATOMIC_ACQUIRE);if(seq&1)continue;uint32_t magic=__atomic_load_n(&q->magic,__ATOMIC_RELAXED);
   for(unsigned i=0;i<WORDS;++i)w[i]=__atomic_load_n(q->words+i,__ATOMIC_RELAXED);
   __atomic_thread_fence(__ATOMIC_ACQUIRE);
   if(seq==__atomic_load_n(&q->sequence,__ATOMIC_RELAXED)&&magic==MAGIC){memcpy(&s,w,sizeof(s));return valid(s);}}
  return false;}
};
constexpr size_t WIRE_SIZE=32+CAPACITY*32+4;
inline void put(uint8_t*p,uint32_t v){v=htonl(v);memcpy(p,&v,4);}
inline uint32_t get(const uint8_t*p){uint32_t v;memcpy(&v,p,4);return ntohl(v);}
inline uint32_t hash(const uint8_t*p,size_t n){uint32_t v=2166136261u;for(size_t i=0;i<n;++i)v=(v^p[i])*16777619u;return v;}
inline bool packet(const uint8_t*p,size_t n){return n>=4&&get(p)==MAGIC;}
inline bool encode(uint8_t*p,const Snapshot&s,uint64_t now){
 if(!fresh(s,now)||!valid(s))return false;
 memset(p,0,WIRE_SIZE);put(p,MAGIC);put(p+4,1);put(p+8,s.session);put(p+12,s.source);put(p+16,s.count);put(p+20,uint32_t((deadline(s)-now)/1000000ULL));put(p+24,s.quality);
 for(unsigned i=0;i<s.count;++i)for(unsigned j=0;j<8;++j)put(p+32+i*32+j*4,s.stripes[i].xy[j]);
 put(p+WIRE_SIZE-4,hash(p,WIRE_SIZE-4));return true;}
class Receiver {
 uint32_t session_=0,source_=0,retired_[16]={};unsigned retired_count_=0;uint64_t until_=0;
 public:
 bool ingest(const uint8_t*p,size_t n,uint64_t received,uint64_t now,Snapshot&out){
  if(n!=WIRE_SIZE||!packet(p,n)||get(p+4)!=1||get(p+WIRE_SIZE-4)!=hash(p,WIRE_SIZE-4)||received>now)return false;
  Snapshot s={};s.session=get(p+8);s.source=get(p+12);s.count=get(p+16);s.quality=get(p+24);uint32_t remain=get(p+20);
  if(!remain||remain>750||s.count>CAPACITY)return false;
  for(unsigned i=0;i<s.count;++i)for(unsigned j=0;j<8;++j)s.stripes[i].xy[j]=get(p+32+i*32+j*4);
  if(!valid(s))return false;
  uint64_t end=received+uint64_t(remain)*1000000ULL;if(end<=now)return false;
  if(session_&&session_!=s.session){for(unsigned i=0;i<std::min(retired_count_,16u);++i)if(retired_[i]==s.session)return false;retired_[retired_count_++%16]=session_;}
  else if(session_&&int32_t(s.source-source_)<0)return false;
  else if(session_&&s.source==source_)end=std::min(end,until_);
  if(end<=now)return false;
  session_=s.session;source_=s.source;until_=end;expires(s,end);out=s;return true;}
};
}
#endif
