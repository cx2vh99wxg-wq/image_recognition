#include "yolo_integration.h"
#include "preprocess.h"
#include "person_alert_effects.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

int yolo_engine_init(YOLOEngine* engine, const char* model_path, const char* labels_path) {
    if (!engine || !model_path || !labels_path) {
        printf("YOLO引擎初始化参数无效\n");
        return -1;
    }
    
    printf("正在初始化YOLO引擎...\n");
    
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
    }
    
    // 获取模型输入尺寸
    engine->model_width = engine->input_attrs[0].dims[2];  // 通常是 [1, 3, 640, 640]
    engine->model_height = engine->input_attrs[0].dims[1];
    if (engine->model_width != YOLO_MODEL_INPUT_SIZE || engine->model_height != YOLO_MODEL_INPUT_SIZE) {
        printf("警告: 模型输入尺寸 %dx%d 与预期 %dx%d 不符\n", 
               engine->model_width, engine->model_height, YOLO_MODEL_INPUT_SIZE, YOLO_MODEL_INPUT_SIZE);
        engine->model_width = YOLO_MODEL_INPUT_SIZE;
        engine->model_height = YOLO_MODEL_INPUT_SIZE;
    }
    
    // 分配输入数据缓冲区
    size_t input_size = engine->model_width * engine->model_height * 3; // RGB
    engine->input_data = (uint8_t*)malloc(input_size);
    if (!engine->input_data) {
        printf("分配输入数据缓冲区失败\n");
        yolo_engine_cleanup(engine);
        return -1;
    }
    
    // 初始化后处理模块
    printf("加载标签文件: %s\n", labels_path);
    if (initPostProcess(labels_path) != 0) {
        printf("后处理模块初始化失败\n");
        yolo_engine_cleanup(engine);
        return -1;
    }
    printf("标签文件加载完成，支持80个COCO类别\n");
    
    printf("YOLO引擎初始化成功\n");
    engine->is_initialized = 1;
    return 0;
}

int yolo_engine_detect(YOLOEngine* engine, const uint8_t* rgb_data, int width, int height, 
                      detect_result_group_t* results) {
    if (!engine || !rgb_data || !results || !engine->is_initialized) {
        return -1;
    }
    
    // 预处理：将输入图像调整到模型输入尺寸
    BOX_RECT letterbox_pads;
    float scale;
    letterbox_simple(rgb_data, engine->input_data, width, height, 
                    engine->model_width, engine->model_height, &letterbox_pads, &scale);
    
    // 设置输入
    engine->inputs[0].index = 0;
    engine->inputs[0].type = RKNN_TENSOR_UINT8;
    engine->inputs[0].size = engine->model_width * engine->model_height * 3;
    engine->inputs[0].fmt = RKNN_TENSOR_NHWC;
    engine->inputs[0].buf = engine->input_data;
    
    // 设置输出
    for (int i = 0; i < engine->io_num.n_output; i++) {
        engine->outputs[i].want_float = 0; // 使用量化输出
    }
    
    // 执行推理
    int ret = rknn_inputs_set(engine->ctx, engine->io_num.n_input, engine->inputs);
    if (ret < 0) {
        printf("设置输入失败: %d\n", ret);
        return -1;
    }
    
    ret = rknn_run(engine->ctx, NULL);
    if (ret < 0) {
        printf("模型推理失败: %d\n", ret);
        return -1;
    }
    
    ret = rknn_outputs_get(engine->ctx, engine->io_num.n_output, engine->outputs, NULL);
    if (ret < 0) {
        printf("获取输出失败: %d\n", ret);
        return -1;
    }
    
    // 后处理 - 使用letterbox参数进行正确的坐标映射
    int post_ret = post_process_simple((int8_t*)engine->outputs[0].buf,
                                      (int8_t*)engine->outputs[1].buf, 
                                      (int8_t*)engine->outputs[2].buf,
                                      engine->model_height, engine->model_width,
                                      0.25f, 0.45f, letterbox_pads, scale, scale, results);
    
    if (post_ret != 0) {
        results->count = 0;
    }
    
    // 释放输出
    rknn_outputs_release(engine->ctx, engine->io_num.n_output, engine->outputs);
    
    return 0;
}

