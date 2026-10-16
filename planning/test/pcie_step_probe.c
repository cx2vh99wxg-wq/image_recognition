/*
 * pcie_step_probe.c — PCIe 采集链路「单步最小化」诊断工具（【人员 B】· 上板专用）
 *
 * 背景：S 板「FPGA → 板内 PCIe → RK3568」DMA 读数通路此前从未被验证过。
 *   实测发现：一旦某个动作触发 FPGA 的 DMA 传输，整机可能瞬时冻结（连 SSH
 *   和鼠标都无响应，内核日志来不及写盘）。驱动反汇编确认：
 *     · MAP 命令      —— 只分配缓冲、算命令字，**不碰硬件**；
 *     · DMA_WR 命令   —— 写 0x100/0x110/0x120 三个寄存器，**触发一次传输**；
 *                        （0x110 的写法 = tx_restart，见 FPGA 侧
 *                         ips2l_pcie_dma_controller.v:549）
 *     · RD_KRNL 命令  —— 纯内存拷贝，**不碰硬件、不等待**；
 *     · 驱动无中断处理、无循环、无等待。
 *   ⇒ 全部风险集中在「DMA_WR 触发」这一下，因此需要一步一停地定位。
 *
 * 本工具的设计要点：
 *   · 每次运行只做**一个**动作（步骤号参数化），一步一停；
 *   · 每个动作前后把标记写入磁盘日志并 fsync —— 即使下一步把整机冻死、
 *     断电重启，磁盘上仍保留最后一行记录：看 pcie_step.log 的最后一行，
 *     就知道冻在**哪一个 ioctl** 里；
 *   · 每步结束顺带抓一段 dmesg（驱动自带调试打印：缓冲地址、参数等）。
 *
 * 步骤（每次只跑一个，从 0 开始按序推进；全部通过说明链路已通）：
 *   0  打开 /dev/pango_pci_driver（不碰硬件）
 *   1  +MAP：分配 DMA 缓冲、计算命令字（不碰硬件；dmesg 会打印缓冲地址）
 *   2  +触发 1 次传输（默认 1520 字节 = 380 dword；--len 1 表示 4 字节）
 *   3  +连续触发 2 次（背靠背、间隔 0ms）—— 测「上一次传输未完成时再写寄存器」
 *   4  +触发 1 次 → 等 --settle-ms（默认 200ms）→ 读回缓冲（RD_KRNL）
 *   5  +整帧：480 行 × (触发+读回)（等价 stereo_local_preview 的第一帧）
 *   6  +多帧（--frames N，默认 5）
 *
 * 判读（重点）：
 *   · 日志最后一行若是「>> ...」，说明冻结发生在这一行标注的动作里；
 *   · 哪个步骤冻结，就回答哪个问题：
 *       2 --len 1 冻  → 哪怕 4 字节传输也会冻 ⇒ 传输机制本身有问题（FPGA/链路）
 *       2 冻（1520B）→ 与传输规模相关（但 1520B 与 FPGA 侧长度定义精确一致）
 *       3 冻         → 「上一次传输未完成时再写寄存器」出问题（握手/背压）
 *       5 冻         → 连续高频触发/累积效应
 *       全部通过      → 主机侧与链路层全部就绪，回到真实采集流程验证
 *
 * 注意：
 *   · 必须 root 运行（设备节点 crw------- root 权限）；
 *   · 本工具**故意不做 UNMAP**：避免在（可能未完成的）传输期间释放缓冲，
 *     缓冲泄漏到重启为止——上板诊断工具，可接受；
 *   · 日志默认**追加**到 ./pcie_step.log（磁盘文件，重启不丢）。
 */
#include "pcie_capture.h"

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>

static FILE *g_log = NULL;   /* 磁盘日志；每条记录都 fsync，防整机冻结丢日志 */

