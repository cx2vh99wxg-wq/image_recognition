#ifndef _RKNN_YOLOV5_DEMO_PREPROCESS_H_
#define _RKNN_YOLOV5_DEMO_PREPROCESS_H_

#include <stdio.h>
#include "postprocess.h"

/* RGB888 按比例缩放后，用灰色居中补齐；pads 输出四边填充量，scale
 * 输出缩放比。调用方提供的目标区须至少容纳 dst_w×dst_h×3 字节。 */
void letterbox_simple(const uint8_t *src_rgb, uint8_t *dst_rgb, int src_w, int src_h, int dst_w,
                      int dst_h, BOX_RECT *pads, float *scale);

#endif /* _RKNN_YOLOV5_DEMO_PREPROCESS_H_：本节对应的标识或作用域。 */