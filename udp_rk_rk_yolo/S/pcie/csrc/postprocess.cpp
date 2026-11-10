#include "postprocess.h"
#include "../../../common/perception/include/yolo_quantized_decode.h"
#include "person_brake_trigger.h"
#include "person_display_candidates.h"
#include "detection_classes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>

static yolo_quantized::Labels labels;

extern "C"
{
    int initPostProcess(const char *label_file_path)
    {
        return labels.load(label_file_path, "\r\n");
    }

    int post_process_simple(int8_t *input0, int8_t *input1, int8_t *input2, int model_in_h,
                            int model_in_w, float conf_threshold, float nms_threshold,
                            BOX_RECT pads, float scale_w, float scale_h,
                            detect_result_group_t *group)
    {
        (void)nms_threshold;
        if (!group)
            return -1;

        memset(group, 0, sizeof(*group));
        if (!input0 || !input1 || !input2 || model_in_h <= 0 || model_in_w <= 0 ||
            !isfinite(scale_w) || !isfinite(scale_h) || scale_w <= 0 || scale_h <= 0)
            return -1;

        int8_t *const outputs[3] = {input0, input1, input2};
        yolo_quantized::scan(outputs, model_in_h, model_in_w, conf_threshold,
                             fminf(conf_threshold, PERSON_BRAKE_CONFIDENCE_THRESHOLD), true,
                             pads, scale_w, scale_h, labels,
                             [group](const detect_result_t &result, bool display_eligible) {
            if (person_brake_should_trigger(result.name, result.prop))
            {
                ++group->person_brake_count;
                if (group->person_brake_count == 1 || result.prop > group->person_brake_result.prop)
                    group->person_brake_result = result;
                person_display_add(*group,result);
            }
            if (display_eligible && group->count < OBJ_NUMB_MAX_SIZE)
                group->results[group->count++] = result;
            return true;
        },s_detection_classes());
        return 0;
    }

    void deinitPostProcess()
    {
        labels.reset();
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
