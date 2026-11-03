#include "yolop_opencv_integration.h"
#include "../../../common/perception/include/rknn_model_file.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <algorithm>

using namespace std;

// 简单图像处理函数（替代OpenCV）。

// 以最近邻快速适配目标尺寸。
void fast_resize_rgb888(const uint8_t *src, int src_w, int src_h, uint8_t *dst, int dst_w,
                        int dst_h)
{
    int x_ratio = (src_w << 16) / dst_w + 1;
    int y_ratio = (src_h << 16) / dst_h + 1;

    for (int y = 0; y < dst_h; y++)
    {
        int src_y = (y * y_ratio) >> 16;
        src_y = src_y < src_h ? src_y : src_h - 1;

        for (int x = 0; x < dst_w; x++)
        {
            int src_x = (x * x_ratio) >> 16;
            src_x = src_x < src_w ? src_x : src_w - 1;

            int src_idx = (src_y * src_w + src_x) * 3;
            int dst_idx = (y * dst_w + x) * 3;

            dst[dst_idx] = src[src_idx];
            dst[dst_idx + 1] = src[src_idx + 1];
            dst[dst_idx + 2] = src[src_idx + 2];
        }
    }
}

// 计算两个候选框的交并比。
static uint8_t font_5x7[128][7];
static bool font_initialized = false;

// 为字体数据建立初始状态。
void init_font()
{
    if (font_initialized)
        return;

    // 每个支持的字符对应一条记录，未提供的 ASCII 字位保持空白。
    struct Glyph5x7
    {
        unsigned int code;
        uint8_t rows[7];
    };
    static const Glyph5x7 glyphs[] = {
        {65, {0x70, 0x88, 0x88, 0xF8, 0x88, 0x88, 0x88}},
        {67, {0x70, 0x88, 0x80, 0x80, 0x80, 0x88, 0x70}},
        {82, {0xF0, 0x88, 0x88, 0xF0, 0xA0, 0x90, 0x88}},
        {84, {0xF8, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20}},
        {85, {0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70}},
        {83, {0x70, 0x88, 0x80, 0x70, 0x08, 0x88, 0x70}},
        {66, {0xF0, 0x88, 0x88, 0xF0, 0x88, 0x88, 0xF0}},
        {73, {0x70, 0x20, 0x20, 0x20, 0x20, 0x20, 0x70}},
        {75, {0x88, 0x90, 0xA0, 0xC0, 0xA0, 0x90, 0x88}},
        {69, {0xF8, 0x80, 0x80, 0xF0, 0x80, 0x80, 0xF8}},
        {97, {0x00, 0x00, 0x70, 0x08, 0x78, 0x88, 0x78}},
        {114, {0x00, 0x00, 0xB0, 0xC8, 0x80, 0x80, 0x80}},
        {99, {0x00, 0x00, 0x70, 0x80, 0x80, 0x88, 0x70}},
        {48, {0x70, 0x88, 0x88, 0x88, 0x88, 0x88, 0x70}},
        {49, {0x20, 0x60, 0x20, 0x20, 0x20, 0x20, 0x70}},
        {50, {0x70, 0x88, 0x08, 0x10, 0x20, 0x40, 0xF8}},
        {51, {0x70, 0x88, 0x08, 0x30, 0x08, 0x88, 0x70}},
        {52, {0x10, 0x30, 0x50, 0x90, 0xF8, 0x10, 0x10}},
        {53, {0xF8, 0x80, 0xF0, 0x08, 0x08, 0x88, 0x70}},
        {54, {0x70, 0x88, 0x80, 0xF0, 0x88, 0x88, 0x70}},
        {55, {0xF8, 0x08, 0x10, 0x20, 0x40, 0x80, 0x80}},
        {56, {0x70, 0x88, 0x88, 0x70, 0x88, 0x88, 0x70}},
        {57, {0x70, 0x88, 0x88, 0x78, 0x08, 0x88, 0x70}},
        {37, {0x88, 0x90, 0x10, 0x20, 0x40, 0x48, 0x88}},
        {58, {0x00, 0x00, 0x20, 0x00, 0x00, 0x20, 0x00}},
    };
    memset(font_5x7, 0, sizeof(font_5x7));
    for (const Glyph5x7 &glyph : glyphs)
    {
        memcpy(font_5x7[glyph.code], glyph.rows, sizeof(glyph.rows));
    }
    font_initialized = true;
}

