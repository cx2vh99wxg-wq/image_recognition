#ifndef YOLOP_OPENCV_INTEGRATION_H
#define YOLOP_OPENCV_INTEGRATION_H

#include "rknn_api.h"
#include "postprocess.h"  // 包含原有的后处理头文件
#include <cstdint>
#include <vector>
#include <string>

// ===== 纯C++接口 - YOLOP多任务检测引擎 =====

/**
 * YOLOP引擎类 - 完全C++实现
 * 支持车辆检测、车道线分割、可行驶区域分割三大任务
 */
class YOLOPEngine {
public:
    /**
     * 构造函数 - 创建YOLOP引擎
     * @param model_path RKNN模型文件路径
     * @param label_path BDD100K标签文件路径
     */
    YOLOPEngine(const std::string& model_path, const std::string& label_path);
    
    /**
     * 析构函数 - 自动释放资源
     */
    ~YOLOPEngine();

    /**
     * YOLOP多任务检测（目标检测+分割）
     * @param rgb_data RGB888格式的图像数据
     * @param width 图像宽度
     * @param height 图像高度
     * @param results 输出检测结果（兼容原detect_result_group_t结构）
     * @return 成功返回0，失败返回-1
     */
    int detect(const uint8_t* rgb_data, int width, int height, 
               detect_result_group_t* results);

    /**
     * 绘制完整结果（检测框+分割）到图像上
     * 注意：必须在detect()之后立即调用，因为使用缓存的分割输出
     * @param rgb_data RGB888格式的图像数据（会被修改）
     * @param width 图像宽度
     * @param height 图像高度
     * @param results 检测结果
     */
    void drawResults(uint8_t* rgb_data, int width, int height, 
                     const detect_result_group_t* results);

    /**
     * 纯检测模式（无绘制，高性能）
     * @param rgb_data RGB888格式的图像数据
     * @param width 图像宽度
     * @param height 图像高度
     * @param results 输出检测结果
     * @return 成功返回0，失败返回-1
     */
    int detectOnly(const uint8_t* rgb_data, int width, int height,
                   detect_result_group_t* results);

    /**
     * 只绘制检测框（不含分割）
     * @param rgb_data RGB888格式的图像数据（会被修改）
     * @param width 图像宽度
     * @param height 图像高度
     * @param results 检测结果
     */
    void drawBoundingBoxes(uint8_t* rgb_data, int width, int height,
                           const detect_result_group_t* results);

    /**
     * 检查引擎是否初始化成功
     * @return 成功返回true，失败返回false
     */
    bool isValid() const { return ctx != 0; }

private:
    // 禁止拷贝
    YOLOPEngine(const YOLOPEngine&) = delete;
    YOLOPEngine& operator=(const YOLOPEngine&) = delete;

    // RKNN上下文和模型信息
    rknn_context ctx;
    std::vector<std::string> class_names;
    int inpWidth, inpHeight;
    rknn_input_output_num io_num;
    rknn_tensor_attr* input_attrs;
    rknn_tensor_attr* output_attrs;
    
    // 缓存最后一次推理的分割输出（用于绘制）
    std::vector<float> cached_drivable_seg;  // 可行驶区域分割
    std::vector<float> cached_lane_seg;      // 车道线分割
    int cached_seg_width;
    int cached_seg_height;

    // 内部辅助函数
    bool loadLabels(const std::string& label_path);
    bool initModel(const std::string& model_path);
    void preprocessImage(const uint8_t* rgb_data, int width, int height, 
                         std::vector<uint8_t>& input_data);
    void postprocessDetection(rknn_output* outputs, int img_width, int img_height,
                             detect_result_group_t* results);
    void drawSegmentation(uint8_t* rgb_data, int width, int height);
};

// ===== 便捷函数接口（兼容原有C风格调用）=====

