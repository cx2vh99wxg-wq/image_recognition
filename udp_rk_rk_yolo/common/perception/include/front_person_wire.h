#ifndef FRONT_PERSON_WIRE_H
#define FRONT_PERSON_WIRE_H
#include "front_person_ipc.h"
#include <arpa/inet.h>
#include <algorithm>
namespace front_person_wire
{
constexpr size_t SIZE = 72;
inline uint32_t checksum(const uint8_t *p, size_t n)
{
    uint32_t v = 2166136261u;
    for (size_t i = 0; i < n; ++i)
        v = (v ^ p[i]) * 16777619u;
    return v;
}
inline void put(uint8_t *p, uint32_t v)
{
    v = htonl(v);
    memcpy(p, &v, 4);
}
inline uint32_t get(const uint8_t *p)
{
    uint32_t v;
    memcpy(&v, p, 4);
    return ntohl(v);
}
inline bool is_packet(const uint8_t *p, size_t n)
{
    return n >= 4 && get(p) == 0x46505331u;
}
inline bool valid(const FrontPersonData &d)
{
    const bool roi=(d.flags & FRONT_PERSON_ROI_MODE)!=0;
    const bool road=(d.flags & FRONT_PERSON_ROAD_VALID)!=0;
    if (roi) {
        if (d.risk_count>100000 || d.outside_count>100000 || d.unknown_count>100000 ||
            d.count!=d.risk_count+d.outside_count+d.unknown_count || d.road_pixels>320*240 ||
            !!(d.flags & FRONT_PERSON_IN_PATH)!=!!d.risk_count ||
            !!(d.flags & FRONT_PERSON_PATH_UNKNOWN)!=!!d.unknown_count ||
            (road && (d.road_pixels<64 || d.road_source_id!=d.source_id)) ||
            (!road && (d.risk_count || d.outside_count))) return false;
    } else if ((d.flags & ~7u) || d.risk_count || d.outside_count || d.unknown_count ||
               d.road_pixels || d.road_source_id) return false;
    return d.session && (d.flags & ~127u)==0 && d.quality<=100 && d.count<=100000 &&
           d.x<=320 && d.y<=240 && d.width<=320-d.x && d.height<=240-d.y &&
           (!(d.flags & FRONT_PERSON_FOUND) ||
            ((d.flags & (FRONT_PERSON_VALID|FRONT_PERSON_READY))==3 && d.count && d.width && d.height)) &&
           ((d.flags & FRONT_PERSON_FOUND) || !d.count);
}
inline bool encode(uint8_t *p, const FrontPersonData &d, uint64_t now)
{
    if (!front_person_fresh(&d, now) || !valid(d))
        return false;
    const uint32_t age =
        (uint32_t)((FRONT_PERSON_TTL_NS - (front_person_deadline(&d) - now) + 999999ULL) / 1000000ULL);
    const uint32_t v[] = {
        0x46505331u, 0x00020048u, d.session, d.source_id, d.count, age,
        d.flags, d.quality, d.x, d.y, d.width, d.height, d.risk_count,d.outside_count,
        d.unknown_count,d.road_pixels,d.road_source_id};
    for (int i = 0; i < 17; ++i)
        put(p + i * 4, v[i]);
    put(p + 68, checksum(p, 68));
    return true;
}
class Receiver
{
    uint32_t session_ = 0, id_ = 0, retired_[16] = {};
    unsigned retired_count_ = 0;
    uint64_t deadline_ = 0, last_peer_ = 0;

  public:
    bool ingest(const uint8_t *p, size_t n, uint64_t received, uint64_t now, FrontPersonData &d)
    {
        if (n != SIZE || get(p) != 0x46505331u ||
            (get(p + 4) != 0x00010048u && get(p + 4) != 0x00020048u) ||
            get(p + 68) != checksum(p, 68))
            return false;
        uint32_t session = get(p + 8), id = get(p + 12), age = get(p + 20);
        if (!session || age >= 750 || received > now)
            return false;
        FrontPersonData q = {};
        q.session = session;
        q.source_id = id;
        q.count=get(p+16); q.flags=get(p+24); q.quality=get(p+28);
        q.x=get(p+32); q.y=get(p+36); q.width=get(p+40); q.height=get(p+44);
        if (get(p+4)==0x00010048u) {
            if (q.flags & ~7u) return false;
            for (int i=48;i<68;i+=4) if(get(p+i)) return false;
        } else {
            q.risk_count=get(p+48); q.outside_count=get(p+52); q.unknown_count=get(p+56);
            q.road_pixels=get(p+60); q.road_source_id=get(p+64);
        }
        if (!valid(q))
            return false;
        bool changed = session != session_;
        if (changed)
        {
            for (unsigned i = 0; i < std::min(16u, retired_count_); ++i)
                if (session == retired_[i])
                    return false;
            if (session_ && now - last_peer_ < 750000000ULL && id > 8)
                return false;
        }
        else if (id != id_ && (int32_t)(id - id_) <= 0)
            return false;
        uint64_t expiry = received + (750 - age) * 1000000ULL;
        if (!changed && id == id_ && deadline_)
            expiry = std::min(expiry, deadline_);
        if (expiry <= now)
            return false;
        if (changed)
        {
            if (session_)
                retired_[retired_count_++ % 16] = session_;
            session_ = session;
        }
        id_ = id;
        deadline_ = expiry;
        last_peer_ = now;
        front_person_set_deadline(&q, expiry);
        d = q;
        return true;
    }
};
}
#endif