void yolo_engine_cleanup(YOLOEngine* engine) {
    if (!engine) return;
    
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
    
    if (engine->is_initialized) {
        deinitPostProcess();
        engine->is_initialized = 0;
    }
}

// 简单的字符绘制函数 - 绘制5x7像素的字符
static void draw_char(uint8_t* rgb_data, int width, int height, int x, int y, 
                     char c, uint8_t r, uint8_t g, uint8_t b) {
    // 简化的5x7字体点阵 - 主要字符
    static const uint8_t font_5x7[][7] = {
        // 0
        {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E},
        // 1  
        {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},
        // 2
        {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F},
        // 3
        {0x1F, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0E},
        // 4
        {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02},
        // 5
        {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E},
        // 6
        {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E},
        // 7
        {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},
        // 8
        {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E},
        // 9
        {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C},
        // . (point)
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C},
        // % 
        {0x18, 0x19, 0x02, 0x04, 0x08, 0x13, 0x03},
        // space
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        // A
        {0x0E, 0x11, 0x11, 0x11, 0x1F, 0x11, 0x11},
        // B
        {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E},
        // C
        {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E},
        // E
        {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F},
        // P
        {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10},
        // R
        {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11},
        // S
        {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E},
        // O
        {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},
        // N
        {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11},
        // T
        {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},
        // L
        {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F},
        // I
        {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E},
        // D
        {0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C},
        // F
        {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10},
        // G
        {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0E},
        // H
        {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11},
        // J
        {0x0F, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0C},
        // K
        {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11},
        // M
        {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11},
        // U
        {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},
        // V
        {0x11, 0x11, 0x11, 0x11, 0x0A, 0x0A, 0x04},
        // W
        {0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11},
        // X
        {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11},
        // Y
        {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04},
        // Z
        {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F},
    };
    
    int char_index = -1;
    
    // 映射字符到字体索引
    if (c >= '0' && c <= '9') {
        char_index = c - '0';
    } else if (c == '.') {
        char_index = 10;
    } else if (c == '%') {
        char_index = 11;
    } else if (c == ' ') {
        char_index = 12;
    } else if (c == 'A' || c == 'a') {
        char_index = 13;
    } else if (c == 'B' || c == 'b') {
        char_index = 14;
    } else if (c == 'C' || c == 'c') {
        char_index = 15;
    } else if (c == 'E' || c == 'e') {
        char_index = 16;
    } else if (c == 'P' || c == 'p') {
        char_index = 17;
    } else if (c == 'R' || c == 'r') {
        char_index = 18;
    } else if (c == 'S' || c == 's') {
        char_index = 19;
    } else if (c == 'O' || c == 'o') {
        char_index = 20;
    } else if (c == 'N' || c == 'n') {
        char_index = 21;
    } else if (c == 'T' || c == 't') {
        char_index = 22;
    } else if (c == 'L' || c == 'l') {
        char_index = 23;
    } else if (c == 'I' || c == 'i') {
        char_index = 24;
    } else if (c == 'D' || c == 'd') {
        char_index = 25;
    } else if (c == 'F' || c == 'f') {
        char_index = 26;
    } else if (c == 'G' || c == 'g') {
        char_index = 27;
    } else if (c == 'H' || c == 'h') {
        char_index = 28;
    } else if (c == 'J' || c == 'j') {
        char_index = 29;
    } else if (c == 'K' || c == 'k') {
        char_index = 30;
    } else if (c == 'M' || c == 'm') {
        char_index = 31;
    } else if (c == 'U' || c == 'u') {
        char_index = 32;
    } else if (c == 'V' || c == 'v') {
        char_index = 33;
    } else if (c == 'W' || c == 'w') {
        char_index = 34;
    } else if (c == 'X' || c == 'x') {
        char_index = 35;
    } else if (c == 'Y' || c == 'y') {
        char_index = 36;
    } else if (c == 'Z' || c == 'z') {
        char_index = 37;
    } else if (c == '-') {
        char_index = 12; // 用空格代替短横线
    } else if (c == '_') {
        char_index = 12; // 用空格代替下划线
    } else {
        // 对于不支持的字符，不打印警告（避免日志过多），直接用空格代替
        char_index = 12; // 默认空格
    }
    
    if (char_index < 0 || char_index >= 38) {
        return;
    }
    
    const uint8_t* char_data = font_5x7[char_index];
    
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 5; col++) {
            if (char_data[row] & (1 << (4 - col))) {
                int px = x + col;
                int py = y + row;
                if (px >= 0 && px < width && py >= 0 && py < height) {
                    int idx = (py * width + px) * 3;
                    rgb_data[idx] = r;
                    rgb_data[idx + 1] = g;
                    rgb_data[idx + 2] = b;
                }
            }
        }
    }
}

