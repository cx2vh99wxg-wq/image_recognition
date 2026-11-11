#ifndef DISPLAY_PEOPLE_H
#define DISPLAY_PEOPLE_H
// Display-only sidecar. No actuator, brake or permission consumer uses it.
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <sys/shm.h>
namespace display_people {
constexpr unsigned CAPACITY=12;
constexpr int FRONT_KEY=0x12345678+16, REAR_KEY=0x12345678+17;
constexpr uint64_t TTL=750000000ULL;
enum View : uint32_t {Front=0,Rear=1,LeftRear=2,RightRear=3};
enum Flag : uint32_t {Ground=1,InRoad=2,OffRoad=4,RoadAligned=8};
struct Person {uint32_t view,x,y,w,h,quality,flags; int32_t gx,gy;};
struct Snapshot {uint32_t session,source,count,ready,deadline_lo,deadline_hi; Person people[CAPACITY];};
static_assert(sizeof(Snapshot)==24+CAPACITY*36,"display-only ABI");
inline uint64_t deadline(const Snapshot&s){return (uint64_t(s.deadline_hi)<<32)|s.deadline_lo;}
inline void expires(Snapshot&s,uint64_t t){s.deadline_lo=uint32_t(t);s.deadline_hi=uint32_t(t>>32);}
inline bool fresh(const Snapshot&s,uint64_t now){return s.ready && deadline(s)>now && deadline(s)-now<=TTL;}
inline bool valid(const Snapshot&s){
 if(!s.session || s.count>CAPACITY || s.ready>1)return false;
 for(unsigned i=0;i<s.count;++i){const auto&p=s.people[i];
  if(p.view>3 || p.x>=320 || p.y>=240 || !p.w || !p.h || p.w>320-p.x || p.h>240-p.y ||
     p.quality>100 || (p.flags&~15u) || ((p.flags&6u)==6u) || std::abs(int64_t(p.gx))>100000 || std::abs(int64_t(p.gy))>100000)return false;}
 return true;
}
inline float overlap(const Person&a,const Person&b){
 if(a.view!=b.view)return 0;
 const int w=std::max(0,int(std::min(a.x+a.w,b.x+b.w))-int(std::max(a.x,b.x)));
 const int h=std::max(0,int(std::min(a.y+a.h,b.y+b.h))-int(std::max(a.y,b.y)));
 return float(w*h)/std::max(1,int(a.w*a.h+b.w*b.h)-w*h);
}
inline void add(Snapshot&s,const Person&p){
 for(unsigned i=0;i<s.count;++i)if(overlap(s.people[i],p)>.4f){if(p.quality>s.people[i].quality)s.people[i]=p;return;}
 if(s.count<CAPACITY)s.people[s.count++]=p;
 else {unsigned low=0;for(unsigned i=1;i<CAPACITY;++i)if(s.people[i].quality<s.people[low].quality)low=i;if(p.quality>s.people[low].quality)s.people[low]=p;}
}
inline Person box(unsigned view,int x,int y,int w,int h,float score){
 Person p={};p.view=view;p.x=std::max(0,std::min(319,x));p.y=std::max(0,std::min(239,y));
 p.w=std::max(1,std::min(320-int(p.x),w));p.h=std::max(1,std::min(240-int(p.y),h));
 p.quality=unsigned(std::max(0.f,std::min(100.f,score*100.f)));return p;
}
template<class Record, uint32_t Magic>
struct SnapshotIPC {
 static_assert(sizeof(Record)%4==0,"snapshot must contain complete words");
 static constexpr unsigned WORDS=sizeof(Record)/4;
 struct Segment {uint32_t sequence,magic,words[WORDS];};
 int id=-1;Segment*segment=nullptr;
 ~SnapshotIPC(){if(segment)shmdt(segment);}
 bool open(int key,bool writer){
  int next=shmget(key,sizeof(Segment),0666|(writer?IPC_CREAT:0));
  if(next<0){if(segment)shmdt(segment);segment=nullptr;id=-1;return false;}
  if(segment&&id==next)return true;
  if(segment)shmdt(segment);
  void*p=shmat(next,nullptr,writer?0:SHM_RDONLY);segment=p==(void*)-1?nullptr:(Segment*)p;id=next;return segment;
 }
 void write(const Record&s){if(!segment)return;uint32_t w[WORDS];memcpy(w,&s,sizeof(s));
  auto*q=segment;uint32_t seq=__atomic_load_n(&q->sequence,__ATOMIC_RELAXED);
  __atomic_store_n(&q->sequence,(seq+1)|1u,__ATOMIC_SEQ_CST);__atomic_store_n(&q->magic,Magic,__ATOMIC_RELAXED);
  for(unsigned i=0;i<WORDS;++i)__atomic_store_n(q->words+i,w[i],__ATOMIC_RELAXED);
  __atomic_store_n(&q->sequence,(seq+2)&~1u,__ATOMIC_RELEASE);
 }
 bool read(Record&s)const{if(!segment)return false;uint32_t w[WORDS];auto*q=segment;
  for(unsigned k=0;k<4;++k){uint32_t seq=__atomic_load_n(&q->sequence,__ATOMIC_ACQUIRE);if(seq&1)continue;
   uint32_t magic=__atomic_load_n(&q->magic,__ATOMIC_RELAXED);
   for(unsigned i=0;i<WORDS;++i)w[i]=__atomic_load_n(q->words+i,__ATOMIC_RELAXED);
   __atomic_thread_fence(__ATOMIC_ACQUIRE);
   if(seq==__atomic_load_n(&q->sequence,__ATOMIC_RELAXED)&&magic==Magic){memcpy(&s,w,sizeof(s));return valid(s);}}
  return false;
 }
};
using IPC=SnapshotIPC<Snapshot,0x50475031u>;
}
#endif
