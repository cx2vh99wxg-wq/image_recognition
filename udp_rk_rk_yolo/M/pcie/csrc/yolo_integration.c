#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#include "../../../common/image/frame_pixels.h"
#include "yolo_integration.h"
#include "fp16_expand.h"
#include "overlay_blend_lut.h"
#include "../../../common/perception/include/yolo_engine_resources.h"
#include "shared_memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <time.h>

/* 逐项车道诊断会在每次推理中多次刷新终端，
 * 因此需显式启用；采集程序仍输出汇总性能数据。 */
static int lane_debug_enabled(void)
{
    static int enabled = -1;
    if (enabled < 0)
    {
        const char *value = getenv("VISION_VERBOSE");
        enabled = value && atoi(value) != 0;
    }
    return enabled;
}
#define LANE_TRACE(...)                                                                            \
    do                                                                                             \
    {                                                                                              \
        if (lane_debug_enabled())                                                                  \
            printf(__VA_ARGS__);                                                                   \
    } while (0)
#define LANE_FLUSH()                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (lane_debug_enabled())                                                                  \
            fflush(stdout);                                                                        \
    } while (0)

static void lane_error(const char *format, ...)
{
    static time_t last = 0;
    time_t now = time(NULL);
    if (!lane_debug_enabled() && last && now >= last && now - last < 5)
        return;
    last = now;
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}

static uint64_t yolo_clock_ns(void)
{
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t)) return 0;
    return (uint64_t)t.tv_sec * 1000000000ULL + t.tv_nsec;
}

static void prepare_outputs(YOLOEngine *e, uint32_t count)
{
    memset(e->outputs, 0, e->io_num.n_output * sizeof(*e->outputs));
    for (uint32_t i = 0; i < count; ++i) {
        rknn_output *out = &e->outputs[i];
        out->index = i; out->want_float = 1;
        if (i < 2 && e->output_mode) {
            out->is_prealloc = 1;
            out->want_float = e->output_mode != 2;
            out->buf = e->output_mode == 2
                ? (void *)(i ? e->native_lane : e->native_road)
                : (void *)(i ? e->lane_line_buffer : e->drivable_area_buffer);
            out->size = e->model_width * e->model_height * (i ? 1 : 2) *
                        (e->output_mode == 2 ? sizeof(uint16_t) : sizeof(float));
        }
    }
}

// 本节给出弯向分类的编号。
typedef enum
{
    TURN_STRAIGHT = 0, // 分类结果：直线路段。
    TURN_LEFT = 1,     // 分类结果：向左弯曲。
    TURN_RIGHT = 2     // 分类结果：向右弯曲。
} TurnType;

