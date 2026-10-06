/*
 * person_detect.h — S 端行人检测（【人员 B】，YOLOv5s / RKNN）
 *
 * 输入：S 端本地 PCIe 图（RGB888）或 UDP 远端图（视部署，v1 用本地图）
 * 输出：PersonState（写 shm_person 0x1234567D，B 决策 + C 安全兜底可读）
 *
 * 实现策略：
 *  - 板端（__linux__ 且非 USE_STUB）：RKNN 推理 YOLOv5s，取 class=person 最大框；
 *  - 本机 / USE_STUB：桩，返回"无行人"（PERSON_NONE），保证纯逻辑可单测、可联调。
 */
#ifndef PERSON_DETECT_H
#define PERSON_DETECT_H

#include <stdint.h>
#include "driving_types.h"
#include "driving_config.h"   /* PersonState */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct person_detect_ctx person_detect_ctx_t;

/* model_path 可为 NULL（桩模式忽略）。成功 0，失败 -1。 */
int  person_detect_init(person_detect_ctx_t **ctx, const char *model_path,
                        int img_w, int img_h);

/* 输入 RGB888 图，输出 PersonState。桩模式恒 PERSON_NONE。 */
int  person_detect_run(person_detect_ctx_t *ctx, const uint8_t *rgb888,
                       uint32_t frame_id, uint64_t now_us, PersonState *out);

void person_detect_deinit(person_detect_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* PERSON_DETECT_H */
