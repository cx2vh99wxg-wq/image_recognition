#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "shared_memory.h"
#include "video_transport_v2.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <csignal>
#include <new>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

static_assert(sizeof(SharedMemoryControl) == 104, "Image control ABI must remain 104 bytes");
static_assert(sizeof(CurveDetectionSharedMemory) == 72, "Curve shared ABI must remain 72 bytes");
#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define UDP_PORT 8888
#define FRAME_BUFFER_SIZE (640 * 480 * 2)
#define MAX_PACKET_SIZE 1400
#define FRAME_HEADER_MAGIC 0xFF00FF00u
#define FRAME_TRAILER_MAGIC 0x00FF00FFu
#define LINE_HEADER_MAGIC 0xAA55AA55u
struct FrameHeader {
    uint32_t magic, frame_id, width, height, data_size, line_count, checksum, timestamp;
    uint32_t curve_type, curve_confidence, curve_angle, reserved;
};
struct FrameTrailer { uint32_t magic, frame_id, checksum, total_lines; };
struct LineHeader { uint32_t magic; uint16_t line_id, line_size; uint32_t line_checksum; };
struct Datagram { uint8_t data[MAX_PACKET_SIZE]; sockaddr_in source; unsigned size; uint64_t received_ns; };
struct NetworkReceiver {
    int socket_fd;
    uint8_t *frame_buffer, *legacy_buffer;
    video_v2::Assembler* assembler;
    video_v2::CurveReceiverState curve;
    bool compatibility_logged;
    uint64_t active_source, last_peer_ns;
    bool have_source;
    unsigned protocol;
    bool legacy_active, legacy_pending, legacy_ready, legacy_have_id;
    uint32_t legacy_id, legacy_line, legacy_sum, pending_sum;
    FrameHeader legacy_header;
    video_v2::Metadata legacy_metadata;
    uint64_t legacy_started, legacy_complete, legacy_incomplete, legacy_corrupt, legacy_assembly_ns;
    uint64_t report_start, report_bytes, report_published;
    uint64_t previous_complete, previous_incomplete, previous_corrupt, previous_assembly_ns;
    uint64_t previous_duplicates, previous_skipped, kernel_drops;
};
static volatile sig_atomic_t keep_running = 1;
static SharedMemoryManager shm_manager;
static int curve_detection_available = 0;
static uint64_t monotonic_ns() {
    timespec t; clock_gettime(CLOCK_MONOTONIC, &t);
    return (uint64_t)t.tv_sec * 1000000000ULL + t.tv_nsec;
}
void signal_handler(int) { keep_running = 0; }
void network_receiver_cleanup(NetworkReceiver* r) {
    if (!r) return;
    if (r->socket_fd >= 0) close(r->socket_fd);
    free(r->frame_buffer); free(r->legacy_buffer); delete r->assembler;
    r->socket_fd = -1; r->frame_buffer = r->legacy_buffer = nullptr; r->assembler = nullptr;
}
int network_receiver_init(NetworkReceiver* r) {
    if (!r) return -1;
    *r = NetworkReceiver{}; r->socket_fd = -1;
    r->frame_buffer = (uint8_t*)std::malloc(FRAME_BUFFER_SIZE);
    r->legacy_buffer = (uint8_t*)std::malloc(FRAME_BUFFER_SIZE);
    try { r->assembler = new video_v2::Assembler(); }
    catch (const std::bad_alloc&) { network_receiver_cleanup(r); return -1; }
    if (!r->frame_buffer || !r->legacy_buffer) { network_receiver_cleanup(r); return -1; }
    r->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (r->socket_fd < 0) { network_receiver_cleanup(r); return -1; }
    int one = 1, buffer = 2 * FRAME_BUFFER_SIZE;
    setsockopt(r->socket_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    setsockopt(r->socket_fd, SOL_SOCKET, SO_RCVBUF, &buffer, sizeof(buffer));
#ifdef SO_RXQ_OVFL
    setsockopt(r->socket_fd, SOL_SOCKET, SO_RXQ_OVFL, &one, sizeof(one));
#endif
    setsockopt(r->socket_fd, SOL_SOCKET, SO_TIMESTAMPNS, &one, sizeof(one));
    timeval timeout = {0, 100000};
    setsockopt(r->socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    sockaddr_in address = {};
    address.sin_family = AF_INET; address.sin_addr.s_addr = INADDR_ANY; address.sin_port = htons(UDP_PORT);
    if (bind(r->socket_fd, (sockaddr*)&address, sizeof(address))) {
        perror("UDP bind"); network_receiver_cleanup(r); return -1;
    }
    socklen_t size = sizeof(buffer);
    getsockopt(r->socket_fd, SOL_SOCKET, SO_RCVBUF, &buffer, &size);
    printf("[UDP_RX] socket_rcvbuf=%d batch=%u max_inflight=2 protocol=V3/V2/legacy curve_ttl_ms=500\n", buffer, video_v2::BATCH);
    r->report_start = monotonic_ns(); return 0;
}
static void report_receiver(NetworkReceiver* r) {
    uint64_t now = monotonic_ns();
    if (now - r->report_start < 5000000000ULL) return;
    const video_v2::Statistics& v2 = r->assembler->statistics();
    uint64_t complete = v2.complete + r->legacy_complete;
    uint64_t incomplete = v2.incomplete + r->legacy_incomplete;
    uint64_t corrupt = v2.corrupt + r->legacy_corrupt;
    uint64_t assembly = v2.assembly_ns + r->legacy_assembly_ns;
    uint64_t completed = complete - r->previous_complete;
    double seconds = (now - r->report_start) / 1e9;
    printf("[UDP_RX] publish_fps=%.2f complete_fps=%.2f udp_mbps=%.2f assembly_ms=%.2f incomplete=%llu corrupt=%llu duplicate=%llu skip_complete=%llu kernel_drop_total=%llu\n",
           r->report_published / seconds, completed / seconds, r->report_bytes * 8.0 / seconds / 1e6,
           completed ? (assembly - r->previous_assembly_ns) / 1e6 / completed : 0.0,
           (unsigned long long)(incomplete - r->previous_incomplete),
           (unsigned long long)(corrupt - r->previous_corrupt),
           (unsigned long long)(v2.duplicates - r->previous_duplicates),
           (unsigned long long)(v2.skipped_complete - r->previous_skipped),
           (unsigned long long)r->kernel_drops);
    r->previous_complete = complete; r->previous_incomplete = incomplete;
    r->previous_corrupt = corrupt; r->previous_assembly_ns = assembly;
    r->previous_duplicates = v2.duplicates; r->previous_skipped = v2.skipped_complete;
    r->report_start = now; r->report_bytes = r->report_published = 0;
}
static bool legacy_header(const uint8_t* data, size_t size, FrameHeader& h) {
    if (size != sizeof(h)) return false;
    std::memcpy(&h, data, sizeof(h));
    return h.magic == FRAME_HEADER_MAGIC && h.width == IMAGE_WIDTH && h.height == IMAGE_HEIGHT &&
           h.data_size == FRAME_BUFFER_SIZE && h.line_count == IMAGE_HEIGHT &&
           h.curve_type <= 3 && h.curve_confidence <= 100;
}
static void reset_legacy(NetworkReceiver* r, bool failed) {
    if (failed && r->legacy_active) ++r->legacy_incomplete;
    r->legacy_active = r->legacy_pending = false; r->legacy_line = r->legacy_sum = 0;
}
static void ingest_legacy(NetworkReceiver* r, const uint8_t* data, size_t size,
                           uint64_t now, bool header_found, const FrameHeader& header) {
    if (header_found) {
        reset_legacy(r, true);
        if (r->legacy_have_id && !video_v2::newer(header.frame_id, r->legacy_id)) return;
        r->legacy_header = header; r->legacy_started = now; r->legacy_active = true; return;
    }
    if (!r->legacy_active) return;
    if (now - r->legacy_started > video_v2::FRAME_TIMEOUT_NS) { reset_legacy(r, true); return; }
    if (r->legacy_line == IMAGE_HEIGHT && size == sizeof(FrameTrailer)) {
        FrameTrailer trailer; std::memcpy(&trailer, data, sizeof(trailer));
        if (trailer.magic == FRAME_TRAILER_MAGIC && trailer.frame_id == r->legacy_header.frame_id &&
            trailer.total_lines == IMAGE_HEIGHT && trailer.checksum == r->legacy_sum) {
            std::memcpy(r->frame_buffer, r->legacy_buffer, FRAME_BUFFER_SIZE);
            r->legacy_metadata.frame_id = trailer.frame_id; r->legacy_metadata.checksum = trailer.checksum;
            r->legacy_metadata.curve_type = r->legacy_header.curve_type;
            r->legacy_metadata.version = 1; r->legacy_metadata.curve_confidence = 0;
            r->legacy_metadata.curve_angle = r->legacy_header.curve_angle;
            r->legacy_ready = true; r->legacy_have_id = true; r->legacy_id = trailer.frame_id;
            ++r->legacy_complete; r->legacy_assembly_ns += now - r->legacy_started;
            reset_legacy(r, false); return;
        }
    } else if (!r->legacy_pending && r->legacy_line < IMAGE_HEIGHT && size == sizeof(LineHeader)) {
        LineHeader line; std::memcpy(&line, data, sizeof(line));
        if (line.magic == LINE_HEADER_MAGIC && line.line_id == r->legacy_line &&
            line.line_size == video_v2::LINE_BYTES) {
            r->pending_sum = line.line_checksum; r->legacy_pending = true; return;
        }
    } else if (r->legacy_pending && size == video_v2::LINE_BYTES &&
               video_v2::checksum(data, size) == r->pending_sum) {
        std::memcpy(r->legacy_buffer + r->legacy_line * video_v2::LINE_BYTES, data, size);
        r->legacy_sum += r->pending_sum; ++r->legacy_line; r->legacy_pending = false; return;
    }
    ++r->legacy_corrupt; reset_legacy(r, true);
}
static void ingest_datagram(NetworkReceiver* r, const Datagram& packet) {
    if (!packet.size || packet.size > MAX_PACKET_SIZE) return;
    uint64_t now = monotonic_ns();
    FrameHeader old_header = {};
    bool v2 = video_v2::is_packet(packet.data, packet.size);
    bool old = !v2 && legacy_header(packet.data, packet.size, old_header);
    uint64_t source = ((uint64_t)packet.source.sin_addr.s_addr << 16) | packet.source.sin_port;
    if (v2) {
        if (r->protocol == 1 && r->have_source && now - r->last_peer_ns < video_v2::FRAME_TIMEOUT_NS) return;
        // The assembler owns V2/V3 session transitions and retired-session rejection.
        r->protocol = 2;
        if (r->assembler->ingest(packet.data, packet.size, source, now, packet.received_ns) >= 0)
            r->last_peer_ns = now;
        return;
    }
    if (r->protocol == 2) {
        if (!old || now - r->last_peer_ns < video_v2::FRAME_TIMEOUT_NS) return;
        r->assembler->reset_session(); r->have_source = false;
    }
    if (!r->have_source || source != r->active_source) {
        if (!old || (r->have_source && now - r->last_peer_ns < video_v2::FRAME_TIMEOUT_NS)) return;
        reset_legacy(r, true); r->legacy_have_id = r->legacy_ready = false;
        r->active_source = source; r->have_source = true;
    }
    r->protocol = 1; r->last_peer_ns = now;
    ingest_legacy(r, packet.data, packet.size, now, old, old_header);
}
static void publish_curve(const video_v2::CurveEvent& event) {
    if (curve_detection_available)
        curve_detection_update(event.frame_id, (CurveType)event.type, event.confidence, event.angle);
}

int network_receive_with_sync(NetworkReceiver* r) {
    if (!r || r->socket_fd < 0 || !r->assembler) return -1;
    report_receiver(r);
    video_v2::CurveEvent curve_event;
    if (r->curve.expire(monotonic_ns(), curve_event)) publish_curve(curve_event);
    unsigned processed = 0;
    while (keep_running && processed < 2048) {
        Datagram packets[video_v2::BATCH] = {};
        iovec vectors[video_v2::BATCH] = {};
        mmsghdr messages[video_v2::BATCH] = {};
        alignas(cmsghdr) unsigned char controls[video_v2::BATCH][CMSG_SPACE(sizeof(uint32_t)) + CMSG_SPACE(sizeof(timespec))] = {};
        for (unsigned i = 0; i < video_v2::BATCH; ++i) {
            vectors[i].iov_base = packets[i].data; vectors[i].iov_len = sizeof(packets[i].data);
            messages[i].msg_hdr.msg_name = &packets[i].source;
            messages[i].msg_hdr.msg_namelen = sizeof(packets[i].source);
            messages[i].msg_hdr.msg_iov = &vectors[i]; messages[i].msg_hdr.msg_iovlen = 1;
            messages[i].msg_hdr.msg_control = controls[i];
            messages[i].msg_hdr.msg_controllen = sizeof(controls[i]);
        }
        int count = recvmmsg(r->socket_fd, messages, video_v2::BATCH,
                            processed ? MSG_DONTWAIT : MSG_WAITFORONE, nullptr);
        if (count < 0) {
            if (errno == EINTR && keep_running) continue;
            if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) { perror("UDP receive"); return -1; }
            break;
        }
        if (!count) break;
        for (int i = 0; i < count; ++i) {
            r->report_bytes += messages[i].msg_len;
            if (messages[i].msg_hdr.msg_flags & MSG_TRUNC) { ++r->legacy_corrupt; continue; }
            const uint64_t processed_at = monotonic_ns();
            packets[i].received_ns = processed_at;
            for (cmsghdr* c = CMSG_FIRSTHDR(&messages[i].msg_hdr); c; c = CMSG_NXTHDR(&messages[i].msg_hdr, c)) {
#ifdef SO_RXQ_OVFL
                if (c->cmsg_level == SOL_SOCKET && c->cmsg_type == SO_RXQ_OVFL &&
                    c->cmsg_len >= CMSG_LEN(sizeof(uint32_t))) {
                    uint32_t drops; std::memcpy(&drops, CMSG_DATA(c), sizeof(drops)); r->kernel_drops = drops;
                }
#endif
                if (c->cmsg_level == SOL_SOCKET && c->cmsg_type == SO_TIMESTAMPNS &&
                    c->cmsg_len >= CMSG_LEN(sizeof(timespec))) {
                    timespec queued, real_now; std::memcpy(&queued, CMSG_DATA(c), sizeof(queued));
                    clock_gettime(CLOCK_REALTIME, &real_now);
                    int64_t age = (int64_t)(real_now.tv_sec - queued.tv_sec) * 1000000000LL +
                                  real_now.tv_nsec - queued.tv_nsec;
                    if (age > 0 && (uint64_t)age < processed_at) packets[i].received_ns = processed_at - age;
                }
            }
            packets[i].size = messages[i].msg_len; ingest_datagram(r, packets[i]);
        }
        processed += count;
    }
    video_v2::Metadata meta;
    bool ready = r->protocol == 2 ? r->assembler->take_latest(r->frame_buffer, meta) : r->legacy_ready;
    if (!ready) {
        if (r->curve.expire(monotonic_ns(), curve_event)) publish_curve(curve_event);
        return 0;
    }
    if (r->protocol == 1) { meta = r->legacy_metadata; r->legacy_ready = false; }
    if (meta.version < 3 && !r->compatibility_logged) {
        printf("[UDP_RX] legacy/V2 video accepted; curve confidence forced to zero (no NN identity/age)\n");
        r->compatibility_logged = true;
    }
    if (r->curve.observe(meta, monotonic_ns(), curve_event)) publish_curve(curve_event);
    ++r->report_published; return FRAME_BUFFER_SIZE;
}
static void save_frame(const uint8_t* pixels, const char* name) {
    FILE* file = std::fopen(name, "wb"); if (!file) return;
    std::fprintf(file, "P6\n640 480\n255\n");
    std::vector<uint8_t> row(640 * 3);
    for (unsigned y = 0; y < 480; ++y) {
        for (unsigned x = 0; x < 640; ++x) {
            uint16_t pixel; std::memcpy(&pixel, pixels + (y * 640 + x) * 2, 2);
            row[x * 3] = ((pixel >> 11) & 31) * 255 / 31;
            row[x * 3 + 1] = ((pixel >> 5) & 63) * 255 / 63;
            row[x * 3 + 2] = (pixel & 31) * 255 / 31;
        }
        std::fwrite(row.data(), 1, row.size(), file);
    }
    std::fclose(file);
}
int main() {
    setvbuf(stdout, nullptr, _IOLBF, 0);
    signal(SIGINT, signal_handler); signal(SIGTERM, signal_handler);
    if (shared_memory_init_udp(&shm_manager, 1)) return 1;
    curve_detection_available = curve_detection_shm_init(1) == 0;
    NetworkReceiver receiver;
    if (network_receiver_init(&receiver)) {
        if (curve_detection_available) curve_detection_shm_cleanup();
        shared_memory_cleanup(&shm_manager); return 1;
    }
    uint64_t frames = 0;
    while (keep_running) {
        int result = network_receive_with_sync(&receiver);
        if (result < 0) break;
        if (!result) continue;
        if (shared_memory_write_image(&shm_manager, receiver.frame_buffer, IMAGE_WIDTH, IMAGE_HEIGHT)) {
            fprintf(stderr, "UDP image publication failed\n"); break;
        }
        ++frames;
    }
    if (frames) save_frame(receiver.frame_buffer, "udp_last_frame.ppm");
    network_receiver_cleanup(&receiver); shared_memory_cleanup(&shm_manager);
    if (curve_detection_available) curve_detection_shm_cleanup();
    return 0;
}
