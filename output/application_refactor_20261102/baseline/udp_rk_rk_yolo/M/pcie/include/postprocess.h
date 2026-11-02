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
} detect_result_group_t;

#ifdef __cplusplus
extern "C" {
#endif

/* 从文本文件顺序读取最多 80 个类别名；成功返回 0，文件不可用返回 -1。 */
int initPostProcess(const char* label_file_path);

/* C 接口：解码三个量化输出层，按原扫描顺序返回最多 64 个候选框。
 * nms_threshold 当前仅保留在接口中，现有实现不执行非极大值抑制。 */
int post_process_simple(int8_t *input0, int8_t *input1, int8_t *input2, 
                       int model_in_h, int model_in_w,
                       float conf_threshold, float nms_threshold, 
                       BOX_RECT pads, float scale_w, float scale_h,
                       detect_result_group_t *group);

void deinitPostProcess();

#ifdef __cplusplus
}

/* C++ 接口保持旧签名，并委托 C 接口处理；qnt_zps 与 qnt_scales 当前未使用。 */
int post_process(int8_t *input0, int8_t *input1, int8_t *input2, int model_in_h, int model_in_w,
                 float conf_threshold, float nms_threshold, BOX_RECT pads, float scale_w, float scale_h,
                 std::vector<int32_t> &qnt_zps, std::vector<float> &qnt_scales,
                 detect_result_group_t *group);
#endif
#endif /* _RKNN_YOLOV5_DEMO_POSTPROCESS_H_ */