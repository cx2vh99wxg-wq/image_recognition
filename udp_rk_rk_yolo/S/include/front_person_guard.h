#ifndef FRONT_PERSON_GUARD_H
#define FRONT_PERSON_GUARD_H
#include "front_person_ipc.h"
#define FRONT_GUARD_KEY (0x12345678 + 15)
#define FRONT_CLEAR_NS 3000000000ULL
enum { FRONT_STOP_REQUEST = 0x10, FRONT_RESUME_ALLOWED = 0x20 };

typedef struct {
    uint32_t session, source_id;
    uint64_t evidence_deadline, clear_since, clear_latest;
    unsigned have_source;
} FrontPersonGuard;

static inline unsigned front_person_guard_observe(FrontPersonGuard *p, const FrontPersonData *d,
                                                   uint64_t now, int link_ok)
{
    const unsigned valid = d && d->session && front_person_fresh(d, now) &&
        (d->flags & (FRONT_PERSON_VALID | FRONT_PERSON_READY)) ==
            (FRONT_PERSON_VALID | FRONT_PERSON_READY);
    if (!valid || !link_ok)
    {
        p->clear_since = p->clear_latest = 0;
        return FRONT_STOP_REQUEST;
    }

    const int changed = !p->have_source || d->session != p->session;
    const int32_t step = (int32_t)(d->source_id - p->source_id);
    const int person = (d->flags & FRONT_PERSON_FOUND) != 0;
    if (changed || now >= p->evidence_deadline || person)
        p->clear_since = p->clear_latest = 0;
    if (!changed && step < 0)
    {
        p->clear_since = p->clear_latest = 0;
        return FRONT_STOP_REQUEST;
    }
    if (changed || step > 0)
    {
        p->have_source = 1;
        p->session = d->session;
        p->source_id = d->source_id;
        p->evidence_deadline = front_person_deadline(d);
        if (!person)
        {
            if (!p->clear_since)
                p->clear_since = now;
            p->clear_latest = now;
        }
    }
    if (front_person_hazard(d))
        return FRONT_STOP_REQUEST;
    return p->clear_since && p->clear_latest - p->clear_since >= FRONT_CLEAR_NS
        ? FRONT_RESUME_ALLOWED : 0;
}
typedef struct
{
    uint32_t blocked, raw_request, auto_enabled, link_ok;
    uint32_t deadline_lo, deadline_hi, person, valid;
} FrontGuardData;
typedef struct
{
    uint32_t sequence, magic, words[8];
} FrontGuardSegment;
typedef struct
{
    int id;
    FrontGuardSegment *segment;
} FrontGuardIPC;
static inline uint64_t front_guard_deadline(const FrontGuardData *d)
{
    return ((uint64_t)d->deadline_hi << 32) | d->deadline_lo;
}
static inline void front_guard_set_deadline(FrontGuardData *d, uint64_t t)
{
    d->deadline_lo = (uint32_t)t;
    d->deadline_hi = (uint32_t)(t >> 32);
}
static inline int front_guard_fresh(const FrontGuardData *d, uint64_t now)
{
    uint64_t t = front_guard_deadline(d);
    return t > now && t - now <= 400000000ULL;
}
static inline void front_guard_close(FrontGuardIPC *p)
{
    if (p->segment)
        shmdt(p->segment);
    p->segment = 0;
    p->id = -1;
}
static inline int front_guard_open(FrontGuardIPC *p, int writer)
{
    int id = shmget(FRONT_GUARD_KEY, sizeof(FrontGuardSegment), 0666 | (writer ? IPC_CREAT : 0));
    if (id < 0)
    {
        front_guard_close(p);
        return -1;
    }
    if (p->segment && p->id == id)
        return 0;
    front_guard_close(p);
    void *q = shmat(id, 0, 0);
    if (q == (void *)-1)
        return -1;
    p->segment = (FrontGuardSegment *)q;
    p->id = id;
    return 0;
}
static inline void front_guard_write(FrontGuardIPC *p, const FrontGuardData *d)
{
    if (!p->segment)
        return;
    FrontGuardSegment *s = p->segment;
    uint32_t words[8];
    memcpy(words, d, sizeof(words));
    uint32_t seq = __atomic_load_n(&s->sequence, __ATOMIC_RELAXED);
    __atomic_store_n(&s->sequence, (seq + 1) | 1u, __ATOMIC_SEQ_CST);
    __atomic_store_n(&s->magic, 0x46474331u, __ATOMIC_RELAXED);
    for (int i = 0; i < 8; ++i)
        __atomic_store_n(&s->words[i], words[i], __ATOMIC_RELAXED);
    __atomic_store_n(&s->sequence, (seq + 2) & ~1u, __ATOMIC_RELEASE);
}
static inline int front_guard_read(FrontGuardIPC *p, FrontGuardData *d)
{
    if (!p->segment)
        return -1;
    FrontGuardSegment *s = p->segment;
    uint32_t words[8];
    for (int n = 0; n < 4; ++n)
    {
        uint32_t seq = __atomic_load_n(&s->sequence, __ATOMIC_ACQUIRE);
        if (seq & 1)
            continue;
        uint32_t magic = __atomic_load_n(&s->magic, __ATOMIC_RELAXED);
        for (int i = 0; i < 8; ++i)
            words[i] = __atomic_load_n(&s->words[i], __ATOMIC_RELAXED);
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        if (seq == __atomic_load_n(&s->sequence, __ATOMIC_RELAXED) && magic == 0x46474331u)
        {
            memcpy(d, words, sizeof(words));
            return 0;
        }
    }
    return -1;
}
#endif
