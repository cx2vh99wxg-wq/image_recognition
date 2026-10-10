#ifndef RKNN_VISION_H
#define RKNN_VISION_H
#include "rknn_api.h"
#include "yolo_decode.h"
typedef struct {
    rknn_context ctx;
    rknn_tensor_attr input, output[5];
    int width, height, count, multitask, logits;
    int heads[3], drive, lane;
    uint8_t *pixels;
    float anchors[18];
} rknn_vision_t;
/* Profiles: repository YOLOv5 (probabilities), YOLOPv2 (raw detection logits).
 * Output shapes/formats are checked at runtime; incompatible exports fail closed. */
int rknn_vision_init(rknn_vision_t *m, const char *path, int multitask);
int rknn_vision_run(rknn_vision_t *m, const uint8_t *rgb, int w, int h,
                    int class_filter, yolo_boxes_t *boxes, float *lane, float *drive);
void rknn_vision_close(rknn_vision_t *m);
#endif
