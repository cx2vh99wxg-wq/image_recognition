#include "yolo_integration.h"
#include "shared_memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <time.h>

/* Verbose lane diagnostics redraw the terminal dozens of times per inference.
 * Keep them opt-in; summary timing remains available in the capture program. */
static int lane_debug_enabled(void) {
    static int enabled = -1;
    if (enabled < 0) {
        const char* value = getenv("VISION_VERBOSE");
        enabled = value && atoi(value) != 0;
    }
    return enabled;
}
#define LANE_TRACE(...) do { if (lane_debug_enabled()) printf(__VA_ARGS__); } while (0)
#define LANE_FLUSH() do { if (lane_debug_enabled()) fflush(stdout); } while (0)

static void lane_error(const char* format, ...) {
    static time_t last = 0;
    time_t now = time(NULL);
    if (!lane_debug_enabled() && last && now >= last && now - last < 5) return;
    last = now;
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

// 弯道类型定义
typedef enum {
    TURN_STRAIGHT = 0,    // 直道
    TURN_LEFT = 1,        // 左弯道
    TURN_RIGHT = 2        // 右弯道
} TurnType;

// 弯道检测函数：基于绘制的红色车道线进行垂直投影分析（多区域采样+加权计算）
static TurnType detect_turn_direction_from_image(const uint8_t* rgb_data, int width, int height) {
    if (!rgb_data) {
        LANE_TRACE(">>> 图像数据指针为空 <<<\n");
        LANE_FLUSH();
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
    LANE_TRACE(">>> 多区域采样结果:\n");
    for (int r = 0; r < NUM_REGIONS; r++) {
        LANE_TRACE("    区域%d (Y=%d-%d): %d像素, 权重%.1f\n",
               r, region_y[r], region_y[r] + region_height, totals[r], weights[r]);
    }
    LANE_FLUSH();

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
        LANE_TRACE(">>> 有效区域不足 (仅%d个) <<<\n", valid_regions);
        LANE_FLUSH();
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

    LANE_TRACE(">>> 最远区域重心: %.1f (Y=%d)\n",
           far_region >= 0 ? centers[far_region] : -1,
           far_region >= 0 ? region_y[far_region] : -1);
    LANE_TRACE(">>> 最近区域重心: %.1f (Y=%d)\n",
           near_region >= 0 ? centers[near_region] : -1,
           near_region >= 0 ? region_y[near_region] : -1);
    LANE_TRACE(">>> 加权偏移量: %.2f <<<\n", weighted_shift);
    LANE_FLUSH();

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

    LANE_TRACE(">>> 总像素: %d, 使用阈值: %.1f <<<\n", total_pixels, threshold);
    LANE_FLUSH();

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
    // The deployed model already consumes 640x480. Avoid per-pixel float
    // multiply/address work when resizing is the identity operation.
    if (src_width == model_width && src_height == model_height) {
        memcpy(input_data, rgb_data, (size_t)src_width * src_height * 3);
        return;
    }
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
    for (uint32_t i = 0; i < engine->io_num.n_input; i++) {
        engine->input_attrs[i].index = i;
        ret = rknn_query(engine->ctx, RKNN_QUERY_INPUT_ATTR, &engine->input_attrs[i], sizeof(rknn_tensor_attr));
        if (ret < 0) {
            printf("查询输入属性%u失败: %d\n", i, ret);
            yolo_engine_cleanup(engine);
            return -1;
        }
    }

    // 查询输出属性
    for (uint32_t i = 0; i < engine->io_num.n_output; i++) {
        engine->output_attrs[i].index = i;
        ret = rknn_query(engine->ctx, RKNN_QUERY_OUTPUT_ATTR, &engine->output_attrs[i], sizeof(rknn_tensor_attr));
        if (ret < 0) {
            printf("查询输出属性%u失败: %d\n", i, ret);
            yolo_engine_cleanup(engine);
            return -1;
        }
        printf("输出 %u: %s\n", i, engine->output_attrs[i].name);
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
    engine->requested_output_count = 2;
    engine->is_initialized = 1;
    return 0;
}

int yolo_engine_detect(YOLOEngine* engine, const uint8_t* rgb_data, int width, int height,
                      yolopv2_result_t* results) {
    if (!engine || !rgb_data || !results || !engine->is_initialized ||
        width <= 0 || height <= 0 || engine->io_num.n_output < 2 ||
        !engine->lane_line_buffer || !engine->drivable_area_buffer) return -1;

    memset(results, 0, sizeof(*results));
    preprocess_image(rgb_data, engine->input_data, width, height,
                     engine->model_width, engine->model_height);
    engine->inputs[0].index = 0;
    engine->inputs[0].type = RKNN_TENSOR_UINT8;
    engine->inputs[0].size = engine->model_width * engine->model_height * 3;
    engine->inputs[0].fmt = RKNN_TENSOR_NHWC;
    engine->inputs[0].buf = engine->input_data;
    int ret = rknn_inputs_set(engine->ctx, engine->io_num.n_input, engine->inputs);
    if (ret < 0) { lane_error("YOLOP inputs_set failed: %d\n", ret); return -1; }
    ret = rknn_run(engine->ctx, NULL);
    if (ret < 0) { lane_error("YOLOP inference failed: %d\n", ret); return -1; }

    /* All model computation is retained. Only unused object-detection heads
     * are omitted from CPU output conversion; lanes and drivable area remain
     * full-resolution float tensors with exactly the same thresholds. */
    uint32_t count = engine->requested_output_count;
    if (count != engine->io_num.n_output) count = 2;
    memset(engine->outputs, 0, engine->io_num.n_output * sizeof(*engine->outputs));
    for (uint32_t i = 0; i < count; ++i) {
        engine->outputs[i].index = i;
        engine->outputs[i].want_float = 1;
    }
    ret = rknn_outputs_get(engine->ctx, count, engine->outputs, NULL);
    if (ret < 0 && count == 2 && engine->io_num.n_output > 2) {
        /* Some deployed runtimes require a full request. Probe just once,
         * then cache the compatible count and release that same count. */
        count = engine->io_num.n_output;
        engine->requested_output_count = count;
        memset(engine->outputs, 0, count * sizeof(*engine->outputs));
        for (uint32_t i = 0; i < count; ++i) {
            engine->outputs[i].index = i;
            engine->outputs[i].want_float = 1;
        }
        fprintf(stderr, "YOLOP runtime requires all %u outputs; caching fallback\n", count);
        ret = rknn_outputs_get(engine->ctx, count, engine->outputs, NULL);
    }
    if (ret < 0) { lane_error("YOLOP outputs_get failed: %d\n", ret); return -1; }

    size_t drivable_size = (size_t)engine->model_width * engine->model_height * 2 * sizeof(float);
    size_t lane_size = (size_t)engine->model_width * engine->model_height * sizeof(float);
    int valid = engine->outputs[0].buf && engine->outputs[1].buf &&
                engine->outputs[0].size >= drivable_size &&
                engine->outputs[1].size >= lane_size;
    if (valid) {
        memcpy(engine->drivable_area_buffer, engine->outputs[0].buf, drivable_size);
        memcpy(engine->lane_line_buffer, engine->outputs[1].buf, lane_size);
    }
    int release_result = rknn_outputs_release(engine->ctx, count, engine->outputs);
    if (!valid || release_result < 0) {
        lane_error("YOLOP invalid segmentation tensors or release failure\n");
        return -1;
    }
    results->lane_line_data = engine->lane_line_buffer;
    results->drivable_area_data = engine->drivable_area_buffer;
    results->width = engine->model_width;
    results->height = engine->model_height;
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

int yolo_overlay_lane_and_drivable(uint8_t* rgb_data, int width, int height,
                           const yolopv2_result_t* results) {
    if (!rgb_data || !results || !results->lane_line_data || !results->drivable_area_data ||
        width <= 0 || height <= 0 || results->width <= 0 || results->height <= 0) return -1;
    float ratiow = (float)width / results->width;
    float ratioh = (float)height / results->height;
    int area = results->height * results->width;
    int same_size = width == results->width && height == results->height;
    int lane_pixel_count = 0;

    /* One pass instead of two. On the deployed same-size path no floating
     * division is needed. The original blend expression and lane threshold
     * are retained, so the rendered pixels and curve classification match. */
    for (int i = 0; i < height; ++i) {
        int y = same_size ? i : (int)(i / ratioh);
        for (int j = 0; j < width; ++j) {
            int x = same_size ? j : (int)(j / ratiow);
            if (x < 0 || x >= results->width || y < 0 || y >= results->height) continue;
            int mask = y * results->width + x;
            int idx = (i * width + j) * 3;
            if (results->drivable_area_data[mask] < results->drivable_area_data[area + mask]) {
                rgb_data[idx] = (uint8_t)(rgb_data[idx] * 0.6 + 0 * 0.4);
                rgb_data[idx + 1] = (uint8_t)(rgb_data[idx + 1] * 0.6 + 255 * 0.4);
                rgb_data[idx + 2] = (uint8_t)(rgb_data[idx + 2] * 0.6 + 0 * 0.4);
            }
            if (results->lane_line_data[mask] > 0.3) {
                rgb_data[idx] = 0;
                rgb_data[idx + 1] = 0;
                rgb_data[idx + 2] = 255;
                ++lane_pixel_count;
            }
        }
    }
    LANE_TRACE("绘制 %d 个红色车道线像素\n", lane_pixel_count);

    return lane_pixel_count;
}

int yolo_analyze_annotated_curve(const uint8_t* rgb_data, int width, int height,
                                 int lane_pixel_count, YOLOCurveResult* curve) {
    if (!rgb_data || !curve || width <= 0 || height <= 0 || lane_pixel_count < 0) return -1;
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

    curve->curve_type = curve_type_shm;
    curve->confidence = confidence;
    curve->angle = angle;
    return 0;
}

void draw_lane_and_drivable(uint8_t* rgb_data, int width, int height,
                           const yolopv2_result_t* results) {
    int lane_pixel_count = yolo_overlay_lane_and_drivable(rgb_data, width, height, results);
    YOLOCurveResult curve;
    if (yolo_analyze_annotated_curve(rgb_data, width, height, lane_pixel_count, &curve) != 0) return;
    static uint32_t curve_frame_id = 0;
    curve_detection_update(++curve_frame_id, (CurveType)curve.curve_type, curve.confidence, curve.angle);
    LANE_TRACE("Lane result: source=%u type=%u confidence=%u angle=%u\n",
               curve_frame_id, curve.curve_type, curve.confidence, curve.angle);
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
