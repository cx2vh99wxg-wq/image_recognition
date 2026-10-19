#ifndef _RKNN_YOLOV5_DEMO_PREPROCESS_H_  
#define _RKNN_YOLOV5_DEMO_PREPROCESS_H_

#include <stdio.h>
#include "postprocess.h"

/* 将 RGB888 输入按比例缩放并以灰色居中补边；pads 返回四周填充像素数，
 * scale 返回实际缩放比例。调用方需提供容纳 dst_w×dst_h×3 字节的缓冲区。 */
void letterbox_simple(const uint8_t* src_rgb, uint8_t* dst_rgb, 
                     int src_w, int src_h, int dst_w, int dst_h, 
                     BOX_RECT* pads, float* scale);

#endif /* _RKNN_YOLOV5_DEMO_PREPROCESS_H_ */