/*
 * udp_m_send_main.c — M 端 UDP 发送器主程序（【人员 B】）
 *
 * 运行于 M 板 RK3568：
 *   读取 A 写出的 shm_lane(0x1234567E) + shm_pcie_img(0x12345679)，
 *   按 udp_proto 协议发往 S 板 UDP_IP_S:UDP_PORT。
 *
 * 用法：
 *   ./udp_m_send_main            # 真实模式：读 A 的共享内存
 *   ./udp_m_send_main --stub     # 桩模式：不读硬件图，发纯色图（联调用）
 *
 * 信号：SIGINT/SIGTERM 优雅退出。
 */
#include "shm_ipc.h"
#include "udp_proto.h"
#include "driving_config.h"
#include "time_util.h"

#define LOG_TAG "UDP_M"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#if defined(__linux__)
#include <unistd.h>
#define SLEEP_MS(ms) usleep((useconds_t)(ms) * 1000u)
#else
#define SLEEP_MS(ms) (void)(ms)
#endif

#include "udp_sender.h"

static volatile int g_keep_running = 1;
static void on_signal(int sig) { (void)sig; g_keep_running = 0; }

static uint8_t *g_img565 = NULL;   /* 图像缓冲 */

/* 桩模式：构造一张灰色渐变图，便于无硬件联调 UDP 链路 */
static void fill_stub_image(void)
{
    if (!g_img565) return;
    for (int y = 0; y < IMG_HEIGHT; y++) {
        uint8_t *row = g_img565 + (size_t)y * IMG_WIDTH * IMG_BPP_565;
        uint8_t v = (uint8_t)(y * 255 / IMG_HEIGHT);
        for (int x = 0; x < IMG_WIDTH; x++) {
            row[x * 2 + 0] = v;          /* 565 低字节 */
            row[x * 2 + 1] = (uint8_t)((v >> 3) << 3); /* 565 高字节近似 */
        }
    }
}

int main(int argc, char **argv)
{
    int use_stub = 0;
    for (int i = 1; i < argc; i++)
        if (strcmp(argv[i], "--stub") == 0) use_stub = 1;

    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    LOGI("M端UDP发送器启动 (stub=%d)\n", use_stub);

    g_img565 = (uint8_t *)malloc(IMG_FRAME_BYTES);
    if (!g_img565) { LOGE("图像缓冲分配失败\n"); return -1; }

    /* 读者：打开 A 的共享段（写者未启动会立即报错） */
    if (!use_stub) {
        void *ptr = NULL;
        if (shm_open(SHM_KEY_LANE, SHM_LANE_SIZE, &ptr) != 0) {
            LOGE("shm_lane 不可用：请先启动感知进程\n");
            free(g_img565);
            return -1;
        }
        if (shm_open(SHM_KEY_PCIE_IMG, SHM_IMG_SIZE, &ptr) != 0) {
            LOGE("shm_pcie_img 不可用：桩模式请用 --stub\n");
            free(g_img565);
            return -1;
        }
    }

    udp_sender_t *sender = NULL;
    if (udp_sender_init(&sender, UDP_IP_S, UDP_PORT) != 0) {
        LOGE("UDP 发送器初始化失败（本机无 socket，请上板运行）\n");
        free(g_img565);
        return -1;
    }
    LOGI("目标: %s:%d\n", UDP_IP_S, UDP_PORT);

    LaneResult lane;
    memset(&lane, 0, sizeof(lane));
    lane.version = LANERESULT_VERSION;

    uint32_t last_lane_fid = 0xFFFFFFFFu;
    uint32_t hb_seq = 0;
    uint64_t last_hb_us = 0;
    /* 用启动时刻初始化：now_us_mono() 是 CLOCK_MONOTONIC 绝对值，若初值为 0，
     * 第一行统计会在启动瞬间立刻打印（"已发帧=1"），看起来像统计间隔不对。 */
    uint64_t last_stat_us = now_us_mono();
    uint32_t sent_frames = 0, sent_blocks_fail = 0;
    int aux_tl = 0, aux_lm = 0, aux_zb = 0;   /* 本帧是否携带三感知（供统计打印） */

    while (g_keep_running) {
        uint64_t now = now_us_mono();

        /* 读最新车道结果（有更新才发帧） */
        int lane_new = 0;
        if (!use_stub) {
            if (shm_read_lane(&lane) == 0 && lane.frame_id != last_lane_fid) {
                last_lane_fid = lane.frame_id;
                lane_new = 1;
            }
        } else {
            /* 桩：每 200ms 伪造一帧直道 */
            static uint64_t last_stub_us = 0;
            if (now - last_stub_us > 200000u) {
                last_stub_us = now;
                lane.frame_id++;
                lane.direction = LANE_STRAIGHT;
                lane.confidence = 95;
                lane.curve_offset = 0;
                lane.lane_pixel_cnt = 1000;
                lane.timestamp_us = now;
                lane_new = 1;
            }
        }

        if (lane_new) {
            if (use_stub) fill_stub_image();
            else if (shm_read_pcie_img(g_img565, IMG_FRAME_BYTES) != 0) {
                LOGW("读 shm_pcie_img 失败\n");
            }

            /* 附带 A 端的三项感知（红绿灯/虚实线/斑马线）。
             * 它们与本帧图像同属一帧：A 的 main_perception 在同一轮里写这三段，
             * 这里按"读到就带、读不到就置 NULL"处理——缺失不阻塞发帧，
             * 帧头 flags 会如实反映本帧携带了哪几项（见 udp_proto.h 摘要布局）。
             * shm_read_* 内部即 shm_open（带退避/限流日志），A 未启动时开销可忽略。 */
            TrafficLightResult tl;  LaneMarkResult lm;  ZebraResult zb;
            const int has_tl = (shm_read_traffic_light(&tl) == 0 && tl.version == TL_VERSION);
            const int has_lm = (shm_read_lane_mark(&lm)   == 0 && lm.version == LANEMARK_VERSION);
            const int has_zb = (shm_read_zebra(&zb)       == 0 && zb.version == ZEBRA_VERSION);
            aux_tl = has_tl; aux_lm = has_lm; aux_zb = has_zb;

            int n = udp_sender_send_frame_ex(sender, g_img565, IMG_FRAME_BYTES, &lane,
                                             has_tl ? &tl : NULL,
                                             has_lm ? &lm : NULL,
                                             has_zb ? &zb : NULL);
            if (n > 0) {
                sent_frames++;
            } else {
                sent_blocks_fail++;
            }
        }

        /* 心跳：200ms 周期 */
        if (now - last_hb_us >= (uint64_t)UDP_HEARTBEAT_MS * 1000u) {
            last_hb_us = now;
            udp_sender_send_heartbeat(sender, ++hb_seq);
        }

        /* 统计（每 2s） */
        if (now - last_stat_us >= 2000000u) {
            last_stat_us = now;
            LOGI("已发帧=%u 失败=%u 心跳=%u | dir=%d off=%d conf=%u | 附带[红绿灯=%d 虚实线=%d 斑马线=%d]\n",
                 sent_frames, sent_blocks_fail, hb_seq,
                 (int)lane.direction, lane.curve_offset, lane.confidence,
                 aux_tl, aux_lm, aux_zb);
        }

        SLEEP_MS(5);
    }

    LOGI("M端UDP发送器退出（共发 %u 帧）\n", sent_frames);
    udp_sender_close(sender);
    free(g_img565);
    return 0;
}
