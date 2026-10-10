/*
 * udp_receiver.c — S 端 UDP 接收器实现（【人员 B】）
 *
 * reassembly 为纯逻辑（本机可测）；socket 层仅 Linux 板端编译。
 */
#include "udp_receiver.h"
#include "driving_config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ==================== 整帧重组状态机 ==================== */

int udp_reassembly_init(udp_reassembly_t *r, uint8_t *frame_buf, size_t cap)
{
    if (!r || !frame_buf || cap == 0) return -1;
    memset(r, 0, sizeof(*r));
    r->frame_buf = frame_buf;
    r->frame_cap = cap;
    return 0;
}

void udp_reassembly_reset(udp_reassembly_t *r)
{
    if (!r) return;
    /* 仅复位“进行中帧”状态，必须保留 frame_buf / frame_cap：
     * 旧实现用 memset 整体清零，会把 frame_cap 清成 0，导致超时 reset 后
     * begin_frame() 恒因 `data_size > frame_cap` 失败，重组器永久失效。
     * （该缺陷曾使“残帧超时丢弃”路径一旦触发，后续所有帧都无法再重组） */
    free(r->got);
    r->got = NULL;
    r->cur_frame_id    = 0;
    r->block_size      = 0;
    r->block_count     = 0;
    r->received_blocks = 0;
    r->have_hdr        = 0;
    memset(&r->hdr, 0, sizeof(r->hdr));
}

static int begin_frame(udp_reassembly_t *r, const udp_frame_hdr_t *hdr)
{
    if (hdr->data_size > r->frame_cap) return -1;   /* 缓冲不够 */
    if (hdr->data_size == 0 || hdr->block_size == 0 || hdr->block_size > UDP_BLOCK_SIZE ||
        hdr->block_count != (hdr->data_size+hdr->block_size-1u)/hdr->block_size) return -1;

    uint8_t *next_got = (uint8_t *)calloc(hdr->block_count, 1);
    if (!next_got) return -1;
    free(r->got);
    r->got = next_got;

    r->cur_frame_id    = hdr->frame_id;
    r->block_size      = hdr->block_size;
    r->block_count     = hdr->block_count;
    r->received_blocks = 0;
    r->have_hdr        = 1;
    r->hdr             = *hdr;
    return 0;
}

int udp_reassembly_feed(udp_reassembly_t *r, const uint8_t *pkt, size_t len,
                        udp_frame_hdr_t *out_hdr,
                        uint8_t **frame_out, size_t *frame_len)
{
    if (!r || !pkt || !out_hdr || !frame_out || !frame_len) return -1;

    udp_pkt_type_t t = udp_identify(pkt, len);

    if (t == UDP_PKT_FRAME_HDR) {
        udp_frame_hdr_t hdr;
        if (udp_unpack_frame_hdr(pkt, len, &hdr) != 0) return -1;
        if (begin_frame(r, &hdr) != 0) { r->stat_begin_fail++; return -1; }
        r->stat_hdr_rx++;
        return 0;   /* 帧头本身不构成"完整帧" */
    }

    if (t == UDP_PKT_DATA) {
        if (!r->have_hdr) { r->stat_drop_nohdr++; return 0; }   /* 无帧头先来，丢弃 */

        const udp_data_hdr_t *dh = (const udp_data_hdr_t *)pkt;
        if (dh->frame_id != r->cur_frame_id) { r->stat_drop_fid++; return 0; }  /* 旧帧残块，丢弃 */
        if (dh->block_idx >= r->block_count) { r->stat_drop_other++; return 0; }

        const size_t payload_len = len - sizeof(udp_data_hdr_t);
        if (payload_len > r->block_size) { r->stat_drop_other++; return 0; }
        /* Exact length on EVERY block, including the last. Otherwise a short
         * tail reuses old metadata, or a long tail writes past frame_cap. */
        size_t offset=(size_t)dh->block_idx*r->block_size;
        size_t expected=r->hdr.data_size-offset;
        if(expected>r->block_size)expected=r->block_size;
        if (payload_len != expected) {
            r->stat_drop_other++;
            return 0;
        }

        if (r->got[dh->block_idx]) { r->stat_drop_dup++; return 0; }   /* 重复块，丢弃 */

        if (udp_block_checksum(pkt + sizeof(udp_data_hdr_t), payload_len) != dh->checksum) {
            r->stat_drop_checksum++;
            return 0;   /* 校验失败，丢弃该块 */
        }

        size_t off = (size_t)dh->block_idx * r->block_size;
        memcpy(r->frame_buf + off, pkt + sizeof(udp_data_hdr_t), payload_len);
        r->got[dh->block_idx] = 1;
        r->received_blocks++;
        r->stat_block_ok++;

        if (r->received_blocks == r->block_count) {
            *out_hdr   = r->hdr;
            *frame_out = r->frame_buf;
            *frame_len = r->hdr.data_size;
            free(r->got);
            r->got = NULL;
            r->have_hdr = 0;
            return 1;
        }
        return 0;
    }

    return 0;   /* 心跳/命令等不参与重组 */
}

