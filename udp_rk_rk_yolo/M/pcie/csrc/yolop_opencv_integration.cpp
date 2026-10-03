#include "yolop_opencv_integration.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <algorithm>

using namespace std;

// ===== 简单图像处理函数（替代OpenCV）=====

// 简单矩形结构
struct SimpleRect {
    int x, y, width, height;
    SimpleRect() : x(0), y(0), width(0), height(0) {}
    SimpleRect(int x_, int y_, int w_, int h_) : x(x_), y(y_), width(w_), height(h_) {}
};

// 快速最近邻缩放
void fast_resize_rgb888(const uint8_t* src, int src_w, int src_h,
                        uint8_t* dst, int dst_w, int dst_h) {
    int x_ratio = (src_w << 16) / dst_w + 1;
    int y_ratio = (src_h << 16) / dst_h + 1;
    
    for (int y = 0; y < dst_h; y++) {
        int src_y = (y * y_ratio) >> 16;
        src_y = src_y < src_h ? src_y : src_h - 1;
        
        for (int x = 0; x < dst_w; x++) {
            int src_x = (x * x_ratio) >> 16;
            src_x = src_x < src_w ? src_x : src_w - 1;
            
            int src_idx = (src_y * src_w + src_x) * 3;
            int dst_idx = (y * dst_w + x) * 3;
            
            dst[dst_idx] = src[src_idx];         // R
            dst[dst_idx + 1] = src[src_idx + 1]; // G
            dst[dst_idx + 2] = src[src_idx + 2]; // B
        }
    }
}

// 计算IoU
float calculate_iou(const SimpleRect& a, const SimpleRect& b) {
    int x1 = max(a.x, b.x);
    int y1 = max(a.y, b.y);
    int x2 = min(a.x + a.width, b.x + b.width);
    int y2 = min(a.y + a.height, b.y + b.height);
    
    if (x2 <= x1 || y2 <= y1) return 0.0f;
    
    int intersection = (x2 - x1) * (y2 - y1);
    int area_a = a.width * a.height;
    int area_b = b.width * b.height;
    int union_area = area_a + area_b - intersection;
    
    return (float)intersection / union_area;
}

// 简化NMS实现
void simple_nms(vector<SimpleRect>& boxes, vector<float>& confidences, 
                vector<int>& class_ids, vector<int>& indices, 
                float nms_threshold) {
    // 按置信度排序
    vector<int> sorted_indices(confidences.size());
    for (size_t i = 0; i < confidences.size(); i++) {
        sorted_indices[i] = i;
    }
    
    sort(sorted_indices.begin(), sorted_indices.end(),
         [&](int a, int b) { return confidences[a] > confidences[b]; });
    
    vector<bool> suppressed(boxes.size(), false);
    
    for (size_t i = 0; i < sorted_indices.size(); i++) {
        int idx = sorted_indices[i];
        if (suppressed[idx]) continue;
        
        indices.push_back(idx);
        
        // 抑制重叠框
        for (size_t j = i + 1; j < sorted_indices.size(); j++) {
            int idx2 = sorted_indices[j];
            if (suppressed[idx2]) continue;
            
            if (calculate_iou(boxes[idx], boxes[idx2]) > nms_threshold) {
                suppressed[idx2] = true;
            }
        }
    }
}

// 简单的5x7像素字体（位图字体）
static uint8_t font_5x7[128][7];
static bool font_initialized = false;

