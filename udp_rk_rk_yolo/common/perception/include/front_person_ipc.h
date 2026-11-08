#ifndef FRONT_PERSON_IPC_H
#define FRONT_PERSON_IPC_H
#include "front_person.h"
#include <sys/shm.h>
#include <string.h>
#include <time.h>
#define FRONT_PERSON_KEY (0x12345678 + 14)
#define FRONT_PERSON_TTL_NS 750000000ULL
typedef struct
{
    uint32_t sequence, magic, words[16];
} FrontPersonSegment;
typedef struct
{
    int id;
    FrontPersonSegment *segment;
} FrontPersonIPC;
static inline uint64_t front_person_clock_ns(void)
{
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t))
        return 0;
    return (uint64_t)t.tv_sec * 1000000000ULL + t.tv_nsec;
}
static inline uint64_t front_person_deadline(const FrontPersonData *d)
{
    return ((uint64_t)d->deadline_hi << 32) | d->deadline_lo;
}
static inline void front_person_set_deadline(FrontPersonData *d, uint64_t ns)
{
    d->deadline_lo = (uint32_t)ns;
    d->deadline_hi = (uint32_t)(ns >> 32);
}
static inline int front_person_fresh(const FrontPersonData *d, uint64_t now)
{
    uint64_t t = front_person_deadline(d);
    return t > now && t - now <= FRONT_PERSON_TTL_NS;
}
static inline void front_person_ipc_close(FrontPersonIPC *p)
{
    if (p->segment)
        shmdt(p->segment);
    p->segment = 0;
    p->id = -1;
}
static inline int front_person_ipc_open(FrontPersonIPC *p, int writer)
{
    int id =
        shmget(FRONT_PERSON_KEY, sizeof(FrontPersonSegment), 0666 | (writer ? IPC_CREAT : 0));
    if (id < 0)
    {
        front_person_ipc_close(p);
        return -1;
    }
    if (p->segment && p->id == id)
        return 0;
    front_person_ipc_close(p);
    void *q = shmat(id, 0, 0);
    if (q == (void *)-1)
        return -1;
    p->segment = (FrontPersonSegment *)q;
    p->id = id;
    return 0;
}
static inline void front_person_ipc_write(FrontPersonIPC *p, const FrontPersonData *d)
{
    if (!p->segment)
        return;
    uint32_t words[16];
    memcpy(words, d, sizeof(words));
    FrontPersonSegment *s = p->segment;
    uint32_t seq = __atomic_load_n(&s->sequence, __ATOMIC_RELAXED);
    __atomic_store_n(&s->sequence, (seq + 1) | 1u, __ATOMIC_SEQ_CST);
    __atomic_store_n(&s->magic, 0x46504931u, __ATOMIC_RELAXED);
    for (int i = 0; i < 16; ++i)
        __atomic_store_n(&s->words[i], words[i], __ATOMIC_RELAXED);
    __atomic_store_n(&s->sequence, (seq + 2) & ~1u, __ATOMIC_RELEASE);
}
static inline int front_person_ipc_read(FrontPersonIPC *p, FrontPersonData *d)
{
    if (!p->segment)
        return -1;
    FrontPersonSegment *s = p->segment;
    uint32_t words[16];
    for (int n = 0; n < 4; ++n)
    {
        uint32_t seq = __atomic_load_n(&s->sequence, __ATOMIC_ACQUIRE);
        if (seq & 1)
            continue;
        uint32_t magic = __atomic_load_n(&s->magic, __ATOMIC_RELAXED);
        for (int i = 0; i < 16; ++i)
            words[i] = __atomic_load_n(&s->words[i], __ATOMIC_RELAXED);
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        if (seq == __atomic_load_n(&s->sequence, __ATOMIC_RELAXED) && magic == 0x46504931u)
        {
            memcpy(d, words, sizeof(words));
            return 0;
        }
    }
    return -1;
}
#endif