// 弯道检测函数：基于绘制的红色车道线进行垂直投影分析（多区域采样+加权计算）
static TurnType detect_turn_direction_from_image(const uint8_t *rgb_data, int width, int height)
{
    if (!rgb_data)
    {
        LANE_TRACE(">>> 图像数据指针为空 <<<\n");
        LANE_FLUSH();
        return TURN_STRAIGHT;
    }

    // 只分析左上角320x240区域的下半部分
    int roi_width = 320;
    int roi_height = 240;
    int half_start = roi_height / 2; // 120，从本处开始

    // 保证ROI不超出图像边界
    if (roi_width > width)
        roi_width = width;
    if (roi_height > height)
        roi_height = height;

// 本节声明8个采样区域（从远到近），每个区域高度8像素。
#define NUM_REGIONS 8
    int region_y[NUM_REGIONS] = {
        half_start + 5,  // Y=125 (最远，权重最大)
        half_start + 15, // 参数记录为 Y=135。
        half_start + 25, // 参数记录为 Y=145。
        half_start + 35, // 参数记录为 Y=155。
        half_start + 50, // 参数记录为 Y=170。
        half_start + 65, // 参数记录为 Y=185。
        half_start + 80, // 参数记录为 Y=200。
        half_start + 95  // Y=215 (最近，权重最小)
    };
    int region_height = 8; // 每个区域高度8像素

    // 权重：远处的区域权重更大（从2.0到0.5）
    float weights[NUM_REGIONS] = {2.0f, 1.7f, 1.5f, 1.3f, 1.0f, 0.8f, 0.6f, 0.5f};

    // 保存每个区域的投影数组和重心。
    int projections[NUM_REGIONS][320];
    float centers[NUM_REGIONS];
    int totals[NUM_REGIONS];

    // 建立本段初始状态。
    for (int r = 0; r < NUM_REGIONS; r++)
    {
        memset(projections[r], 0, sizeof(int) * 320);
        centers[r] = 0;
        totals[r] = 0;
    }

    // 对每个区域进行垂直投影
    for (int r = 0; r < NUM_REGIONS; r++)
    {
        int y_start = region_y[r];
        int y_end = region_y[r] + region_height;

        for (int y = y_start; y < y_end && y < height; y++)
        {
            for (int x = 0; x < roi_width; x++)
            {
                int idx = (y * width + x) * 3;
                uint8_t b = rgb_data[idx];
                uint8_t g = rgb_data[idx + 1];
                uint8_t r_val = rgb_data[idx + 2];

                // 按红色条件筛选像素。
                if (r_val > 200 && g < 50 && b < 50)
                {
                    projections[r][x]++;
                    totals[r]++;
                }
            }
        }
    }

    // 打印每个区域的统计信息
    LANE_TRACE(">>> 多区域采样结果:\n");
    for (int r = 0; r < NUM_REGIONS; r++)
    {
        LANE_TRACE("    区域%d (Y=%d-%d): %d像素, 权重%.1f\n", r, region_y[r],
                   region_y[r] + region_height, totals[r], weights[r]);
    }
    LANE_FLUSH();

    // 求出每个区域的重心。
    int valid_regions = 0;
    for (int r = 0; r < NUM_REGIONS; r++)
    {
        if (totals[r] > 10)
        { // 至少10个像素才有效
            for (int x = 0; x < roi_width; x++)
            {
                centers[r] += x * projections[r][x];
            }
            centers[r] /= totals[r];
            valid_regions++;
        }
        else
        {
            centers[r] = -1; // 撤销该项的有效状态。
        }
    }

    if (valid_regions < 2)
    {
        LANE_TRACE(">>> 有效区域不足 (仅%d个) <<<\n", valid_regions);
        LANE_FLUSH();
        return TURN_STRAIGHT;
    }

    // 使用加权最小二乘法计算趋势
    // 求出加权偏移量：远处区域 - 近处区域，加上权重。
    float weighted_shift = 0;
    float total_weight = 0;

    // 找到最远和最近的有效区域
    int far_region = -1, near_region = -1;
    for (int r = 0; r < NUM_REGIONS; r++)
    {
        if (centers[r] >= 0)
        {
            if (far_region == -1)
                far_region = r;
            near_region = r;
        }
    }

    if (far_region != -1 && near_region != -1 && far_region != near_region)
    {
        // 求出所有相邻区域对的偏移，并加权。
        for (int r = 0; r < NUM_REGIONS - 1; r++)
        {
            if (centers[r] >= 0 && centers[r + 1] >= 0)
            {
                float local_shift = centers[r] - centers[r + 1];
                weighted_shift += local_shift * weights[r];
                total_weight += weights[r];
            }
        }

        if (total_weight > 0)
        {
            weighted_shift /= total_weight;
        }
    }

    LANE_TRACE(">>> 最远区域重心: %.1f (Y=%d)\n", far_region >= 0 ? centers[far_region] : -1,
               far_region >= 0 ? region_y[far_region] : -1);
    LANE_TRACE(">>> 最近区域重心: %.1f (Y=%d)\n", near_region >= 0 ? centers[near_region] : -1,
               near_region >= 0 ? region_y[near_region] : -1);
    LANE_TRACE(">>> 加权偏移量: %.2f <<<\n", weighted_shift);
    LANE_FLUSH();

    // 动态阈值：根据总像素数调整
    int total_pixels = 0;
    for (int r = 0; r < NUM_REGIONS; r++)
    {
        total_pixels += totals[r];
    }

    float threshold = 8.0f; // 尚未加入动态修正的判定门槛。
    if (total_pixels < 100)
    {
        threshold = 12.0f; // 像素少时提高阈值，减少误判
    }
    else if (total_pixels > 300)
    {
        threshold = 6.0f; // 像素多时降低阈值，提高灵敏度
    }

    LANE_TRACE(">>> 总像素: %d, 使用阈值: %.1f <<<\n", total_pixels, threshold);
    LANE_FLUSH();

    // 依据当前几何证据判定弯向。
    if (weighted_shift > threshold)
    {
        return TURN_RIGHT;
    }
    else if (weighted_shift < -threshold)
    {
        return TURN_LEFT;
    }

    return TURN_STRAIGHT;
}