// 处理简单文字绘制的接口。
void draw_text(uint8_t *rgb_data, int img_width, int img_height, const char *text, int x, int y,
               uint8_t r, uint8_t g, uint8_t b)
{
    if (!rgb_data || !text)
        return;

    // 保证字体已初始化
    init_font();

    int char_width = 6; // 5像素宽度 + 1像素间距
    int char_height = 7;

    for (int i = 0; text[i] != '\0'; i++)
    {
        char c = text[i];
        if (c < 0 || c >= 128)
            continue; // 跳过非ASCII字符


        int start_x = x + i * char_width;
        int start_y = y;

        // 按字形位图写入字符像素。
        for (int row = 0; row < char_height; row++)
        {
            if (start_y + row >= img_height)
                break;

            uint8_t bitmap_row = font_5x7[(int)c][row];

            for (int col = 0; col < 5; col++)
            { // 字形宽 5 像素。
                if (start_x + col >= img_width)
                    break;

                // 核对该像素是否需要绘制。
                if (bitmap_row & (0x80 >> col))
                {
                    int pixel_x = start_x + col;
                    int pixel_y = start_y + row;

                    if (pixel_x >= 0 && pixel_x < img_width && pixel_y >= 0 && pixel_y < img_height)
                    {
                        int idx = (pixel_y * img_width + pixel_x) * 3;
                        rgb_data[idx] = r;
                        rgb_data[idx + 1] = g;
                        rgb_data[idx + 2] = b;
                    }
                }
            }
        }
    }
}

// YOLOP模型参数。
#define YOLOP_INPUT_WIDTH 640
#define YOLOP_INPUT_HEIGHT 640
#define YOLOP_CHANNELS 3

// YOLOP Anchor配置 (基于YOLOv5架构，优化用来交通场景)
const float ANCHORS[3][6] = {
    {3, 9, 5, 11, 4, 20},     // 8x stride - 检测小目标(行人、自行车)
    {7, 18, 6, 39, 12, 31},   // 16x stride - 检测中等目标(轿车)
    {19, 50, 38, 81, 68, 157} // 32x stride - 检测大目标(卡车、公交车)
};
const float STRIDES[3] = {8.0f, 16.0f, 32.0f};

// 本节给出目标检测相关参数。
#define CONF_THRESHOLD 0.25f
#define NMS_THRESHOLD 0.45f
#define OBJ_THRESHOLD 0.5f
#define MAX_DETECTIONS 64

// 下面的索引对应模型各输出张量。
#define OUTPUT_DET 0 // 模型目标检测分支的输出。
#define OUTPUT_DA 1  // 分割结果：可通行道路区域。
#define OUTPUT_LL 2  // 分割结果：道路标线。

// YOLOPEngine实现。

YOLOPEngine::YOLOPEngine(const string &model_path, const string &label_path)
    : ctx(0), inpWidth(YOLOP_INPUT_WIDTH), inpHeight(YOLOP_INPUT_HEIGHT), input_attrs(nullptr),
      output_attrs(nullptr), cached_seg_width(0), cached_seg_height(0)
{

    if (!loadLabels(label_path))
    {
        cerr << "[YOLOP] 加载标签文件失败: " << label_path << endl;
        return;
    }

    if (!initModel(model_path))
    {
        cerr << "[YOLOP] 初始化模型失败: " << model_path << endl;
        return;
    }

    cout << "[YOLOP] 引擎初始化成功" << endl;
}

YOLOPEngine::~YOLOPEngine()
{
    if (input_attrs)
    {
        delete[] input_attrs;
        input_attrs = nullptr;
    }
    if (output_attrs)
    {
        delete[] output_attrs;
        output_attrs = nullptr;
    }
    if (ctx)
    {
        rknn_destroy(ctx);
        ctx = 0;
    }
    cout << "[YOLOP] 引擎已销毁" << endl;
}