static void mark(const char *fmt, ...)
{
    va_list ap;
    /* stdout：终端实时可见 */
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    fflush(stdout);
    /* 日志文件：写入后立刻 fsync —— 这是"冻结后还能查案"的关键 */
    if (g_log) {
        va_start(ap, fmt);
        vfprintf(g_log, fmt, ap);
        va_end(ap);
        fflush(g_log);
        fsync(fileno(g_log));
    }
}

static double mono_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1e6;
}

/* 把 dmesg 尾部追加到日志（抓驱动调试打印：MAP 的缓冲地址、命令字等） */
static void dmesg_tail_to_log(void)
{
    if (!g_log) return;
    FILE *p = popen("dmesg 2>/dev/null | tail -40", "r");
    if (!p) return;
    mark("---- dmesg tail（本步产生的内核打印）----\n");
    char line[512];
    while (fgets(line, sizeof(line), p)) {
        fputs(line, g_log);
    }
    fflush(g_log);
    fsync(fileno(g_log));
    pclose(p);
    mark("---- dmesg end ----\n");
}

/* 触发一次传输：ioctl(PANGO_IO_DMA_WR)。前后各写一行标记（fsync）。 */
static int do_trigger(frame_grabber_t *g, const char *tag, int idx)
{
    mark("  >> %s #%d：即将 ioctl(PANGO_IO_DMA_WR) 触发传输（%u 字节）\n",
         tag, idx, g->dma.current_len * 4u);
    errno = 0;
    int r = ioctl(g->fd, PANGO_IO_DMA_WR, &g->dma);
    if (r != 0)
        mark("  << %s #%d：返回 %d（errno=%d %s）\n", tag, idx, r, errno, strerror(errno));
    else
        mark("  << %s #%d：返回 0（命令已下发）\n", tag, idx);
    return r;
}

/* 读回内核缓冲：ioctl(PANGO_IO_RD_KRNL)，并打印前 32 字节。 */
static int do_read_krnl(frame_grabber_t *g, const char *tag)
{
    mark("  >> %s：即将 ioctl(PANGO_IO_RD_KRNL) 读回缓冲\n", tag);
    errno = 0;
    int r = ioctl(g->fd, PANGO_IO_RD_KRNL, &g->dma);
    if (r != 0) {
        mark("  << %s：返回 %d（errno=%d %s）\n", tag, r, errno, strerror(errno));
        return r;
    }
    char hex[80];
    int n = 0;
    for (int i = 0; i < 32; i++)
        n += snprintf(hex + n, sizeof(hex) - (size_t)n, "%02x", g->dma.read_buf[i]);
    mark("  << %s：返回 0；read_buf 前 32 字节 = %s\n", tag, hex);
    return 0;
}

/* 统计前导像素（与 stereo_local_preview 的口径一致） */
static void count_leads(const pango_dma_xfer *dma, int lead_px,
                        uint64_t *white, uint64_t *nonzero)
{
    for (int x = 0; x < lead_px; x++) {
        uint8_t lo = dma->read_buf[2 * x];
        uint8_t hi = dma->read_buf[2 * x + 1];
        *nonzero += (lo != 0 || hi != 0);
        *white   += (lo == 0xff && hi == 0xff);
    }
}

