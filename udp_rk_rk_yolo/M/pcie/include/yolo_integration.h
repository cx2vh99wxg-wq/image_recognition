#ifndef _YOLO_INTEGRATION_H_
#define _YOLO_INTEGRATION_H_

#include <stdint.h>
#include "rknn_api.h"

#ifdef __cplusplus
extern "C" {
#endif

// YOLOPv2模型相关配置（车道线和可行驶区域检测）
#define YOLOPV2_MODEL_INPUT_WIDTH 640
#define YOLOPV2_MODEL_INPUT_HEIGHT 480

// 模型路径通过编译时宏定义传入 (相对于build目录)
#ifndef YOLOPV2_MODEL_PATH
    #define YOLOPV2_MODEL_PATH "model/yolopv2_Nx3x480x640_rk3568.rknn"
#endif

// YOLOPv2引擎结构体（支持车道线和可行驶区域）
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

    // 输出数据缓冲区（用于保存检测结果）
    float* lane_line_buffer;      // 车道线数据缓冲区
    float* drivable_area_buffer;  // 可行驶区域数据缓冲区
} YOLOEngine;

// YOLOPv2检测结果结构（车道线和可行驶区域）
typedef struct {
    float* lane_line_data;      // 车道线数据 (1, 1, 480, 640)
    float* drivable_area_data;  // 可行驶区域数据 (1, 2, 480, 640)
    int width;
    int height;
} yolopv2_result_t;

// YOLO引擎函数
int yolo_engine_init(YOLOEngine* engine, const char* model_path, const char* labels_path);
int yolo_engine_detect(YOLOEngine* engine, const uint8_t* rgb_data, int width, int height,
                      yolopv2_result_t* results);
void yolo_engine_cleanup(YOLOEngine* engine);

// 绘制车道线和可行驶区域到图像上
void draw_lane_and_drivable(uint8_t* rgb_data, int width, int height,
                           const yolopv2_result_t* results);

// RGB565转RGB888（针对PCIE数据流）
void rgb565_to_rgb888_yolo(const uint16_t* rgb565_data, uint8_t* rgb888_data,
                          int width, int height);

#ifdef __cplusplus
}
#endif

#endif // _YOLO_INTEGRATION_H_