bool YOLOPEngine::loadLabels(const string &label_path)
{
    ifstream file(label_path);
    if (!file.is_open())
    {
        cerr << "[YOLOP ERROR] 无法打开标签文件: " << label_path << endl;
        return false;
    }

    string line;
    class_names.clear();
    while (getline(file, line))
    {
        if (!line.empty())
        {
            class_names.push_back(line);
        }
    }
    file.close();

    cout << "[YOLOP] 加载 " << class_names.size() << " 个类别标签: ";
    for (size_t i = 0; i < class_names.size(); i++)
    {
        cout << class_names[i];
        if (i < class_names.size() - 1)
            cout << ", ";
    }
    cout << endl;
    return !class_names.empty();
}

bool YOLOPEngine::initModel(const string &model_path)
{
    // 加载RKNN模型
    FILE *fp = fopen(model_path.c_str(), "rb");
    if (!fp)
    {
        cerr << "[YOLOP] 无法打开模型文件: " << model_path << endl;
        return false;
    }

    size_t model_size;
    void *model_data = read_rknn_model(fp, &model_size, 0);
    if (!model_data)
        return false;

    // 为RKNN上下文建立初始状态。
    int ret = rknn_init(&ctx, model_data, model_size, 0, nullptr);
    free(model_data);

    if (ret != RKNN_SUCC)
    {
        cerr << "[YOLOP] rknn_init失败: " << ret << endl;
        return false;
    }

    // 查询输入输出数量
    ret = rknn_query(ctx, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num));
    if (ret != RKNN_SUCC)
    {
        cerr << "[YOLOP] 查询IO数量失败" << endl;
        return false;
    }

    cout << "[YOLOP] 模型输入数: " << io_num.n_input << ", 输出数: " << io_num.n_output << endl;

    // 读取模型各输入张量的描述。
    input_attrs = new rknn_tensor_attr[io_num.n_input];
    memset(input_attrs, 0, sizeof(rknn_tensor_attr) * io_num.n_input);
    for (uint32_t i = 0; i < io_num.n_input; i++)
    {
        input_attrs[i].index = i;
        ret = rknn_query(ctx, RKNN_QUERY_INPUT_ATTR, &input_attrs[i], sizeof(rknn_tensor_attr));
        if (ret != RKNN_SUCC)
        {
            cerr << "[YOLOP] 查询输入属性失败" << endl;
            return false;
        }
    }

    // 读取模型各输出张量的描述。
    output_attrs = new rknn_tensor_attr[io_num.n_output];
    memset(output_attrs, 0, sizeof(rknn_tensor_attr) * io_num.n_output);
    for (uint32_t i = 0; i < io_num.n_output; i++)
    {
        output_attrs[i].index = i;
        ret = rknn_query(ctx, RKNN_QUERY_OUTPUT_ATTR, &output_attrs[i], sizeof(rknn_tensor_attr));
        if (ret != RKNN_SUCC)
        {
            cerr << "[YOLOP] 查询输出属性失败" << endl;
            return false;
        }

        // 打印输出维度信息
        cout << "[YOLOP] 输出[" << i << "] 维度: ";
        for (uint32_t j = 0; j < output_attrs[i].n_dims; j++)
        {
            cout << output_attrs[i].dims[j];
            if (j < output_attrs[i].n_dims - 1)
                cout << "x";
        }
        cout << ", 元素数: " << output_attrs[i].n_elems << endl;
    }

    return true;
}

void YOLOPEngine::preprocessImage(const uint8_t *rgb_data, int width, int height,
                                  vector<uint8_t> &input_data)
{
    input_data.resize(inpWidth * inpHeight * YOLOP_CHANNELS);

    if (width == inpWidth && height == inpHeight)
    {
        // 尺寸匹配，直接拷贝
        memcpy(input_data.data(), rgb_data, input_data.size());
    }
    else
    {
        // 采用快速路径适配图像尺寸。
        fast_resize_rgb888(rgb_data, width, height, input_data.data(), inpWidth, inpHeight);
    }
}