/* 一整帧：每行「触发 + 读回 + 拷出」。per_row_marks=1 时每行打 3 条标记。 */
static int run_frame(frame_grabber_t *g, uint8_t *frame_out, int per_row_marks,
                     int rows, uint64_t *white, uint64_t *nonzero)
{
    for (int y = 0; y < rows; y++) {
        /* 与真实采集一致：设行偏移（注：驱动实际忽略用户侧偏移，保持一致写法） */
        g->dma.offset_addr = (uint32_t)(y * (g->lead_px + g->img_w) * 2);
        if (per_row_marks) mark("  [row %d/%d] >> trig\n", y, rows);
        errno = 0;
        if (ioctl(g->fd, PANGO_IO_DMA_WR, &g->dma) != 0) {
            mark("  !! row %d: DMA_WR 失败（errno=%d %s）\n", y, errno, strerror(errno));
            return -1;
        }
        if (per_row_marks) mark("  [row %d/%d] >> read\n", y, rows);
        errno = 0;
        if (ioctl(g->fd, PANGO_IO_RD_KRNL, &g->dma) != 0) {
            mark("  !! row %d: RD_KRNL 失败（errno=%d %s）\n", y, errno, strerror(errno));
            return -1;
        }
        count_leads(&g->dma, g->lead_px, white, nonzero);
        if (frame_out) {
            memcpy(frame_out + (size_t)y * (size_t)g->img_w * 2u,
                   g->dma.read_buf + (size_t)g->lead_px * 2u,
                   (size_t)g->img_w * 2u);
        }
        if (per_row_marks && (y % 20 == 19 || y == rows - 1))
            mark("  [row %d/%d] ok\n", y + 1, rows);
    }
    return 0;
}

static void usage(const char *name)
{
    fprintf(stderr,
        "用法: %s <step> [选项]\n"
        "  step: 0=open 1=+MAP 2=+触发 3=+连续两次触发 4=+触发并读回\n"
        "        5=+整帧 6=+多帧\n"
        "  选项: --len N(默认380 dword) --count K(step2,默认1) --gap-ms G(默认0)\n"
        "        --settle-ms S(默认200) --rows R(默认480) --frames F(默认5)\n"
        "        --log PATH(默认 ./pcie_step.log)\n", name);
}

