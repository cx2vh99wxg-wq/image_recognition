#include "postprocess.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>

/* 最多缓存 80 个固定长度类别名，标签是否已装载由独立状态记录。 */
static char labels[OBJ_CLASS_NUM][OBJ_NAME_MAX_SIZE];
static int labels_loaded = 0;

extern "C"
{

    /* 逐行读取标签文本，对超长名称作截断；标签数量不足仍保留警告。 */
    int initPostProcess(const char *label_file_path)
    {
        if (!label_file_path)
        {
            printf("标签文件路径无效\n");
            return -1;
        }

        FILE *fp = fopen(label_file_path, "r");
        if (!fp)
        {
            printf("无法打开标签文件: %s\n", label_file_path);
            return -1;
        }

        char line[256];
        int count = 0;
        while (fgets(line, sizeof(line), fp) && count < OBJ_CLASS_NUM)
        {

            char *newline = strchr(line, '\n');
            if (newline)
                *newline = '\0';

            size_t len = strlen(line);
            if (len >= OBJ_NAME_MAX_SIZE)
            {
                len = OBJ_NAME_MAX_SIZE - 1;
            }
            memcpy(labels[count], line, len);
            labels[count][len] = '\0';
            count++;
        }

        fclose(fp);

        if (count < OBJ_CLASS_NUM)
        {
            printf("警告: 只加载了 %d 个标签，期望 %d 个\n", count, OBJ_CLASS_NUM);
        }

        labels_loaded = 1;
        printf("成功加载 %d 个标签\n", count);
        return 0;
    }

    /* 三套锚框分别用于 8、16、32 像素步长的模型输出。 */
    static const int anchor0[6] = {10, 13, 16, 30, 33, 23};
    static const int anchor1[6] = {30, 61, 62, 45, 59, 119};
    static const int anchor2[6] = {116, 90, 156, 198, 373, 326};

    /* 按既有乘法次序将仿射 int8 量化值还原成浮点数。 */
    static float deqnt_affine_to_f32(int8_t qnt, int32_t zp, float scale)
    {
        return ((float)qnt - (float)zp) * scale;
    }

    /* 浮点置信阈值映射为 int8 后，继续限于 -128..127。 */
    static int8_t qnt_f32_to_affine(float f32, int32_t zp, float scale)
    {
        float dst_val = (f32 / scale) + zp;
        return (int8_t)(dst_val <= -128 ? -128 : (dst_val >= 127 ? 127 : dst_val));
    }

    /* 单层按网格、锚框、类别通道顺序解码，合格框依序加入三个结果向量。 */
    static int process_yolo_layer(int8_t *input, const int *anchor, int grid_h, int grid_w,
                                  int height, int width, int stride, std::vector<float> &boxes,
                                  std::vector<float> &objProbs, std::vector<int> &classId,
                                  float threshold, int32_t zp, float scale)
    {
        int validCount = 0;
        int grid_len = grid_h * grid_w;
        int8_t thres_i8 = qnt_f32_to_affine(threshold, zp, scale);

        for (int a = 0; a < 3; a++)
        {
            for (int i = 0; i < grid_h; i++)
            {
                for (int j = 0; j < grid_w; j++)
                {
                    int8_t box_confidence =
                        input[(PROP_BOX_SIZE * a + 4) * grid_len + i * grid_w + j];
                    if (box_confidence >= thres_i8)
                    {
                        int offset = (PROP_BOX_SIZE * a) * grid_len + i * grid_w + j;
                        int8_t *in_ptr = input + offset;

                        float box_x = (deqnt_affine_to_f32(*in_ptr, zp, scale)) * 2.0 - 0.5;
                        float box_y =
                            (deqnt_affine_to_f32(in_ptr[grid_len], zp, scale)) * 2.0 - 0.5;
                        float box_w = (deqnt_affine_to_f32(in_ptr[2 * grid_len], zp, scale)) * 2.0;
                        float box_h = (deqnt_affine_to_f32(in_ptr[3 * grid_len], zp, scale)) * 2.0;

                        box_x = (box_x + j) * (float)stride;
                        box_y = (box_y + i) * (float)stride;
                        box_w = box_w * box_w * (float)anchor[a * 2];
                        box_h = box_h * box_h * (float)anchor[a * 2 + 1];
                        box_x -= (box_w / 2.0);
                        box_y -= (box_h / 2.0);

                        int8_t maxClassProbs = in_ptr[5 * grid_len];
                        int maxClassId = 0;
                        for (int k = 1; k < OBJ_CLASS_NUM; ++k)
                        {
                            int8_t prob = in_ptr[(5 + k) * grid_len];
                            if (prob > maxClassProbs)
                            {
                                maxClassId = k;
                                maxClassProbs = prob;
                            }
                        }

                        if (maxClassProbs > thres_i8)
                        {
                            objProbs.push_back((deqnt_affine_to_f32(maxClassProbs, zp, scale)) *
                                               (deqnt_affine_to_f32(box_confidence, zp, scale)));
                            classId.push_back(maxClassId);
                            validCount++;
                            boxes.push_back(box_x);
                            boxes.push_back(box_y);
                            boxes.push_back(box_w);
                            boxes.push_back(box_h);
                        }
                    }
                }
            }
        }
        return validCount;
    }

    /* 依次处理三层输出，最多把 64 个候选框映回原图；
 * 固定量化参数和直接截断保留，nms_threshold 在本路径不参与筛选。 */
    int post_process_simple(int8_t *input0, int8_t *input1, int8_t *input2, int model_in_h,
                            int model_in_w, float conf_threshold, float nms_threshold,
                            BOX_RECT pads, float scale_w, float scale_h,
                            detect_result_group_t *group)
    {
        if (!group)
            return -1;

        memset(group, 0, sizeof(detect_result_group_t));

        int32_t default_zp = -128;
        float default_scale = 0.003921569;

        std::vector<float> filterBoxes;
        std::vector<float> objProbs;
        std::vector<int> classId;

        /* 三层仍按步长 8、16、32 解码，其扫描次序同时决定截断时保留哪些结果。 */
        int8_t *const inputs[3] = {input0, input1, input2};
        const int *const anchors[3] = {anchor0, anchor1, anchor2};
        const int strides[3] = {8, 16, 32};
        int validCount = 0;
        for (int layer = 0; layer < 3; ++layer)
        {
            const int stride = strides[layer];
            validCount +=
                process_yolo_layer(inputs[layer], anchors[layer], model_in_h / stride,
                                   model_in_w / stride, model_in_h, model_in_w, stride, filterBoxes,
                                   objProbs, classId, conf_threshold, default_zp, default_scale);
        }

        if (validCount <= 0)
        {
            return 0;
        }

        int final_count = 0;
        for (int i = 0; i < validCount && final_count < OBJ_NUMB_MAX_SIZE; i++)
        {

            float box_x = filterBoxes[i * 4 + 0];
            float box_y = filterBoxes[i * 4 + 1];
            float box_w = filterBoxes[i * 4 + 2];
            float box_h = filterBoxes[i * 4 + 3];

            float x1 = box_x - pads.left;
            float y1 = box_y - pads.top;
            float x2 = x1 + box_w;
            float y2 = y1 + box_h;

            x1 = x1 / scale_w;
            y1 = y1 / scale_h;
            x2 = x2 / scale_w;
            y2 = y2 / scale_h;

            if (x1 < 0)
                x1 = 0;
            if (y1 < 0)
                y1 = 0;
            if (x2 < 0)
                x2 = 0;
            if (y2 < 0)
                y2 = 0;

            group->results[final_count].box.left = (int)x1;
            group->results[final_count].box.top = (int)y1;
            group->results[final_count].box.right = (int)x2;
            group->results[final_count].box.bottom = (int)y2;
            group->results[final_count].prop = objProbs[i];

            if (classId[i] < OBJ_CLASS_NUM && labels_loaded)
            {
                strncpy(group->results[final_count].name, labels[classId[i]],
                        OBJ_NAME_MAX_SIZE - 1);
                group->results[final_count].name[OBJ_NAME_MAX_SIZE - 1] = '\0';
            }
            else
            {
                snprintf(group->results[final_count].name, OBJ_NAME_MAX_SIZE, "class_%d",
                         classId[i]);
            }

            final_count++;
        }

        group->count = final_count;

        return 0;
    }

    /* 只复位已装载标志，标签数组留待下次初始化覆盖。 */
    void deinitPostProcess()
    {
        labels_loaded = 0;
    }
}

/* C++ 入口复用该简化处理路径，传入的量化参数向量目前不参与计算。 */
int post_process(int8_t *input0, int8_t *input1, int8_t *input2, int model_in_h, int model_in_w,
                 float conf_threshold, float nms_threshold, BOX_RECT pads, float scale_w,
                 float scale_h, std::vector<int32_t> &qnt_zps, std::vector<float> &qnt_scales,
                 detect_result_group_t *group)
{

    return post_process_simple(input0, input1, input2, model_in_h, model_in_w, conf_threshold,
                               nms_threshold, pads, scale_w, scale_h, group);
}