namespace
{
struct CandidateBox
{
    int x, y, width, height;
    float confidence;
    int class_id;
};

static float candidate_iou(const CandidateBox &a, const CandidateBox &b)
{
    int x1 = max(a.x, b.x);
    int y1 = max(a.y, b.y);
    int x2 = min(a.x + a.width, b.x + b.width);
    int y2 = min(a.y + a.height, b.y + b.height);

    if (x2 <= x1 || y2 <= y1)
        return 0.0f;

    int intersection = (x2 - x1) * (y2 - y1);
    int area_a = a.width * a.height;
    int area_b = b.width * b.height;
    int union_area = area_a + area_b - intersection;

    return (float)intersection / union_area;
}

// 按置信度排序。重叠抑制对所有类别统一执行。
vector<int> select_candidates(const vector<CandidateBox> &candidates)
{
    vector<int> order(candidates.size());
    for (size_t i = 0; i < order.size(); ++i)
        order[i] = i;

    sort(order.begin(), order.end(), [&](int a, int b) {
        return candidates[a].confidence > candidates[b].confidence;
    });

    vector<bool> suppressed(candidates.size(), false);
    vector<int> selected;
    for (size_t i = 0; i < order.size(); ++i)
    {
        const int winner = order[i];
        if (suppressed[winner])
            continue;

        selected.push_back(winner);
        for (size_t j = i + 1; j < order.size(); ++j)
        {
            const int next = order[j];
            if (!suppressed[next] && candidate_iou(candidates[winner], candidates[next]) > NMS_THRESHOLD)
                suppressed[next] = true;
        }
    }
    return selected;
}
}

void YOLOPEngine::postprocessDetection(rknn_output *outputs, int img_width, int img_height,
                                       detect_result_group_t *results)
{
    if (!outputs || !results)
        return;

    results->count = 0;

    // 读取检测输出（输出0包含所有3个尺度的数据）。
    float *pdata = (float *)outputs[0].buf;
    if (!pdata)
    {
        cerr << "[YOLOP] 检测输出为空!" << endl;
        return;
    }

    vector<CandidateBox> candidates;

    // 求出缩放比例（假设没有padding，直接缩放）。
    float scale_x = (float)img_width / inpWidth;
    float scale_y = (float)img_height / inpHeight;

    int num_classes = (int)class_names.size();
    int nout = num_classes + 5; // x,y,w,h,conf + classes

    cout << "[YOLOP] 开始解析YOLOP检测输出，YOLOP类别数=" << num_classes << ", nout=" << nout
         << endl;

    // 遍历三个尺度的输出（8x, 16x, 32x stride）
    for (int n = 0; n < 3; n++)
    {
        const int num_grid_x = (int)(inpWidth / STRIDES[n]);
        const int num_grid_y = (int)(inpHeight / STRIDES[n]);
        const float current_stride = STRIDES[n];

        cout << "[YOLOP] 处理尺度 " << n << ": stride=" << current_stride << ", grid=" << num_grid_x
             << "x" << num_grid_y << endl;

        // 遍历3个anchor
        for (int q = 0; q < 3; q++)
        {
            const float anchor_w = ANCHORS[n][q * 2];
            const float anchor_h = ANCHORS[n][q * 2 + 1];

            // 按网格位置逐项扫描。
            for (int i = 0; i < num_grid_y; i++)
            {
                for (int j = 0; j < num_grid_x; ++j, pdata += nout)
                {
                    const float box_score = pdata[4];
                    if (box_score <= OBJ_THRESHOLD)
                        continue;

                    // 找到最大类别分数
                    float max_class_score = pdata[5];
                    int class_idx = 0;
                    for (int k = 1; k < num_classes; k++)
                    {
                        if (pdata[5 + k] > max_class_score)
                        {
                            max_class_score = pdata[5 + k];
                            class_idx = k;
                        }
                    }

                    const float final_score = max_class_score * box_score;

                    if (final_score > CONF_THRESHOLD)
                    {
                        // YOLOP边界框解码（使用YOLOv5格式）
                        float cx = (pdata[0] * 2.0f - 0.5f + j) * current_stride;
                        float cy = (pdata[1] * 2.0f - 0.5f + i) * current_stride;
                        float w = powf(pdata[2] * 2.0f, 2.0f) * anchor_w;
                        float h = powf(pdata[3] * 2.0f, 2.0f) * anchor_h;

                        // 将模型坐标映射回采集帧。
                        int left = (int)((cx - 0.5f * w) * scale_x);
                        int top = (int)((cy - 0.5f * h) * scale_y);
                        int width = (int)(w * scale_x);
                        int height = (int)(h * scale_y);

                        candidates.push_back({left, top, width, height, final_score, class_idx});
                    }
                }
            }
        }
    }

    cout << "[YOLOP] 检测到 " << candidates.size() << " 个候选框" << endl;

    const vector<int> indices = select_candidates(candidates);

    cout << "[YOLOP] NMS后保留 " << indices.size() << " 个目标" << endl;

    // 按抑制后的顺序填充输出列表。
    for (size_t i = 0; i < indices.size(); i++)
    {
        if (results->count >= OBJ_NUMB_MAX_SIZE)
            break;
        const CandidateBox &candidate = candidates[indices[i]];
        detect_result_t &obj = results->results[results->count];

        obj.box.left = max(candidate.x, 0);
        obj.box.top = max(candidate.y, 0);
        obj.box.right = min(candidate.x + candidate.width, img_width - 1);
        obj.box.bottom = min(candidate.y + candidate.height, img_height - 1);
        obj.prop = candidate.confidence;

        const int class_id = candidate.class_id;
        if (class_id >= 0 && class_id < (int)class_names.size())
        {
            strncpy(obj.name, class_names[class_id].c_str(), sizeof(obj.name) - 1);
            obj.name[sizeof(obj.name) - 1] = '\0';
        }
        else
        {
            snprintf(obj.name, sizeof(obj.name), "class_%d", class_id);
            cerr << "[YOLOP WARNING] 检测到无效类别ID: " << class_id
                 << ", 类别总数: " << class_names.size() << endl;
        }

        cout << "[YOLOP] 检测结果 #" << i << ": " << obj.name << " (类别ID=" << class_id
             << ", 置信度=" << obj.prop << ")" << endl;

        results->count++;
    }
}

