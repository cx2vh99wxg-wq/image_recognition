/* ===========================================================================
 * 文件：main.cpp
 * 归属：人员 A（感知与预处理）· M 端（采集端）程序入口
 * 说明：A 的主循环。职责边界（见分工方案）：
 *   - 采集 640x480 图像（剥离每行 120 前导像素，配置化在 driving_config.h）
 *   - RGB565↔RGB888、letterbox 预处理
 *   - 跑 YOLOPv2 得到车道线分割，计算 LaneResult（真实置信度 + 像素偏移）
 *   - 把 LaneResult 写 shm_lane(0x1234567E)、把原图写 shm_pcie_img(0x12345679)
 *   不属于 A 的：X11 显示、UDP 收发、共享内存底层实现、SPI/电机——分别由 B/C。
 *   本文件只调用 B 提供的 shm_ipc 接口（声明见 common/include/shm_ipc.h）。
 * ======================================================================== */
#include "pcie_capture.h"
#include "yolo_integration.h"
#include "shm_ipc.h"
#include "driving_config.h"
#include "pcie_dma_read_test.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>

/* 是否把带车道线的图存成 PPM（里程碑 M1：能存下一张真实 640x480 图）*/
#define ENABLE_LOCAL_PPM   1

/* 是否启用 X11 本地显示（默认关：显示是 B 的职责；需要时由 B 的渲染模块做）*/
#ifndef ENABLE_RENDERING
#define ENABLE_RENDERING   0
#endif

static volatile int g_running = 1;
static pcie_ctx_t   g_pcie;
static lane_engine_t g_lane;

/* 全局缓冲 */
static uint8_t *g_raw565  = NULL;   /* 原始（含前导）RGB565 */
static uint8_t *g_img565  = NULL;   /* 剥离前导后的 640x480 RGB565 */
static uint8_t *g_img888  = NULL;   /* 模型输入用 RGB888 */

static void on_signal(int sig)
{
    g_running = 0;
    fprintf(stderr, "\n收到信号 %d，安全退出\n", sig);
}

/* ---------------------------------------------------------------------------
 * 采集实现（A 的 PCIe 采集能力，沿用原 DMA 逐行读取原理，套用新接口）
 * ------------------------------------------------------------------------- */
int pcie_capture_open(pcie_ctx_t *ctx)
{
    if (!ctx) return -1;
    memset(ctx, 0, sizeof(*ctx));
    ctx->dev_fd = open(PCIE_DEVICE, O_RDWR);
    if (ctx->dev_fd < 0) {
        fprintf(stderr, "[PCIE] 设备打开失败: %s (%s)\n", PCIE_DEVICE, strerror(errno));
        return -1;
    }
    printf("[PCIE] 设备已打开\n");
    return 0;
}

int pcie_capture_start_stream(pcie_ctx_t *ctx)
{
    if (!ctx || ctx->dev_fd < 0) return -1;
    /* 每行 DMA 长度 = (前导 + 有效) * 2 字节 */
    ((DMA_OPERATION*)ctx->dma_op)->current_len = (PCIE_LINE_PIXELS) * 2 / 4;
    ((DMA_OPERATION*)ctx->dma_op)->offset_addr = 0;
    if (ioctl(ctx->dev_fd, PCI_MAP_ADDR_CMD, ctx->dma_op) != 0) {
        fprintf(stderr, "[PCIE] DMA 地址映射失败: %s\n", strerror(errno));
        return -1;
    }
    if (ioctl(ctx->dev_fd, PCI_DMA_WRITE_CMD, ctx->dma_op) != 0) {
        fprintf(stderr, "[PCIE] DMA 读地址重置失败: %s\n", strerror(errno));
        return -1;
    }
    printf("[PCIE] 视频流采集已启动\n");
    return 0;
}

int pcie_capture_grab(pcie_ctx_t *ctx, uint8_t *out_rgb565, size_t out_bytes)
{
    if (!ctx || ctx->dev_fd < 0 || !out_rgb565) return -1;
    if (out_bytes < IMG_FRAME_BYTES) return -1;

    DMA_OPERATION *dma = (DMA_OPERATION*)ctx->dma_op;
    for (int y = 0; y < IMG_HEIGHT; y++) {
        dma->offset_addr = (uint32_t)(y * PCIE_LINE_PIXELS * 2);
        ioctl(ctx->dev_fd, PCI_DMA_WRITE_CMD, dma);
        ioctl(ctx->dev_fd, PCI_READ_FROM_KERNEL_CMD, dma);
        /* 剥掉本行前导像素，仅拷贝有效 640 宽 */
        memcpy(out_rgb565 + y * IMG_WIDTH * 2,
               dma->data.read_buf + PCIE_LEAD_PIXELS * 2,
               IMG_WIDTH * 2);
    }
    return 0;
}

void pcie_capture_stop_stream(pcie_ctx_t *ctx)
{
    if (ctx && ctx->dev_fd >= 0)
        ioctl(ctx->dev_fd, PCI_UMAP_ADDR_CMD, ctx->dma_op);
}

void pcie_capture_close(pcie_ctx_t *ctx)
{
    if (ctx && ctx->dev_fd >= 0) { close(ctx->dev_fd); ctx->dev_fd = -1; }
}

void strip_leading_pixels(const uint8_t *raw_row, uint8_t *dst_row)
{
    memcpy(dst_row, raw_row + PCIE_LEAD_PIXELS * 2, IMG_WIDTH * 2);
}

void rearrange_row_columns(uint16_t *row, int width, int from, int to)
{
    (void)row; (void)width; (void)from; (void)to; /* 默认空操作 */
}

