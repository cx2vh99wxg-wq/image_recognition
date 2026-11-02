#ifndef FPGA_VIDEO_TRANSPORT_V2_H
#define FPGA_VIDEO_TRANSPORT_V2_H

#include <arpa/inet.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace video_v2 {
constexpr uint32_t MAGIC = 0x46505632u; // "FPV2", independent datagrams.
constexpr uint32_t VERSION = 3;
constexpr uint64_t CURVE_TTL_NS = 500000000ULL;
constexpr uint32_t AGE_OBSERVED = 0x80000000u, AGE_UNKNOWN = 0xffffffffu, AGE_MASK = 0x7fffffffu;
constexpr unsigned WIDTH = 640, HEIGHT = 480, LINE_BYTES = WIDTH * 2;
constexpr size_t FRAME_BYTES = LINE_BYTES * HEIGHT;
constexpr unsigned BATCH = 32;
constexpr uint64_t FRAME_TIMEOUT_NS = 250000000ULL;

struct Metadata {
    uint32_t frame_id = 0, checksum = 0;
    uint32_t curve_type = 0, curve_confidence = 0, curve_angle = 0;
    uint32_t version = VERSION, session_id = 0, source_frame_id = 0, curve_frame_id = 0, curve_age_ms = AGE_UNKNOWN;
    uint64_t source = 0, first_packet_ns = 0;
};
struct WireHeaderV2 {
    uint32_t magic, version, frame_id, width, height, line_id, line_bytes;
    uint32_t frame_checksum, line_checksum, curve_type, curve_confidence, curve_angle;
};
struct WireHeader : WireHeaderV2 {
    uint32_t session_id, source_frame_id, curve_frame_id, curve_age_ms;
};
static_assert(sizeof(WireHeaderV2) == 48, "V2 header stays 48 bytes");
static_assert(sizeof(WireHeader) == 64, "V3 header must be 64 bytes");
static_assert(sizeof(WireHeader) + LINE_BYTES <= 1472, "No IPv4 fragmentation at MTU 1500");

inline uint32_t checksum(const uint8_t* data, size_t size) {
    uint32_t result = 0;
    for (size_t i = 0; i < size; ++i) result += data[i];
    return result;
}
inline bool newer(uint32_t a, uint32_t b) { return static_cast<int32_t>(a - b) > 0; }
inline WireHeader make_header(const Metadata& m, unsigned row, uint32_t row_sum) {
    WireHeader h = {};
    h.magic = htonl(MAGIC); h.version = htonl(VERSION);
    h.frame_id = htonl(m.frame_id); h.width = htonl(WIDTH); h.height = htonl(HEIGHT);
    h.line_id = htonl(row); h.line_bytes = htonl(LINE_BYTES);
    h.frame_checksum = htonl(m.checksum); h.line_checksum = htonl(row_sum);
    h.curve_type = htonl(m.curve_type); h.curve_confidence = htonl(m.curve_confidence);
    h.curve_angle = htonl(m.curve_angle);
    h.session_id = htonl(m.session_id); h.source_frame_id = htonl(m.source_frame_id);
    h.curve_frame_id = htonl(m.curve_frame_id); h.curve_age_ms = htonl(m.curve_age_ms);
    return h;
}
inline bool is_packet(const uint8_t* data, size_t size) {
    if (size != sizeof(WireHeader) + LINE_BYTES && size != sizeof(WireHeaderV2) + LINE_BYTES) return false;
    uint32_t magic, version;
    std::memcpy(&magic, data, 4); std::memcpy(&version, data + 4, 4);
    return ntohl(magic) == MAGIC && ((ntohl(version) == VERSION && size == sizeof(WireHeader) + LINE_BYTES) ||
                                   (ntohl(version) == 2 && size == sizeof(WireHeaderV2) + LINE_BYTES));
}