// 绘制文本字符串
static void draw_text(uint8_t* rgb_data, int width, int height, int x, int y, 
                     const char* text, uint8_t r, uint8_t g, uint8_t b) {
    int char_width = 6; // 5像素宽度 + 1像素间距
    int pos_x = x;
    
    for (int i = 0; text[i] != '\0' && pos_x < width - 5; i++) {
        if (pos_x >= 0 && y >= 0 && y + 7 < height) {
            draw_char(rgb_data, width, height, pos_x, y, text[i], r, g, b);
        }
        pos_x += char_width;
    }
}

// 绘制填充的矩形背景
static void draw_filled_rect(uint8_t* rgb_data, int width, int height, 
                            int x1, int y1, int x2, int y2,
                            uint8_t r, uint8_t g, uint8_t b) {
    for (int y = y1; y <= y2 && y < height; y++) {
        for (int x = x1; x <= x2 && x < width; x++) {
            if (x >= 0 && y >= 0) {
                int idx = (y * width + x) * 3;
                rgb_data[idx] = r;
                rgb_data[idx + 1] = g;
                rgb_data[idx + 2] = b;
            }
        }
    }
}

// 计算两个边界框的重叠率(IoU)
static float calculate_iou(const BOX_RECT* box1, const BOX_RECT* box2) {
    int x1 = (box1->left > box2->left) ? box1->left : box2->left;
    int y1 = (box1->top > box2->top) ? box1->top : box2->top;
    int x2 = (box1->right < box2->right) ? box1->right : box2->right;
    int y2 = (box1->bottom < box2->bottom) ? box1->bottom : box2->bottom;
    
    if (x2 <= x1 || y2 <= y1) return 0.0f; // 无重叠
    
    int intersection = (x2 - x1) * (y2 - y1);
    int area1 = (box1->right - box1->left) * (box1->bottom - box1->top);
    int area2 = (box2->right - box2->left) * (box2->bottom - box2->top);
    int union_area = area1 + area2 - intersection;
    
    return (union_area > 0) ? (float)intersection / union_area : 0.0f;
}