int main(int argc, char **argv)
{
    if (argc < 2) { usage(argv[0]); return 2; }

    int step = atoi(argv[1]);
    if (step < 0 || step > 6) { usage(argv[0]); return 2; }

    unsigned long len      = 380;          /* 每行传输 dword 数（380*4=1520 字节） */
    int           count    = 1;            /* step2 触发次数 */
    int           gap_ms   = 0;            /* step2 多次触发间隔 */
    int           settle_ms = 200;         /* step4 触发后等待、再读回 */
    int           rows     = 480;
    int           frames   = 5;
    const char   *logpath  = "pcie_step.log";

    for (int i = 2; i < argc; i++) {
        if      (!strcmp(argv[i], "--len")    && i + 1 < argc) len       = strtoul(argv[++i], NULL, 10);
        else if (!strcmp(argv[i], "--count")  && i + 1 < argc) count     = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--gap-ms") && i + 1 < argc) gap_ms    = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--settle-ms") && i + 1 < argc) settle_ms = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--rows")   && i + 1 < argc) rows      = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--frames") && i + 1 < argc) frames    = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--log")    && i + 1 < argc) logpath   = argv[++i];
        else { usage(argv[0]); return 2; }
    }
    /* 传输长度必须能放进 FPGA 命令字 10 位字段（dword 数-1 <= 0x3FF） */
    if (len < 1 || len > 1024) { fprintf(stderr, "--len 越界（1..1024 dword）\n"); return 2; }
    if (count < 1) count = 1;
    if (frames < 1) frames = 1;
    if (rows < 1 || rows > 480) rows = 480;

    g_log = fopen(logpath, "a");
    if (!g_log) { fprintf(stderr, "无法写日志 %s\n", logpath); return 1; }

    time_t now = time(NULL);
    char tbuf[64];
    strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", localtime(&now));
    mark("\n===== RUN step=%d len=%lu(dword=%lu字节) count=%d gap=%dms settle=%dms rows=%d frames=%d @ %s =====\n",
         step, len, len * 4, count, gap_ms, settle_ms, rows, frames, tbuf);

    frame_grabber_t g;
    if (pcie_capture_open(&g, CAP_IMG_W, CAP_IMG_H, CAP_LEAD_PIX) != 0) {
        mark("!! open %s 失败\n", PANGO_PCIE_DEV);
        fclose(g_log);
        return 1;
    }
    mark("  [ok] open %s 成功\n", PANGO_PCIE_DEV);
    if (step == 0) goto done;

    /* ---- MAP（不碰硬件；dmesg 里会留下缓冲地址打印） ---- */
    g.dma.current_len = (uint32_t)len;
    g.dma.offset_addr = 0;
    g.dma.cmd = 0;
    memset(g.dma.read_buf, 0, PANGO_DMA_PACKET);
    memset(g.dma.write_buf, 0, PANGO_DMA_PACKET);
    mark("  >> 即将 ioctl(PANGO_IO_MAP)：分配 2×%lu 字节 DMA 缓冲\n", len * 4);
    errno = 0;
    if (ioctl(g.fd, PANGO_IO_MAP, &g.dma) != 0) {
        mark("  << MAP 失败（errno=%d %s）\n", errno, strerror(errno));
        goto done;
    }
    mark("  << MAP 返回 0（缓冲已就绪，尚无硬件动作）\n");
    if (step == 1) goto done;

    /* ---- 步骤 2/3/4：触发系列 ---- */
    if (step == 2 || step == 3) {
        int n = (step == 3) ? 2 : count;
        for (int k = 1; k <= n; k++) {
            char tag[32];
            snprintf(tag, sizeof(tag), "触发");
            if (do_trigger(&g, tag, k) != 0) goto done;
            if (k < n && gap_ms > 0) {
                struct timespec ts = { .tv_sec = gap_ms / 1000,
                                       .tv_nsec = (long)(gap_ms % 1000) * 1000000L };
                nanosleep(&ts, NULL);
            }
        }
        mark("  [ok] 本次共触发 %d 次传输\n", n);
        goto done;
    }

    if (step == 4) {
        if (do_trigger(&g, "触发", 1) != 0) goto done;
        mark("  .. 等待 %d ms 让传输落地\n", settle_ms);
        struct timespec ts = { .tv_sec = settle_ms / 1000,
                               .tv_nsec = (long)(settle_ms % 1000) * 1000000L };
        nanosleep(&ts, NULL);
        if (do_read_krnl(&g, "读回") != 0) goto done;
        mark("  [ok] 触发+读回完成（全 0 可能=传输未完成或数据确为 0）\n");
        goto done;
    }

    /* ---- 步骤 5/6：整帧 / 多帧 ---- */
    {
        uint8_t *frame = malloc((size_t)CAP_IMG_W * CAP_IMG_H * 2u);
        if (!frame) { mark("!! malloc 帧缓冲失败\n"); goto done; }
        int nframes = (step == 5) ? 1 : frames;
        int fail = 0;
        for (int f = 0; f < nframes; f++) {
            uint64_t white = 0, nonzero = 0;
            double t0 = mono_ms();
            mark("  >> 第 %d/%d 帧开始（%d 行 × 触发+读回）\n", f + 1, nframes, rows);
            if (run_frame(&g, frame, f == 0, rows, &white, &nonzero) != 0) { fail = 1; break; }
            double dt = mono_ms() - t0;
            mark("  << 第 %d/%d 帧完成：用时 %.1f ms，前导[白=%llu 非零=%llu]（满值应为 %d）\n",
                 f + 1, nframes, dt,
                 (unsigned long long)white, (unsigned long long)nonzero,
                 CAP_LEAD_PIX * rows);
            if (dt > 0.1) mark("     （≈%.1f 帧/s）\n", 1000.0 / dt);
        }
        free(frame);
        if (!fail) mark("  [ok] 全部帧完成\n");
    }

done:
    dmesg_tail_to_log();
    mark("===== DONE step=%d =====\n", step);
    pcie_capture_close(&g);   /* 故意不 UNMAP：避免在传输未完成时释放缓冲 */
    fclose(g_log);
    return 0;
}