/* ==================== Linux socket 接收层 ==================== */

#if defined(__linux__)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>

struct udp_receiver {
    int      fd;
    uint64_t pkts;        /* 应用层已取出的数据报数 */
    uint64_t bytes;
    int      rcvbuf_eff;  /* SO_RCVBUF 实际生效值（内核可能翻倍后按 rmem_max 截断） */
};

int udp_receiver_init(udp_receiver_t **r, uint16_t port)
{
    if (!r) return -1;
    udp_receiver_t *u = (udp_receiver_t *)calloc(1, sizeof(*u));
    if (!u) return -1;

    u->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (u->fd < 0) { free(u); return -1; }

    /* 请求大接收缓冲：一帧 440 包会在毫秒级突发到达，若缓冲只能装 ~100 包，
     * 而接收线程正忙于渲染 LCD，多出来的包会被内核静默丢弃（RcvbufErrors）。
     * 注意：内核会把请求值翻倍后按 net.core.rmem_max 截断，所以必须配合
     * scripts/tune_net.sh 提高 rmem_max，否则这里设置的值是无效的。 */
    int rcv = UDP_RECV_BUF_BYTES;
    if (setsockopt(u->fd, SOL_SOCKET, SO_RCVBUF, &rcv, sizeof(rcv)) != 0)
        fprintf(stderr, "[UDP-RECV] 设置 SO_RCVBUF 失败: %s\n", strerror(errno));

    socklen_t olen = (socklen_t)sizeof(u->rcvbuf_eff);
    if (getsockopt(u->fd, SOL_SOCKET, SO_RCVBUF, &u->rcvbuf_eff, &olen) != 0)
        u->rcvbuf_eff = -1;
    fprintf(stderr,"[UDP-RECV] effective SO_RCVBUF=%d bytes\n",u->rcvbuf_eff);
    if(u->rcvbuf_eff<UDP_RECV_BUF_BYTES)
        fprintf(stderr,"[UDP-RECV] Buffer capped: run sudo bash scripts/tune_net.sh before launch if frames drop.\n");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if (bind(u->fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        fprintf(stderr, "[UDP-RECV] bind :%u 失败: %s\n", (unsigned)port, strerror(errno));
        close(u->fd);
        free(u);
        return -1;
    }
    *r = u;
    return 0;
}

/* 等待可读（不取包）：>0 可读，0 超时，-1 出错 */
int udp_receiver_wait(udp_receiver_t *r, uint64_t timeout_ms)
{
    if (!r) return -1;
    struct pollfd pfd;
    pfd.fd = r->fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    int pr = poll(&pfd, 1, (int)timeout_ms);
    if (pr == 0) return 0;
    if (pr < 0) return (errno == EINTR) ? 0 : -1;
    return 1;
}

/* 非阻塞取一个包：>0 字节数，0 当前无数据，-1 出错 */
int udp_receiver_recv_nb(udp_receiver_t *r, uint8_t *buf, size_t cap)
{
    if (!r || !buf || cap == 0) return -1;
    ssize_t n = recv(r->fd, buf, cap, MSG_DONTWAIT);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return 0;   /* 队列已取干 */
        if (errno == EINTR) return 0;
        return -1;
    }
    r->pkts++;
    r->bytes += (uint64_t)n;
    return (int)n;
}