void YOLOPEngine::drawBoundingBoxes(uint8_t *rgb_data, int width, int height,
                                    const detect_result_group_t *results)
{
    if (!rgb_data || !results)
        return;

    // 简化绘制：直接在RGB888数据上绘制红色边界框
    for (int i = 0; i < results->count; i++)
    {
        const detect_result_t &obj = results->results[i];

        const auto edges = yolop_overlay::draw_outline(rgb_data, width, height, obj.box, 255, 0);

        // 绘制简单的类别标记（数字ID或固定文字）
        char simple_label[16];

        // 根据YOLOP交通场景类别显示标签
        if (strstr(obj.name, "car") || strstr(obj.name, "CAR"))
        {
            strcpy(simple_label, "CAR");
        }
        else if (strstr(obj.name, "truck") || strstr(obj.name, "TRUCK"))
        {
            strcpy(simple_label, "TRUCK");
        }
        else if (strstr(obj.name, "bus") || strstr(obj.name, "BUS"))
        {
            strcpy(simple_label, "BUS");
        }
        else if (strstr(obj.name, "person") || strstr(obj.name, "PERSON"))
        {
            strcpy(simple_label, "PERSON");
        }
        else if (strstr(obj.name, "bike") || strstr(obj.name, "BIKE") ||
                 strstr(obj.name, "bicycle") || strstr(obj.name, "motorcycle"))
        {
            strcpy(simple_label, "BIKE");
        }
        else
        {
            // 对于其他交通相关目标，显示置信度百分比
            int confidence = (int)(obj.prop * 100);
            snprintf(simple_label, sizeof(simple_label), "%d%%", confidence);
        }

        // 绘制标签背景（黑色矩形）
        int label_x = edges.left;
        int label_y = (edges.top > 10) ? edges.top - 10 : edges.top + 15;      // 计算标签在图像中的放置位置。
        int label_width = (int)strlen(simple_label) * 6 + 4; // 6像素每字符 + 边距
        int label_height = 9;                                // 字体高度 + 边距

        // 先铺设文字标签的背景区域。
        for (int ly = label_y; ly < label_y + label_height && ly < height; ly++)
        {
            for (int lx = label_x; lx < label_x + label_width && lx < width; lx++)
            {
                if (lx >= 0 && ly >= 0)
                {
                    int bg_idx = (ly * width + lx) * 3;
                    rgb_data[bg_idx] = 0; // 标签底色取黑色。
                    rgb_data[bg_idx + 1] = 0;
                    rgb_data[bg_idx + 2] = 0;
                }
            }
        }

        // 在背景上写入白色字符。
        draw_text(rgb_data, width, height, simple_label, label_x + 2, label_y + 1, 255, 255,
                  255); // 标签字色取白色。
    }
}

