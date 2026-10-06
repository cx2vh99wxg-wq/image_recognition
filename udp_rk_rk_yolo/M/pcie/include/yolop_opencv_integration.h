/* ===========================================================================
 * 文件：yolop_opencv_integration.h
 * 归属：人员 A（感知与预处理）· YOLOP 多任务分割（C++ 实现）
 * 说明：纯 C++ 封装的 YOLOP 引擎，专注“可行驶区域 + 车道线”两路分割。
 *       不再包含行人检测/目标框绘制（那是 B 的行人检测职责），也不内置
 *       5x7 点阵字体（那是显示层 B 的事）。
 *       对外提供 detect()/drawSeg()，并可直接从分割结果抽取 LaneResult。
 * 依赖：rknn_api.h（厂商 SDK）、postprocess.h（仅复用 BOX_RECT 便于兼容）、
 *       common/driving_types.h（契约）。
 * ======================================================================== */
#ifndef YOLOP_OPENCV_INTEGRATION_H_A
#define YOLOP_OPENCV_INTEGRATION_H_A

#include "rknn_api.h"
#include "driving_types.h"
#include <cstdint>
#include <vector>
#include <string>

class YOLOPEngine {
public:
    explicit YOLOPEngine(const std::string& model_path);
    ~YOLOPEngine();

    bool isValid() const { return ctx != 0; }

    /* 推理：rgb888(w,h) → 缓存分割结果 */
    int inference(const uint8_t* rgb, int w, int h);

    /* 把可行驶区域(绿)+车道线(红) 叠加到 rgb 上（仅显示用） */
    void drawSegmentation(uint8_t* rgb, int w, int h) const;

    /* 从缓存的分割结果抽取 LaneResult（真实置信度 + 像素偏移） */
    void extractLaneResult(LaneResult* out, uint32_t frame_id) const;

private:
    rknn_context ctx;
    rknn_input_output_num io_num;
    rknn_tensor_attr* in_attr;
    rknn_tensor_attr* out_attr;

    int inpW, inpH;
    std::vector<uint8_t> input_buf;
    std::vector<float> drivable_seg;   /* (1,2,H,W) */
    std::vector<float> lane_seg;       /* (1,1,H,W) */
    int segW, segH;

    bool loadModel(const std::string& path);
    void preprocess(const uint8_t* rgb, int w, int h);
};

/* C 风格便捷包装（兼容旧调用习惯） */
inline YOLOPEngine* yolop_create(const char* model_path) {
    YOLOPEngine* e = new (std::nothrow) YOLOPEngine(model_path ? model_path : "");
    return (e && e->isValid()) ? e : (delete e, nullptr);
}
inline int yolop_inference(YOLOPEngine* e, const uint8_t* rgb, int w, int h) {
    return e ? e->inference(rgb, w, h) : -1;
}
inline void yolop_destroy(YOLOPEngine* e) { delete e; }

#endif /* YOLOP_OPENCV_INTEGRATION_H_A */
