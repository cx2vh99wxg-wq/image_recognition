/*
 * main_planning.c — S 端决策+显示主循环（【人员 B】，运行于 S 板 RK3568）
 *
 * 流程（单进程内完成，决策与 C 的控制循环通过 shm_cmd 解耦）：
 *   1. UDP 接收（M 端图像+车道结果）→ 写 shm_udp_img + 转发 shm_lane
 *   2. 读本地 shm_pcie_img（S 板本地图，缺失则降级只显示远端）
 *   3. 本地 YOLOv5s 行人检测 → shm_person
 *   4. 决策状态机（车道 + 行人 → ControlCommandMsg）→ shm_cmd
 *   5. LCD(X11) 双路拼接渲染（左本地 / 右远端）
 *
 * 用法：
 *   ./planning_main [--model <person模型路径>] [--no-lcd]
 *
 * 注意：S 板是否具备"本地 PCIe 采集"需团队按硬件确认（部署拓扑见分工方案
 * 第二节）；本程序对本地图缺失做了降级（只显示远端图），不影响联调。
 */
#include "shm_ipc.h"
#include "udp_proto.h"
#include "udp_receiver.h"
#include "decision.h"
#include "person_detect.h"
#include "render_lcd.h"
#include "driving_config.h"
#include "time_util.h"

#define LOG_TAG "PLANNING"
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

static volatile int g_keep_running = 1;
static void on_signal(int sig) { (void)sig; g_keep_running = 0; }

/* RGB565 → RGB888（本文件内的小工具，避免跨模块依赖感知层） */
static void rgb565_to_888(const uint8_t *src, uint8_t *dst, int n)
{
    for (int i = 0; i < n; i++) {
        uint16_t p = (uint16_t)(src[0] | ((uint16_t)src[1] << 8));
        uint8_t r5 = (uint8_t)((p >> 11) & 0x1F);
        uint8_t g6 = (uint8_t)((p >> 5) & 0x3F);
        uint8_t b5 = (uint8_t)(p & 0x1F);
        dst[0] = (uint8_t)((r5 << 3) | (r5 >> 2));
        dst[1] = (uint8_t)((g6 << 2) | (g6 >> 4));
        dst[2] = (uint8_t)((b5 << 3) | (b5 >> 2));
        src += 2; dst += 3;
    }
}

