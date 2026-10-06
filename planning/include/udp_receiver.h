/*
 * udp_receiver.h — S 端 UDP 接收器（【人员 B】）
 *
 * 两部分：
 *  - udp_reassembly_*：整帧重组状态机（纯逻辑，不依赖 socket，本机可单测）；
 *  - udp_receiver_*   ：Linux UDP socket 接收层。
 *
 * 重组规则：收到帧头即开始新帧（丢弃上一帧残块）；按块号位图收齐全部块
 * 即输出完整帧（帧头 + 图像 buffer）。块丢失/乱序由位图容忍；残帧超时由
 * 调用方按 UDP_FRAME_TIMEOUT_MS 调用 udp_reassembly_reset 丢弃。
 */
#ifndef UDP_RECEIVER_H
#define UDP_RECEIVER_H

#include <stdint.h>
#include <stddef.h>
#include "udp_proto.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---- 整帧重组状态机（纯逻辑） ---- */
typedef struct {
    uint8_t  *frame_buf;        /* 调用方提供的整帧缓冲（≥ data_size） */
    size_t    frame_cap;
    uint32_t  cur_frame_id;
    uint32_t  block_size;
    uint32_t  block_count;
    uint32_t  received_blocks;  /* 已收块数 */
    uint8_t  *got;              /* 位图 got[block_count] */
    int       have_hdr;         /* 已收到帧头 */
    udp_frame_hdr_t hdr;        /* 最近帧头 */
} udp_reassembly_t;

int  udp_reassembly_init(udp_reassembly_t *r, uint8_t *frame_buf, size_t cap);
void udp_reassembly_reset(udp_reassembly_t *r);

/* 喂入一个 UDP 数据报。
 * 返回：1=本帧重组完成（*out_hdr 有效，*frame_out=frame_buf，*frame_len=数据字节数）；
 *       0=仍在收块（正常）；-1=非法包/失败。 */
int  udp_reassembly_feed(udp_reassembly_t *r, const uint8_t *pkt, size_t len,
                         udp_frame_hdr_t *out_hdr,
                         uint8_t **frame_out, size_t *frame_len);

/* ---- Linux socket 接收层（仅板端） ---- */
typedef struct udp_receiver udp_receiver_t;

int  udp_receiver_init(udp_receiver_t **r, uint16_t port);        /* 非 Linux 返回 -1 */
int  udp_receiver_recv(udp_receiver_t *r, uint8_t *buf, size_t cap,
                       uint64_t timeout_ms);                       /* 返回接收字节数；超时 0；失败 -1 */
void udp_receiver_close(udp_receiver_t *r);

#ifdef __cplusplus
}
#endif

#endif /* UDP_RECEIVER_H */
