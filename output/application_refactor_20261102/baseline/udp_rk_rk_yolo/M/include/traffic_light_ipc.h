#ifndef TRAFFIC_LIGHT_IPC_H
#define TRAFFIC_LIGHT_IPC_H
#include "traffic_light.h"
#include <sys/shm.h>
#include <string.h>
#include <time.h>
#define TRAFFIC_LIGHT_KEY (0x12345678+11)
#define TRAFFIC_TTL_NS 750000000ULL
typedef struct {uint32_t sequence,magic,words[16];} TrafficLightSegment;
typedef struct {int id;TrafficLightSegment* segment;} TrafficLightIPC;
static inline uint64_t traffic_clock_ns(void){struct timespec t;if(clock_gettime(CLOCK_MONOTONIC,&t))return 0;return (uint64_t)t.tv_sec*1000000000ULL+t.tv_nsec;}
static inline uint64_t traffic_deadline(const TrafficLightData* d){return ((uint64_t)d->deadline_hi<<32)|d->deadline_lo;}
static inline void traffic_set_deadline(TrafficLightData* d,uint64_t ns){d->deadline_lo=(uint32_t)ns;d->deadline_hi=(uint32_t)(ns>>32);}
static inline int traffic_fresh(const TrafficLightData* d,uint64_t now){uint64_t t=traffic_deadline(d);return t>now&&t-now<=TRAFFIC_TTL_NS;}
static inline void traffic_ipc_close(TrafficLightIPC* p){if(p->segment)shmdt(p->segment);p->segment=0;p->id=-1;}
static inline int traffic_ipc_open(TrafficLightIPC* p,int writer){
    int id=shmget(TRAFFIC_LIGHT_KEY,sizeof(TrafficLightSegment),0666|(writer?IPC_CREAT:0));
    if(id<0){traffic_ipc_close(p);return -1;}if(p->segment&&p->id==id)return 0;
    traffic_ipc_close(p);void* q=shmat(id,0,0);if(q==(void*)-1)return -1;p->segment=(TrafficLightSegment*)q;p->id=id;return 0;
}
static inline void traffic_ipc_write(TrafficLightIPC* p,const TrafficLightData* d){
    if(!p->segment)return;
    uint32_t words[16];memcpy(words,d,sizeof(words));
    TrafficLightSegment* s=p->segment;uint32_t seq=__atomic_load_n(&s->sequence,__ATOMIC_RELAXED);
    __atomic_store_n(&s->sequence,(seq+1)|1u,__ATOMIC_SEQ_CST);__atomic_store_n(&s->magic,0x54464931u,__ATOMIC_RELAXED);
    for(int i=0;i<16;++i)__atomic_store_n(&s->words[i],words[i],__ATOMIC_RELAXED);
    __atomic_store_n(&s->sequence,(seq+2)&~1u,__ATOMIC_RELEASE);
}
static inline int traffic_ipc_read(TrafficLightIPC* p,TrafficLightData* d){
    if(!p->segment)return -1;
    TrafficLightSegment* s=p->segment;uint32_t words[16];
    for(int n=0;n<4;++n){uint32_t seq=__atomic_load_n(&s->sequence,__ATOMIC_ACQUIRE);if(seq&1)continue;
        uint32_t magic=__atomic_load_n(&s->magic,__ATOMIC_RELAXED);for(int i=0;i<16;++i)words[i]=__atomic_load_n(&s->words[i],__ATOMIC_RELAXED);
        __atomic_thread_fence(__ATOMIC_ACQUIRE);if(seq==__atomic_load_n(&s->sequence,__ATOMIC_RELAXED)&&magic==0x54464931u){memcpy(d,words,sizeof(words));return 0;}}
    return -1;
}
#endif