// 初始化字体数据
void init_font() {
    if (font_initialized) return;
    
    // 清零所有字符
    memset(font_5x7, 0, sizeof(font_5x7));
    
    // 大写字母 A
    font_5x7[65][0] = 0x70; font_5x7[65][1] = 0x88; font_5x7[65][2] = 0x88; font_5x7[65][3] = 0xF8;
    font_5x7[65][4] = 0x88; font_5x7[65][5] = 0x88; font_5x7[65][6] = 0x88;
    // 字母 C
    font_5x7[67][0] = 0x70; font_5x7[67][1] = 0x88; font_5x7[67][2] = 0x80; font_5x7[67][3] = 0x80;
    font_5x7[67][4] = 0x80; font_5x7[67][5] = 0x88; font_5x7[67][6] = 0x70;
    // 字母 R
    font_5x7[82][0] = 0xF0; font_5x7[82][1] = 0x88; font_5x7[82][2] = 0x88; font_5x7[82][3] = 0xF0;
    font_5x7[82][4] = 0xA0; font_5x7[82][5] = 0x90; font_5x7[82][6] = 0x88;
    // 字母 T
    font_5x7[84][0] = 0xF8; font_5x7[84][1] = 0x20; font_5x7[84][2] = 0x20; font_5x7[84][3] = 0x20;
    font_5x7[84][4] = 0x20; font_5x7[84][5] = 0x20; font_5x7[84][6] = 0x20;
    // 字母 U
    font_5x7[85][0] = 0x88; font_5x7[85][1] = 0x88; font_5x7[85][2] = 0x88; font_5x7[85][3] = 0x88;
    font_5x7[85][4] = 0x88; font_5x7[85][5] = 0x88; font_5x7[85][6] = 0x70;
    // 字母 S
    font_5x7[83][0] = 0x70; font_5x7[83][1] = 0x88; font_5x7[83][2] = 0x80; font_5x7[83][3] = 0x70;
    font_5x7[83][4] = 0x08; font_5x7[83][5] = 0x88; font_5x7[83][6] = 0x70;
    // 字母 B
    font_5x7[66][0] = 0xF0; font_5x7[66][1] = 0x88; font_5x7[66][2] = 0x88; font_5x7[66][3] = 0xF0;
    font_5x7[66][4] = 0x88; font_5x7[66][5] = 0x88; font_5x7[66][6] = 0xF0;
    // 字母 I
    font_5x7[73][0] = 0x70; font_5x7[73][1] = 0x20; font_5x7[73][2] = 0x20; font_5x7[73][3] = 0x20;
    font_5x7[73][4] = 0x20; font_5x7[73][5] = 0x20; font_5x7[73][6] = 0x70;
    // 字母 K
    font_5x7[75][0] = 0x88; font_5x7[75][1] = 0x90; font_5x7[75][2] = 0xA0; font_5x7[75][3] = 0xC0;
    font_5x7[75][4] = 0xA0; font_5x7[75][5] = 0x90; font_5x7[75][6] = 0x88;
    // 字母 E
    font_5x7[69][0] = 0xF8; font_5x7[69][1] = 0x80; font_5x7[69][2] = 0x80; font_5x7[69][3] = 0xF0;
    font_5x7[69][4] = 0x80; font_5x7[69][5] = 0x80; font_5x7[69][6] = 0xF8;
    
    // 小写字母
    // 字母 a
    font_5x7[97][0] = 0x00; font_5x7[97][1] = 0x00; font_5x7[97][2] = 0x70; font_5x7[97][3] = 0x08;
    font_5x7[97][4] = 0x78; font_5x7[97][5] = 0x88; font_5x7[97][6] = 0x78;
    // 字母 r
    font_5x7[114][0] = 0x00; font_5x7[114][1] = 0x00; font_5x7[114][2] = 0xB0; font_5x7[114][3] = 0xC8;
    font_5x7[114][4] = 0x80; font_5x7[114][5] = 0x80; font_5x7[114][6] = 0x80;
    // 字母 c
    font_5x7[99][0] = 0x00; font_5x7[99][1] = 0x00; font_5x7[99][2] = 0x70; font_5x7[99][3] = 0x80;
    font_5x7[99][4] = 0x80; font_5x7[99][5] = 0x88; font_5x7[99][6] = 0x70;
    
    // 数字 0-9
    font_5x7[48][0] = 0x70; font_5x7[48][1] = 0x88; font_5x7[48][2] = 0x88; font_5x7[48][3] = 0x88;
    font_5x7[48][4] = 0x88; font_5x7[48][5] = 0x88; font_5x7[48][6] = 0x70;
    font_5x7[49][0] = 0x20; font_5x7[49][1] = 0x60; font_5x7[49][2] = 0x20; font_5x7[49][3] = 0x20;
    font_5x7[49][4] = 0x20; font_5x7[49][5] = 0x20; font_5x7[49][6] = 0x70;
    font_5x7[50][0] = 0x70; font_5x7[50][1] = 0x88; font_5x7[50][2] = 0x08; font_5x7[50][3] = 0x10;
    font_5x7[50][4] = 0x20; font_5x7[50][5] = 0x40; font_5x7[50][6] = 0xF8;
    font_5x7[51][0] = 0x70; font_5x7[51][1] = 0x88; font_5x7[51][2] = 0x08; font_5x7[51][3] = 0x30;
    font_5x7[51][4] = 0x08; font_5x7[51][5] = 0x88; font_5x7[51][6] = 0x70;
    font_5x7[52][0] = 0x10; font_5x7[52][1] = 0x30; font_5x7[52][2] = 0x50; font_5x7[52][3] = 0x90;
    font_5x7[52][4] = 0xF8; font_5x7[52][5] = 0x10; font_5x7[52][6] = 0x10;
    font_5x7[53][0] = 0xF8; font_5x7[53][1] = 0x80; font_5x7[53][2] = 0xF0; font_5x7[53][3] = 0x08;
    font_5x7[53][4] = 0x08; font_5x7[53][5] = 0x88; font_5x7[53][6] = 0x70;
    font_5x7[54][0] = 0x70; font_5x7[54][1] = 0x88; font_5x7[54][2] = 0x80; font_5x7[54][3] = 0xF0;
    font_5x7[54][4] = 0x88; font_5x7[54][5] = 0x88; font_5x7[54][6] = 0x70;
    font_5x7[55][0] = 0xF8; font_5x7[55][1] = 0x08; font_5x7[55][2] = 0x10; font_5x7[55][3] = 0x20;
    font_5x7[55][4] = 0x40; font_5x7[55][5] = 0x80; font_5x7[55][6] = 0x80;
    font_5x7[56][0] = 0x70; font_5x7[56][1] = 0x88; font_5x7[56][2] = 0x88; font_5x7[56][3] = 0x70;
    font_5x7[56][4] = 0x88; font_5x7[56][5] = 0x88; font_5x7[56][6] = 0x70;
    font_5x7[57][0] = 0x70; font_5x7[57][1] = 0x88; font_5x7[57][2] = 0x88; font_5x7[57][3] = 0x78;
    font_5x7[57][4] = 0x08; font_5x7[57][5] = 0x88; font_5x7[57][6] = 0x70;
    
    // 特殊字符
    // 空格 (32) - 已经被memset清零了
    // 百分号 %
    font_5x7[37][0] = 0x88; font_5x7[37][1] = 0x90; font_5x7[37][2] = 0x10; font_5x7[37][3] = 0x20;
    font_5x7[37][4] = 0x40; font_5x7[37][5] = 0x48; font_5x7[37][6] = 0x88;
    // 冒号 :
    font_5x7[58][0] = 0x00; font_5x7[58][1] = 0x00; font_5x7[58][2] = 0x20; font_5x7[58][3] = 0x00;
    font_5x7[58][4] = 0x00; font_5x7[58][5] = 0x20; font_5x7[58][6] = 0x00;
    
    font_initialized = true;
}

