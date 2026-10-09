/*
 * udp_sender.c — M 端 UDP 发送器实现（【人员 B】）
 *
 * 一帧 = 1 个帧头包 + ceil(data_size/block_size) 个数据块包。
 * 块大小默认 UDP_BLOCK_SIZE（1400B），每块带 payload 累加和。
 * 相比旧版"每行一个包"（482 包/帧），本实现约 1+439 包/帧，包数减 ~10%。
 *
 * 非 Linux 环境编译为桩（init 失败），socket 代码仅 Linux 板端编译。
 */
#include "udp_sender.h"
#include "udp_proto.h"
#include "driving_config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined(__linux__)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

struct udp_sender {
    int fd;
    struct sockaddr_in peer;
};

int udp_sender_init(udp_sender_t **s, const char *remote_ip, uint16_t port)
{
    if (!s || !remote_ip) return -1;
    udp_sender_t *u = (udp_sender_t *)calloc(1, sizeof(*u));
    if (!u) return -1;

    u->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (u->fd < 0) { free(u); return -1; }

    int snd = UDP_SEND_BUF_BYTES;
    setsockopt(u->fd, SOL_SOCKET, SO_SNDBUF, &snd, sizeof(snd));

    memset(&u->peer, 0, sizeof(u->peer));
    u->peer.sin_family = AF_INET;
    u->peer.sin_port = htons(port);
    if (inet_pton(AF_INET, remote_ip, &u->peer.sin_addr) <= 0) {
        close(u->fd);
        free(u);
        return -1;
    }
    *s = u;
    return 0;
}

int udp_sender_send_frame_ex(udp_sender_t *s, const void *rgb565, size_t bytes,
                             const LaneResult         *lane,
                             const TrafficLightResult *tl,
                             const LaneMarkResult     *lm,
                             const ZebraResult        *zebra)
{
    if (!s || !rgb565 || bytes == 0) return -1;

    udp_frame_hdr_t hdr;
    if (udp_pack_frame_hdr(&hdr, lane, IMG_WIDTH, IMG_HEIGHT, UDP_BLOCK_SIZE) != 0)
        return -1;
    /* 三感知摘要走帧头 reserved[4]（16B 已存在字段，不改包尺寸）：
     * 必须在 pack_frame_hdr 之后调用——后者 memset 清零整帧头。 */
    udp_hdr_set_aux(&hdr, tl, lm, zebra);
    if (bytes != hdr.data_size) return -1;

    const socklen_t alen = (socklen_t)sizeof(s->peer);
    ssize_t total = 0;

    /* 1. 帧头包 */
    ssize_t n = sendto(s->fd, &hdr, sizeof(hdr), 0,
                       (struct sockaddr *)&s->peer, alen);
    if (n != (ssize_t)sizeof(hdr)) return -1;
    total += n;

    /* 2. 数据块包 */
    const uint8_t *p = (const uint8_t *)rgb565;
    const uint32_t bs = hdr.block_size;
    const uint32_t count = hdr.block_count;
    uint8_t pkt[sizeof(udp_data_hdr_t) + UDP_BLOCK_SIZE];

    for (uint32_t i = 0; i < count; i++) {
        size_t off = (size_t)i * bs;
        size_t len = (off + bs <= bytes) ? bs : (bytes - off);

        udp_data_hdr_t *dh = (udp_data_hdr_t *)pkt;
        dh->magic    = UDP_MAGIC_DATA;
        dh->frame_id = hdr.frame_id;
        dh->block_idx = i;
        dh->checksum = udp_block_checksum(p + off, len);
        memcpy(pkt + sizeof(udp_data_hdr_t), p + off, len);

        ssize_t sn = sendto(s->fd, pkt, sizeof(udp_data_hdr_t) + len, 0,
                            (struct sockaddr *)&s->peer, alen);
        if (sn != (ssize_t)(sizeof(udp_data_hdr_t) + len)) return -1;
        total += sn;

        /* 轻度整形：440 包一次性糊上去会在对端内核队列形成微突发，对端缓冲不够
         * 就会静默丢包（收帧恒 0 的根因之一）。每 UDP_SEND_PACE_EVERY 块让出
         * 一下 CPU，把突发拉平；对整帧耗时影响约 1~2ms，可忽略。
         * 置 UDP_SEND_PACE_EVERY=0 可关闭。 */
#if defined(__linux__)
        if (UDP_SEND_PACE_EVERY > 0 && (i % UDP_SEND_PACE_EVERY) == (UDP_SEND_PACE_EVERY - 1))
            usleep((useconds_t)UDP_SEND_PACE_US);
#endif
    }
    return (int)total;
}

/* 基础版 = 扩展版不带三感知（保持既有调用点与单测不变） */
int udp_sender_send_frame(udp_sender_t *s, const void *rgb565, size_t bytes,
                          const LaneResult *lane)
{
    return udp_sender_send_frame_ex(s, rgb565, bytes, lane, NULL, NULL, NULL);
}

int udp_sender_send_heartbeat(udp_sender_t *s, uint32_t seq)
{
    if (!s) return -1;
    udp_heartbeat_t hb;
    if (udp_pack_heartbeat(&hb, seq) != 0) return -1;
    ssize_t n = sendto(s->fd, &hb, sizeof(hb), 0,
                       (struct sockaddr *)&s->peer, (socklen_t)sizeof(s->peer));
    return (n == (ssize_t)sizeof(hb)) ? 0 : -1;
}

void udp_sender_close(udp_sender_t *s)
{
    if (!s) return;
    if (s->fd >= 0) close(s->fd);
    free(s);
}

#else  /* 非 Linux：桩 */

struct udp_sender { int fd; };

int udp_sender_init(udp_sender_t **s, const char *remote_ip, uint16_t port)
{
    (void)s; (void)remote_ip; (void)port;
    fprintf(stderr, "[UDP-SEND] 本机构建：socket 仅 Linux 板端可用，请用 udp_mock.py 联调\n");
    return -1;
}

int udp_sender_send_frame_ex(udp_sender_t *s, const void *rgb565, size_t bytes,
                             const LaneResult         *lane,
                             const TrafficLightResult *tl,
                             const LaneMarkResult     *lm,
                             const ZebraResult        *zebra)
{
    (void)s; (void)rgb565; (void)bytes; (void)lane;
    (void)tl; (void)lm; (void)zebra;
    return -1;
}

int udp_sender_send_frame(udp_sender_t *s, const void *rgb565, size_t bytes,
                          const LaneResult *lane)
{
    (void)s; (void)rgb565; (void)bytes; (void)lane;
    return -1;
}

int udp_sender_send_heartbeat(udp_sender_t *s, uint32_t seq)
{
    (void)s; (void)seq;
    return -1;
}

void udp_sender_close(udp_sender_t *s) { (void)s; }

#endif /* __linux__ */