// 预处理函数：将RGB888图像转换为NHWC格式的UINT8数据
static void preprocess_image(const uint8_t *rgb_data, uint8_t *input_data, int src_width,
                             int src_height, int model_width, int model_height)
{
    // 已部署模型直接接收 640x480；尺寸不变时，
    // 省去逐像素浮点乘法和寻址重算。
    if (src_width == model_width && src_height == model_height)
    {
        memcpy(input_data, rgb_data, (size_t)src_width * src_height * 3);
        return;
    }
    // 简单的resize + RGB转换 + NHWC格式
    float scale_w = (float)src_width / model_width;
    float scale_h = (float)src_height / model_height;

    for (int h = 0; h < model_height; h++)
    {
        for (int w = 0; w < model_width; w++)
        {
            int src_x = (int)(w * scale_w);
            int src_y = (int)(h * scale_h);

            if (src_x >= src_width)
                src_x = src_width - 1;
            if (src_y >= src_height)
                src_y = src_height - 1;

            // NHWC格式：input_data[h][w][c]
            int dst_idx = (h * model_width + w) * 3;
            int src_idx = (src_y * src_width + src_x) * 3;

            input_data[dst_idx + 0] = rgb_data[src_idx + 0];
            input_data[dst_idx + 1] = rgb_data[src_idx + 1];
            input_data[dst_idx + 2] = rgb_data[src_idx + 2];
        }
    }
}

static int tensor_image_shape(const rknn_tensor_attr *attr, int channels, int *width, int *height)
{
    if (attr->n_dims != 4 || attr->dims[0] != 1)
        return -1;
    if (attr->fmt == RKNN_TENSOR_NCHW && attr->dims[1] == (uint32_t)channels)
    {
        *height = attr->dims[2];
        *width = attr->dims[3];
        return 0;
    }
    if (attr->fmt == RKNN_TENSOR_NHWC && attr->dims[3] == (uint32_t)channels)
    {
        *height = attr->dims[1];
        *width = attr->dims[2];
        return 0;
    }
    return -1;
}

int yolo_engine_init(YOLOEngine *engine, const char *model_path, const char *labels_path)
{
    if (!engine || !model_path)
    {
        printf("YOLOPv2引擎初始化参数无效\n");
        return -1;
    }

    // labels_path参数保留用来接口兼容性，但YOLOPv2无需标签文件
    (void)labels_path;

    printf("正在初始化YOLOPv2引擎（车道线和可行驶区域检测）...\n");

    memset(engine, 0, sizeof(YOLOEngine));

    if (yolo_open_resources(engine, model_path) != 0)
        return -1;

    if (yolo_query_attrs(engine->ctx, engine->input_attrs, engine->io_num.n_input,
                         RKNN_QUERY_INPUT_ATTR, 1) != 0 ||
        yolo_query_attrs(engine->ctx, engine->output_attrs, engine->io_num.n_output,
                         RKNN_QUERY_OUTPUT_ATTR, 1) != 0)
    {
        yolo_engine_cleanup(engine);
        return -1;
    }

    const rknn_tensor_attr *out = engine->output_attrs;
    int width = 0, height = 0, road_w = 0, road_h = 0, lane_w = 0, lane_h = 0;
    if (engine->io_num.n_input != 1 || engine->io_num.n_output < 2 ||
        tensor_image_shape(engine->input_attrs, 3, &width, &height) != 0 ||
        !((width == 640 && height == 480) || (width == 320 && height == 240)) ||
        tensor_image_shape(&out[0], 2, &road_w, &road_h) != 0 ||
        tensor_image_shape(&out[1], 1, &lane_w, &lane_h) != 0 ||
        road_w != width || road_h != height || lane_w != width || lane_h != height)
    {
        fprintf(stderr, "YOLOPv2 unexpected input/segmentation tensors: input=%dx%d road=%dx%d lane=%dx%d\n",
                width, height, road_w, road_h, lane_w, lane_h);
        yolo_engine_cleanup(engine);
        return -1;
    }
    engine->model_width = width;
    engine->model_height = height;

    printf("模型输入尺寸: %dx%d\n", engine->model_width, engine->model_height);

    // 为输入数据缓冲区申请所需空间。
    size_t input_size = engine->model_width * engine->model_height * 3; // 像素通道排列为 RGB。
    engine->input_data = (uint8_t *)malloc(input_size);
    if (!engine->input_data)
    {
        printf("分配输入数据缓冲区失败\n");
        yolo_engine_cleanup(engine);
        return -1;
    }

    // 为输出数据缓冲区申请所需空间。
    size_t lane_size = engine->model_width * engine->model_height * sizeof(float); // 参数记录为 (1,1,480,640)。
    size_t drivable_size =
        engine->model_width * engine->model_height * 2 * sizeof(float); // 参数记录为 (1,2,480,640)。

    engine->lane_line_buffer = (float *)malloc(lane_size);
    engine->drivable_area_buffer = (float *)malloc(drivable_size);

    if (!engine->lane_line_buffer || !engine->drivable_area_buffer)
    {
        printf("分配输出数据缓冲区失败\n");
        yolo_engine_cleanup(engine);
        return -1;
    }

    printf("输出缓冲区: 车道线=%zu bytes, 可行驶区域=%zu bytes\n", lane_size, drivable_size);

    printf("YOLOPv2引擎初始化成功\n");
    // The native FP16 path was compared bit-for-bit with SDK float output.
    // Unsupported layouts/types retain the original SDK path; mode 0 is an explicit rollback.
    const char *mode = getenv("VISION_LANE_OUTPUT_MODE");
    if (!mode) mode="2";
    if (mode && (!strcmp(mode,"1") || !strcmp(mode,"2")) &&
        out[0].fmt == RKNN_TENSOR_NCHW && out[1].fmt == RKNN_TENSOR_NCHW) {
        engine->output_mode = (unsigned)(mode[0]-'0');
        if (engine->output_mode == 2) {
            if (out[0].type != RKNN_TENSOR_FLOAT16 || out[1].type != RKNN_TENSOR_FLOAT16)
                engine->output_mode = 0;
            else {
                engine->native_road = malloc(drivable_size/2);
                engine->native_lane = malloc(lane_size/2);
                if (!engine->native_road || !engine->native_lane) {
                    free(engine->native_road);free(engine->native_lane);
                    engine->native_road=NULL;engine->native_lane=NULL;engine->output_mode=0;
                    fprintf(stderr,"YOLOP native buffers unavailable; using original SDK float output\n");
                }
            }
        }
    }
    engine->requested_output_count = 2;
    engine->is_initialized = 1;
    return 0;
}

