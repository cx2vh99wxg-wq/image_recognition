#include "yolo_integration.h"
#include "shared_memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// 弯道类型定义
typedef enum {
    TURN_STRAIGHT = 0,    // 直道
    TURN_LEFT = 1,        // 左弯道
    TURN_RIGHT = 2        // 右弯道
} TurnType;

// 弯道检测函数：基于绘制的红色车道线进行垂直投影分析（多区域采样+加权计算）
static TurnType detect_turn_direction_from_image(const uint8_t* rgb_data, int width, int height) {
    if (!rgb_data) {
        printf(">>> 图像数据指针为空 <<<\n");
        fflush(stdout);
        return TURN_STRAIGHT;
    }

    // 只分析左上角320x240区域的下半部分
    int roi_width = 320;
    int roi_height = 240;
    int half_start = roi_height / 2;  // 120，从这里开始

    // 确保ROI不超出图像边界
    if (roi_width > width) roi_width = width;
    if (roi_height > height) roi_height = height;

    // 定义8个采样区域（从远到近），每个区域高度8像素
    #define NUM_REGIONS 8
    int region_y[NUM_REGIONS] = {
        half_start + 5,    // Y=125 (最远，权重最大)
        half_start + 15,   // Y=135
        half_start + 25,   // Y=145
        half_start + 35,   // Y=155
        half_start + 50,   // Y=170
        half_start + 65,   // Y=185
        half_start + 80,   // Y=200
        half_start + 95    // Y=215 (最近，权重最小)
    };
    int region_height = 8;  // 每个区域高度8像素

    // 权重：远处的区域权重更大（从2.0到0.5）
    float weights[NUM_REGIONS] = {2.0f, 1.7f, 1.5f, 1.3f, 1.0f, 0.8f, 0.6f, 0.5f};

    // 存储每个区域的投影数组和重心
    int projections[NUM_REGIONS][320];
    float centers[NUM_REGIONS];
    int totals[NUM_REGIONS];

    // 初始化
    for (int r = 0; r < NUM_REGIONS; r++) {
        memset(projections[r], 0, sizeof(int) * 320);
        centers[r] = 0;
        totals[r] = 0;
    }

    // 对每个区域进行垂直投影
    for (int r = 0; r < NUM_REGIONS; r++) {
        int y_start = region_y[r];
        int y_end = region_y[r] + region_height;

        for (int y = y_start; y < y_end && y < height; y++) {
            for (int x = 0; x < roi_width; x++) {
                int idx = (y * width + x) * 3;
                uint8_t b = rgb_data[idx];
                uint8_t g = rgb_data[idx + 1];
                uint8_t r_val = rgb_data[idx + 2];

                // 检测红色像素
                if (r_val > 200 && g < 50 && b < 50) {
                    projections[r][x]++;
                    totals[r]++;
                }
            }
        }
    }

    // 打印每个区域的统计信息
    printf(">>> 多区域采样结果:\n");
    for (int r = 0; r < NUM_REGIONS; r++) {
        printf("    区域%d (Y=%d-%d): %d像素, 权重%.1f\n",
               r, region_y[r], region_y[r] + region_height, totals[r], weights[r]);
    }
    fflush(stdout);

    // 计算每个区域的重心
    int valid_regions = 0;
    for (int r = 0; r < NUM_REGIONS; r++) {
        if (totals[r] > 10) {  // 至少10个像素才有效
            for (int x = 0; x < roi_width; x++) {
                centers[r] += x * projections[r][x];
            }
            centers[r] /= totals[r];
            valid_regions++;
        } else {
            centers[r] = -1;  // 标记为无效
        }
    }

    if (valid_regions < 2) {
        printf(">>> 有效区域不足 (仅%d个) <<<\n", valid_regions);
        fflush(stdout);
        return TURN_STRAIGHT;
    }

    // 使用加权最小二乘法计算趋势
    // 计算加权偏移量：远处区域 - 近处区域，加上权重
    float weighted_shift = 0;
    float total_weight = 0;

    // 找到最远和最近的有效区域
    int far_region = -1, near_region = -1;
    for (int r = 0; r < NUM_REGIONS; r++) {
        if (centers[r] >= 0) {
            if (far_region == -1) far_region = r;
            near_region = r;
        }
    }

    if (far_region != -1 && near_region != -1 && far_region != near_region) {
        // 计算所有相邻区域对的偏移，并加权
        for (int r = 0; r < NUM_REGIONS - 1; r++) {
            if (centers[r] >= 0 && centers[r + 1] >= 0) {
                float local_shift = centers[r] - centers[r + 1];
                weighted_shift += local_shift * weights[r];
                total_weight += weights[r];
            }
        }

        if (total_weight > 0) {
            weighted_shift /= total_weight;
        }
    }

    printf(">>> 最远区域重心: %.1f (Y=%d)\n",
           far_region >= 0 ? centers[far_region] : -1,
           far_region >= 0 ? region_y[far_region] : -1);
    printf(">>> 最近区域重心: %.1f (Y=%d)\n",
           near_region >= 0 ? centers[near_region] : -1,
           near_region >= 0 ? region_y[near_region] : -1);
    printf(">>> 加权偏移量: %.2f <<<\n", weighted_shift);
    fflush(stdout);

    // 动态阈值：根据总像素数调整
    int total_pixels = 0;
    for (int r = 0; r < NUM_REGIONS; r++) {
        total_pixels += totals[r];
    }

    float threshold = 8.0f;  // 基础阈值
    if (total_pixels < 100) {
        threshold = 12.0f;  // 像素少时提高阈值，减少误判
    } else if (total_pixels > 300) {
        threshold = 6.0f;   // 像素多时降低阈值，提高灵敏度
    }

    printf(">>> 总像素: %d, 使用阈值: %.1f <<<\n", total_pixels, threshold);
    fflush(stdout);

    // 判断弯道方向
    if (weighted_shift > threshold) {
        return TURN_RIGHT;
    } else if (weighted_shift < -threshold) {
        return TURN_LEFT;
    }

    return TURN_STRAIGHT;
}

