#ifndef VEHICLE_SPEED_STATUS_H
#define VEHICLE_SPEED_STATUS_H
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/shm.h>
/* Estimated remote gear, NOT a wheel-speed sensor. The FPGA counts the
 * shared active-low key and starts at the operator's known gear-1 baseline. */
typedef struct {unsigned gear,known,pressed,driving;} SpeedSnapshot;
static inline uint8_t speed_crc8(uint8_t value){
    uint8_t c=0xa7;
    for(int i=0;i<8;++i)c=(uint8_t)((c&0x80)?(c<<1)^7:c<<1);
    c^=value;
    for(int i=0;i<8;++i)c=(uint8_t)((c&0x80)?(c<<1)^7:c<<1);
    return c;
}
static inline int speed_decode_packet(const uint8_t bytes[4],SpeedSnapshot* out){
    unsigned v=bytes[2],gear=v&7;
    if((bytes[0]>>4)!=(bytes[0]&15)||bytes[1]!=0xa7||
       bytes[3]!=speed_crc8(bytes[2])||!(v&0x80)||(v&8)||gear>3||((v&0x40)&&!gear))return -1;
    SpeedSnapshot s={gear,(v>>6)&1,(v>>5)&1,(v>>4)&1};*out=s;return 0;
}
#define SPEED_WARNING_KEY (0x12345678+13)
#define SPEED_WARNING_TTL_NS 2000000000ULL
typedef struct {
    uint32_t gear,known,link_ok,read_errno;
    uint32_t pressed,driving,raw_byte,poll_id;
    uint32_t deadline_lo,deadline_hi;
} SpeedWarningData;
typedef struct {uint32_t sequence,magic,words[10];} SpeedWarningSegment;
typedef struct {int id;SpeedWarningSegment* segment;} SpeedWarningIPC;
static inline uint64_t speed_clock_ns(void){struct timespec t;if(clock_gettime(CLOCK_MONOTONIC,&t))return 0;return (uint64_t)t.tv_sec*1000000000ULL+t.tv_nsec;}
static inline uint64_t speed_deadline(const SpeedWarningData* d){return ((uint64_t)d->deadline_hi<<32)|d->deadline_lo;}
static inline void speed_set_deadline(SpeedWarningData* d,uint64_t t){d->deadline_lo=(uint32_t)t;d->deadline_hi=(uint32_t)(t>>32);}
static inline int speed_fresh(const SpeedWarningData* d,uint64_t now){uint64_t t=speed_deadline(d);return d->link_ok&&t>now&&t-now<=SPEED_WARNING_TTL_NS;}
static inline void speed_ipc_close(SpeedWarningIPC* p){if(p->segment)shmdt(p->segment);p->segment=0;p->id=-1;}
static inline int speed_ipc_open(SpeedWarningIPC* p,int writer){
    int id=shmget(SPEED_WARNING_KEY,sizeof(SpeedWarningSegment),0666|(writer?IPC_CREAT:0));
    if(id<0){speed_ipc_close(p);return -1;}if(p->segment&&p->id==id)return 0;
    speed_ipc_close(p);void* q=shmat(id,0,0);if(q==(void*)-1)return -1;
    p->segment=(SpeedWarningSegment*)q;p->id=id;return 0;
}
static inline void speed_ipc_write(SpeedWarningIPC* p,const SpeedWarningData* d){
    if(!p->segment)return;
    SpeedWarningSegment* s=p->segment;uint32_t words[10];memcpy(words,d,sizeof(words));
    uint32_t seq=__atomic_load_n(&s->sequence,__ATOMIC_RELAXED);
    __atomic_store_n(&s->sequence,(seq+1)|1u,__ATOMIC_SEQ_CST);__atomic_store_n(&s->magic,0x53504431u,__ATOMIC_RELAXED);
    for(int i=0;i<10;++i)__atomic_store_n(&s->words[i],words[i],__ATOMIC_RELAXED);
    __atomic_store_n(&s->sequence,(seq+2)&~1u,__ATOMIC_RELEASE);
}
static inline int speed_ipc_read(SpeedWarningIPC* p,SpeedWarningData* d){
    if(!p->segment)return -1;
    SpeedWarningSegment* s=p->segment;uint32_t words[10];
    for(int n=0;n<4;++n){uint32_t seq=__atomic_load_n(&s->sequence,__ATOMIC_ACQUIRE);if(seq&1)continue;
        uint32_t magic=__atomic_load_n(&s->magic,__ATOMIC_RELAXED);
        for(int i=0;i<10;++i)words[i]=__atomic_load_n(&s->words[i],__ATOMIC_RELAXED);
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        if(seq==__atomic_load_n(&s->sequence,__ATOMIC_RELAXED)&&magic==0x53504431u){memcpy(d,words,sizeof(words));return 0;}}
    return -1;
}
#endif
