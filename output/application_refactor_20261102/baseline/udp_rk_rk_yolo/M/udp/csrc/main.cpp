#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "shared_memory.h"
#include "video_transport_v2.h"
#include "line_style_wire.h"
#include "traffic_light_wire.h"
#include <errno.h>
#include <signal.h>
#include <sys/uio.h>

//=========================================================================
// *作   者：科研轮转（UDP模块）
// *完成时间：2025年10月17日
// *实现功能：UDP网络传输模块，负责将图像数据发送到S端
// *传输格式：支持同步的帧传输，包含帧头、行头、行数据、帧尾
//=========================================================================

static_assert(sizeof(SharedMemoryControl) == 104, "Image control ABI must remain 104 bytes");
static_assert(sizeof(CurveDetectionSharedMemory) == 72, "Curve shared ABI must remain 72 bytes");
#define IMAGE_WIDTH  640
#define IMAGE_HEIGHT 480
#define UDP_PORT 8888
#define TARGET_IP "192.168.100.20"  // S端接收IP地址
#define MAX_PACKET_SIZE 1400

//=========================================================================
// 帧同步和行同步结构定义 - 640*480专用
//=========================================================================
#define FRAME_HEADER_MAGIC 0xFF00FF00  /* 帧头魔数 */
#define FRAME_TRAILER_MAGIC 0x00FF00FF /* 帧尾魔数 */
#define LINE_HEADER_MAGIC 0xAA55AA55   /* 行头魔数 */

typedef struct {
    uint32_t magic;        /* 魔数: FRAME_HEADER_MAGIC */
    uint32_t frame_id;     /* 帧ID */
    uint32_t width;        /* 图像宽度 */
    uint32_t height;       /* 图像高度 */
    uint32_t data_size;    /* 帧数据大小 */
    uint32_t line_count;   /* 行数 */
    uint32_t checksum;     /* 帧校验和 */
    uint32_t timestamp;    /* 时间戳 */
    uint32_t curve_type;   /* 弯道类型: 0=无, 1=左弯, 2=右弯, 3=左右弯 */
    uint32_t curve_confidence; /* 弯道检测置信度 (0-100) */
    uint32_t curve_angle;  /* 弯道角度 (度数) */
    uint32_t reserved;     /* 预留字段 */
} FrameHeader;

typedef struct {
    uint32_t magic;        /* 魔数: FRAME_TRAILER_MAGIC */
    uint32_t frame_id;     /* 帧ID */
    uint32_t checksum;     /* 帧校验和 */
    uint32_t total_lines;  /* 总行数 */
} FrameTrailer;

typedef struct {
    uint32_t magic;        /* 魔数: LINE_HEADER_MAGIC */
    uint16_t line_id;      /* 行号 */
    uint16_t line_size;    /* 行数据大小 */
    uint32_t line_checksum;/* 行校验和 */
} LineHeader;

typedef struct {
    int socket_fd;
    struct sockaddr_in target_addr;
    unsigned bitrate_mbit;
    uint32_t wire_sequence, session_id;
    uint64_t report_start, sent_frames, sent_bytes, failed_frames, send_ns;
} NetworkManager;

static volatile sig_atomic_t sender_running = 1;
static void sender_signal_handler(int) { sender_running = 0; }
static uint64_t monotonic_ns() {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (uint64_t)now.tv_sec * 1000000000ULL + now.tv_nsec;
}
static void report_sender(NetworkManager* manager) {
    uint64_t now = monotonic_ns();
    if (!manager->report_start) manager->report_start = now;
    if (now - manager->report_start < 5000000000ULL) return;
    double seconds = (now - manager->report_start) / 1e9;
    printf("[UDP_TX] fps=%.2f payload_mbps=%.2f send_ms=%.2f failed=%llu pacing_mbps=%u\n",
           manager->sent_frames / seconds, manager->sent_bytes * 8.0 / seconds / 1e6,
           manager->sent_frames ? manager->send_ns / 1e6 / manager->sent_frames : 0.0,
           (unsigned long long)manager->failed_frames, manager->bitrate_mbit);
    manager->report_start = now;
    manager->sent_frames = manager->sent_bytes = manager->failed_frames = manager->send_ns = 0;
}

//=========================================================================
// 函数声明
//=========================================================================
int network_manager_init(NetworkManager* net_mgr, const char* target_ip);
int network_send_frame_with_sync(NetworkManager* net_mgr, const uint8_t* frame_data, uint32_t frame_id,
                                 uint32_t curve_type, uint32_t curve_confidence, uint32_t curve_angle);
