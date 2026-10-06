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
    if (hdr->block_count == 0 || hdr->block_size == 0) return -1;

    free(r->got);
    r->got = (uint8_t *)calloc(hdr->block_count, 1);
    if (!r->got) return -1;

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
        if (begin_frame(r, &hdr) != 0) return -1;
        return 0;   /* 帧头本身不构成"完整帧" */
    }

    if (t == UDP_PKT_DATA) {
        if (!r->have_hdr) return 0;   /* 无帧头先来，丢弃 */

        const udp_data_hdr_t *dh = (const udp_data_hdr_t *)pkt;
        if (dh->frame_id != r->cur_frame_id) return 0;   /* 旧帧残块，丢弃 */
        if (dh->block_idx >= r->block_count) return 0;

        const size_t payload_len = len - sizeof(udp_data_hdr_t);
        if (payload_len > r->block_size) return 0;
        /* 非最后一块必须满长；最后一块允许小于 block_size */
        if (dh->block_idx < r->block_count - 1u && payload_len != r->block_size) return 0;

        if (r->got[dh->block_idx]) return 0;   /* 重复块，丢弃 */

        if (udp_block_checksum(pkt + sizeof(udp_data_hdr_t), payload_len) != dh->checksum)
            return 0;   /* 校验失败，丢弃该块 */

        size_t off = (size_t)dh->block_idx * r->block_size;
        memcpy(r->frame_buf + off, pkt + sizeof(udp_data_hdr_t), payload_len);
        r->got[dh->block_idx] = 1;
        r->received_blocks++;

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

struct udp_receiver {
    int fd;
};

int udp_receiver_init(udp_receiver_t **r, uint16_t port)
{
    if (!r) return -1;
    udp_receiver_t *u = (udp_receiver_t *)calloc(1, sizeof(*u));
    if (!u) return -1;

    u->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (u->fd < 0) { free(u); return -1; }

    int rcv = UDP_RECV_BUF_BYTES;
    setsockopt(u->fd, SOL_SOCKET, SO_RCVBUF, &rcv, sizeof(rcv));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    if (bind(u->fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        close(u->fd);
        free(u);
        return -1;
    }
    *r = u;
    return 0;
}

int udp_receiver_recv(udp_receiver_t *r, uint8_t *buf, size_t cap,
                      uint64_t timeout_ms)
{
    if (!r || !buf || cap == 0) return -1;

    struct pollfd pfd;
    pfd.fd = r->fd;
    pfd.events = POLLIN;
    int pr = poll(&pfd, 1, (int)timeout_ms);
    if (pr == 0) return 0;                  /* 超时 */
    if (pr < 0) return -1;

    ssize_t n = recv(r->fd, buf, cap, 0);
    if (n < 0) return -1;
    return (int)n;
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

void udp_receiver_close(udp_receiver_t *r) { (void)r; }

#endif /* __linux__ */