// 去重检测结果
static int deduplicate_detections(detect_result_group_t* results, float iou_threshold) {
    if (!results || results->count <= 1) return 0;
    
    int removed = 0;
    
    for (int i = 0; i < results->count; i++) {
        if (results->results[i].prop <= 0) continue; // 已被标记删除
        
        for (int j = i + 1; j < results->count; j++) {
            if (results->results[j].prop <= 0) continue; // 已被标记删除
            
            // 检查是否是相同类别
            if (strcmp(results->results[i].name, results->results[j].name) == 0) {
                float iou = calculate_iou(&results->results[i].box, &results->results[j].box);
                
                if (iou > iou_threshold) {
                    // 重叠度高，保留置信度更高的，删除置信度低的
                    if (results->results[i].prop >= results->results[j].prop) {
                        // 更新i的置信度为最高值
                        if (results->results[j].prop > results->results[i].prop) {
                            results->results[i].prop = results->results[j].prop;
                        }
                        results->results[j].prop = -1; // 标记j为删除
                        removed++;
                    } else {
                        // 更新j的置信度为最高值
                        results->results[j].prop = results->results[j].prop;
                        results->results[i].prop = -1; // 标记i为删除
                        removed++;
                        break; // i已被删除，跳出内层循环
                    }
                }
            }
        }
    }
    
    // 压缩数组，移除被标记删除的元素
    int write_idx = 0;
    for (int read_idx = 0; read_idx < results->count; read_idx++) {
        if (results->results[read_idx].prop > 0) {
            if (write_idx != read_idx) {
                results->results[write_idx] = results->results[read_idx];
            }
            write_idx++;
        }
    }
    results->count = write_idx;
    
    return removed;
}