// NN snapshots are aged from the original input capture, never from raw video updates.
class CurveSenderState {
    bool have_ = false, expired_ = false;
    uint32_t id_ = 0, type_ = 0, confidence_ = 0, angle_ = 0;
    uint64_t captured_ = 0, observed_ = 0;
public:
    void reset() { *this = CurveSenderState{}; }
    void observe(uint32_t id, uint32_t type, uint32_t confidence, uint32_t angle,
                 uint64_t captured, uint64_t now) {
        if (!have_ || newer(id, id_)) {
            have_ = true; expired_ = false; id_ = id; type_ = type;
            confidence_ = confidence; angle_ = angle; captured_ = captured; observed_ = now;
        } else if (id == id_ && !confidence) {
            confidence_ = 0; expired_ = true;
        }
    }
    void fill(Metadata& meta, uint64_t now) {
        meta.curve_frame_id = id_; meta.curve_type = type_; meta.curve_angle = angle_;
        if (!have_) { meta.curve_confidence = 0; meta.curve_age_ms = AGE_UNKNOWN; return; }
        const uint64_t origin = captured_ ? captured_ : observed_;
        const uint64_t age = now >= origin ? now - origin : CURVE_TTL_NS;
        if (age >= CURVE_TTL_NS) expired_ = true;
        meta.curve_age_ms = static_cast<uint32_t>(std::min<uint64_t>(age / 1000000ULL, AGE_MASK - 1));
        if (!captured_) meta.curve_age_ms |= AGE_OBSERVED;
        meta.curve_confidence = expired_ ? 0 : confidence_;
    }
};
struct CurveEvent { uint32_t frame_id = 0, type = 0, confidence = 0, angle = 0; };
class CurveReceiverState {
    bool have_ = false, expired_ = false;
    uint32_t session_ = 0;
    uint64_t source_ = 0, deadline_ = 0;
    CurveEvent current_;
public:
    bool observe(const Metadata& meta, uint64_t now, CurveEvent& event) {
        if (meta.version < 3) {
            if (!have_ || current_.confidence) {
                have_ = true; expired_ = true; current_ = CurveEvent{}; event = current_; return true;
            }
            return false;
        }
        const bool session_changed = !have_ || source_ != meta.source || session_ != meta.session_id;
        if (!session_changed && meta.curve_frame_id != current_.frame_id &&
            !newer(meta.curve_frame_id, current_.frame_id)) return false;
        if (session_changed || meta.curve_frame_id != current_.frame_id) {
            source_ = meta.source; session_ = meta.session_id; have_ = true;
            current_.frame_id = meta.curve_frame_id; current_.type = meta.curve_type;
            current_.angle = meta.curve_angle;
            uint64_t age = meta.curve_age_ms == AGE_UNKNOWN ? CURVE_TTL_NS :
                           static_cast<uint64_t>(meta.curve_age_ms & AGE_MASK) * 1000000ULL;
            if (meta.first_packet_ns && now >= meta.first_packet_ns) age += now - meta.first_packet_ns;
            expired_ = !meta.curve_confidence || age >= CURVE_TTL_NS;
            current_.confidence = expired_ ? 0 : meta.curve_confidence;
            deadline_ = expired_ ? now : now + CURVE_TTL_NS - age;
            event = current_; return true;
        }
        // Repeated video metadata must not extend the original NN deadline or revive expiry.
        if (!expired_ && (!meta.curve_confidence || meta.curve_age_ms == AGE_UNKNOWN ||
            static_cast<uint64_t>(meta.curve_age_ms & AGE_MASK) * 1000000ULL >= CURVE_TTL_NS)) {
            expired_ = true; current_.confidence = 0; event = current_; return true;
        }
        return expire(now, event);
    }
    bool expire(uint64_t now, CurveEvent& event) {
        if (have_ && !expired_ && now >= deadline_) {
            expired_ = true; current_.confidence = 0; event = current_; return true;
        }
        return false;
    }
};

struct Statistics {
    uint64_t complete = 0, published = 0, incomplete = 0;
    uint64_t corrupt = 0, duplicates = 0, old_packets = 0, skipped_complete = 0;
    uint64_t assembly_ns = 0;
};

// Exactly two in-flight frames. Missing lines never block a newer complete frame.
class Assembler {
    struct Slot {
        bool used = false, complete = false;
        Metadata meta;
        uint64_t source = 0, started = 0;
        unsigned count = 0;
        uint32_t sum = 0;
        std::array<uint8_t, HEIGHT> seen = {};
        std::vector<uint8_t> pixels;
        Slot() : pixels(FRAME_BYTES) {}
        void reset() { used = complete = false; count = 0; sum = 0; seen.fill(0); }
    };
    Slot slots_[2];
    bool have_source_ = false, have_published_ = false;
    uint64_t source_ = 0, last_packet_ns_ = 0;
    uint32_t session_ = 0, version_ = 0;
    struct Retired { uint64_t source = 0; uint32_t session = 0, version = 0; };
    std::array<Retired, 8> retired_ = {};
    unsigned retired_count_ = 0, retired_next_ = 0;
    uint32_t published_id_ = 0;
    Statistics stats_;

    void abandon(Slot& slot) {
        if (slot.used) {
            if (slot.complete) ++stats_.skipped_complete;
            else ++stats_.incomplete;
        }
        slot.reset();
    }
public:
    const Statistics& statistics() const { return stats_; }
    void reset_session() {
        for (Slot& slot : slots_) abandon(slot);
        have_source_ = have_published_ = false;
        source_ = last_packet_ns_ = 0;
        retired_count_ = retired_next_ = 0;
    }

