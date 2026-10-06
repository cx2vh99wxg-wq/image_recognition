#ifndef _RKNN_YOLOV5_DEMO_PREPROCESS_H_  
#define _RKNN_YOLOV5_DEMO_PREPROCESS_H_

#include <stdio.h>
#include "postprocess.h"

// 简化的预处理函数，不依赖OpenCV和RGA
void letterbox_simple(const uint8_t* src_rgb, uint8_t* dst_rgb, 
                     int src_w, int src_h, int dst_w, int dst_h, 
                     BOX_RECT* pads, float* scale);

#endif //_RKNN_YOLOV5_DEMO_PREPROCESS_H_