void rgb565_to_rgb888(const uint16_t *src565, uint8_t *dst888, size_t pixels)
{
    for (size_t i = 0; i < pixels; i++) {
        uint16_t p = src565[i];
        uint8_t r = (uint8_t)((p >> 11) & 0x1F);
        uint8_t g = (uint8_t)((p >> 5)  & 0x3F);
        uint8_t b = (uint8_t)( p        & 0x1F);
        dst888[i*3+0] = (uint8_t)((r << 3) | (r >> 2));
        dst888[i*3+1] = (uint8_t)((g << 2) | (g >> 4));
        dst888[i*3+2] = (uint8_t)((b << 3) | (b >> 2));
    }
}

int save_rgb888_to_ppm(const uint8_t *rgb888, const char *path)
{
    if (!rgb888 || !path) return -1;
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;
    fprintf(fp, "P6\n%d %d\n255\n", IMG_WIDTH, IMG_HEIGHT);
    fwrite(rgb888, 1, (size_t)IMG_WIDTH * IMG_HEIGHT * 3, fp);
    fclose(fp);
    return 0;
}

double timeval_diff_ms(struct timeval a, struct timeval b)
{
    return (a.tv_sec - b.tv_sec) * 1000.0 + (a.tv_usec - b.tv_usec) / 1000.0;
}

int main(void)
{
    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    printf("========================================\n");
    printf(" A · 感知层主程序（M 端）启动\n");
    printf(" 图像 %dx%d，前导像素 %d（剥离）\n", IMG_WIDTH, IMG_HEIGHT, PCIE_LEAD_PIXELS);
    printf("========================================\n");

    /* 1. 分配缓冲（含前导的原始行） */
    g_raw565 = (uint8_t*)malloc(PCIE_FRAME_BYTES);
    g_img565 = (uint8_t*)malloc(IMG_FRAME_BYTES);
    g_img888 = (uint8_t*)malloc((size_t)IMG_WIDTH * IMG_HEIGHT * 3);
    if (!g_raw565 || !g_img565 || !g_img888) {
        fprintf(stderr, "[FATAL] 缓冲分配失败\n");
        return -1;
    }

    /* 2. 打开 PCIe 设备 */
    if (pcie_capture_open(&g_pcie) != 0) return -1;
    if (pcie_capture_start_stream(&g_pcie) != 0) return -1;

    /* 3. 初始化 YOLOPv2 车道线引擎 */
    if (lane_engine_init(&g_lane, YOLO_MODEL_PATH) != 0) {
        fprintf(stderr, "[FATAL] 车道线引擎初始化失败\n");
        return -1;
    }

    /* 4. 创建 A 要写的共享内存段（lane + 原图），最终拼接时链接 B 的 shm_ipc.c */
    void *p_lane = NULL, *p_img = NULL;
    if (shm_create("shm_lane",     SHM_KEY_LANE,     SHM_LANE_SIZE, &p_lane) != 0 ||
        shm_create("shm_pcie_img", SHM_KEY_PCIE_IMG, SHM_IMG_SIZE, &p_img) != 0) {
        fprintf(stderr, "[WARN] 共享内存创建失败（需 B 的 shm_ipc.c 实现）\n");
    }

    printf("=== 感知主循环开始（Ctrl+C 退出）===\n");
    uint32_t frame_id = 0;
    struct timeval t0; gettimeofday(&t0, NULL);

    while (g_running) {
        /* 步骤1：采集一帧（剥离前导 → 640x480 RGB565） */
        if (pcie_capture_grab(&g_pcie, g_img565, IMG_FRAME_BYTES) != 0) {
            fprintf(stderr, "[ERR] 帧采集失败\n");
            break;
        }
        /* 步骤2：转 RGB888 供模型 */
        rgb565_to_rgb888((uint16_t*)g_img565, g_img888, (size_t)IMG_WIDTH * IMG_HEIGHT);

        /* 步骤3：跑车道线模型 → 分割结果 */
        lane_seg_t seg;
        if (lane_engine_run(&g_lane, g_img888, IMG_WIDTH, IMG_HEIGHT, &seg) == 0) {
            /* 步骤4：从分割图计算 LaneResult（真实置信度 + 像素偏移） */
            LaneResult res;
            lane_result_from_seg(&seg, &res, frame_id);
            shm_write_lane(&res);          /* 交给 B（决策）*/
#if ENABLE_LOCAL_PPM
            lane_overlay_draw(g_img888, IMG_WIDTH, IMG_HEIGHT, &seg);
#endif
            /* 调试打印 */
            const char *dir[] = {"STRAIGHT", "LEFT", "RIGHT", "UNKNOWN"};
            printf("[帧%d] 方向=%s 置信度=%u%% 偏移=%dpx 车道线像素=%u\n",
                   frame_id, dir[res.direction], res.confidence,
                   res.curve_offset, res.lane_pixel_cnt);
        }

        /* 步骤5：把原图（RGB565）交给 B/显示 */
        shm_write_pcie_img(g_img565, IMG_FRAME_BYTES);

#if ENABLE_LOCAL_PPM
        if (frame_id % 100 == 0)
            save_rgb888_to_ppm(g_img888, "last_frame_with_lane.ppm");
#endif

        frame_id++;
    }

    /* 清理 */
    printf("\n=== 清理 ===\n");
    pcie_capture_stop_stream(&g_pcie);
    lane_engine_free(&g_lane);
    pcie_capture_close(&g_pcie);
    free(g_raw565); free(g_img565); free(g_img888);
    printf("=== A · 感知层主程序结束 ===\n");
    return 0;
}