int main(int argc, char **argv)
{
    const char *model_path = NULL;
    int use_lcd = 1;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--model") == 0 && i + 1 < argc) model_path = argv[++i];
        else if (strcmp(argv[i], "--no-lcd") == 0) use_lcd = 0;
    }

    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    LOGI("S端决策进程启动 (lcd=%d)\n", use_lcd);

    /* ---- 创建 B 的共享段 ---- */
    void *dummy = NULL;
    if (shm_create("udp_img",   SHM_KEY_UDP_IMG,   SHM_IMG_SIZE,    &dummy) != 0 ||
        shm_create("cmd",       SHM_KEY_CMD,       SHM_CMD_SIZE,    &dummy) != 0 ||
        shm_create("display",   SHM_KEY_DISPLAY,   SHM_DISPLAY_SIZE,&dummy) != 0 ||
        shm_create("person",    SHM_KEY_PERSON,    SHM_PERSON_SIZE, &dummy) != 0) {
        LOGE("创建共享段失败（请先运行 scripts/start_s.sh 清理残留）\n");
        return -1;
    }

    /* ---- UDP 接收器 ---- */
    udp_receiver_t *recv = NULL;
    if (udp_receiver_init(&recv, UDP_PORT) != 0) {
        LOGE("UDP 接收器初始化失败: %d\n", UDP_PORT);
        return -1;
    }
    LOGI("UDP 监听 :%d\n", UDP_PORT);

    /* ---- 整帧重组 ---- */
    static uint8_t s_frame[IMG_FRAME_BYTES];
    static uint8_t s_pkt[sizeof(udp_data_hdr_t) + UDP_BLOCK_SIZE + 64];
    udp_reassembly_t reass;
    if (udp_reassembly_init(&reass, s_frame, sizeof(s_frame)) != 0) {
        LOGE("重组器初始化失败\n");
        udp_receiver_close(recv);
        return -1;
    }

    /* ---- 行人检测（失败不致命，退化为无行人） ---- */
    person_detect_ctx_t *person = NULL;
    if (person_detect_init(&person, model_path, IMG_WIDTH, IMG_HEIGHT) != 0)
        LOGW("行人检测初始化失败（无行人模式继续）\n");

    /* ---- LCD 渲染（失败不致命） ---- */
    render_lcd_ctx_t *lcd = NULL;
    if (use_lcd && render_lcd_init(&lcd, IMG_WIDTH * 2, IMG_HEIGHT) != 0)
        LOGW("LCD 初始化失败（无显示模式继续）\n");

    /* ---- 决策 ---- */
    decision_ctx_t dec;
    decision_init(&dec);

    static uint8_t s_local565[IMG_FRAME_BYTES];
    static uint8_t s_remote565[IMG_FRAME_BYTES];
    static uint8_t s_local888[(size_t)IMG_WIDTH * IMG_HEIGHT * 3];

    DisplayMode mode = DISPLAY_MODE_SPLIT;
    ControlCommandMsg cmd;
    uint32_t frames_recv = 0, frames_lcd = 0;
    uint64_t last_pkt_us = 0, last_stat_us = 0;
    int local_ok = 0;

    while (g_keep_running) {
        uint64_t now = now_us_mono();

        /* ---- 1. UDP 收包并重组 ---- */
        int n = udp_receiver_recv(recv, s_pkt, sizeof(s_pkt), 10);
        if (n > 0) {
            last_pkt_us = now;
            udp_frame_hdr_t hdr;
            uint8_t *frame = NULL;
            size_t frame_len = 0;
            int done = udp_reassembly_feed(&reass, s_pkt, (size_t)n, &hdr, &frame, &frame_len);
            if (done == 1) {
                frames_recv++;
                /* 写远端图 + 转发车道结果到 S 板 shm_lane */
                if (shm_write_udp_img(frame, frame_len) != 0) LOGW("写 shm_udp_img 失败\n");
                LaneResult lane;
                if (udp_hdr_to_lane(&hdr, &lane) == 0) {
                    if (shm_write_lane(&lane) != 0) LOGW("转发 shm_lane 失败\n");
                }
            }
        }
        /* 重组超时：丢弃残帧 */
        if (reass.have_hdr && (now - last_pkt_us) > (uint64_t)UDP_FRAME_TIMEOUT_MS * 1000u)
            udp_reassembly_reset(&reass);

        /* ---- 2. 读本地图（S 板本地 PCIe，缺失降级） ---- */
        if (shm_read_pcie_img(s_local565, IMG_FRAME_BYTES) == 0)
            local_ok = 1;

        /* ---- 3. 行人检测（本地图 RGB565→888 后送 YOLOv5s） ---- */
        PersonState pstate;
        memset(&pstate, 0, sizeof(pstate));
        pstate.version = PERSON_VERSION;
        if (person && local_ok) {
            rgb565_to_888(s_local565, s_local888, IMG_WIDTH * IMG_HEIGHT);
            person_detect_run(person, s_local888, frames_recv, now, &pstate);
        }
        if (shm_write_person(&pstate) != 0) LOGW("写 shm_person 失败\n");

        /* ---- 4. 决策（车道来自 UDP 转发，行人来自本地检测） ---- */
        LaneResult lane;
        LaneResult *lp = NULL;
        if (shm_read_lane(&lane) == 0 && lane.version == LANERESULT_VERSION)
            lp = &lane;

        PersonState *p_in = (pstate.version == PERSON_VERSION) ? &pstate : NULL;
        if (decision_step(&dec, lp, NULL, NULL, NULL, p_in, now, &cmd) != 0) {
            LOGW("决策失败\n");
        } else if (cmd.command != CMD_NONE) {
            if (shm_write_cmd(&cmd) != 0) LOGW("写 shm_cmd 失败\n");
        }

        /* ---- 5. LCD 渲染 ---- */
        if (lcd) {
            if (shm_read_udp_img(s_remote565, IMG_FRAME_BYTES) != 0 && frames_recv > 0)
                LOGW("读 shm_udp_img 失败\n");
            if (render_lcd_draw(lcd,
                                local_ok ? s_local565 : NULL,
                                frames_recv ? s_remote565 : NULL,
                                mode, frames_recv) == 0)
                frames_lcd++;
        }

        /* ---- 统计（每 2s） ---- */
        if (now - last_stat_us >= 2000000u) {
            last_stat_us = now;
            LOGI("收帧=%u LCD帧=%u | 最近命令=%d enable=%u pri=%u conf=%u\n",
                 frames_recv, frames_lcd,
                 (int)cmd.command, (unsigned)cmd.enable,
                 (unsigned)cmd.priority, (unsigned)cmd.confidence);
        }

        SLEEP_MS(5);
    }

    LOGI("S端决策进程退出（共收 %u 帧）\n", frames_recv);
    render_lcd_deinit(lcd);
    if (person) person_detect_deinit(person);
    udp_reassembly_reset(&reass);
    udp_receiver_close(recv);
    return 0;
}
