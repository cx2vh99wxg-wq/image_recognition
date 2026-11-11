#ifndef PERCEPTION_DISPLAY_VEHICLES_H
#define PERCEPTION_DISPLAY_VEHICLES_H

#include "display_people_wire.h"

namespace display_vehicles {

enum Kind : uint32_t { Bicycle = 1, Car = 2, Motorcycle = 3, Bus = 5 };
constexpr unsigned CAPACITY = display_people::CAPACITY;
constexpr uint32_t MAGIC = 0x56454831u;
constexpr int KEY = 0x12345678 + 19;
constexpr uint64_t TTL = display_people::TTL;

struct Vehicle {
    uint32_t kind;
    display_people::Person footprint;
};

struct Snapshot {
    uint32_t session, source, count, deadline_lo, deadline_hi;
    Vehicle vehicles[CAPACITY];
};

static_assert(sizeof(Vehicle) == 40 && sizeof(Snapshot) == 20 + CAPACITY * 40,
              "vehicle display ABI");

inline uint64_t deadline(const Snapshot &s)
{
    return (uint64_t(s.deadline_hi) << 32) | s.deadline_lo;
}

inline void expires(Snapshot &s, uint64_t t)
{
    s.deadline_lo = uint32_t(t);
    s.deadline_hi = uint32_t(t >> 32);
}

inline bool fresh(const Snapshot &s, uint64_t now)
{
    return deadline(s) > now && deadline(s) - now <= TTL;
}

inline bool valid(const Snapshot &s)
{
    if (!s.session || s.count > CAPACITY)
        return false;
    for (unsigned i = 0; i < s.count; ++i) {
        const auto &v = s.vehicles[i];
        if (v.kind != Bicycle && v.kind != Car && v.kind != Motorcycle && v.kind != Bus)
            return false;
        const auto &p = v.footprint;
        if (p.view != display_people::Front || p.x >= 320 || p.y >= 240 || !p.w || !p.h ||
            p.w > 320 - p.x || p.h > 240 - p.y || p.quality > 100 || (p.flags & ~15u) ||
            (p.flags & 6u) == 6u || std::abs(int64_t(p.gx)) > 100000 || std::abs(int64_t(p.gy)) > 100000)
            return false;
    }
    return true;
}

using IPC = display_people::SnapshotIPC<Snapshot, MAGIC>;
using display_people::get;
using display_people::put;
using display_people::hash;
constexpr size_t WIRE_SIZE = 24 + CAPACITY * sizeof(Vehicle) + 4;

inline bool packet(const uint8_t *p, size_t n) { return n >= 4 && get(p) == MAGIC; }

inline bool encode(uint8_t *p, const Snapshot &s, uint64_t now)
{
    if (!fresh(s, now) || !valid(s))
        return false;
    const uint32_t remaining_ms = (deadline(s) - now) / 1000000ULL;
    if (!remaining_ms)
        return false;
    std::memset(p, 0, WIRE_SIZE);
    put(p, MAGIC);
    put(p + 4, 1);
    put(p + 8, s.session);
    put(p + 12, s.source);
    put(p + 16, s.count);
    put(p + 20, remaining_ms);
    for (unsigned i = 0; i < s.count; ++i) {
        uint32_t words[sizeof(Vehicle) / 4];
        std::memcpy(words, &s.vehicles[i], sizeof(words));
        for (unsigned j = 0; j < sizeof(words) / 4; ++j)
            put(p + 24 + i * sizeof(Vehicle) + j * 4, words[j]);
    }
    put(p + WIRE_SIZE - 4, hash(p, WIRE_SIZE - 4));
    return true;
}

class Receiver {
    uint32_t session_ = 0, source_ = 0, retired_[16] = {}, retired_count_ = 0;

  public:
    bool ingest(const uint8_t *p, size_t n, uint64_t received, uint64_t now, Snapshot &out)
    {
        if (n != WIRE_SIZE || !packet(p, n) || get(p + 4) != 1 ||
            get(p + WIRE_SIZE - 4) != hash(p, WIRE_SIZE - 4) || received > now)
            return false;
        Snapshot s = {};
        s.session = get(p + 8);
        s.source = get(p + 12);
        s.count = get(p + 16);
        const uint32_t remaining = get(p + 20);
        if (!remaining || remaining > TTL / 1000000ULL || s.count > CAPACITY)
            return false;
        for (unsigned i = 0; i < s.count; ++i) {
            uint32_t words[sizeof(Vehicle) / 4];
            for (unsigned j = 0; j < sizeof(words) / 4; ++j)
                words[j] = get(p + 24 + i * sizeof(Vehicle) + j * 4);
            std::memcpy(&s.vehicles[i], words, sizeof(words));
        }
        if (!valid(s))
            return false;
        const uint64_t end = received + uint64_t(remaining) * 1000000ULL;
        if (end <= now)
            return false;
        if (session_ && session_ != s.session) {
            for (unsigned i = 0; i < std::min(retired_count_, 16u); ++i)
                if (retired_[i] == s.session)
                    return false;
            retired_[retired_count_++ % 16] = session_;
        } else if (session_) {
            // The sender periodically repeats a snapshot. A duplicate must not extend its lifetime.
            if (int32_t(s.source - source_) <= 0)
                return false;
        }
        session_ = s.session;
        source_ = s.source;
        expires(s, end);
        out = s;
        return true;
    }
};
}
#endif