// 简单文字绘制函数
void draw_text(uint8_t* rgb_data, int img_width, int img_height,
               const char* text, int x, int y, 
               uint8_t r, uint8_t g, uint8_t b) {
    if (!rgb_data || !text) return;
    
    // 确保字体已初始化
    init_font();
    
    int char_width = 6;  // 5像素宽度 + 1像素间距
    int char_height = 7;
    
    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];
        if (c < 0 || c >= 128) continue;  // 跳过非ASCII字符
        
        // 计算字符起始位置
        int start_x = x + i * char_width;
        int start_y = y;
        
        // 绘制字符
        for (int row = 0; row < char_height; row++) {
            if (start_y + row >= img_height) break;
            
            uint8_t bitmap_row = font_5x7[(int)c][row];
            
            for (int col = 0; col < 5; col++) {  // 5像素宽度
                if (start_x + col >= img_width) break;
                
                // 检查该像素是否需要绘制
                if (bitmap_row & (0x80 >> col)) {
                    int pixel_x = start_x + col;
                    int pixel_y = start_y + row;
                    
                    if (pixel_x >= 0 && pixel_x < img_width && 
                        pixel_y >= 0 && pixel_y < img_height) {
                        int idx = (pixel_y * img_width + pixel_x) * 3;
                        rgb_data[idx] = r;     // R
                        rgb_data[idx + 1] = g; // G  
                        rgb_data[idx + 2] = b; // B
                    }
                }
            }
        }
    }
}

// ===== YOLOP模型参数 =====
#define YOLOP_INPUT_WIDTH  640
#define YOLOP_INPUT_HEIGHT 640
#define YOLOP_CHANNELS     3

// YOLOP Anchor配置 (基于YOLOv5架构，优化用于交通场景)
const float ANCHORS[3][6] = {
    {3, 9, 5, 11, 4, 20},      // 8x stride - 检测小目标(行人、自行车)
    {7, 18, 6, 39, 12, 31},    // 16x stride - 检测中等目标(轿车)
    {19, 50, 38, 81, 68, 157}  // 32x stride - 检测大目标(卡车、公交车)
};
const float STRIDES[3] = {8.0f, 16.0f, 32.0f};

// 检测参数
#define CONF_THRESHOLD  0.25f
#define NMS_THRESHOLD   0.45f
#define OBJ_THRESHOLD   0.5f
#define MAX_DETECTIONS  64

// 输出索引
#define OUTPUT_DET      0  // 检测输出
#define OUTPUT_DA       1  // 可行驶区域
#define OUTPUT_LL       2  // 车道线

// ===== YOLOPEngine实现 =====