/**
 * 创建YOLOP引擎（C风格包装）
 * @param model_path RKNN模型文件路径
 * @param label_path 标签文件路径
 * @return 引擎指针，失败返回nullptr
 */
inline YOLOPEngine* yolop_engine_create(const char* model_path, const char* label_path) {
    try {
        YOLOPEngine* engine = new YOLOPEngine(model_path, label_path);
        if (engine && engine->isValid()) {
            return engine;
        }
        delete engine;
        return nullptr;
    } catch (...) {
        return nullptr;
    }
}

/**
 * 执行检测（C风格包装）
 */
inline int yolop_engine_detect(YOLOPEngine* engine, const uint8_t* rgb_data,
                               int width, int height, detect_result_group_t* results) {
    if (!engine || !rgb_data || !results) return -1;
    return engine->detect(rgb_data, width, height, results);
}

/**
 * 绘制结果（C风格包装）
 */
inline void draw_yolop_results(YOLOPEngine* engine, uint8_t* rgb_data, 
                               int width, int height, const detect_result_group_t* results) {
    if (engine && rgb_data && results) {
        engine->drawResults(rgb_data, width, height, results);
    }
}

/**
 * 销毁引擎（C风格包装）
 */
inline void yolop_engine_destroy(YOLOPEngine* engine) {
    delete engine;
}

// ===== 兼容性别名（与原YOLO接口完全兼容）=====
inline int yolo_engine_detect(YOLOPEngine* engine, const uint8_t* rgb_data,
                              int width, int height, detect_result_group_t* results) {
    return yolop_engine_detect(engine, rgb_data, width, height, results);
}

inline void draw_detection_results(uint8_t* rgb_data, int width, int height,
                                   const detect_result_group_t* results) {
    // 兼容性函数：绘制检测框
    if (!rgb_data || !results || results->count == 0) return;
    
    // 简单的边界框绘制（不需要OpenCV）
    for (int i = 0; i < results->count; i++) {
        const detect_result_t& obj = results->results[i];
        
        // 绘制矩形框（使用简单的像素操作）
        int left = obj.box.left;
        int top = obj.box.top;
        int right = obj.box.right;
        int bottom = obj.box.bottom;
        
        // 确保坐标在有效范围内
        if (left < 0) left = 0;
        if (top < 0) top = 0;
        if (right >= width) right = width - 1;
        if (bottom >= height) bottom = height - 1;
        
        // 绘制红色边界框（RGB888格式，线宽2像素）
        int thickness = 2;
        
        // 上边框
        for (int y = top; y < top + thickness && y < height; y++) {
            for (int x = left; x <= right && x < width; x++) {
                int idx = (y * width + x) * 3;
                rgb_data[idx] = 0;      // R
                rgb_data[idx + 1] = 0;  // G
                rgb_data[idx + 2] = 255; // B (红色)
            }
        }
        
        // 下边框
        for (int y = bottom - thickness + 1; y <= bottom && y >= 0; y++) {
            for (int x = left; x <= right && x < width; x++) {
                int idx = (y * width + x) * 3;
                rgb_data[idx] = 0;
                rgb_data[idx + 1] = 0;
                rgb_data[idx + 2] = 255;
            }
        }
        
        // 左边框
        for (int x = left; x < left + thickness && x < width; x++) {
            for (int y = top; y <= bottom && y < height; y++) {
                int idx = (y * width + x) * 3;
                rgb_data[idx] = 0;
                rgb_data[idx + 1] = 0;
                rgb_data[idx + 2] = 255;
            }
        }
        
        // 右边框
        for (int x = right - thickness + 1; x <= right && x >= 0; x++) {
            for (int y = top; y <= bottom && y < height; y++) {
                int idx = (y * width + x) * 3;
                rgb_data[idx] = 0;
                rgb_data[idx + 1] = 0;
                rgb_data[idx + 2] = 255;
            }
        }
    }
}

#endif // YOLOP_OPENCV_INTEGRATION_H