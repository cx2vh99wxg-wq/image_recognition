/*
 * main_perception.c — 感知进程入口（【人员 A · 感知】，运行于 M 端 RK3568）
 *
 * 流程：初始化 → 采集→转换→分割→判定 → 把 LaneResult 写 shm_lane、
 * 把 640x480 RGB565 写 shm_pcie_img（交给 B 做决策 / LCD 显示）。
 * 不在此做 UDP、X11 渲染、FSPI/电机——那些分别属于 B / C。
 */
#include "perception_api.h"
#include "pcie_capture.h"
#include "shm_ipc.h"
#include "driving_config.h"

#define LOG_TAG "PERCEPTION"   /* 感知模块日志前缀 */
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>

static volatile int g_keep_running = 1;

static void on_signal(int sig)
{
    (void)sig;
    g_keep_running = 0;
}

/* 默认模型路径：相对路径依赖运行目录；上板建议用 --model 传绝对路径 */
#define DEFAULT_MODEL_PATH "model/yolopv2_Nx3x480x640_rk3568.rknn"

/* 检查模型文件是否存在（尽早暴露路径错误） */
static int model_file_exists(const char *path)
{
    if (!path || !path[0]) return 0;
    FILE *fp = fopen(path, "rb");
    if (!fp) return 0;
    fclose(fp);
    return 1;
}