YOLOPEngine::YOLOPEngine(const string& model_path, const string& label_path)
    : ctx(0), inpWidth(YOLOP_INPUT_WIDTH), inpHeight(YOLOP_INPUT_HEIGHT),
      input_attrs(nullptr), output_attrs(nullptr),
      cached_seg_width(0), cached_seg_height(0) {
    
    if (!loadLabels(label_path)) {
        cerr << "[YOLOP] 加载标签文件失败: " << label_path << endl;
        return;
    }
    
    if (!initModel(model_path)) {
        cerr << "[YOLOP] 初始化模型失败: " << model_path << endl;
        return;
    }
    
    cout << "[YOLOP] 引擎初始化成功" << endl;
}

YOLOPEngine::~YOLOPEngine() {
    if (input_attrs) {
        delete[] input_attrs;
        input_attrs = nullptr;
    }
    if (output_attrs) {
        delete[] output_attrs;
        output_attrs = nullptr;
    }
    if (ctx) {
        rknn_destroy(ctx);
        ctx = 0;
    }
    cout << "[YOLOP] 引擎已销毁" << endl;
}

bool YOLOPEngine::loadLabels(const string& label_path) {
    ifstream file(label_path);
    if (!file.is_open()) {
        cerr << "[YOLOP ERROR] 无法打开标签文件: " << label_path << endl;
        return false;
    }
    
    string line;
    class_names.clear();
    while (getline(file, line)) {
        if (!line.empty()) {
            class_names.push_back(line);
        }
    }
    file.close();
    
    cout << "[YOLOP] 加载 " << class_names.size() << " 个类别标签: ";
    for (size_t i = 0; i < class_names.size(); i++) {
        cout << class_names[i];
        if (i < class_names.size() - 1) cout << ", ";
    }
    cout << endl;
    return !class_names.empty();
}

bool YOLOPEngine::initModel(const string& model_path) {
    // 加载RKNN模型
    FILE* fp = fopen(model_path.c_str(), "rb");
    if (!fp) {
        cerr << "[YOLOP] 无法打开模型文件: " << model_path << endl;
        return false;
    }
    
    fseek(fp, 0, SEEK_END);
    size_t model_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    void* model_data = malloc(model_size);
    if (!model_data) {
        fclose(fp);
        return false;
    }
    
    if (fread(model_data, 1, model_size, fp) != model_size) {
        free(model_data);
        fclose(fp);
        return false;
    }
    fclose(fp);
    
    // 初始化RKNN上下文
    int ret = rknn_init(&ctx, model_data, model_size, 0, nullptr);
    free(model_data);
    
    if (ret != RKNN_SUCC) {
        cerr << "[YOLOP] rknn_init失败: " << ret << endl;
        return false;
    }
    
    // 查询输入输出数量
    ret = rknn_query(ctx, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num));
    if (ret != RKNN_SUCC) {
        cerr << "[YOLOP] 查询IO数量失败" << endl;
        return false;
    }
    
    cout << "[YOLOP] 模型输入数: " << io_num.n_input 
         << ", 输出数: " << io_num.n_output << endl;
    
    // 查询输入属性
    input_attrs = new rknn_tensor_attr[io_num.n_input];
    memset(input_attrs, 0, sizeof(rknn_tensor_attr) * io_num.n_input);
    for (uint32_t i = 0; i < io_num.n_input; i++) {
        input_attrs[i].index = i;
        ret = rknn_query(ctx, RKNN_QUERY_INPUT_ATTR, &input_attrs[i], sizeof(rknn_tensor_attr));
        if (ret != RKNN_SUCC) {
            cerr << "[YOLOP] 查询输入属性失败" << endl;
            return false;
        }
    }
    
    // 查询输出属性
    output_attrs = new rknn_tensor_attr[io_num.n_output];
    memset(output_attrs, 0, sizeof(rknn_tensor_attr) * io_num.n_output);
    for (uint32_t i = 0; i < io_num.n_output; i++) {
        output_attrs[i].index = i;
        ret = rknn_query(ctx, RKNN_QUERY_OUTPUT_ATTR, &output_attrs[i], sizeof(rknn_tensor_attr));
        if (ret != RKNN_SUCC) {
            cerr << "[YOLOP] 查询输出属性失败" << endl;
            return false;
        }
        
        // 打印输出维度信息
        cout << "[YOLOP] 输出[" << i << "] 维度: ";
        for (uint32_t j = 0; j < output_attrs[i].n_dims; j++) {
            cout << output_attrs[i].dims[j];
            if (j < output_attrs[i].n_dims - 1) cout << "x";
        }
        cout << ", 元素数: " << output_attrs[i].n_elems << endl;
    }
    
    return true;
}

