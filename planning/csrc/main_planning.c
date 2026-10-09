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
 
 
 型路径>] [--no-lcd]
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

    /* ---- 创建 B 的共享段 ----
     * 注意 lane 这一项不能漏：shm_write_lane() 内部走的是 shm_open()（"只打开
     * 已存在段"的读者语义），而 driving_config.h 的契约是「A 写(M板) / B 转发写
     * (S板) / B、C 读」——S 端自己就是写者，必须先建段。少了它，每收到一帧就会
     * 打一行"转发 shm_lane 失败"，且 C 的控制进程永远读不到车道状态。
     * （shm_create 用 IPC_CREAT|0666、不含 IPC_EXCL，复用重启后的残留段不会失败。） */
    void *dummy = NULL;
    if (shm_create("udp_img",   SHM_KEY_UDP_IMG,   SHM_IMG_SIZE,    &dummy) != 0 ||
        shm_create("cmd",       SHM_KEY_CMD,       SHM_CMD_SIZE,    &dummy) != 0 ||
        shm_create("display",   SHM_KEY_DISPLAY,   SHM_DISPLAY_SIZE,&dummy) != 0 ||
        shm_create("person",    SHM_KEY_PERSON,    SHM_PERSON_SIZE, &dummy) != 0 ||
        shm_create("lane",      SHM_KEY_LANE,      SHM_LANE_SIZE,   &dummy) != 0) {
        LOGE("创建共享段失败（请先运行 scripts/start_s.sh 清理残留）\n");
        return -1;
    }

    /* ---- UDP 接收器 ---- */
    udp_receiver_t *recv = NULL;
    if (udp_receiver_init(&recv, UDP_PORT) != 0) {
        LOGE("UDP 接收器初始化失败: %d（端口被占用？先 pkill -f planning_main）\n", UDP_PORT);
        return -1;
    }
    {
        /* 打印"实际生效"的接收缓冲：SO_RCVBUF 会被内核按 net.core.rmem_max 静默截断，
         * 若这里远小于 2MB，就说明 tune_net.sh 还没执行，突发丢包风险很高。 */
        udp_recv_stats_t st;
        memset(&st, 0, sizeof(st));
        if (udp_receiver_stats(recv, &st) == 0) {
            LOGI("UDP 监听 :%d  SO_RCVBUF=%d 字节\n", UDP_PORT, st.rcvbuf_eff);
            if (st.rcvbuf_eff > 0 && st.rcvbuf_eff < (int)(2 * 1024 * 1024))
                LOGW("接收缓冲偏小！一帧=440 包(≈0.9MB)会突发到达，容易溢出丢包。"
                     "请先执行 sudo ./scripts/tune_net.sh（提高 net.core.rmem_max）\n");
        }
    }

    /* ---- 内核 UDP 计数基线 ----
     * /proc/net/snmp 的 Udp 计数是"系统开机以来"的累计值，直接打印出来会是
     * 十几万的存量（联调早期旧代码攒下的），一眼看去像"现在还在疯狂丢包"，
     * 极易误导排查方向。这里记下启动时刻的快照，日志里只报"本段时间的增量"。 */
    uint64_t kern_rcvbuferr_base = 0, kern_inerr_base = 0, kern_indatagram_base = 0;
    {
        udp_recv_stats_t st0;
        memset(&st0, 0, sizeof(st0));
        if (udp_receiver_stats(recv, &st0) == 0) {
            kern_rcvbuferr_base   = st0.kern_rcvbuferr;
            kern_inerr_base       = st0.kern_inerr;
            kern_indatagram_base  = st0.kern_indatagrams;
        }
    }

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
    uint64_t last_pkt_us = 0;
    int local_ok = 0;

    /* 统计窗口（每 2s）：用"窗口内取出的包数 / 窗口时长"直接量化消费速率，
     * 这是判断"是否被消费能力拖死"的唯一直接指标（M 端产出≈2200 包/s）。
     * 注意 last_stat_us 必须用"启动时刻"初始化：now_us_mono() 返回的是
     * CLOCK_MONOTONIC 绝对值（开机以来的微秒数），若沿用初值 0，第一轮
     * dt 会等于整机开机时长（几十万秒），算出的速率会失真成 ~0。 */
    uint64_t last_stat_us = now_us_mono();
    uint64_t win_pkts = 0;        /* 本窗口内从 socket 取出的包数 */
    uint64_t win_loops = 0;       /* 本窗口内跑过的主循环轮数（用于算真实轮周期） */
    uint64_t win_lcd_seen = 0;    /* 上次统计时的 LCD 帧数（只用于报"本期新增"） */

    LaneResult cur_lane;        /* 本周期最新收到的车道结果（仅 lane_fresh=1 时有效） */
    int lane_fresh = 0;         /* 本周期是否收到“新的一帧”车道结果 */

    /* 三感知（由 M 端随帧头 reserved[4] 摘要携带，布局见 common/udp_proto.h）。
     * 语义与 lane 一致：*_fresh 仅在"本帧确实携带该项"（帧头 flags 置位）时为真，
     * 交决策做超龄判断；cur_* 同时供 LCD 叠加层（箭头/红绿灯/报警）使用。 */
    TrafficLightResult cur_tl;  int tl_fresh = 0;
    LaneMarkResult     cur_lm;  int lm_fresh = 0;
    ZebraResult        cur_zb;  int zb_fresh = 0;
    memset(&cur_tl, 0, sizeof(cur_tl));
    memset(&cur_lm, 0, sizeof(cur_lm));
    memset(&cur_zb, 0, sizeof(cur_zb));

    /* 按键模拟（赛题三："通过按键模拟切换车道/停止/行驶"）：
     * man_on/man_cmd 是人工注入的命令，优先级高于自动决策；a 键交还自动。
     * force_lane_change 供"强制实线变道报警"判定。 */
    int           man_on  = 0;
    ControlCommand man_cmd = CMD_NONE;
    int           force_lane_change = 0;

    while (g_keep_running) {
        uint64_t now = now_us_mono();
        win_loops++;

        /* ---- 1. UDP 收包并重组（每轮把内核队列收干！） ----
         * 关键：一帧 = 1 帧头 + 439 个数据块（614400B / 1400B）= 440 包，M 端在一个
         * 突发里连续发出（桩模式 5 帧/s ≈ 2200 包/s）。旧实现每轮只 recv "一个"
         * 数据报，而一轮还要渲染 LCD（数十 ms），消费能力 ≈ 20~50 包/s，比产出低
         * 两个数量级 → 内核 UDP 接收队列反复溢出、静默丢包 → 439 块永远凑不齐 →
         * "收帧"恒为 0（而 LCD 帧照常增长，因为它不依赖收包）。
         * 修复：poll 到可读后，用非阻塞 recv 把队列里已有的包一次收干再走后续流程。 */
        int any_pkt = 0;                 /* 本轮是否收到过包（用于超时判定/日志） */
        int any_frame = 0;               /* 本轮是否重组出完整帧 */
        udp_frame_hdr_t last_hdr;        /* 本轮最后一个完整帧的帧头 */
        uint8_t *last_frame = NULL;
        size_t   last_frame_len = 0;

        if (udp_receiver_wait(recv, UDP_RECV_WAIT_MS) > 0) {
            for (int k = 0; k < UDP_RECV_DRAIN_MAX; k++) {
                int n = udp_receiver_recv_nb(recv, s_pkt, sizeof(s_pkt));
                if (n <= 0) break;               /* 队列已取干（或出错） */
                any_pkt = 1;
                win_pkts++;                      /* 计入本统计窗口的消费量 */

                uint8_t *frame = NULL;
                size_t frame_len = 0;
                int done = udp_reassembly_feed(&reass, s_pkt, (size_t)n,
                                               &last_hdr, &frame, &frame_len);
                if (done == 1) {
                    frames_recv++;
                    any_frame = 1;
                    last_frame = frame;          /* 本轮可能重组出多帧，只落地最后一帧 */
                    last_frame_len = frame_len;
                }
            }
        }

        if (any_frame) {
            /* 写远端图（LCD 右半屏数据源）+ 转发车道结果到 S 板 shm_lane */
            if (shm_write_udp_img(last_frame, last_frame_len) != 0) LOGW("写 shm_udp_img 失败\n");
            if (udp_hdr_to_lane(&last_hdr, &cur_lane) == 0) {
                if (shm_write_lane(&cur_lane) != 0) LOGW("转发 shm_lane 失败\n");
                lane_fresh = 1;   /* 仅在真正收到新帧时置位，供决策做超龄判断 */
            }
            /* 取出本帧附带的三感知摘要（帧头 reserved[4]）。
             * 返回位掩码标明本帧到底带了哪几项——没带的不置 fresh，决策端会
             * 走"超龄降级"而不是拿旧值硬撑。 */
            const uint32_t aux = udp_hdr_get_aux(&last_hdr, &cur_tl, &cur_lm, &cur_zb);
            tl_fresh = (aux & UDP_AUX_HAS_TL)    ? 1 : 0;
            lm_fresh = (aux & UDP_AUX_HAS_LM)    ? 1 : 0;
            zb_fresh = (aux & UDP_AUX_HAS_ZEBRA) ? 1 : 0;
        }

        /* 重组超时：丢弃残帧（有包进来就不算超时，避免正常收块期间误清） */
        if (any_pkt) last_pkt_us = now;
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

        /* ---- 4. 决策（车道来自 UDP 转发，行人来自本地检测） ----
         * 只在“收到新的一帧”时才把车道传给决策；否则传 NULL，由 decision
         * 内部缓存 + 超龄机制（DEC_LANE_MAX_AGE_MS）决定是否沿用上一帧结果。
         * 旧实现每周期都读 sticky 的 shm_lane 并恒传非 NULL，导致“超龄降级”
         * 永不生效——M 端断流后 S 端仍会按最后一条旧车道继续行驶。 */
        LaneResult *lp = lane_fresh ? &cur_lane : NULL;

        /* 三感知同 lane 语义：只在"本帧携带"时传入，decision 内部缓存 + 超龄降级 */
        TrafficLightResult *tlp = tl_fresh ? &cur_tl : NULL;
        LaneMarkResult     *lmp = lm_fresh ? &cur_lm : NULL;
        ZebraResult        *zbp = zb_fresh ? &cur_zb : NULL;

        PersonState *p_in = (pstate.version == PERSON_VERSION) ? &pstate : NULL;
        if (decision_step(&dec, lp, tlp, lmp, zbp, p_in, now, &cmd) != 0)
            LOGW("决策失败\n");
        lane_fresh = 0;                       /* 复位，等待下一帧 */
        tl_fresh = lm_fresh = zb_fresh = 0;

        /* ---- 4b. 按键模拟注入（赛题三"通过按键模拟…"，见 render_lcd.h）----
         * 在显示窗口上按键即可覆盖自动决策，用于现场演示：
         *   ←/→ 切到左/右车道   ↑ 行驶   空格 急停
         *   c   强制变道（当该侧是实线时触发报警）   a 交还自动 */
        if (lcd) {
            switch (render_lcd_poll_key(lcd)) {
                case RENDER_KEY_LANE_LEFT:
                    man_on = 1; man_cmd = CMD_LEFT;  LOGI("按键：模拟左车道/左转\n"); break;
                case RENDER_KEY_LANE_RIGHT:
                    man_on = 1; man_cmd = CMD_RIGHT; LOGI("按键：模拟右车道/右转\n"); break;
                case RENDER_KEY_FORWARD:
                    man_on = 1; man_cmd = CMD_GO;    LOGI("按键：模拟行驶\n"); break;
                case RENDER_KEY_STOP:
                    man_on = 1; man_cmd = CMD_STOP;  LOGI("按键：模拟停止\n"); break;
                case RENDER_KEY_LANE_CHANGE:
                    man_on = 1; man_cmd = CMD_LEFT; force_lane_change = 1;
                    LOGI("按键：强制变道（该侧为实线时会报警）\n"); break;
                case RENDER_KEY_AUTO:
                    man_on = 0; man_cmd = CMD_NONE; force_lane_change = 0;
                    LOGI("按键：交还自动决策\n"); break;
                default: break;
            }
        }
        if (man_on && man_cmd != CMD_NONE) {
            cmd.command    = man_cmd;             /* 人工优先于自动决策 */
            cmd.enable     = 1;
            cmd.priority   = CMD_PRI_EMERGENCY;
            cmd.confidence = 100;
        }

        if (cmd.command != CMD_NONE) {
            if (shm_write_cmd(&cmd) != 0) LOGW("写 shm_cmd 失败\n");
        }

        /* ---- 5. LCD 渲染（按需：本轮重组出完整帧才重绘，首轮除外） ----
         * 一次渲染 = memset 2.46MB + 61 万像素 RGB565→BGRX 转换 + XPutImage
         * 2.46MB，实测把单轮周期拉到 ≈171ms；而 M 端 5 帧/s 时多数轮次根本
         * 收不到完整帧，无条件重绘纯属浪费，还会挤占下一轮的收包窗口。
         * 决策不需要每帧落地（状态机用最新帧即可），跳帧只影响画面流畅度。 */

        /* 报警判定（赛题三测评的两个报警场景）：
         *   ① 实线变道：正在变道/压线（或按键"强制变道"），且该侧车道线是实线；
         *   ② 红灯闯行：红灯点亮且置信度足够，但输出命令仍是行驶类（含按键强制行驶）。
         * 数据来自 A 端感知（随帧头摘要送达），也可由按键注入直接复现。 */
        int alarm = 0;
        {
            const int lm_ok    = (cur_lm.version == LANEMARK_VERSION);
            const int changing = lm_ok && (cur_lm.lane_change || cur_lm.crossing);
            int side_left  = lm_ok && (cur_lm.crossing_left  ||
                                       (cur_lm.ego_offset_px < 0));
            int side_right = lm_ok && (cur_lm.crossing_right ||
                                       (cur_lm.ego_offset_px > 0));

            /* 人工"强制变道"未伴随实际位移时，用注入方向判断所压侧 */
            if (force_lane_change && !side_left && !side_right) {
                side_left  = (man_cmd == CMD_LEFT);
                side_right = (man_cmd == CMD_RIGHT);
            }

            if (lm_ok && (changing || force_lane_change) &&
                ((side_left  && cur_lm.left_type  == LANE_MARK_SOLID) ||
                 (side_right && cur_lm.right_type == LANE_MARK_SOLID)))
                alarm = 1;

            if (cur_tl.version == TL_VERSION && cur_tl.detected &&
                cur_tl.state == TL_RED && cur_tl.confidence >= DEC_BRAKE_CONF_MIN &&
                (cmd.command == CMD_GO || cmd.command == CMD_LEFT ||
                 cmd.command == CMD_RIGHT))
                alarm = 1;
        }

        if (lcd && (any_frame || frames_lcd == 0)) {
            if (shm_read_udp_img(s_remote565, IMG_FRAME_BYTES) != 0 && frames_recv > 0)
                LOGW("读 shm_udp_img 失败\n");
            /* 先登记叠加层状态，再绘制（render_lcd_draw 内部上屏前应用）：
             * 箭头指示行驶行为 + 红绿灯三色指示灯 + 报警红框。 */
            render_lcd_overlay(lcd, (int)cmd.command, (int)cur_tl.state,
                               alarm, frames_lcd);
            if (render_lcd_draw(lcd,
                                local_ok ? s_local565 : NULL,
                                frames_recv ? s_remote565 : NULL,
                                mode, frames_recv) == 0)
                frames_lcd++;
        }

        /* ---- 统计（每 2s） ----
         * 分层打印，便于一眼定位故障层：
         *   socket收包=0                       → 包没进进程（网络/IP/端口）
         *   socket收包增长但 重组接受块 不涨    → 丢块（内核溢出/校验/乱序）
         *   接受块增长但 收帧=0                → 块收不齐（丢包导致）→ 看内核溢出计数
         *   收帧增长                           → 链路正常 */
        if (now - last_stat_us >= 2000000u) {
            uint64_t dt_us = now - last_stat_us;
            last_stat_us = now;

            udp_recv_stats_t st;
            memset(&st, 0, sizeof(st));
            udp_receiver_stats(recv, &st);

            /* 消费速率 = 窗口内取出的包数 / 窗口时长。
             * M 端 5 帧/s × 440 包 ≈ 2200 包/s，所以：
             *   速率只有几十包/s        → 瓶颈在"消费能力"（渲染/打印拖慢主循环）
             *   速率已接近 2200 但仍收帧=0 → 丢包发生在更早的层，即网卡 RX ring /
             *                              softnet backlog（socket 缓冲之前），
             *                              要看 ip -s link、/proc/net/softnet_stat */
            double pkt_rate = dt_us ? (double)win_pkts * 1000000.0 / (double)dt_us : 0.0;
            /* 轮周期用"独立轮数计数器"算，不能用 LCD 帧数反推：渲染改成按需触发后，
             * LCD 帧数不再等于轮数，拿它相除得到的是"渲染间隔"而不是主循环周期。 */
            double loop_ms = win_loops ? ((double)dt_us / 1000.0) / (double)win_loops : 0.0;
            uint32_t lcd_delta = frames_lcd - (uint32_t)win_lcd_seen;
            win_lcd_seen = frames_lcd;

            LOGI("收帧=%u LCD帧=%u(+%u) | socket收包=%llu 重组接受块=%u\n",
                 frames_recv, frames_lcd, lcd_delta,
                 (unsigned long long)st.pkts, reass.stat_block_ok);
            LOGI("  消费速率=%.0f 包/s（M端产出≈2200）主循环周期≈%.1fms\n",
                 pkt_rate, loop_ms);
            LOGI("  丢块[校验=%u 重复=%u 无帧头=%u 旧帧=%u 异常=%u 帧头失败=%u] "
                 "内核增量[队列溢出=+%llu InErrors=+%llu InDatagrams=+%llu] rcvbuf=%d\n",
                 reass.stat_drop_checksum, reass.stat_drop_dup, reass.stat_drop_nohdr,
                 reass.stat_drop_fid, reass.stat_drop_other, reass.stat_begin_fail,
                 (unsigned long long)(st.kern_rcvbuferr  - kern_rcvbuferr_base),
                 (unsigned long long)(st.kern_inerr      - kern_inerr_base),
                 (unsigned long long)(st.kern_indatagrams- kern_indatagram_base),
                 st.rcvbuf_eff);
            LOGI("  最近命令=%d enable=%u pri=%u conf=%u 报警=%d | 感知[红绿灯=%d(检测%d 置信%u) "
                 "虚实线[L=%d R=%d 压线%d 变道%d] 斑马线=%d] %s\n",
                 (int)cmd.command, (unsigned)cmd.enable,
                 (unsigned)cmd.priority, (unsigned)cmd.confidence, alarm,
                 (int)cur_tl.state, (int)cur_tl.detected, (unsigned)cur_tl.confidence,
                 (int)cur_lm.left_type, (int)cur_lm.right_type,
                 (int)cur_lm.crossing, (int)cur_lm.lane_change,
                 (int)cur_zb.detected,
                 man_on ? "(人工按键中，按 a 交还自动)" : "");
            win_pkts = 0;
            win_loops = 0;
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