int main(int argc, char **argv)
{
    perception_cfg_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.img_w = IMG_WIDTH;
    cfg.img_h = IMG_HEIGHT;
    cfg.lead_px = PCIE_LEAD_PIXELS;
    cfg.model_path = DEFAULT_MODEL_PATH;
    cfg.use_stub = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--stub") == 0) cfg.use_stub = 1;
        else if (strcmp(argv[i], "--model") == 0 && i + 1 < argc) cfg.model_path = argv[++i];
    }

    if (!cfg.use_stub && !model_file_exists(cfg.model_path)) {
        LOGE("模型文件不存在: %s\n", cfg.model_path);
        LOGE("请在模型所在目录启动，或使用 --model <绝对路径> 指定\n");
        return -1;
    }

    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    LOGI("感知进程启动 (stub=%d)\n", cfg.use_stub);

    /* 创建 A 写出的共享内存（主结果 + 图像 + 10-05 新增三感知） */
    void *lane_ptr = NULL, *img_ptr = NULL;
    void *tl_ptr = NULL, *zebra_ptr = NULL, *lm_ptr = NULL;
    if (shm_create("lane", SHM_KEY_LANE, SHM_LANE_SIZE, &lane_ptr) != 0) {
        LOGE("创建 shm_lane 失败\n");
        return -1;
    }
    if (shm_create("pcie_img", SHM_KEY_PCIE_IMG, SHM_IMG_SIZE, &img_ptr) != 0) {
        LOGE("创建 shm_pcie_img 失败\n");
        shm_close(lane_ptr, SHM_LANE_SIZE);
        return -1;
    }
    if (shm_create("traffic_light", SHM_KEY_TRAFFIC_LIGHT, SHM_TL_SIZE, &tl_ptr) != 0) {
        LOGE("创建 shm_traffic_light 失败\n");
        shm_close(lane_ptr, SHM_LANE_SIZE);
        shm_close(img_ptr, SHM_IMG_SIZE);
        return -1;
    }
    if (shm_create("zebra", SHM_KEY_ZEBRA, SHM_ZEBRA_SIZE, &zebra_ptr) != 0) {
        LOGE("创建 shm_zebra 失败\n");
        shm_close(lane_ptr, SHM_LANE_SIZE);
        shm_close(img_ptr, SHM_IMG_SIZE);
        shm_close(tl_ptr, SHM_TL_SIZE);
        return -1;
    }
    if (shm_create("lane_mark", SHM_KEY_LANE_MARK, SHM_LANE_MARK_SIZE, &lm_ptr) != 0) {
        LOGE("创建 shm_lane_mark 失败\n");
        shm_close(lane_ptr, SHM_LANE_SIZE);
        shm_close(img_ptr, SHM_IMG_SIZE);
        shm_close(tl_ptr, SHM_TL_SIZE);
        shm_close(zebra_ptr, SHM_ZEBRA_SIZE);
        return -1;
    }

    perception_ctx_t ctx;
    if (perception_init(&ctx, &cfg) != 0) {
        LOGE("感知流水线初始化失败\n");
        shm_close(lane_ptr, SHM_LANE_SIZE);
        shm_close(img_ptr, SHM_IMG_SIZE);
        shm_close(tl_ptr, SHM_TL_SIZE);
        shm_close(zebra_ptr, SHM_ZEBRA_SIZE);
        shm_close(lm_ptr, SHM_LANE_MARK_SIZE);
        return -1;
    }

    /* 复用一块 RGB565 缓冲用于写图像共享内存 */
    uint8_t *img565 = (uint8_t *)malloc(IMG_FRAME_BYTES);
    if (!img565) {
        LOGE("分配图像缓冲失败\n");
        perception_deinit(&ctx);
        shm_close(lane_ptr, SHM_LANE_SIZE);
        shm_close(img_ptr, SHM_IMG_SIZE);
        shm_close(tl_ptr, SHM_TL_SIZE);
        shm_close(zebra_ptr, SHM_ZEBRA_SIZE);
        shm_close(lm_ptr, SHM_LANE_MARK_SIZE);
        return -1;
    }

    LaneResult result;
    uint32_t fps_cnt = 0;
    while (g_keep_running) {
        if (perception_step(&ctx, &result) != 0) {
            LOGW("感知一帧失败，重试\n");
            continue;
        }

        if (shm_write_lane(&result) != 0) LOGW("写 shm_lane 失败\n");

        /* 桩模式同样写入安全默认值（reset 结果），避免 B 读到上一进程残留脏数据 */
        const LaneMarkResult *lm = perception_lane_marks(&ctx);
        const ZebraResult    *zb = perception_zebra(&ctx);
        const TrafficLightResult *tl = perception_traffic_light(&ctx);
        if (lm) { if (shm_write_lane_mark(lm) != 0) LOGW("写 shm_lane_mark 失败\n"); }
        if (zb) { if (shm_write_zebra(zb) != 0) LOGW("写 shm_zebra 失败\n"); }
        if (tl) { if (shm_write_traffic_light(tl) != 0) LOGW("写 shm_traffic_light 失败\n"); }

        /* 桩模式下无真实图像，不写 pcie_img（避免 B 端显示未初始化花屏） */
        if (!cfg.use_stub) {
            rgb888_to_rgb565_pcie(ctx.rgb888, (uint16_t *)img565,
                                  (size_t)ctx.cfg.img_w * ctx.cfg.img_h);
            if (shm_write_pcie_img(img565, IMG_FRAME_BYTES) != 0)
                LOGW("写 shm_pcie_img 失败\n");
        }

        if ((++fps_cnt % 200) == 0) {
            const char *tl_str = "未知";
            if (tl) {
                if (tl->state == TL_RED) tl_str = "红灯";
                else if (tl->state == TL_YELLOW) tl_str = "黄灯";
                else if (tl->state == TL_GREEN) tl_str = "绿灯";
                else tl_str = "无";
            }
            LOGI("帧 %u: dir=%d off=%d conf=%u px=%u | 线 L=%d R=%d 压线=%u 实线变道=%u | 斑马线=%u | 红绿灯=%s conf=%u\n",
                 result.frame_id, result.direction, result.curve_offset,
                 result.confidence, result.lane_pixel_cnt,
                 lm ? (int)lm->left_type : -1,
                 lm ? (int)lm->right_type : -1,
                 lm ? (unsigned)lm->crossing : 0u,
                 lm ? (unsigned)lane_mark_is_solid_cross(lm) : 0u,
                 zb ? zb->detected : 0u,
                 tl_str,
                 tl ? tl->confidence : 0u);
        }
    }

    LOGI("感知进程退出\n");
    free(img565);
    perception_deinit(&ctx);
    shm_close(lane_ptr, SHM_LANE_SIZE);
    shm_close(img_ptr, SHM_IMG_SIZE);
    shm_close(tl_ptr, SHM_TL_SIZE);
    shm_close(zebra_ptr, SHM_ZEBRA_SIZE);
    shm_close(lm_ptr, SHM_LANE_MARK_SIZE);
    return 0;
}