void network_manager_cleanup(NetworkManager* net_mgr);
uint32_t calculate_checksum(const uint8_t* data, size_t size);

//=========================================================================
// 网络传输函数实现
//=========================================================================

/**************************************************************************
** 函数名称:    network_manager_init - 初始化UDP网络传输
****************************************************************************/
int network_manager_init(NetworkManager* net_mgr, const char* target_ip) {
    if (!net_mgr || !target_ip) {
        printf("网络管理器参数无效\n");
        return -1;
    }
    
    memset(net_mgr, 0, sizeof(*net_mgr));
    net_mgr->socket_fd = -1;
    net_mgr->session_id = (uint32_t)(monotonic_ns() ^ ((uint64_t)getpid() << 17));
    if (!net_mgr->session_id) net_mgr->session_id = 1;
    net_mgr->bitrate_mbit = 200;
    const char* rate = getenv("VIDEO_UDP_MBIT");
    if (rate && *rate) {
        char* end = NULL;
        unsigned long value = strtoul(rate, &end, 10);
        if (end && !*end && value <= 1000) net_mgr->bitrate_mbit = (unsigned)value;
    }
    printf("正在初始化UDP网络传输...\n");
    
    // 创建UDP socket
    net_mgr->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (net_mgr->socket_fd < 0) {
        perror("UDP Socket创建失败");
        return -1;
    }
    
    // 设置socket选项
    int broadcast = 1;
    if (setsockopt(net_mgr->socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
        perror("设置socket广播选项失败");
        close(net_mgr->socket_fd);
        return -1;
    }
    
    // 设置发送缓冲区大小
    int send_buf_size = 256 * 1024; // bounded queue; paced batches, not old frames
    if (setsockopt(net_mgr->socket_fd, SOL_SOCKET, SO_SNDBUF, &send_buf_size, sizeof(send_buf_size)) < 0) {
        perror("设置发送缓冲区大小失败");
    }
    
    struct timeval timeout = {0, 10000};
    setsockopt(net_mgr->socket_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    // 配置目标地址
    memset(&net_mgr->target_addr, 0, sizeof(net_mgr->target_addr));
    net_mgr->target_addr.sin_family = AF_INET;
    net_mgr->target_addr.sin_port = htons(UDP_PORT);
    
    if (inet_pton(AF_INET, target_ip, &net_mgr->target_addr.sin_addr) <= 0) {
        printf("目标IP地址格式无效: %s\n", target_ip);
        close(net_mgr->socket_fd);
        return -1;
    }
    
    printf("UDP网络传输初始化成功\n");
    printf("目标地址: %s:%d\n", target_ip, UDP_PORT);
    printf("发送缓冲区: %d字节\n", send_buf_size);
    
    return 0;
}

/**************************************************************************
** 函数名称:    calculate_checksum - 计算简单校验和
****************************************************************************/
uint32_t calculate_checksum(const uint8_t* data, size_t size) {
    uint32_t checksum = 0;
    for (size_t i = 0; i < size; i++) {
        checksum += data[i];
    }
    return checksum;
}

/**************************************************************************
** 函数名称:    network_send_frame_with_sync - 发送带同步的640*480帧
****************************************************************************/
static int network_send_legacy(NetworkManager* net_mgr, const uint8_t* frame_data, uint32_t frame_id,
                                 uint32_t curve_type, uint32_t curve_confidence, uint32_t curve_angle) {
    if (!net_mgr || !frame_data) return -1;

    // PCIe共享内存已经是RGB565，直接传输原始像素，不再重复转换颜色。
    const uint32_t frame_checksum = calculate_checksum(frame_data, PCIE_FRAME_SIZE);
    FrameHeader header = {};
    header.magic = FRAME_HEADER_MAGIC;
    header.frame_id = frame_id;
    header.width = IMAGE_WIDTH;
    header.height = IMAGE_HEIGHT;
    header.data_size = PCIE_FRAME_SIZE;
    header.line_count = IMAGE_HEIGHT;
    header.checksum = frame_checksum;
    header.timestamp = (uint32_t)time(NULL);
    header.curve_type = curve_type;
    header.curve_confidence = curve_confidence;
    header.curve_angle = curve_angle;

    const struct sockaddr* target = (const struct sockaddr*)&net_mgr->target_addr;
    const socklen_t target_len = sizeof(net_mgr->target_addr);
    if (sendto(net_mgr->socket_fd, &header, sizeof(header), 0,
               target, target_len) != (ssize_t)sizeof(header)) {
        perror("帧头发送失败");
        return -1;
    }

    for (uint16_t line = 0; line < IMAGE_HEIGHT; ++line) {
        const uint8_t* line_data = frame_data + (size_t)line * IMAGE_WIDTH * 2;
        LineHeader line_header = {};
        line_header.magic = LINE_HEADER_MAGIC;
        line_header.line_id = line;
        line_header.line_size = IMAGE_WIDTH * 2;
        line_header.line_checksum = calculate_checksum(line_data, line_header.line_size);

        if (sendto(net_mgr->socket_fd, &line_header, sizeof(line_header), 0,
                   target, target_len) != (ssize_t)sizeof(line_header) ||
            sendto(net_mgr->socket_fd, line_data, line_header.line_size, 0,
                   target, target_len) != (ssize_t)line_header.line_size) {
            perror("图像行发送失败");
            return -1;
        }
        // 每帧约615KB，在100Mb/s链路上适当限流，避免队列溢出。
        usleep(100);
    }

    FrameTrailer trailer = {};
    trailer.magic = FRAME_TRAILER_MAGIC;
    trailer.frame_id = frame_id;
    trailer.checksum = frame_checksum;
    trailer.total_lines = IMAGE_HEIGHT;
    if (sendto(net_mgr->socket_fd, &trailer, sizeof(trailer), 0,
               target, target_len) != (ssize_t)sizeof(trailer)) {
        perror("帧尾发送失败");
        return -1;
    }
    return PCIE_FRAME_SIZE;
}

static int network_send_frame_source(NetworkManager* net_mgr, const uint8_t* frame_data,
                                     video_v2::Metadata meta) {
    if (!net_mgr || !frame_data || net_mgr->socket_fd < 0) return -1;
    uint64_t started = monotonic_ns();
    meta.frame_id = ++net_mgr->wire_sequence;
    meta.session_id = net_mgr->session_id;
    const char* legacy = getenv("VIDEO_UDP_LEGACY");
    if (legacy && *legacy == '1') {
        int result = network_send_legacy(net_mgr, frame_data, meta.frame_id,
                                         meta.curve_type, 0, meta.curve_angle);
        if (result > 0) { ++net_mgr->sent_frames; net_mgr->sent_bytes += result;
                         net_mgr->send_ns += monotonic_ns() - started; }
        else ++net_mgr->failed_frames;
        return result;
    }
    uint32_t line_sums[IMAGE_HEIGHT];
    for (unsigned row = 0; row < IMAGE_HEIGHT; ++row) {
        line_sums[row] = video_v2::checksum(frame_data + row * video_v2::LINE_BYTES,
                                          video_v2::LINE_BYTES);
        meta.checksum += line_sums[row];
    }
    uint64_t wire_bytes = 0;
    for (unsigned first = 0; first < IMAGE_HEIGHT && sender_running; first += video_v2::BATCH) {
        unsigned count = std::min(video_v2::BATCH, IMAGE_HEIGHT - first);
        video_v2::WireHeader headers[video_v2::BATCH];
        struct iovec vectors[video_v2::BATCH][2] = {};
        struct mmsghdr messages[video_v2::BATCH] = {};
        for (unsigned i = 0; i < count; ++i) {
            unsigned row = first + i;
            headers[i] = video_v2::make_header(meta, row, line_sums[row]);
            vectors[i][0].iov_base = &headers[i];
            vectors[i][0].iov_len = sizeof(headers[i]);
            vectors[i][1].iov_base = (void*)(frame_data + row * video_v2::LINE_BYTES);
            vectors[i][1].iov_len = video_v2::LINE_BYTES;
            messages[i].msg_hdr.msg_name = &net_mgr->target_addr;
            messages[i].msg_hdr.msg_namelen = sizeof(net_mgr->target_addr);
            messages[i].msg_hdr.msg_iov = vectors[i];
            messages[i].msg_hdr.msg_iovlen = 2;
        }
        unsigned sent = 0;
        while (sent < count && sender_running) {
            int result = sendmmsg(net_mgr->socket_fd, messages + sent, count - sent, 0);
            if (result < 0 && errno == EINTR) continue;
            if (result <= 0) { ++net_mgr->failed_frames; return -1; }
            for (int i = 0; i < result; ++i) {
                if (messages[sent + i].msg_len != sizeof(video_v2::WireHeader) + video_v2::LINE_BYTES) {
                    ++net_mgr->failed_frames; return -1;
                }
            }
            sent += result;
        }
        if (!sender_running) return 0;
        wire_bytes += count * (sizeof(video_v2::WireHeader) + video_v2::LINE_BYTES + 66);
        if (net_mgr->bitrate_mbit) {
            uint64_t deadline = started + wire_bytes * 8000ULL / net_mgr->bitrate_mbit;
            struct timespec wake = {(time_t)(deadline / 1000000000ULL),
                                    (long)(deadline % 1000000000ULL)};
            while (sender_running && clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &wake, NULL) == EINTR) {}
        }
    }
    if (!sender_running) return 0;
    ++net_mgr->sent_frames;
    net_mgr->sent_bytes += video_v2::FRAME_BYTES;
    net_mgr->send_ns += monotonic_ns() - started;
    return video_v2::FRAME_BYTES;
}

// Source-compatible helper for standalone transport diagnostics.
int network_send_frame_with_sync(NetworkManager* net_mgr, const uint8_t* frame_data,
                                 uint32_t frame_id, uint32_t curve_type,
                                 uint32_t curve_confidence, uint32_t curve_angle) {
    video_v2::Metadata meta;
    meta.source_frame_id = frame_id; meta.curve_type = curve_type;
    meta.curve_confidence = curve_confidence; meta.curve_angle = curve_angle;
    return network_send_frame_source(net_mgr, frame_data, meta);
}

/**************************************************************************
** 函数名称:    network_manager_cleanup - 清理网络资源
****************************************************************************/
void network_manager_cleanup(NetworkManager* net_mgr) {
    if (net_mgr && net_mgr->socket_fd >= 0) {
        close(net_mgr->socket_fd);
        net_mgr->socket_fd = -1;
        printf("UDP网络连接已关闭\n");
    }
}

//=========================================================================
// 主函数 - UDP传输测试
//=========================================================================

// Producer image/curve handles are renewed together after a capture-only restart.
// A named semaphore may have been recreated even when the SysV id was reused.
struct SourceConnection {
    SharedMemoryManager image = {};
    bool attached = false, curve_attached = false, have_generation = false, have_input = false;
    uint64_t generation = 0, last_probe = 0, last_success = 0, curve_failure_since = 0;
    uint32_t last_input = 0;
    video_v2::CurveSenderState curve;
};
static void detach_source(SourceConnection& source) {
    if (source.attached) shared_memory_cleanup(&source.image);
    if (source.curve_attached) curve_detection_shm_cleanup();
    source.attached = source.curve_attached = false;
}
static void probe_source(SourceConnection& source, NetworkManager& net, uint64_t now) {
    if (source.last_probe && now - source.last_probe < 100000000ULL) return;
    source.last_probe = now;
    const int current_id = shmget(SHM_PCIE_IMAGE_KEY, sizeof(PCIeSharedMemory), 0666);
    uint64_t current_generation = source.attached ? shared_memory_image_generation(&source.image) : 0;
    bool renew = !source.attached || current_id != source.image.shm_id;
    if (source.attached) {
        const SharedMemoryControl* control = &((PCIeSharedMemory*)source.image.shm_ptr)->control;
        renew = renew || !__atomic_load_n(&control->writer_active, __ATOMIC_ACQUIRE) ||
                (source.have_generation && current_generation != source.generation);
        // Legacy producers lack a generation marker. A changing header with no
        // coherent snapshot for 500ms can indicate an obsolete semaphore handle.
        if (!current_generation && source.last_success && now - source.last_success > 500000000ULL &&
            control->frame_ready && control->frame_id != source.image.last_frame_id) renew = true;
    }
    if (!renew) {
        if (!source.curve_attached) source.curve_attached = curve_detection_shm_init(0) == 0;
        return;
    }
    const int previous_id = source.attached ? source.image.shm_id : -1;
    detach_source(source);
    if (current_id < 0 || shared_memory_init(&source.image, SHM_TYPE_PCIE, 0)) return;
    source.attached = true;
    current_generation = shared_memory_image_generation(&source.image);
    bool changed = !source.have_generation || current_generation != source.generation ||
                   (!current_generation && previous_id != source.image.shm_id);
    if (changed) {
        source.generation = current_generation; source.have_generation = true;
        source.have_input = false; source.curve.reset();
        if (++net.session_id == 0) ++net.session_id;
        printf("[UDP_TX] producer_session=%u generation=%llu shmid=%d wire_sequence=%u\n",
               net.session_id, (unsigned long long)source.generation, source.image.shm_id, net.wire_sequence);
    }
    source.last_success = now; source.curve_failure_since = 0;
    source.curve_attached = curve_detection_shm_init(0) == 0;
}
int main(int argc, char* argv[]) {
    setvbuf(stdout, NULL, _IOLBF, 0);
    signal(SIGINT, sender_signal_handler); signal(SIGTERM, sender_signal_handler);
    NetworkManager network;
    if (network_manager_init(&network, argc > 1 ? argv[1] : TARGET_IP)) return 1;
    uint8_t* pixels = (uint8_t*)malloc(PCIE_FRAME_SIZE);
    if (!pixels) { network_manager_cleanup(&network); return 1; }
    SourceConnection source;
    LineStyleIPC line_ipc={-1,nullptr};
    TrafficLightIPC traffic_ipc={-1,nullptr};
    uint64_t last_line_send=0;
    uint64_t last_traffic_send=0;
    printf("[UDP_TX] protocol=V3 packet=%zu curve_ttl_ms=500 (legacy/V2 video has no trusted NN identity)\n",
           sizeof(video_v2::WireHeader) + video_v2::LINE_BYTES);
    while (sender_running) {
        uint64_t now = monotonic_ns();
        report_sender(&network); probe_source(source, network, now);
        if(!last_traffic_send||now-last_traffic_send>=100000000ULL) {
            traffic_ipc_open(&traffic_ipc,0);TrafficLightData traffic={};uint8_t packet[traffic_wire::SIZE];
            if(!traffic_ipc_read(&traffic_ipc,&traffic)&&traffic_wire::encode(packet,traffic,now))
                sendto(network.socket_fd,packet,sizeof(packet),MSG_DONTWAIT,(sockaddr*)&network.target_addr,sizeof(network.target_addr));
            last_traffic_send=now;
        }
        if(!last_line_send||now-last_line_send>=50000000ULL) {
            line_ipc_open(&line_ipc,LINE_STYLE_KEY,0);LineStyleData line={};uint8_t packet[line_wire::SIZE];
            if(!line_ipc_read(&line_ipc,&line)&&line_wire::encode(packet,line,now))
                sendto(network.socket_fd,packet,sizeof(packet),MSG_DONTWAIT,(sockaddr*)&network.target_addr,sizeof(network.target_addr));
            last_line_send=now;
        }
        if (!source.attached) { usleep(10000); continue; }
        uint32_t width = 0, height = 0, frame_id = 0;
        const uint64_t generation_before = shared_memory_image_generation(&source.image);
        int result = pcie_shared_memory_read_frame(&source.image, pixels, &width, &height, &frame_id);
        if (result || generation_before != shared_memory_image_generation(&source.image) ||
            generation_before != source.generation) { usleep(1000); continue; }
        source.last_success = now;
        if (source.have_input && frame_id == source.last_input) { usleep(1000); continue; }
        if (!generation_before && source.have_input && !video_v2::newer(frame_id, source.last_input)) {
            // Old producers can still reset their frame counter without restarting UDP.
            if (++network.session_id == 0) ++network.session_id;
            source.curve.reset();
        }
        source.last_input = frame_id; source.have_input = true;
        if (width != IMAGE_WIDTH || height != IMAGE_HEIGHT) continue;
        if (source.curve_attached) {
            uint32_t id, confidence, angle; CurveType type; uint64_t capture_ns;
            if (!curve_detection_read_source(&id, &type, &confidence, &angle, &capture_ns)) {
                source.curve.observe(id, (uint32_t)type, confidence, angle, capture_ns, now);
                source.curve_failure_since = 0;
            } else {
                // Contended seqlocks do not block video or refresh stale curve metadata.
                if (!source.curve_failure_since) source.curve_failure_since = now;
                if (now - source.curve_failure_since > 1000000000ULL) {
                    curve_detection_shm_cleanup(); source.curve_attached = false;
                    source.curve_failure_since = 0;
                }
            }
        }
        video_v2::Metadata meta; meta.source_frame_id = frame_id;
        source.curve.fill(meta, monotonic_ns());
        network_send_frame_source(&network, pixels, meta);
    }
    line_ipc_close(&line_ipc);
    traffic_ipc_close(&traffic_ipc);
    detach_source(source); free(pixels); network_manager_cleanup(&network);
    return 0;
}
