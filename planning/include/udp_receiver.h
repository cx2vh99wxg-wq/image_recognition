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

    /* ---- 诊断统计（累积计数，不参与重组逻辑；reset 不清零） ----
     * 用途：区分"根本没收到包"与"收到包但凑不齐帧"这两类完全不同的故障。
     * 若 stat_drop_* 全为 0 且 stat_block_ok 增长 → 收到块但收不齐整帧；
     * 若各计数都不动 → 包没进到本进程（网络/端口/缓冲区问题，看内核统计）。 */
    uint32_t  stat_hdr_rx;      /* 收到的帧头包数 */
    uint32_t  stat_block_ok;    /* 接受（写入缓冲）的数据块数 */
    uint32_t  stat_drop_checksum; /* 校验和不符丢弃 */
    uint32_t  stat_drop_dup;    /* 重复块丢弃 */
    uint32_t  stat_drop_nohdr;  /* 帧头未到先来块 */
    uint32_t  stat_drop_fid;    /* 旧帧残块（frame_id 不匹配） */
    uint32_t  stat_drop_other;  /* 块号越界/长度异常等 */
    uint32_t  stat_begin_fail;  /* 帧头无法开始新帧（缓冲不足/块数非法） */
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

/* 接收统计（诊断用）：把"应用层看到的包"与"内核丢掉的包"区分开 */
typedef struct {
    uint64_t pkts;              /* 本进程已从 socket 取出的数据报数 */
    uint64_t bytes;             /* 对应字节数 */
    int      rcvbuf_eff;        /* getsockopt(SO_RCVBUF) 实际生效值(字节，内核可能翻倍并截断) */
    uint64_t kern_indatagrams;  /* /proc/net/snmp Udp:InDatagrams（系统累计收到） */
    uint64_t kern_rcvbuferr;    /* /proc/net/snmp Udp:RcvbufErrors（内核队列溢出丢失！） */
    uint64_t kern_inerr;        /* /proc/net/snmp Udp:InErrors */
    uint64_t kern_noports;      /* /proc/net/snmp Udp:NoPorts（无监听者时的 ICMP 计数） */
} udp_recv_stats_t;

int  udp_receiver_init(udp_receiver_t **r, uint16_t port);        /* 非 Linux 返回 -1 */
int  udp_receiver_recv(udp_receiver_t *r, uint8_t *buf, size_t cap,
                       uint64_t timeout_ms);                       /* 返回接收字节数；超时 0；失败 -1 */
/* 等待可读：>0 有数据可读，0 超时，-1 出错（非 Linux 返回 -1） */
int  udp_receiver_wait(udp_receiver_t *r, uint64_t timeout_ms);
/* 非阻塞取一个包：>0 字节数，0 当前无数据，-1 出错。
 * 配合 udp_receiver_wait 使用可"一次收干内核队列"，避免每轮只收一包导致溢出。 */
int  udp_receiver_recv_nb(udp_receiver_t *r, uint8_t *buf, size_t cap);
/* 读取接收统计（含内核 Udp 计数）；成功 0，失败 -1 */
int  udp_receiver_stats(udp_receiver_t *r, udp_recv_stats_t *out);
void udp_receiver_close(udp_receiver_t *r);

#ifdef __cplusplus
}
#endif

#endif /* UDP_RECEIVER_H */