void YOLOPEngine::preprocessImage(const uint8_t* rgb_data, int width, int height,
                                   vector<uint8_t>& input_data) {
    input_data.resize(inpWidth * inpHeight * YOLOP_CHANNELS);
    
    if (width == inpWidth && height == inpHeight) {
        // 尺寸匹配，直接拷贝
        memcpy(input_data.data(), rgb_data, input_data.size());
    } else {
        // 快速缩放
        fast_resize_rgb888(rgb_data, width, height,
                          input_data.data(), inpWidth, inpHeight);
    }
}

void YOLOPEngine::postprocessDetection(rknn_output* outputs, int img_width, int img_height,
                                       detect_result_group_t* results) {
    if (!outputs || !results) return;
    
    results->count = 0;
    
    // 获取检测输出（输出0包含所有3个尺度的数据）
    float* pdata = (float*)outputs[0].buf;
    if (!pdata) {
        cerr << "[YOLOP] 检测输出为空!" << endl;
        return;
    }
    
    vector<SimpleRect> boxes;
    vector<float> confidences;
    vector<int> class_ids;
    
    // 计算缩放比例（假设没有padding，直接缩放）
    float scale_x = (float)img_width / inpWidth;
    float scale_y = (float)img_height / inpHeight;
    
    int num_classes = (int)class_names.size();
    int nout = num_classes + 5;  // x,y,w,h,conf + classes
    
    cout << "[YOLOP] 开始解析YOLOP检测输出，YOLOP类别数=" << num_classes << ", nout=" << nout << endl;
    
    // 遍历三个尺度的输出（8x, 16x, 32x stride）
    for (int n = 0; n < 3; n++) {
        const int num_grid_x = (int)(inpWidth / STRIDES[n]);
        const int num_grid_y = (int)(inpHeight / STRIDES[n]);
        const float current_stride = STRIDES[n];
        
        cout << "[YOLOP] 处理尺度 " << n << ": stride=" << current_stride 
             << ", grid=" << num_grid_x << "x" << num_grid_y << endl;
        
        // 遍历3个anchor
        for (int q = 0; q < 3; q++) {
            const float anchor_w = ANCHORS[n][q * 2];
            const float anchor_h = ANCHORS[n][q * 2 + 1];
            
            // 遍历grid
            for (int i = 0; i < num_grid_y; i++) {
                for (int j = 0; j < num_grid_x; j++) {
                    const float box_score = pdata[4];
                    
                    // 早期退出优化
                    if (box_score <= OBJ_THRESHOLD) {
                        pdata += nout;
                        continue;
                    }
                    
                    // 找到最大类别分数
                    float max_class_score = pdata[5];
                    int class_idx = 0;
                    for (int k = 1; k < num_classes; k++) {
                        if (pdata[5 + k] > max_class_score) {
                            max_class_score = pdata[5 + k];
                            class_idx = k;
                        }
                    }
                    
                    const float final_score = max_class_score * box_score;
                    
                    if (final_score > CONF_THRESHOLD) {
                        // YOLOP边界框解码（使用YOLOv5格式）
                        float cx = (pdata[0] * 2.0f - 0.5f + j) * current_stride;
                        float cy = (pdata[1] * 2.0f - 0.5f + i) * current_stride;
                        float w = powf(pdata[2] * 2.0f, 2.0f) * anchor_w;
                        float h = powf(pdata[3] * 2.0f, 2.0f) * anchor_h;
                        
                        // 转换到原图坐标
                        int left = (int)((cx - 0.5f * w) * scale_x);
                        int top = (int)((cy - 0.5f * h) * scale_y);
                        int width = (int)(w * scale_x);
                        int height = (int)(h * scale_y);
                        
                        boxes.push_back(SimpleRect(left, top, width, height));
                        confidences.push_back(final_score);
                        class_ids.push_back(class_idx);
                    }
                    
                    pdata += nout;
                }
            }
        }
    }
    
    cout << "[YOLOP] 检测到 " << boxes.size() << " 个候选框" << endl;
    
    // 简化NMS去重
    vector<int> indices;
    simple_nms(boxes, confidences, class_ids, indices, NMS_THRESHOLD);
    
    cout << "[YOLOP] NMS后保留 " << indices.size() << " 个目标" << endl;
    
    // 填充结果
    for (size_t i = 0; i < indices.size(); i++) {
        if (results->count >= OBJ_NUMB_MAX_SIZE) break;
        int idx = indices[i];
        detect_result_t& obj = results->results[results->count];
        
        obj.box.left = (boxes[idx].x > 0) ? boxes[idx].x : 0;
        obj.box.top = (boxes[idx].y > 0) ? boxes[idx].y : 0;
        obj.box.right = (boxes[idx].x + boxes[idx].width < img_width) ? 
                        boxes[idx].x + boxes[idx].width : img_width - 1;
        obj.box.bottom = (boxes[idx].y + boxes[idx].height < img_height) ? 
                         boxes[idx].y + boxes[idx].height : img_height - 1;
        obj.prop = confidences[idx];
        
        int class_id = class_ids[idx];
        if (class_id >= 0 && class_id < (int)class_names.size()) {
            strncpy(obj.name, class_names[class_id].c_str(), sizeof(obj.name) - 1);
            obj.name[sizeof(obj.name) - 1] = '\0';
        } else {
            snprintf(obj.name, sizeof(obj.name), "class_%d", class_id);
            cerr << "[YOLOP WARNING] 检测到无效类别ID: " << class_id 
                 << ", 类别总数: " << class_names.size() << endl;
        }
        
        cout << "[YOLOP] 检测结果 #" << i << ": " << obj.name 
             << " (类别ID=" << class_id << ", 置信度=" << obj.prop << ")" << endl;
        
        results->count++;
    }
}