void YOLOPEngine::drawSegmentation(uint8_t *rgb_data, int width, int height)
{
    if (!rgb_data || cached_drivable_seg.empty() || cached_lane_seg.empty())
    {
        return;
    }

    const float scale_x = (float)cached_seg_width / width;
    const float scale_y = (float)cached_seg_height / height;
    const int area = cached_seg_width * cached_seg_height;
    const int step = 2;

    // 每个采样点覆盖互不重叠的 2×2 像素块。车道线覆盖通行区底色。
    for (int y = 0; y < height; y += step)
    {
        for (int x = 0; x < width; x += step)
        {
            const int sx = (int)(x * scale_x);
            const int sy = (int)(y * scale_y);
            if (sx >= cached_seg_width || sy >= cached_seg_height)
                continue;

            const int index = sy * cached_seg_width + sx;
            const bool lane = cached_lane_seg[index] < cached_lane_seg[area + index];
            const bool drivable = cached_drivable_seg[index] < cached_drivable_seg[area + index];
            if (!lane && !drivable)
                continue;

            for (int row = y; row < y + step && row < height; ++row)
            {
                for (int col = x; col < x + step && col < width; ++col)
                {
                    uint8_t *pixel = rgb_data + (row * width + col) * 3;
                    if (lane)
                    {
                        pixel[0] = 0;
                        pixel[1] = 0;
                        pixel[2] = 255;
                    }
                    else
                    {
                        pixel[0] = (uint8_t)(pixel[0] * 0.7);
                        pixel[1] = (uint8_t)(pixel[1] * 0.7 + 255 * 0.3);
                        pixel[2] = (uint8_t)(pixel[2] * 0.7);
                    }
                }
            }
        }
    }

    cout << "[YOLOP] 优化绘制完成（绿色=可行驶区域，荧光红=车道线，步长=" << step << "）" << endl;
}

bool YOLOPEngine::runInference(const uint8_t *rgb_data, int width, int height,
                              vector<uint8_t> &input_data, bool verbose)
{
    preprocessImage(rgb_data, width, height, input_data);

    if (verbose)
        cout << "[YOLOP] 预处理完成，输入数据大小: " << input_data.size() << " bytes" << endl;

    rknn_input input = {};
    input.type = RKNN_TENSOR_UINT8;
    input.size = input_data.size();
    input.fmt = RKNN_TENSOR_NHWC;
    input.buf = input_data.data();

    int ret = rknn_inputs_set(ctx, io_num.n_input, &input);
    if (ret != RKNN_SUCC)
    {
        if (verbose)
            cerr << "[YOLOP] 设置输入失败: " << ret << endl;
        return false;
    }

    if (verbose)
        cout << "[YOLOP] 开始推理..." << endl;

    ret = rknn_run(ctx, nullptr);
    if (ret != RKNN_SUCC && verbose)
        cerr << "[YOLOP] 推理失败: " << ret << endl;
    return ret == RKNN_SUCC;
}

