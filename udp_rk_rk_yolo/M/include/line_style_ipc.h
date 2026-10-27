#ifndef LINE_STYLE_IPC_H
#define LINE_STYLE_IPC_H
#include "line_style.h"
#include <stdint.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
/* Independent segments: existing video, curve and brake ABIs are unchanged. */
#define LINE_STYLE_KEY (0x12345678+9)
#define LINE_WARNING_KEY (0x12345678+10)
/* NN takes about 265ms on M. One-second capture-based lifetime covers two
 * inference intervals without repeatedly opening the solid-line gate. */
#define LINE_STYLE_TTL_NS 1000000000ULL
typedef struct {
    uint32_t session, source_id, left, right, left_quality, right_quality;
    uint32_t left_duty, right_duty, left_spans, right_spans;
    uint32_t deadline_lo, deadline_hi;
    // Detection segment: packed analysis reasons/raw candidate types.
    // Warning segment: actual HMI input. Layout remains fourteen u32 words.
    union { uint32_t diagnostic; uint32_t hmi_code; };
    uint32_t blocked;
} LineStyleData;
typedef struct { uint32_t sequence, magic; uint32_t words[14]; } LineStyleSegment;
typedef struct { int id; LineStyleSegment* segment; } LineStyleIPC;
static inline uint64_t line_clock_ns(void) {
    struct timespec t; if(clock_gettime(CLOCK_MONOTONIC,&t))return 0;
    return (uint64_t)t.tv_sec*1000000000ULL+t.tv_nsec;
}
static inline uint64_t line_deadline(const LineStyleData* d) {return ((uint64_t)d->deadline_hi<<32)|d->deadline_lo;}
static inline void line_set_deadline(LineStyleData* d,uint64_t t) {d->deadline_lo=(uint32_t)t;d->deadline_hi=(uint32_t)(t>>32);}
static inline int line_fresh(const LineStyleData* d,uint64_t now) {
    uint64_t deadline=line_deadline(d);return deadline>now&&deadline-now<=1000000000ULL;
}
static inline LineStyleData line_result_data(const YOLOLineStyleResult* r,uint32_t session,uint32_t id,uint64_t captured) {
    LineStyleData d;memset(&d,0,sizeof(d));d.session=session;d.source_id=id;
    d.left=r->left.type;d.right=r->right.type;d.left_quality=r->left.quality;d.right_quality=r->right.quality;
    d.left_duty=r->left.duty;d.right_duty=r->right.duty;d.left_spans=r->left.spans;d.right_spans=r->right.spans;
    d.diagnostic=(r->left.reason&255u)|((r->right.reason&255u)<<8)|((r->left.raw_type&3u)<<16)|((r->right.raw_type&3u)<<18);
    // A tracked/held class keeps the timestamp of its real observation.
    // New NN source IDs cannot extend the lifetime of old line evidence.
    uint64_t evidence=captured;
    if(r->left.type&&r->left.evidence_ns&&r->left.evidence_ns<evidence)evidence=r->left.evidence_ns;
    if(r->right.type&&r->right.evidence_ns&&r->right.evidence_ns<evidence)evidence=r->right.evidence_ns;
    line_set_deadline(&d,evidence+LINE_STYLE_TTL_NS);return d;
}
static inline void line_ipc_close(LineStyleIPC* ipc) {
    if(ipc->segment)shmdt(ipc->segment);
    ipc->segment=0;ipc->id=-1;
}
static inline int line_ipc_open(LineStyleIPC* ipc,int key,int writer) {
    int id=shmget((key_t)key,sizeof(LineStyleSegment),0666|(writer?IPC_CREAT:0));
    if(id<0){line_ipc_close(ipc);return -1;}
    if(ipc->segment&&ipc->id==id)return 0;
    line_ipc_close(ipc);void* p=shmat(id,0,0);if(p==(void*)-1)return -1;
    ipc->id=id;ipc->segment=(LineStyleSegment*)p;return 0;
}
static inline void line_ipc_write(LineStyleIPC* ipc,const LineStyleData* data) {
    if(!ipc->segment)return;
    uint32_t words[14];memcpy(words,data,sizeof(words));
    LineStyleSegment* s=ipc->segment;
    uint32_t seq=__atomic_load_n(&s->sequence,__ATOMIC_RELAXED);
    __atomic_store_n(&s->sequence,(seq+1)|1u,__ATOMIC_SEQ_CST);
    __atomic_store_n(&s->magic,0x4c534931u,__ATOMIC_RELAXED);
    for(int i=0;i<14;++i)__atomic_store_n(&s->words[i],words[i],__ATOMIC_RELAXED);
    __atomic_store_n(&s->sequence,(seq+2)&~1u,__ATOMIC_RELEASE);
}
static inline int line_ipc_read(LineStyleIPC* ipc,LineStyleData* data) {
    if(!ipc->segment)return -1;
    LineStyleSegment* s=ipc->segment;uint32_t words[14];
    for(int retry=0;retry<4;++retry) {
        uint32_t seq=__atomic_load_n(&s->sequence,__ATOMIC_ACQUIRE);if(seq&1)continue;
        uint32_t magic=__atomic_load_n(&s->magic,__ATOMIC_RELAXED);
        for(int i=0;i<14;++i)words[i]=__atomic_load_n(&s->words[i],__ATOMIC_RELAXED);
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        if(seq==__atomic_load_n(&s->sequence,__ATOMIC_ACQUIRE)&&magic==0x4c534931u){memcpy(data,words,sizeof(words));return 0;}
    }
    return -1;
}
#endif