/* 阻塞式取一个包（保留原语义：返回字节数；超时 0；失败 -1） */
int udp_receiver_recv(udp_receiver_t *r, uint8_t *buf, size_t cap,
                      uint64_t timeout_ms)
{
    if (!r || !buf || cap == 0) return -1;

    int w = udp_receiver_wait(r, timeout_ms);
    if (w <= 0) return w;      /* 0=超时；-1=错误 */

    ssize_t n = recv(r->fd, buf, cap, 0);
    if (n < 0) return -1;
    r->pkts++;
    r->bytes += (uint64_t)n;
    return (int)n;
}

/* 解析 /proc/net/snmp 的 Udp 行（第二行是数值行，第一行是字段名） */
static int read_udp_snmp(udp_recv_stats_t *out)
{
    FILE *fp = fopen("/proc/net/snmp", "r");
    if (!fp) return -1;

    char line[512];
    int seen_hdr = 0, got = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "Udp:", 4) != 0) continue;
        if (!seen_hdr) { seen_hdr = 1; continue; }   /* 跳过字段名行 */
        unsigned long long in_dg = 0, no_ports = 0, in_err = 0;
        unsigned long long out_dg = 0, rcvbuf_err = 0, sndbuf_err = 0;
        /* 字段顺序：InDatagrams NoPorts InErrors OutDatagrams RcvbufErrors SndbufErrors ... */
        if (sscanf(line + 4, " %llu %llu %llu %llu %llu %llu",
                   &in_dg, &no_ports, &in_err, &out_dg, &rcvbuf_err, &sndbuf_err) >= 5) {
            out->kern_indatagrams = (uint64_t)in_dg;
            out->kern_noports     = (uint64_t)no_ports;
            out->kern_inerr       = (uint64_t)in_err;
            out->kern_rcvbuferr   = (uint64_t)rcvbuf_err;
            got = 1;
        }
        break;
    }
    fclose(fp);
    return got ? 0 : -1;
}

int udp_receiver_stats(udp_receiver_t *r, udp_recv_stats_t *out)
{
    if (!r || !out) return -1;
    out->pkts        = r->pkts;
    out->bytes       = r->bytes;
    out->rcvbuf_eff  = r->rcvbuf_eff;
    out->kern_indatagrams = 0;
    out->kern_rcvbuferr   = 0;
    out->kern_inerr       = 0;
    out->kern_noports     = 0;
    read_udp_snmp(out);        /* 读不到就保留 0，不影响主流程 */
    return 0;
}

void udp_receiver_close(udp_receiver_t *r)
{
    if (!r) return;
    if (r->fd >= 0) close(r->fd);
    free(r);
}

#else  /* 非 Linux：桩 */

struct udp_receiver { int fd; };

int udp_receiver_init(udp_receiver_t **r, uint16_t port)
{
    (void)r; (void)port;
    fprintf(stderr, "[UDP-RECV] 本机构建：socket 仅 Linux 板端可用\n");
    return -1;
}

int udp_receiver_recv(udp_receiver_t *r, uint8_t *buf, size_t cap, uint64_t timeout_ms)
{
    (void)r; (void)buf; (void)cap; (void)timeout_ms;
    return -1;
}

int udp_receiver_wait(udp_receiver_t *r, uint64_t timeout_ms)
{
    (void)r; (void)timeout_ms;
    return -1;
}

int udp_receiver_recv_nb(udp_receiver_t *r, uint8_t *buf, size_t cap)
{
    (void)r; (void)buf; (void)cap;
    return -1;
}

int udp_receiver_stats(udp_receiver_t *r, udp_recv_stats_t *out)
{
    (void)r; (void)out;
    return -1;
}

void udp_receiver_close(udp_receiver_t *r) { (void)r; }

#endif /* __linux__ */
