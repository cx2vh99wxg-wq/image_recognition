#ifndef TRAFFIC_GO_RULE_H
#define TRAFFIC_GO_RULE_H
#include "traffic_light_ipc.h"

/* reg5 is one atomic permission byte: 0/1 lane denies, 2 red SET,
 * 3 confirmed-green CLEAR, 6 valid, 7 heartbeat. Never write motion reg0.
 * No color, stale metadata, yellow and AUTO_BREAK do not release a red hold. */
enum { TRAFFIC_GO_SET=4, TRAFFIC_GO_CLEAR=8 };
typedef struct { unsigned blocked; } TrafficGoPolicy;
static inline unsigned traffic_go_color(const TrafficLightData* d,uint64_t now) {
    const unsigned required=TRAFFIC_BOX|TRAFFIC_CONFIRMED|TRAFFIC_MODEL_READY;
    if(!traffic_fresh(d,now)||(d->flags&required)!=required||
       (d->flags&TRAFFIC_DISABLED)||d->state!=d->raw_state)return TRAFFIC_UNKNOWN;
    return d->state<=TRAFFIC_GREEN?d->state:TRAFFIC_UNKNOWN;
}
static inline unsigned traffic_go_prepare(TrafficGoPolicy* p,unsigned color) {
    if(color==TRAFFIC_RED)p->blocked=1;
    if(color==TRAFFIC_GREEN)return TRAFFIC_GO_CLEAR;
    return p->blocked?TRAFFIC_GO_SET:0;
}
static inline void traffic_go_commit(TrafficGoPolicy* p,unsigned command,int written) {
    if(written&&(command&TRAFFIC_GO_CLEAR)&&!(command&TRAFFIC_GO_SET))p->blocked=0;
}

/* Independent, single-writer control telemetry. Kept across process restarts
 * to retain the display warning; it is not an electrical pin readback. */
#define TRAFFIC_GO_KEY (0x12345678+12)
typedef struct {
    uint32_t blocked,write_ok,signal_state,signal_fresh;
    uint32_t deadline_lo,deadline_hi,session,source_id;
} TrafficGoData;
typedef struct {uint32_t sequence,magic,words[8];} TrafficGoSegment;
typedef struct {int id;TrafficGoSegment* segment;} TrafficGoIPC;
static inline uint64_t traffic_go_deadline(const TrafficGoData* d){return ((uint64_t)d->deadline_hi<<32)|d->deadline_lo;}
static inline void traffic_go_set_deadline(TrafficGoData* d,uint64_t t){d->deadline_lo=(uint32_t)t;d->deadline_hi=(uint32_t)(t>>32);}
static inline int traffic_go_fresh(const TrafficGoData* d,uint64_t now){uint64_t t=traffic_go_deadline(d);return t>now&&t-now<=400000000ULL;}
static inline void traffic_go_close(TrafficGoIPC* p){if(p->segment)shmdt(p->segment);p->segment=0;p->id=-1;}
static inline int traffic_go_open(TrafficGoIPC* p,int writer){
    int id=shmget(TRAFFIC_GO_KEY,sizeof(TrafficGoSegment),0666|(writer?IPC_CREAT:0));
    if(id<0){traffic_go_close(p);return -1;}if(p->segment&&p->id==id)return 0;
    traffic_go_close(p);void* q=shmat(id,0,0);if(q==(void*)-1)return -1;
    p->segment=(TrafficGoSegment*)q;p->id=id;return 0;
}
static inline void traffic_go_write(TrafficGoIPC* p,const TrafficGoData* d){
    if(!p->segment)return;
    TrafficGoSegment* s=p->segment;uint32_t words[8];memcpy(words,d,sizeof(words));
    uint32_t seq=__atomic_load_n(&s->sequence,__ATOMIC_RELAXED);
    __atomic_store_n(&s->sequence,(seq+1)|1u,__ATOMIC_SEQ_CST);
    __atomic_store_n(&s->magic,0x54474731u,__ATOMIC_RELAXED);
    for(int i=0;i<8;++i)__atomic_store_n(&s->words[i],words[i],__ATOMIC_RELAXED);
    __atomic_store_n(&s->sequence,(seq+2)&~1u,__ATOMIC_RELEASE);
}
static inline int traffic_go_read(TrafficGoIPC* p,TrafficGoData* d){
    if(!p->segment)return -1;
    TrafficGoSegment* s=p->segment;uint32_t words[8];
    for(int n=0;n<4;++n){uint32_t seq=__atomic_load_n(&s->sequence,__ATOMIC_ACQUIRE);if(seq&1)continue;
        uint32_t magic=__atomic_load_n(&s->magic,__ATOMIC_RELAXED);
        for(int i=0;i<8;++i)words[i]=__atomic_load_n(&s->words[i],__ATOMIC_RELAXED);
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        if(seq==__atomic_load_n(&s->sequence,__ATOMIC_RELAXED)&&magic==0x54474731u){memcpy(d,words,sizeof(words));return 0;}}
    return -1;
}
#endif