    // Returns 1 on frame completion, 0 on accepted/ignored packet, -1 on corruption.
    int ingest(const uint8_t* packet, size_t size, uint64_t source, uint64_t now, uint64_t received_ns = 0) {
        if (!is_packet(packet, size)) { ++stats_.corrupt; return -1; }
        WireHeader wire = {};
        const size_t header_size = size - LINE_BYTES;
        std::memcpy(&wire, packet, header_size);
        Metadata meta;
        meta.frame_id = ntohl(wire.frame_id); meta.checksum = ntohl(wire.frame_checksum);
        meta.curve_type = ntohl(wire.curve_type);
        meta.curve_confidence = ntohl(wire.curve_confidence);
        meta.curve_angle = ntohl(wire.curve_angle);
        meta.version = ntohl(wire.version); meta.source = source;
        if (meta.version >= 3) {
            meta.session_id = ntohl(wire.session_id); meta.source_frame_id = ntohl(wire.source_frame_id);
            meta.curve_frame_id = ntohl(wire.curve_frame_id); meta.curve_age_ms = ntohl(wire.curve_age_ms);
        } else { meta.source_frame_id = meta.frame_id; meta.curve_confidence = 0; }
        unsigned row = ntohl(wire.line_id);
        uint32_t row_sum = ntohl(wire.line_checksum);
        if (ntohl(wire.width) != WIDTH ||
            ntohl(wire.height) != HEIGHT || row >= HEIGHT ||
            ntohl(wire.line_bytes) != LINE_BYTES || meta.curve_type > 3 ||
            meta.curve_confidence > 100 || meta.curve_angle > 360 ||
            checksum(packet + header_size, LINE_BYTES) != row_sum) {
            ++stats_.corrupt; return -1;
        }
        const bool changed = !have_source_ || source != source_ || meta.session_id != session_ || meta.version != version_;
        if (changed) {
            for (unsigned i = 0; i < retired_count_; ++i)
                if (retired_[i].source == source && retired_[i].session == meta.session_id && retired_[i].version == meta.version) {
                    ++stats_.old_packets; return 0;
                }
            if (have_source_ && meta.version < 3 && now - last_packet_ns_ <= FRAME_TIMEOUT_NS) {
                ++stats_.old_packets; return 0;
            }
            if (have_source_) {
                retired_[retired_next_].source = source_; retired_[retired_next_].session = session_;
                retired_[retired_next_].version = version_; retired_next_ = (retired_next_ + 1) % retired_.size();
                retired_count_ = std::min<unsigned>(retired_count_ + 1, retired_.size());
            }
            for (Slot& slot : slots_) abandon(slot);
            source_ = source; session_ = meta.session_id; version_ = meta.version;
            have_source_ = true; have_published_ = false;
        }
        last_packet_ns_ = now;
        if (have_published_ && !newer(meta.frame_id, published_id_)) {
            ++stats_.old_packets; return 0;
        }
        for (Slot& slot : slots_)
            if (slot.used && now - slot.started > FRAME_TIMEOUT_NS) abandon(slot);
        Slot* selected = nullptr;
        for (Slot& slot : slots_)
            if (slot.used && slot.meta.frame_id == meta.frame_id) selected = &slot;
        if (!selected) {
            for (Slot& slot : slots_) if (!slot.used) { selected = &slot; break; }
            if (!selected) {
                selected = newer(slots_[0].meta.frame_id, slots_[1].meta.frame_id) ?
                    &slots_[1] : &slots_[0];
                if (!newer(meta.frame_id, selected->meta.frame_id)) {
                    ++stats_.old_packets; return 0;
                }
                abandon(*selected);
            }
            selected->reset(); selected->used = true;
            selected->meta = meta;
            selected->started = received_ns && received_ns <= now ? received_ns : now;
            selected->meta.first_packet_ns = selected->started; selected->source = source;
        }
        if (selected->meta.checksum != meta.checksum ||
            selected->meta.curve_type != meta.curve_type ||
            selected->meta.curve_confidence != meta.curve_confidence ||
            selected->meta.curve_angle != meta.curve_angle ||
            selected->meta.source_frame_id != meta.source_frame_id ||
            selected->meta.curve_frame_id != meta.curve_frame_id ||
            selected->meta.curve_age_ms != meta.curve_age_ms) {
            ++stats_.corrupt; return -1;
        }
        if (selected->seen[row]) { ++stats_.duplicates; return 0; }
        selected->seen[row] = 1;
        std::memcpy(selected->pixels.data() + row * LINE_BYTES,
                    packet + header_size, LINE_BYTES);
        selected->sum += row_sum;
        if (++selected->count != HEIGHT) return 0;
        if (selected->sum != selected->meta.checksum) {
            ++stats_.corrupt; abandon(*selected); return -1;
        }
        selected->complete = true;
        ++stats_.complete; stats_.assembly_ns += now - selected->started;
        return 1;
    }

    bool take_latest(uint8_t* output, Metadata& meta) {
        Slot* newest = nullptr;
        for (Slot& slot : slots_)
            if (slot.used && slot.complete &&
                (!newest || newer(slot.meta.frame_id, newest->meta.frame_id))) newest = &slot;
        if (!newest) return false;
        std::memcpy(output, newest->pixels.data(), FRAME_BYTES);
        meta = newest->meta;
        published_id_ = meta.frame_id; have_published_ = true;
        ++stats_.published;
        newest->reset();
        for (Slot& slot : slots_)
            if (slot.used && !newer(slot.meta.frame_id, published_id_)) abandon(slot);
        return true;
    }
};
} // namespace video_v2
#endif

