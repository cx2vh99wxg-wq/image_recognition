#ifndef _RKNN_YOLOV5_DEMO_POSTPROCESS_H_
#define _RKNN_YOLOV5_DEMO_POSTPROCESS_H_

#include <stdint.h>

#ifdef __cplusplus
#include <vector>
#endif

#define OBJ_NAME_MAX_SIZE 16
#define OBJ_NUMB_MAX_SIZE 64
#define OBJ_CLASS_NUM 80
#define NMS_THRESH 0.45
#define BOX_THRESH 0.25
#define PROP_BOX_SIZE (5 + OBJ_CLASS_NUM)

typedef struct _BOX_RECT
{
    int left;
    int right;
    int top;
    int bottom;
} BOX_RECT;

typedef struct __detect_result_t
{
    char name[OBJ_NAME_MAX_SIZE];
    BOX_RECT box;
    float prop;
} detect_result_t;

typedef struct _detect_result_group_t
{
    int id;
    int count;
    detect_result_t results[OBJ_NUMB_MAX_SIZE];
    /* 所有解码候选都参与统计，不受显示框数量上限限制；
     * 代表框选择满足条件且置信度最高的 person。 */
    int person_brake_count;
    detect_result_t person_brake_result;
    /* Independent bounded display reservoir. Brake statistics above still
     * include every qualifying anchor, without display suppression. */
    int person_display_count;
    detect_result_t person_display_results[16];
} detect_result_group_t;

#ifdef __cplusplus
extern "C"
{
#endif

    /* 图像处理：从文本文件顺序读取最多 80 个类别名；成功返回 0，文件不可用返回 -1。 */
    int initPostProcess(const char *label_file_path);

    /* C 接口按旧扫描顺序解码三层量化输出，最多保留 64 个显示框，
 * 同时遍历全部候选汇总置信度至少 15% 的 person。
 * 仅制动候选门限独立降低，显示仍由 conf_threshold 筛选；
 * nms_threshold 只保留在签名中，本实现不运行 NMS。 */
    int post_process_simple(int8_t *input0, int8_t *input1, int8_t *input2, int model_in_h,
                            int model_in_w, float conf_threshold, float nms_threshold,
                            BOX_RECT pads, float scale_w, float scale_h,
                            detect_result_group_t *group);

    void deinitPostProcess();

#ifdef __cplusplus
}

/* C++ 接口继续保留旧签名并委托 C 接口完成处理；qnt_zps 和 qnt_scales 暂不参与运算。 */
int post_process(int8_t *input0, int8_t *input1, int8_t *input2, int model_in_h, int model_in_w,
                 float conf_threshold, float nms_threshold, BOX_RECT pads, float scale_w,
                 float scale_h, std::vector<int32_t> &qnt_zps, std::vector<float> &qnt_scales,
                 detect_result_group_t *group);
#endif
#endif /* _RKNN_YOLOV5_DEMO_POSTPROCESS_H_：本节对应的标识或作用域。 */
