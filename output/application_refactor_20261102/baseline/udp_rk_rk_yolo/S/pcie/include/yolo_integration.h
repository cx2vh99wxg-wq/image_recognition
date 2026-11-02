#ifndef _YOLO_INTEGRATION_H_
#define _YOLO_INTEGRATION_H_

#include <stdint.h>
#include "rknn_api.h"
#include "postprocess.h"

#ifdef __cplusplus
extern "C" {
#endif

// YOLO模型相关配置
#define YOLO_MODEL_INPUT_SIZE 640

// 检测结果去重配置 (如果未定义则使用默认值)
#ifndef ENABLE_DETECTION_DEDUP
    #define ENABLE_DETECTION_DEDUP 1
#endif

#ifndef DEDUP_IOU_THRESHOLD
    #define DEDUP_IOU_THRESHOLD 0.5f
#endif

// 模型路径通过编译时宏定义传入 (相对于build目录)
#ifndef YOLO_MODEL_PATH
    #define YOLO_MODEL_PATH "../model/RK3566_RK3568/yolov5s-640-640.rknn"
#endif

#ifndef YOLO_LABELS_PATH
    #define YOLO_LABELS_PATH "../model/coco_80_labels_list.txt"
#endif

// YOLO引擎结构体
typedef struct {
    rknn_context ctx;
    rknn_input_output_num io_num;
    rknn_tensor_attr* input_attrs;
    rknn_tensor_attr* output_attrs;
    rknn_input* inputs;
    rknn_output* outputs;
    uint8_t* input_data;
    int model_width;
    int model_height;
    int is_initialized;
} YOLOEngine;

// YOLO引擎函数
int yolo_engine_init(YOLOEngine* engine, const char* model_path, const char* labels_path);
int yolo_engine_detect(YOLOEngine* engine, const uint8_t* rgb_data, int width, int height, 
                      detect_result_group_t* results);
void yolo_engine_cleanup(YOLOEngine* engine);

// 绘制检测结果到图像上
void draw_detection_results(uint8_t* rgb_data, int width, int height, 
                           const detect_result_group_t* results);

// RGB565转RGB888（针对PCIE数据流）
void rgb565_to_rgb888_yolo(const uint16_t* rgb565_data, uint8_t* rgb888_data, 
                          int width, int height);

#ifdef __cplusplus
}
#endif

#endif // _YOLO_INTEGRATION_H_