int YOLOPEngine::detect(const uint8_t *rgb_data, int width, int height,
                        detect_result_group_t *results)
{
    if (!rgb_data || !results || !isValid())
    {
        return -1;
    }

    cout << "[YOLOP] 开始检测，图像尺寸: " << width << "x" << height << endl;

    vector<uint8_t> input_data;
    if (!runInference(rgb_data, width, height, input_data, true))
        return -1;

    cout << "[YOLOP] 推理完成，获取输出..." << endl;

    // 根据当前输出数量申请数组。
    rknn_output *outputs = new rknn_output[io_num.n_output];
    memset(outputs, 0, sizeof(rknn_output) * io_num.n_output);
    for (uint32_t i = 0; i < io_num.n_output; i++)
    {
        outputs[i].want_float = 1; // 要求运行时返回浮点格式的输出。
    }

    int ret = rknn_outputs_get(ctx, io_num.n_output, outputs, nullptr);
    if (ret != RKNN_SUCC)
    {
        cerr << "[YOLOP] 获取输出失败: " << ret << endl;
        delete[] outputs;
        return -1;
    }

    cout << "[YOLOP] 输出获取成功，共 " << io_num.n_output << " 个输出" << endl;

    // 打印输出信息以供调试
    for (uint32_t i = 0; i < io_num.n_output; i++)
    {
        cout << "  输出[" << i << "] 大小: " << outputs[i].size
             << " bytes, 是否有效: " << (outputs[i].buf != nullptr) << endl;
    }

    // 把模型张量转换为调用方使用的检测列表。
    postprocessDetection(outputs, width, height, results);

    // 把分割输出用于后续绘制保存在本地。
    if (io_num.n_output >= 3 && outputs[1].buf && outputs[2].buf)
    {
        cached_seg_width = inpWidth;
        cached_seg_height = inpHeight;

        // 把可行驶区域分割（output[1]）保存在本地。
        int seg_size = inpWidth * inpHeight * 2; // 2通道
        float *drivable_data = (float *)outputs[1].buf;
        cached_drivable_seg.assign(drivable_data, drivable_data + seg_size);

        // 把车道线分割（output[2]）保存在本地。
        float *lane_data = (float *)outputs[2].buf;
        cached_lane_seg.assign(lane_data, lane_data + seg_size);

        cout << "[YOLOP] 已缓存分割输出，尺寸: " << cached_seg_width << "x" << cached_seg_height
             << endl;
    }

    // 解码完成后向 RKNN 归还输出缓冲区。
    rknn_outputs_release(ctx, io_num.n_output, outputs);
    delete[] outputs;

    cout << "[YOLOP] 检测完成，找到 " << results->count << " 个目标" << endl;

    return 0;
}

void YOLOPEngine::drawResults(uint8_t *rgb_data, int width, int height,
                              const detect_result_group_t *results)
{
#ifdef PURE_CAPTURE_MODE
    // 纯采集模式：完全跳过所有绘制操作
    return;
#else
    if (!rgb_data || !results)
        return;

    // 先把分割掩码叠加到源图。
    drawSegmentation(rgb_data, width, height);

    // 检测框覆盖分割底图。
    drawBoundingBoxes(rgb_data, width, height, results);

    cout << "[YOLOP] 已绘制完整结果（分割+检测框）" << endl;
#endif
}

// 添加纯检测模式（无绘制，提高性能）
int YOLOPEngine::detectOnly(const uint8_t *rgb_data, int width, int height,
                            detect_result_group_t *results)
{
    if (!rgb_data || !results || !isValid())
    {
        return -1;
    }

    vector<uint8_t> input_data;
    if (!runInference(rgb_data, width, height, input_data, false))
        return -1;

    // 纯检测模式只请求编号为 0 的输出张量。
    rknn_output outputs[1];
    memset(outputs, 0, sizeof(outputs));
    outputs[0].want_float = 1;
    outputs[0].index = 0; // 本路径只取得目标检测输出。

    int ret = rknn_outputs_get(ctx, 1, outputs, nullptr);
    if (ret != RKNN_SUCC)
        return -1;

    // 把模型张量转换为调用方使用的检测列表。
    postprocessDetection(outputs, width, height, results);

    // 解码完成后向 RKNN 归还输出缓冲区。
    rknn_outputs_release(ctx, 1, outputs);

    return 0;
}