void YOLOPEngine::drawBoundingBoxes(uint8_t* rgb_data, int width, int height,
                                     const detect_result_group_t* results) {
    if (!rgb_data || !results) return;
    
    // 简化绘制：直接在RGB888数据上绘制红色边界框
    for (int i = 0; i < results->count; i++) {
        const detect_result_t& obj = results->results[i];
        
        int left = obj.box.left;
        int top = obj.box.top;
        int right = obj.box.right;
        int bottom = obj.box.bottom;
        
        // 边界检查
        if (left < 0) left = 0;
        if (top < 0) top = 0;
        if (right >= width) right = width - 1;
        if (bottom >= height) bottom = height - 1;
        
        // 绘制红色边界框（线宽2像素）
        int thickness = 2;
        
        // 上边框
        for (int y = top; y < top + thickness && y < height; y++) {
            for (int x = left; x <= right && x < width; x++) {
                int idx = (y * width + x) * 3;
                rgb_data[idx] = 0;      // R
                rgb_data[idx + 1] = 255; // G (绿色)
                rgb_data[idx + 2] = 0;   // B
            }
        }
        
        // 下边框
        for (int y = bottom - thickness + 1; y <= bottom && y >= 0; y++) {
            for (int x = left; x <= right && x < width; x++) {
                int idx = (y * width + x) * 3;
                rgb_data[idx] = 0;
                rgb_data[idx + 1] = 255;
                rgb_data[idx + 2] = 0;
            }
        }
        
        // 左边框
        for (int x = left; x < left + thickness && x < width; x++) {
            for (int y = top; y <= bottom && y < height; y++) {
                int idx = (y * width + x) * 3;
                rgb_data[idx] = 0;
                rgb_data[idx + 1] = 255;
                rgb_data[idx + 2] = 0;
            }
        }
        
        // 右边框
        for (int x = right - thickness + 1; x <= right && x >= 0; x++) {
            for (int y = top; y <= bottom && y < height; y++) {
                int idx = (y * width + x) * 3;
                rgb_data[idx] = 0;
                rgb_data[idx + 1] = 255;
                rgb_data[idx + 2] = 0;
            }
        }
        
        // 绘制简单的类别标记（数字ID或固定文字）
        char simple_label[16];
        
        // 根据YOLOP交通场景类别显示标签
        if (strstr(obj.name, "car") || strstr(obj.name, "CAR")) {
            strcpy(simple_label, "CAR");
        } else if (strstr(obj.name, "truck") || strstr(obj.name, "TRUCK")) {
            strcpy(simple_label, "TRUCK");  
        } else if (strstr(obj.name, "bus") || strstr(obj.name, "BUS")) {
            strcpy(simple_label, "BUS");
        } else if (strstr(obj.name, "person") || strstr(obj.name, "PERSON")) {
            strcpy(simple_label, "PERSON");
        } else if (strstr(obj.name, "bike") || strstr(obj.name, "BIKE") || 
                   strstr(obj.name, "bicycle") || strstr(obj.name, "motorcycle")) {
            strcpy(simple_label, "BIKE");
        } else {
            // 对于其他交通相关目标，显示置信度百分比
            int confidence = (int)(obj.prop * 100);
            snprintf(simple_label, sizeof(simple_label), "%d%%", confidence);
        }
        
        // 绘制标签背景（黑色矩形）
        int label_x = left;
        int label_y = (top > 10) ? top - 10 : top + 15;  // 标签位置
        int label_width = (int)strlen(simple_label) * 6 + 4;   // 6像素每字符 + 边距
        int label_height = 9;  // 字体高度 + 边距
        
        // 绘制标签背景
        for (int ly = label_y; ly < label_y + label_height && ly < height; ly++) {
            for (int lx = label_x; lx < label_x + label_width && lx < width; lx++) {
                if (lx >= 0 && ly >= 0) {
                    int bg_idx = (ly * width + lx) * 3;
                    rgb_data[bg_idx] = 0;     // 黑色背景
                    rgb_data[bg_idx + 1] = 0;
                    rgb_data[bg_idx + 2] = 0;
                }
            }
        }
        
        // 绘制白色文字
        draw_text(rgb_data, width, height, simple_label, 
                  label_x + 2, label_y + 1, 255, 255, 255);  // 白色文字
    }
}