// 预处理函数：将RGB888图像转换为NHWC格式的UINT8数据
static void preprocess_image(const uint8_t* rgb_data, uint8_t* input_data,
                            int src_width, int src_height,
                            int model_width, int model_height) {
    // 简单的resize + RGB转换 + NHWC格式
    float scale_w = (float)src_width / model_width;
    float scale_h = (float)src_height / model_height;

    for (int h = 0; h < model_height; h++) {
        for (int w = 0; w < model_width; w++) {
            int src_x = (int)(w * scale_w);
            int src_y = (int)(h * scale_h);

            if (src_x >= src_width) src_x = src_width - 1;
            if (src_y >= src_height) src_y = src_height - 1;

            // NHWC格式：input_data[h][w][c]
            int dst_idx = (h * model_width + w) * 3;
            int src_idx = (src_y * src_width + src_x) * 3;

            input_data[dst_idx + 0] = rgb_data[src_idx + 0]; // R
            input_data[dst_idx + 1] = rgb_data[src_idx + 1]; // G
            input_data[dst_idx + 2] = rgb_data[src_idx + 2]; // B
        }
    }
}

int yolo_engine_init(YOLOEngine* engine, const char* model_path, const char* labels_path) {
    if (!engine || !model_path) {
        printf("YOLOPv2引擎初始化参数无效\n");
        return -1;
    }

    // labels_path参数保留用于接口兼容性，但YOLOPv2不需要标签文件
    (void)labels_path;

    printf("正在初始化YOLOPv2引擎（车道线和可行驶区域检测）...\n");

    memset(engine, 0, sizeof(YOLOEngine));

    // 加载RKNN模型
    FILE* fp = fopen(model_path, "rb");
    if (!fp) {
        printf("无法打开模型文件: %s\n", model_path);
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    size_t model_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    void* model_data = malloc(model_size);
    if (!model_data) {
        printf("分配模型内存失败\n");
        fclose(fp);
        return -1;
    }

    if (fread(model_data, 1, model_size, fp) != model_size) {
        printf("读取模型文件失败\n");
        free(model_data);
        fclose(fp);
        return -1;
    }
    fclose(fp);

    // 初始化RKNN
    int ret = rknn_init(&engine->ctx, model_data, model_size, 0, NULL);
    free(model_data);

    if (ret < 0) {
        printf("RKNN初始化失败: %d\n", ret);
        return -1;
    }

    // 获取输入输出数量
    ret = rknn_query(engine->ctx, RKNN_QUERY_IN_OUT_NUM, &engine->io_num, sizeof(engine->io_num));
    if (ret < 0) {
        printf("查询输入输出数量失败: %d\n", ret);
        rknn_destroy(engine->ctx);
        return -1;
    }

    printf("模型输入数量: %d, 输出数量: %d\n", engine->io_num.n_input, engine->io_num.n_output);

    // 分配输入输出属性数组
    engine->input_attrs = (rknn_tensor_attr*)malloc(engine->io_num.n_input * sizeof(rknn_tensor_attr));
    engine->output_attrs = (rknn_tensor_attr*)malloc(engine->io_num.n_output * sizeof(rknn_tensor_attr));
    engine->inputs = (rknn_input*)malloc(engine->io_num.n_input * sizeof(rknn_input));
    engine->outputs = (rknn_output*)malloc(engine->io_num.n_output * sizeof(rknn_output));

    if (!engine->input_attrs || !engine->output_attrs || !engine->inputs || !engine->outputs) {
        printf("分配输入输出数组内存失败\n");
        yolo_engine_cleanup(engine);
        return -1;
    }

    memset(engine->input_attrs, 0, engine->io_num.n_input * sizeof(rknn_tensor_attr));
    memset(engine->output_attrs, 0, engine->io_num.n_output * sizeof(rknn_tensor_attr));
    memset(engine->inputs, 0, engine->io_num.n_input * sizeof(rknn_input));
    memset(engine->outputs, 0, engine->io_num.n_output * sizeof(rknn_output));

    // 查询输入属性
    for (int i = 0; i < engine->io_num.n_input; i++) {
        engine->input_attrs[i].index = i;
        ret = rknn_query(engine->ctx, RKNN_QUERY_INPUT_ATTR, &engine->input_attrs[i], sizeof(rknn_tensor_attr));
        if (ret < 0) {
            printf("查询输入属性%d失败: %d\n", i, ret);
            yolo_engine_cleanup(engine);
            return -1;
        }
    }

    // 查询输出属性
    for (int i = 0; i < engine->io_num.n_output; i++) {
        engine->output_attrs[i].index = i;
        ret = rknn_query(engine->ctx, RKNN_QUERY_OUTPUT_ATTR, &engine->output_attrs[i], sizeof(rknn_tensor_attr));
        if (ret < 0) {
            printf("查询输出属性%d失败: %d\n", i, ret);
            yolo_engine_cleanup(engine);
            return -1;
        }
        printf("输出 %d: %s\n", i, engine->output_attrs[i].name);
    }

    // 设置模型输入尺寸
    engine->model_width = YOLOPV2_MODEL_INPUT_WIDTH;
    engine->model_height = YOLOPV2_MODEL_INPUT_HEIGHT;

    printf("模型输入尺寸: %dx%d\n", engine->model_width, engine->model_height);

    // 分配输入数据缓冲区
    size_t input_size = engine->model_width * engine->model_height * 3; // RGB
    engine->input_data = (uint8_t*)malloc(input_size);
    if (!engine->input_data) {
        printf("分配输入数据缓冲区失败\n");
        yolo_engine_cleanup(engine);
        return -1;
    }

    // 分配输出数据缓冲区
    size_t lane_size = engine->model_width * engine->model_height * sizeof(float); // (1,1,480,640)
    size_t drivable_size = engine->model_width * engine->model_height * 2 * sizeof(float); // (1,2,480,640)

    engine->lane_line_buffer = (float*)malloc(lane_size);
    engine->drivable_area_buffer = (float*)malloc(drivable_size);

    if (!engine->lane_line_buffer || !engine->drivable_area_buffer) {
        printf("分配输出数据缓冲区失败\n");
        yolo_engine_cleanup(engine);
        return -1;
    }

    printf("输出缓冲区: 车道线=%zu bytes, 可行驶区域=%zu bytes\n", lane_size, drivable_size);

    printf("YOLOPv2引擎初始化成功\n");
    engine->is_initialized = 1;
    return 0;
}

int yolo_engine_detect(YOLOEngine* engine, const uint8_t* rgb_data, int width, int height,
                      yolopv2_result_t* results) {
    if (!engine || !rgb_data || !results || !engine->is_initialized) {
        // printf("[DEBUG] detect参数检查失败\n");
        return -1;
    }

    // printf("[DEBUG] detect开始: width=%d, height=%d\n", width, height);

    // 预处理：将输入图像调整到模型输入尺寸
    preprocess_image(rgb_data, engine->input_data, width, height,
                    engine->model_width, engine->model_height);
    // printf("[DEBUG] 预处理完成\n");

    // 设置输入
    engine->inputs[0].index = 0;
    engine->inputs[0].type = RKNN_TENSOR_UINT8;
    engine->inputs[0].size = engine->model_width * engine->model_height * 3;
    engine->inputs[0].fmt = RKNN_TENSOR_NHWC;  // 使用NHWC格式
    engine->inputs[0].buf = engine->input_data;

    // 设置输出（获取浮点数输出）
    for (int i = 0; i < engine->io_num.n_output; i++) {
        engine->outputs[i].want_float = 1;
    }
    // printf("[DEBUG] 输入输出设置完成\n");

    // 执行推理
    int ret = rknn_inputs_set(engine->ctx, engine->io_num.n_input, engine->inputs);
    if (ret < 0) {
        printf("设置输入失败: %d\n", ret);
        return -1;
    }
    // printf("[DEBUG] inputs_set完成\n");

    ret = rknn_run(engine->ctx, NULL);
    if (ret < 0) {
        printf("模型推理失败: %d\n", ret);
        return -1;
    }
    // printf("[DEBUG] rknn_run完成\n");

    ret = rknn_outputs_get(engine->ctx, engine->io_num.n_output, engine->outputs, NULL);
    if (ret < 0) {
        printf("获取输出失败: %d\n", ret);
        return -1;
    }
    // printf("[DEBUG] outputs_get完成\n");
    // fflush(stdout);

    // 检查输出数量
    if (engine->io_num.n_output < 5) {
        printf("[DEBUG] ERROR: n_output=%d, 需要至少5个输出!\n", engine->io_num.n_output);
        fflush(stdout);
        return -1;
    }

    // 打印所有输出的大小
    // for (int i = 0; i < engine->io_num.n_output; i++) {
    //     printf("[DEBUG] outputs[%d].size=%u bytes\n", i, engine->outputs[i].size);
    // }
    // fflush(stdout);

    // 实际输出节点顺序（根据size分析）:
    // outputs[0] = seg  : (1, 2, 480, 640) = 2457600 bytes - 可行驶区域分割
    // outputs[1] = ll   : (1, 1, 480, 640) = 1228800 bytes - 车道线
    // outputs[2] = pred0: (1, 255, 60, 80) - 大物体检测（跳过）
    // outputs[3] = pred1: (1, 255, 30, 40) - 中物体检测（跳过）
    // outputs[4] = pred2: (1, 255, 15, 20) - 小物体检测（跳过）

    // 复制可行驶区域数据到缓冲区（输出0）
    size_t expected_drivable_size = engine->model_width * engine->model_height * 2 * sizeof(float);
    size_t actual_drivable_size = engine->outputs[0].size;

    // printf("[DEBUG] 可行驶区域数据: expected=%zu bytes, actual=%u bytes\n",
    //        expected_drivable_size, (unsigned int)actual_drivable_size);
    // fflush(stdout);

    // 检查指针有效性
    if (!engine->drivable_area_buffer) {
        printf("[DEBUG] ERROR: drivable_area_buffer is NULL!\n");
        fflush(stdout);
        return -1;
    }
    if (!engine->outputs[0].buf) {
        printf("[DEBUG] ERROR: outputs[0].buf is NULL!\n");
        fflush(stdout);
        return -1;
    }

    // 使用实际大小进行复制，防止缓冲区溢出
    size_t drivable_copy_size = (actual_drivable_size < expected_drivable_size) ? actual_drivable_size : expected_drivable_size;
    if (actual_drivable_size != expected_drivable_size) {
        printf("[DEBUG] WARNING: 可行驶区域大小不匹配，使用 %zu bytes\n", drivable_copy_size);
        fflush(stdout);
    }

    memcpy(engine->drivable_area_buffer, engine->outputs[0].buf, drivable_copy_size);
    // printf("[DEBUG] 可行驶区域数据复制完成\n");
    // fflush(stdout);

    // 复制车道线数据到缓冲区（输出1）
    size_t expected_lane_size = engine->model_width * engine->model_height * sizeof(float);
    size_t actual_lane_size = engine->outputs[1].size;

    // printf("[DEBUG] 车道线数据: expected=%zu bytes, actual=%u bytes\n",
    //        expected_lane_size, (unsigned int)actual_lane_size);
    // fflush(stdout);

    // 检查指针有效性
    if (!engine->lane_line_buffer) {
        printf("[DEBUG] ERROR: lane_line_buffer is NULL!\n");
        fflush(stdout);
        return -1;
    }
    if (!engine->outputs[1].buf) {
        printf("[DEBUG] ERROR: outputs[1].buf is NULL!\n");
        fflush(stdout);
        return -1;
    }

    // 使用实际大小进行复制，防止缓冲区溢出
    size_t lane_copy_size = (actual_lane_size < expected_lane_size) ? actual_lane_size : expected_lane_size;
    if (actual_lane_size != expected_lane_size) {
        printf("[DEBUG] WARNING: 车道线大小不匹配，使用 %zu bytes\n", lane_copy_size);
        fflush(stdout);
    }

    memcpy(engine->lane_line_buffer, engine->outputs[1].buf, lane_copy_size);
    // printf("[DEBUG] 车道线数据复制完成\n");
    // fflush(stdout);

    // 释放RKNN输出（数据已复制到缓冲区）
    // printf("[DEBUG] 准备释放RKNN输出, n_output=%d\n", engine->io_num.n_output);
    // fflush(stdout);
    rknn_outputs_release(engine->ctx, engine->io_num.n_output, engine->outputs);
    // printf("[DEBUG] RKNN输出释放完成\n");
    // fflush(stdout);

    // 设置结果指针指向内部缓冲区
    results->lane_line_data = engine->lane_line_buffer;
    results->drivable_area_data = engine->drivable_area_buffer;
    results->width = engine->model_width;
    results->height = engine->model_height;

    printf("[DEBUG] detect完成\n");
    return 0;
}

void yolo_engine_cleanup(YOLOEngine* engine) {
    if (!engine) return;

    // 注意：outputs已在detect函数中释放，这里无需再释放

    if (engine->lane_line_buffer) {
        free(engine->lane_line_buffer);
        engine->lane_line_buffer = NULL;
    }

    if (engine->drivable_area_buffer) {
        free(engine->drivable_area_buffer);
        engine->drivable_area_buffer = NULL;
    }

    if (engine->input_data) {
        free(engine->input_data);
        engine->input_data = NULL;
    }

    if (engine->input_attrs) {
        free(engine->input_attrs);
        engine->input_attrs = NULL;
    }

    if (engine->output_attrs) {
        free(engine->output_attrs);
        engine->output_attrs = NULL;
    }

    if (engine->inputs) {
        free(engine->inputs);
        engine->inputs = NULL;
    }

    if (engine->outputs) {
        free(engine->outputs);
        engine->outputs = NULL;
    }

    if (engine->ctx > 0) {
        rknn_destroy(engine->ctx);
        engine->ctx = 0;
    }

    engine->is_initialized = 0;
}

void draw_lane_and_drivable(uint8_t* rgb_data, int width, int height,
                           const yolopv2_result_t* results) {
    if (!rgb_data || !results || !results->lane_line_data || !results->drivable_area_data) {
        // printf("[DEBUG] draw参数检查失败\n");
        // fflush(stdout);
        return;
    }

    // printf("[DEBUG] draw开始: width=%d, height=%d, result_width=%d, result_height=%d\n",
    //        width, height, results->width, results->height);
    // fflush(stdout);

    float ratiow = (float)width / results->width;
    float ratioh = (float)height / results->height;

    int area = results->height * results->width;

    // printf("[DEBUG] draw可行驶区域开始...\n");
    // fflush(stdout);
    // 绘制可行驶区域（绿色半透明）
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            const int x = (int)(j / ratiow);
            const int y = (int)(i / ratioh);

            if (x >= 0 && x < results->width && y >= 0 && y < results->height) {
                // 比较两个通道，选择可行驶区域
                float channel0 = results->drivable_area_data[y * results->width + x];
                float channel1 = results->drivable_area_data[area + y * results->width + x];

                if (channel0 < channel1) {
                    // 可行驶区域 - 绿色半透明叠加
                    int idx = (i * width + j) * 3;
                    rgb_data[idx] = (uint8_t)(rgb_data[idx] * 0.6 + 0 * 0.4);       // R
                    rgb_data[idx + 1] = (uint8_t)(rgb_data[idx + 1] * 0.6 + 255 * 0.4); // G
                    rgb_data[idx + 2] = (uint8_t)(rgb_data[idx + 2] * 0.6 + 0 * 0.4);   // B
                }
            }
        }
    }
    // printf("[DEBUG] draw可行驶区域完成\n");
    // fflush(stdout);

    // printf("[DEBUG] draw车道线开始...\n");
    // fflush(stdout);

    int lane_pixel_count = 0;  // 统计绘制了多少车道线像素

    // 绘制车道线（纯红色）
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            const int x = (int)(j / ratiow);
            const int y = (int)(i / ratioh);

            if (x >= 0 && x < results->width && y >= 0 && y < results->height) {
                float lane_prob = results->lane_line_data[y * results->width + x];

                if (lane_prob > 0.3) {  // 降低阈值从0.5到0.3
                    // 车道线 - 纯红色 (BGR格式)
                    int idx = (i * width + j) * 3;
                    rgb_data[idx] = 0;       // B
                    rgb_data[idx + 1] = 0;   // G
                    rgb_data[idx + 2] = 255; // R
                    lane_pixel_count++;
                }
            }
        }
    }
    printf("绘制 %d 个红色车道线像素\n", lane_pixel_count);
    fflush(stdout);

    // 弯道检测（基于绘制后的红色车道线）
    TurnType turn = detect_turn_direction_from_image(rgb_data, width, height);

    // 计算置信度：基于车道线像素数量
    uint32_t confidence = 0;
    if (lane_pixel_count > 500) {
        confidence = 90;  // 车道线清晰
    } else if (lane_pixel_count > 300) {
        confidence = 75;  // 车道线较清晰
    } else if (lane_pixel_count > 100) {
        confidence = 60;  // 车道线模糊
    } else {
        confidence = 40;  // 车道线不清晰
    }

    // 计算角度：基于弯道类型估算
    uint32_t angle = 0;
    uint32_t curve_type_shm = 0;  // 共享内存中的弯道类型

    switch (turn) {
        case TURN_LEFT:
            curve_type_shm = 1;  // CURVE_LEFT
            angle = 25;  // 左弯约25度
            break;
        case TURN_RIGHT:
            curve_type_shm = 2;  // CURVE_RIGHT
            angle = 25;  // 右弯约25度
            break;
        case TURN_STRAIGHT:
        default:
            curve_type_shm = 0;  // CURVE_NONE
            angle = 0;   // 直道0度
            break;
    }

    // 更新弯道检测共享内存
    static uint32_t curve_frame_id = 0;
    curve_frame_id++;
    curve_detection_update(curve_frame_id, curve_type_shm, confidence, angle);

    printf("\n");
    printf("========================================\n");
    switch (turn) {
        case TURN_LEFT:
            printf("       *** 左弯道 *** (置信度:%u%%, 角度:%u°)\n", confidence, angle);
            printf("       <<<  LEFT TURN  <<<\n");
            break;
        case TURN_RIGHT:
            printf("       *** 右弯道 *** (置信度:%u%%, 角度:%u°)\n", confidence, angle);
            printf("       >>>  RIGHT TURN  >>>\n");
            break;
        case TURN_STRAIGHT:
        default:
            printf("       === 直道 === (置信度:%u%%)\n", confidence);
            printf("       |||  STRAIGHT  |||\n");
            break;
    }
    printf("========================================\n");
    printf("\n");
    fflush(stdout);
}

void rgb565_to_rgb888_yolo(const uint16_t* rgb565_data, uint8_t* rgb888_data,
                          int width, int height) {
    if (!rgb565_data || !rgb888_data) return;

    int total_pixels = width * height;
    for (int i = 0; i < total_pixels; i++) {
        uint16_t pixel = rgb565_data[i];

        // 提取RGB分量 (5-6-5格式)
        uint8_t r = (pixel >> 11) & 0x1F;
        uint8_t g = (pixel >> 5) & 0x3F;
        uint8_t b = pixel & 0x1F;

        // 扩展到8位
        rgb888_data[i * 3] = (r << 3) | (r >> 2);         // R
        rgb888_data[i * 3 + 1] = (g << 2) | (g >> 4);     // G
        rgb888_data[i * 3 + 2] = (b << 3) | (b >> 2);     // B
    }
}