int yolo_engine_detect(YOLOEngine *engine, const uint8_t *rgb_data, int width, int height,
                       yolopv2_result_t *results)
{
    if (!engine || !rgb_data || !results || !engine->is_initialized || width <= 0 || height <= 0 ||
        engine->io_num.n_output < 2 || !engine->lane_line_buffer || !engine->drivable_area_buffer)
        return -1;

    memset(results, 0, sizeof(*results));
    YOLOTiming timing = {0};
    uint64_t begin = yolo_clock_ns();
    preprocess_image(rgb_data, engine->input_data, width, height, engine->model_width,
                     engine->model_height);
    uint64_t mark = yolo_clock_ns(); timing.prepare_ns = mark - begin;
    engine->inputs[0].index = 0;
    engine->inputs[0].type = RKNN_TENSOR_UINT8;
    engine->inputs[0].size = engine->model_width * engine->model_height * 3;
    engine->inputs[0].fmt = RKNN_TENSOR_NHWC;
    engine->inputs[0].buf = engine->input_data;
    int ret = rknn_inputs_set(engine->ctx, engine->io_num.n_input, engine->inputs);
    timing.inputs_ns = yolo_clock_ns() - mark;
    if (ret < 0)
    {
        lane_error("YOLOP inputs_set failed: %d\n", ret);
        return -1;
    }
    mark = yolo_clock_ns();
    ret = rknn_run(engine->ctx, NULL);
    timing.run_ns = yolo_clock_ns() - mark;
    if (ret < 0)
    {
        lane_error("YOLOP inference failed: %d\n", ret);
        return -1;
    }

    /* 模型计算保持完整，只省去 CPU
     * 对未用目标头的输出转换；
     * 车道与可行驶区仍为原分辨率浮点张量，阈值不变。 */
    uint32_t count = engine->requested_output_count;
    if (count != engine->io_num.n_output)
        count = 2;
    prepare_outputs(engine, count);
    mark = yolo_clock_ns();
    ret = rknn_outputs_get(engine->ctx, count, engine->outputs, NULL);
    if (ret < 0 && count == 2 && engine->io_num.n_output > 2)
    {
        /* 部分 RKNN 运行时要求一次取得全套输出，
         * 仅探测一次后缓存兼容数量，释放时使用同一数量。 */
        count = engine->io_num.n_output;
        engine->requested_output_count = count;
        prepare_outputs(engine, count);
        fprintf(stderr, "YOLOP runtime requires all %u outputs; caching fallback\n", count);
        ret = rknn_outputs_get(engine->ctx, count, engine->outputs, NULL);
    }
    if (ret < 0)
    {
        lane_error("YOLOP outputs_get failed: %d\n", ret);
        return -1;
    }

    timing.outputs_ns = yolo_clock_ns() - mark;
    mark = yolo_clock_ns();

    size_t drivable_size = (size_t)engine->model_width * engine->model_height * 2 * sizeof(float);
    size_t lane_size = (size_t)engine->model_width * engine->model_height * sizeof(float);
    const size_t divisor = engine->output_mode == 2 ? 2 : 1;
    int valid = engine->outputs[0].buf && engine->outputs[1].buf &&
                engine->outputs[0].size >= drivable_size/divisor &&
                engine->outputs[1].size >= lane_size/divisor;
    if (engine->output_mode) {
        const void *road=engine->output_mode==2?(void*)engine->native_road:(void*)engine->drivable_area_buffer;
        const void *lane=engine->output_mode==2?(void*)engine->native_lane:(void*)engine->lane_line_buffer;
        valid = valid && engine->outputs[0].buf==road && engine->outputs[1].buf==lane;
    }
    if (valid)
    {
        if (engine->output_mode == 2) {
            fp16_expand(engine->native_road,engine->drivable_area_buffer,drivable_size/sizeof(float));
            fp16_expand(engine->native_lane,engine->lane_line_buffer,lane_size/sizeof(float));
        }
        else if (engine->output_attrs[0].fmt == RKNN_TENSOR_NHWC)
        {
            const float *road = engine->outputs[0].buf;
            const size_t pixels = lane_size / sizeof(float);
            for (size_t i = 0; i < pixels; ++i)
            {
                engine->drivable_area_buffer[i] = road[2 * i];
                engine->drivable_area_buffer[pixels + i] = road[2 * i + 1];
            }
        }
        else if (engine->outputs[0].buf != engine->drivable_area_buffer)
            memcpy(engine->drivable_area_buffer, engine->outputs[0].buf, drivable_size);
        if (engine->output_mode != 2 && engine->outputs[1].buf != engine->lane_line_buffer)
            memcpy(engine->lane_line_buffer, engine->outputs[1].buf, lane_size);
    }
    timing.copy_ns = yolo_clock_ns() - mark;
    mark = yolo_clock_ns();
    int release_result = rknn_outputs_release(engine->ctx, count, engine->outputs);
    timing.release_ns = yolo_clock_ns() - mark;
    if (!valid || release_result < 0)
    {
        lane_error("YOLOP invalid segmentation tensors or release failure\n");
        return -1;
    }
    results->lane_line_data = engine->lane_line_buffer;
    results->drivable_area_data = engine->drivable_area_buffer;
    results->width = engine->model_width;
    results->height = engine->model_height;
    engine->timing = timing;
    return 0;
}