void YOLOPEngine::drawSegmentation(uint8_t* rgb_data, int width, int height) {
    if (!rgb_data || cached_drivable_seg.empty() || cached_lane_seg.empty()) {
        return;
    }
    
    // 计算缩放比例
    float scale_x = (float)cached_seg_width / width;
    float scale_y = (float)cached_seg_height / height;
    
    int area = cached_seg_width * cached_seg_height;
    
    // 优化：降采样绘制以提高性能
    int step = 2;  // 每2个像素采样一次
    
    // 第一步：绘制可行驶区域（绿色半透明）
    for (int i = 0; i < height; i += step) {
        for (int j = 0; j < width; j += step) {
            int x = (int)(j * scale_x);
            int y = (int)(i * scale_y);
            
            if (x < cached_seg_width && y < cached_seg_height) {
                int idx = y * cached_seg_width + x;
                // 比较两个通道（背景 vs 前景）
                if (cached_drivable_seg[idx] < cached_drivable_seg[area + idx]) {
                    // 绘制小方块区域（绿色）
                    for (int dy = 0; dy < step && (i + dy) < height; dy++) {
                        for (int dx = 0; dx < step && (j + dx) < width; dx++) {
                            int pixel_idx = ((i + dy) * width + (j + dx)) * 3;
                            rgb_data[pixel_idx] = (uint8_t)((int)rgb_data[pixel_idx] * 0.7 + 0 * 0.3);     // R
                            rgb_data[pixel_idx + 1] = (uint8_t)((int)rgb_data[pixel_idx + 1] * 0.7 + 255 * 0.3); // G
                            rgb_data[pixel_idx + 2] = (uint8_t)((int)rgb_data[pixel_idx + 2] * 0.7 + 0 * 0.3);   // B
                        }
                    }
                }
            }
        }
    }
    
    // 第二步：绘制车道线（深红色，覆盖在可行驶区域之上）
    for (int i = 0; i < height; i += step) {
        for (int j = 0; j < width; j += step) {
            int x = (int)(j * scale_x);
            int y = (int)(i * scale_y);
            
            if (x < cached_seg_width && y < cached_seg_height) {
                int idx = y * cached_seg_width + x;
                // 比较两个通道（背景 vs 前景）
                if (cached_lane_seg[idx] < cached_lane_seg[area + idx]) {
                    // 绘制小方块区域（荧光黄色 - 超级显眼）
                    for (int dy = 0; dy < step && (i + dy) < height; dy++) {
                        for (int dx = 0; dx < step && (j + dx) < width; dx++) {
                            int pixel_idx = ((i + dy) * width + (j + dx)) * 3;
                            // 荧光红色：BGR格式 = (0, 0, 255)
                            rgb_data[pixel_idx] = 0;       // 第一个通道 = B(蓝) = 0
                            rgb_data[pixel_idx + 1] = 0;   // 第二个通道 = G(绿) = 0  
                            rgb_data[pixel_idx + 2] = 255; // 第三个通道 = R(红) = 255 = BGR(0,0,255) = 荧光红
                        }
                    }
                }
            }
        }
    }
    
    cout << "[YOLOP] 优化绘制完成（绿色=可行驶区域，荧光红=车道线，步长=" << step << "）" << endl;
}

