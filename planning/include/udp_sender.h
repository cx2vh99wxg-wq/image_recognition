/*
 * udp_sender.h — M 端 UDP 发送器（【人员 B】）
 *
 * 数据流：M 板 A 写的 shm_lane + shm_pcie_img → 本模块打包 → 发往 S 板 :8888。
 * 打包格式见 common/udp_proto.h（帧头 + 数据块 + 心跳）。
 *
 * 非 Linux 环境为桩（init 失败返回 -1），保证本机 make 通过；
 * 本机联调请用 planning/test/udp_mock.py 模拟发送。
 */
#ifndef UDP_SENDER_H
#define UDP_SENDER_H

#include <stdint.h>
#include <stddef.h>
#include "driving_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct udp_sender udp_sender_t;

/* 绑定目标（remote_ip 为 S 端 IP，port 为 UDP_PORT）。成功 0，失败 -1。 */
int  udp_sender_init(udp_sender_t **s, const char *remote_ip, uint16_t port);

/* 发送一帧：rgb565 图像（bytes 字节）+ lane 弯道结果。成功返回发送总字节数，失败 -1。 */
int  udp_sender_send_frame(udp_sender_t *s, const void *rgb565, size_t bytes,
                           const LaneResult *lane);

/* 发送一帧（扩展版）：额外携带红绿灯/虚实线/斑马线三项感知摘要（帧头 reserved[4]，
 * 布局见 common/include/udp_proto.h）。任一项可为 NULL —— 表示本帧不携带该项，
 * 接收端按帧头 flags 判定。基础版等价于本函数后三项传 NULL。 */
int  udp_sender_send_frame_ex(udp_sender_t *s, const void *rgb565, size_t bytes,
                              const LaneResult         *lane,
                              const TrafficLightResult *tl,
                              const LaneMarkResult     *lm,
                              const ZebraResult        *zebra);

/* 发送心跳（看门狗）。成功 0，失败 -1。 */
int  udp_sender_send_heartbeat(udp_sender_t *s, uint32_t seq);

void udp_sender_close(udp_sender_t *s);

#ifdef __cplusplus
}
#endif

#endif /* UDP_SENDER_H */