void draw_detection_results(uint8_t* rgb_data, int width, int height, 
                           const detect_result_group_t* results) {
    if (!rgb_data || !results) return;
    
    // 创建可修改的副本进行去重
    detect_result_group_t filtered_results = *results;
    
#if ENABLE_DETECTION_DEDUP
    // 去重处理：合并重叠的同类别检测框
    deduplicate_detections(&filtered_results, DEDUP_IOU_THRESHOLD);
#endif

    // 首先应用人员警示特效 (在绘制边框之前)
    apply_person_alert_effects(rgb_data, width, height, &filtered_results);
    
    for (int i = 0; i < filtered_results.count; i++) {
        const detect_result_t* result = &filtered_results.results[i];
        
        // 边界检查
        int left = result->box.left < 0 ? 0 : (result->box.left >= width ? width-1 : result->box.left);
        int right = result->box.right < 0 ? 0 : (result->box.right >= width ? width-1 : result->box.right);
        int top = result->box.top < 0 ? 0 : (result->box.top >= height ? height-1 : result->box.top);
        int bottom = result->box.bottom < 0 ? 0 : (result->box.bottom >= height ? height-1 : result->box.bottom);
        
        // 选择不同颜色的边界框 - 人员使用白色边框以区分红色警示特效
        uint8_t box_r = 0, box_g = 255, box_b = 0; // 默认绿色边界框
        
        if (strcmp(result->name, "person") == 0) {
            // 人员使用白色边框，与红色警示特效形成对比
            box_r = 255; box_g = 255; box_b = 255;
        } else {
            // 其他目标使用彩色边框
            switch (i % 3) {
                case 0: box_r = 255; box_g = 0; box_b = 0; break;   // 红色
                case 1: box_r = 0; box_g = 255; box_b = 0; break;   // 绿色  
                case 2: box_r = 0; box_g = 0; box_b = 255; break;   // 蓝色
            }
        }
        
        // 绘制矩形框 - 上下边（加粗到3像素）
        for (int thickness = 0; thickness < 3; thickness++) {
            for (int x = left; x <= right; x++) {
                // 上边
                int y_top = top + thickness;
                if (y_top < height) {
                    int idx = (y_top * width + x) * 3;
                    rgb_data[idx] = box_r;
                    rgb_data[idx + 1] = box_g;
                    rgb_data[idx + 2] = box_b;
                }
                // 下边
                int y_bottom = bottom - thickness;
                if (y_bottom >= 0) {
                    int idx = (y_bottom * width + x) * 3;
                    rgb_data[idx] = box_r;
                    rgb_data[idx + 1] = box_g;
                    rgb_data[idx + 2] = box_b;
                }
            }
        }
        
        // 绘制矩形框 - 左右边（加粗到3像素）
        for (int thickness = 0; thickness < 3; thickness++) {
            for (int y = top; y <= bottom; y++) {
                // 左边
                int x_left = left + thickness;
                if (x_left < width) {
                    int idx = (y * width + x_left) * 3;
                    rgb_data[idx] = box_r;
                    rgb_data[idx + 1] = box_g;
                    rgb_data[idx + 2] = box_b;
                }
                // 右边
                int x_right = right - thickness;
                if (x_right >= 0) {
                    int idx = (y * width + x_right) * 3;
                    rgb_data[idx] = box_r;
                    rgb_data[idx + 1] = box_g;
                    rgb_data[idx + 2] = box_b;
                }
            }
        }
        
        // 准备标签文本
        char label_text[64];
        char short_name[16];
        
        // 智能缩写常见的长标签
        if (strcmp(result->name, "traffic light") == 0) {
            strcpy(short_name, "light");
        } else if (strcmp(result->name, "fire hydrant") == 0) {
            strcpy(short_name, "hydrant");
        } else if (strcmp(result->name, "stop sign") == 0) {
            strcpy(short_name, "stop");
        } else if (strcmp(result->name, "parking meter") == 0) {
            strcpy(short_name, "meter");
        } else if (strcmp(result->name, "sports ball") == 0) {
            strcpy(short_name, "ball");
        } else if (strcmp(result->name, "baseball bat") == 0) {
            strcpy(short_name, "bat");
        } else if (strcmp(result->name, "baseball glove") == 0) {
            strcpy(short_name, "glove");
        } else if (strcmp(result->name, "wine glass") == 0) {
            strcpy(short_name, "wine");
        } else if (strcmp(result->name, "hot dog") == 0) {
            strcpy(short_name, "hotdog");
        } else if (strcmp(result->name, "potted plant") == 0) {
            strcpy(short_name, "plant");
        } else if (strcmp(result->name, "dining table") == 0) {
            strcpy(short_name, "table");
        } else if (strcmp(result->name, "cell phone") == 0) {
            strcpy(short_name, "phone");
        } else if (strcmp(result->name, "hair drier") == 0) {
            strcpy(short_name, "drier");
        } else if (strcmp(result->name, "teddy bear") == 0) {
            strcpy(short_name, "teddy");
        } else {
            // 对于短单词，不要截断
            strcpy(short_name, result->name);
        }
        
        snprintf(label_text, sizeof(label_text), "%s %.0f%%", 
                short_name, result->prop * 100.0f);
        
        // 计算标签背景大小
        int text_width = strlen(label_text) * 6; // 每个字符6像素宽
        int text_height = 8; // 7像素高度 + 1像素边距
        
        // 确保标签不会超出图像边界
        int box_width = right - left;
        
        // 对于常见的短单词，设置保护，避免截断
        int min_protect_len = 3; // 默认最小保护长度
        if (strcmp(short_name, "bottle") == 0 || strcmp(short_name, "person") == 0 || 
            strcmp(short_name, "chair") == 0 || strcmp(short_name, "table") == 0) {
            min_protect_len = strlen(short_name); // 完全保护这些单词
        }
        
        // 重新计算，确保不过度截断
        if (text_width > box_width) {
            // 计算能显示多少个字符
            int available_chars = box_width / 6;
            
            if (available_chars >= 8) {
                // 有足够空间显示 name + % 
                // 预留5个字符给"00%"和空格，剩余给name
                int max_name_chars = available_chars - 5;
                if (max_name_chars >= min_protect_len && max_name_chars < strlen(short_name)) {
                    short_name[max_name_chars] = '\0';
                    snprintf(label_text, sizeof(label_text), "%s %.0f%%", 
                            short_name, result->prop * 100.0f);
                    text_width = strlen(label_text) * 6;
                }
            } else if (available_chars >= min_protect_len) {
                // 只显示名称，不显示百分比
                int max_name_chars = available_chars;
                if (max_name_chars < strlen(short_name) && max_name_chars >= min_protect_len) {
                    short_name[max_name_chars] = '\0';
                }
                snprintf(label_text, sizeof(label_text), "%s", short_name);
                text_width = strlen(label_text) * 6;
            }
            // 如果空间不够显示受保护的单词，就保持原样，让它超出边界也没关系
        }
        
        // 标签位置（智能选择最佳位置）
        int label_x = left;
        int label_y = top - text_height - 2;
        
        // 如果上方空间不够，尝试其他位置
        if (label_y < 5) {
            // 尝试放在边界框内部上方
            label_y = top + 3;
            if (label_y + text_height > bottom - 3) {
                // 如果框太小，放在框下方
                label_y = bottom + 2;
                if (label_y + text_height >= height) {
                    // 如果下方也没空间，强制放在图像顶部
                    label_y = 2;
                }
            }
        }
        
        // 确保标签不会超出图像右边界
        if (label_x + text_width >= width) {
            label_x = width - text_width - 2;
            if (label_x < 0) label_x = 0;
        }
        
        // 对于极小的检测框，仍然显示标签但调整策略
        int min_box_size = 20; // 参考框尺寸
        bool is_small_box = ((right - left) < min_box_size || (bottom - top) < min_box_size);
        
        if (is_small_box) {
            // 对于小框，简化标签文本
            if (strlen(short_name) > 4) {
                short_name[4] = '\0'; // 最多4个字符
            }
            snprintf(label_text, sizeof(label_text), "%s", short_name); // 只显示名称，不显示百分比
            text_width = strlen(label_text) * 6;
        }
        
        // 扩大背景区域以提高可读性
        int bg_padding = 2;
        draw_filled_rect(rgb_data, width, height, 
                        label_x - bg_padding, label_y - bg_padding, 
                        label_x + text_width + bg_padding, label_y + text_height + bg_padding,
                        0, 0, 0); // 黑色背景
        
        // 添加白色边框以增强对比度
        for (int border = 0; border < 1; border++) {
            // 上边框
            for (int x = label_x - bg_padding; x <= label_x + text_width + bg_padding; x++) {
                if (x >= 0 && x < width && label_y - bg_padding + border >= 0 && label_y - bg_padding + border < height) {
                    int idx = ((label_y - bg_padding + border) * width + x) * 3;
                    rgb_data[idx] = 255; rgb_data[idx + 1] = 255; rgb_data[idx + 2] = 255;
                }
                if (x >= 0 && x < width && label_y + text_height + bg_padding - border >= 0 && label_y + text_height + bg_padding - border < height) {
                    int idx = ((label_y + text_height + bg_padding - border) * width + x) * 3;
                    rgb_data[idx] = 255; rgb_data[idx + 1] = 255; rgb_data[idx + 2] = 255;
                }
            }
            // 左右边框
            for (int y = label_y - bg_padding; y <= label_y + text_height + bg_padding; y++) {
                if (y >= 0 && y < height && label_x - bg_padding + border >= 0 && label_x - bg_padding + border < width) {
                    int idx = (y * width + (label_x - bg_padding + border)) * 3;
                    rgb_data[idx] = 255; rgb_data[idx + 1] = 255; rgb_data[idx + 2] = 255;
                }
                if (y >= 0 && y < height && label_x + text_width + bg_padding - border >= 0 && label_x + text_width + bg_padding - border < width) {
                    int idx = (y * width + (label_x + text_width + bg_padding - border)) * 3;
                    rgb_data[idx] = 255; rgb_data[idx + 1] = 255; rgb_data[idx + 2] = 255;
                }
            }
        }
        
        // 绘制标签文字（黄色，更显眼）
        draw_text(rgb_data, width, height, label_x, label_y + 1, 
                 label_text, 255, 255, 0); // 黄色文字
    }
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