int YOLOPEngine::detect(const uint8_t* rgb_data, int width, int height,
                        detect_result_group_t* results) {
    if (!rgb_data || !results || !isValid()) {
        return -1;
    }
    
    cout << "[YOLOP] 开始检测，图像尺寸: " << width << "x" << height << endl;
    
    // 预处理图像
    vector<uint8_t> input_data;
    preprocessImage(rgb_data, width, height, input_data);
    
    cout << "[YOLOP] 预处理完成，输入数据大小: " << input_data.size() << " bytes" << endl;
    
    // 设置输入
    rknn_input inputs[1];
    memset(inputs, 0, sizeof(inputs));
    inputs[0].index = 0;
    inputs[0].type = RKNN_TENSOR_UINT8;
    inputs[0].size = input_data.size();
    inputs[0].fmt = RKNN_TENSOR_NHWC;
    inputs[0].buf = input_data.data();
    
    int ret = rknn_inputs_set(ctx, io_num.n_input, inputs);
    if (ret != RKNN_SUCC) {
        cerr << "[YOLOP] 设置输入失败: " << ret << endl;
        return -1;
    }
    
    cout << "[YOLOP] 开始推理..." << endl;
    
    // 执行推理
    ret = rknn_run(ctx, nullptr);
    if (ret != RKNN_SUCC) {
        cerr << "[YOLOP] 推理失败: " << ret << endl;
        return -1;
    }
    
    cout << "[YOLOP] 推理完成，获取输出..." << endl;
    
    // 动态分配输出数组
    rknn_output* outputs = new rknn_output[io_num.n_output];
    memset(outputs, 0, sizeof(rknn_output) * io_num.n_output);
    for (uint32_t i = 0; i < io_num.n_output; i++) {
        outputs[i].want_float = 1;  // 请求浮点数输出
    }
    
    ret = rknn_outputs_get(ctx, io_num.n_output, outputs, nullptr);
    if (ret != RKNN_SUCC) {
        cerr << "[YOLOP] 获取输出失败: " << ret << endl;
        delete[] outputs;
        return -1;
    }
    
    cout << "[YOLOP] 输出获取成功，共 " << io_num.n_output << " 个输出" << endl;
    
    // 打印输出信息以便调试
    for (uint32_t i = 0; i < io_num.n_output; i++) {
        cout << "  输出[" << i << "] 大小: " << outputs[i].size 
             << " bytes, 是否有效: " << (outputs[i].buf != nullptr) << endl;
    }
    
    // 后处理检测结果
    postprocessDetection(outputs, width, height, results);
    
    // 缓存分割输出用于后续绘制
    if (io_num.n_output >= 3 && outputs[1].buf && outputs[2].buf) {
        cached_seg_width = inpWidth;
        cached_seg_height = inpHeight;
        
        // 缓存可行驶区域分割（output[1]）
        int seg_size = inpWidth * inpHeight * 2;  // 2通道
        float* drivable_data = (float*)outputs[1].buf;
        cached_drivable_seg.assign(drivable_data, drivable_data + seg_size);
        
        // 缓存车道线分割（output[2]）
        float* lane_data = (float*)outputs[2].buf;
        cached_lane_seg.assign(lane_data, lane_data + seg_size);
        
        cout << "[YOLOP] 已缓存分割输出，尺寸: " << cached_seg_width 
             << "x" << cached_seg_height << endl;
    }
    
    // 释放输出
    rknn_outputs_release(ctx, io_num.n_output, outputs);
    delete[] outputs;
    
    cout << "[YOLOP] 检测完成，找到 " << results->count << " 个目标" << endl;
    
    return 0;
}

void YOLOPEngine::drawResults(uint8_t* rgb_data, int width, int height,
                               const detect_result_group_t* results) {
#ifdef PURE_CAPTURE_MODE
    // 纯采集模式：完全跳过所有绘制操作
    return;
#else
    if (!rgb_data || !results) return;
    
    // 先绘制分割结果（作为底层）
    drawSegmentation(rgb_data, width, height);
    
    // 再绘制检测框（在分割之上）
    drawBoundingBoxes(rgb_data, width, height, results);
    
    cout << "[YOLOP] 已绘制完整结果（分割+检测框）" << endl;
#endif
}

// 添加纯检测模式（无绘制，提高性能）
int YOLOPEngine::detectOnly(const uint8_t* rgb_data, int width, int height,
                            detect_result_group_t* results) {
    if (!rgb_data || !results || !isValid()) {
        return -1;
    }
    
    // 预处理图像
    vector<uint8_t> input_data;
    preprocessImage(rgb_data, width, height, input_data);
    
    // 设置输入
    rknn_input inputs[1];
    memset(inputs, 0, sizeof(inputs));
    inputs[0].index = 0;
    inputs[0].type = RKNN_TENSOR_UINT8;
    inputs[0].size = input_data.size();
    inputs[0].fmt = RKNN_TENSOR_NHWC;
    inputs[0].buf = input_data.data();
    
    int ret = rknn_inputs_set(ctx, io_num.n_input, inputs);
    if (ret != RKNN_SUCC) return -1;
    
    // 执行推理
    ret = rknn_run(ctx, nullptr);
    if (ret != RKNN_SUCC) return -1;
    
    // 只获取检测输出（跳过分割输出以提高性能）
    rknn_output outputs[1];
    memset(outputs, 0, sizeof(outputs));
    outputs[0].want_float = 1;
    outputs[0].index = 0;  // 仅检测输出
    
    ret = rknn_outputs_get(ctx, 1, outputs, nullptr);
    if (ret != RKNN_SUCC) return -1;
    
    // 后处理检测结果
    postprocessDetection(outputs, width, height, results);
    
    // 释放输出
    rknn_outputs_release(ctx, 1, outputs);
    
    return 0;
}