void yolo_engine_cleanup(YOLOEngine *engine)
{
    if (!engine)
        return;
    free(engine->native_road); engine->native_road = NULL;
    free(engine->native_lane); engine->native_lane = NULL;

    // 需注意：outputs已在detect函数中释放，本处无需再释放

    if (engine->lane_line_buffer)
    {
        free(engine->lane_line_buffer);
        engine->lane_line_buffer = NULL;
    }

    if (engine->drivable_area_buffer)
    {
        free(engine->drivable_area_buffer);
        engine->drivable_area_buffer = NULL;
    }

    yolo_close_resources(engine);

    engine->is_initialized = 0;
}

int yolo_overlay_lane_and_drivable_roi(uint8_t *rgb_data,int width,int height,
                                     const yolopv2_result_t *results,int roi_width,int roi_height)
{
    if (!rgb_data || !results || !results->lane_line_data || !results->drivable_area_data ||
        width <= 0 || height <= 0 || results->width <= 0 || results->height <= 0)
        return -1;
    float ratiow = (float)width / results->width;
    float ratioh = (float)height / results->height;
    int area = results->height * results->width;
    int same_size = width == results->width && height == results->height;
    int lane_pixel_count = 0;
    if(roi_width<0||roi_height<0||roi_width>width||roi_height>height)return -1;

    /* 将两次遍历合并为一次；
     * 尺寸一致时不做浮点除法。混色表达式与车道阈值保持原样，
     * 以维持绘制像素和弯道分类。 */
    for (int i = 0; i < roi_height; ++i)
    {
        int y = same_size ? i : (int)(i / ratioh);
        for (int j = 0; j < roi_width; ++j)
        {
            int x = same_size ? j : (int)(j / ratiow);
            if (x < 0 || x >= results->width || y < 0 || y >= results->height)
                continue;
            int mask = y * results->width + x;
            int idx = (i * width + j) * 3;
            if (results->lane_line_data[mask] > 0.3)
            {
                rgb_data[idx] = 0;
                rgb_data[idx + 1] = 0;
                rgb_data[idx + 2] = 255;
                ++lane_pixel_count;
            }
            else if (results->drivable_area_data[mask] < results->drivable_area_data[area + mask])
            {
                rgb_data[idx]=overlay_darker[rgb_data[idx]];
                rgb_data[idx+1]=overlay_greener[rgb_data[idx+1]];
                rgb_data[idx+2]=overlay_darker[rgb_data[idx+2]];
            }
        }
    }
    LANE_TRACE("绘制 %d 个红色车道线像素\n", lane_pixel_count);

    return lane_pixel_count;
}

int yolo_overlay_lane_and_drivable(uint8_t *rgb_data,int width,int height,const yolopv2_result_t *results)
{
    return yolo_overlay_lane_and_drivable_roi(rgb_data,width,height,results,width,height);
}

int yolo_analyze_annotated_curve(const uint8_t *rgb_data, int width, int height,
                                 int lane_pixel_count, YOLOCurveResult *curve)
{
    if (!rgb_data || !curve || width <= 0 || height <= 0 || lane_pixel_count < 0)
        return -1;
    // 弯道检测（基于绘制后的红色车道线）
    TurnType turn = detect_turn_direction_from_image(rgb_data, width, height);

    // 求出置信度：基于车道线像素数量。
    uint32_t confidence = 0;
    if (lane_pixel_count > 500)
    {
        confidence = 90; // 标线像素证据充分。
    }
    else if (lane_pixel_count > 300)
    {
        confidence = 75; // 标线像素证据较充分。
    }
    else if (lane_pixel_count > 100)
    {
        confidence = 60; // 标线像素证据较弱。
    }
    else
    {
        confidence = 40; // 标线像素证据不足。
    }

    // 求出角度：基于弯道类型估算。
    uint32_t angle = 0;
    uint32_t curve_type_shm = 0; // 共享内存中的弯道类型

    switch (turn)
    {
    case TURN_LEFT:
        curve_type_shm = 1; // 对应 CURVE_LEFT 的左弯类别。
        angle = 25;         // 左弯候选使用约 25 度的估计。
        break;
    case TURN_RIGHT:
        curve_type_shm = 2; // 对应 CURVE_RIGHT 的右弯类别。
        angle = 25;         // 右弯候选使用约 25 度的估计。
        break;
    case TURN_STRAIGHT:
    default:
        curve_type_shm = 0; // 对应 CURVE_NONE 的直道类别。
        angle = 0;          // 直道候选角度设为 0 度。
        break;
    }

    curve->curve_type = curve_type_shm;
    curve->confidence = confidence;
    curve->angle = angle;
    return 0;
}

void draw_lane_and_drivable(uint8_t *rgb_data, int width, int height,
                            const yolopv2_result_t *results)
{
    int lane_pixel_count = yolo_overlay_lane_and_drivable(rgb_data, width, height, results);
    YOLOCurveResult curve;
    if (yolo_analyze_annotated_curve(rgb_data, width, height, lane_pixel_count, &curve) != 0)
        return;
    static uint32_t curve_frame_id = 0;
    curve_detection_update(++curve_frame_id, (CurveType)curve.curve_type, curve.confidence,
                           curve.angle);
    LANE_TRACE("Lane result: source=%u type=%u confidence=%u angle=%u\n", curve_frame_id,
               curve.curve_type, curve.confidence, curve.angle);
}

void rgb565_to_rgb888_yolo(const uint16_t *rgb565_data, uint8_t *rgb888_data, int width, int height)
{
    if (!rgb565_data || !rgb888_data)
        return;

    const int total_pixels = width * height;
    if (total_pixels > 0)
        frame_unpack_rgb565(rgb565_data, rgb888_data, (size_t)total_pixels);
}
