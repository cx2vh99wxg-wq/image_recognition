#ifndef YOLO_ENGINE_RESOURCES_H
#define YOLO_ENGINE_RESOURCES_H

#include "rknn_model_file.h"
#include <string.h>

// Shared by the two C engines. Model dimensions, extra buffers and labels are
// owned by their respective callers.
static inline int yolo_open_resources(YOLOEngine *engine, const char *model_path)
{
    // 加载RKNN模型
    FILE *fp = fopen(model_path, "rb");
    if (!fp)
    {
        printf("无法打开模型文件: %s\n", model_path);
        return -1;
    }

    size_t model_size;
    void *model_data = read_rknn_model(fp, &model_size, 1);
    if (!model_data)
        return -1;

    // 为RKNN建立初始状态。
    int ret = rknn_init(&engine->ctx, model_data, model_size, 0, NULL);
    free(model_data);

    if (ret < 0)
    {
        printf("RKNN初始化失败: %d\n", ret);
        return -1;
    }

    // 读取输入输出数量。
    ret = rknn_query(engine->ctx, RKNN_QUERY_IN_OUT_NUM, &engine->io_num, sizeof(engine->io_num));
    if (ret < 0)
    {
        printf("查询输入输出数量失败: %d\n", ret);
        rknn_destroy(engine->ctx);
        return -1;
    }

    printf("模型输入数量: %d, 输出数量: %d\n", engine->io_num.n_input, engine->io_num.n_output);

    // 为输入输出属性数组申请所需空间。
    engine->input_attrs =
        (rknn_tensor_attr *)malloc(engine->io_num.n_input * sizeof(rknn_tensor_attr));
    engine->output_attrs =
        (rknn_tensor_attr *)malloc(engine->io_num.n_output * sizeof(rknn_tensor_attr));
    engine->inputs = (rknn_input *)malloc(engine->io_num.n_input * sizeof(rknn_input));
    engine->outputs = (rknn_output *)malloc(engine->io_num.n_output * sizeof(rknn_output));

    if (!engine->input_attrs || !engine->output_attrs || !engine->inputs || !engine->outputs)
    {
        printf("分配输入输出数组内存失败\n");
        yolo_engine_cleanup(engine);
        return -1;
    }

    memset(engine->input_attrs, 0, engine->io_num.n_input * sizeof(rknn_tensor_attr));
    memset(engine->output_attrs, 0, engine->io_num.n_output * sizeof(rknn_tensor_attr));
    memset(engine->inputs, 0, engine->io_num.n_input * sizeof(rknn_input));
    memset(engine->outputs, 0, engine->io_num.n_output * sizeof(rknn_output));
    return 0;
}

static inline int yolo_query_attrs(rknn_context ctx, rknn_tensor_attr *attrs, uint32_t count,
                                   rknn_query_cmd command, int verbose_outputs)
{
    const int output = command == RKNN_QUERY_OUTPUT_ATTR;
    for (uint32_t index = 0; index < count; ++index)
    {
        rknn_tensor_attr *attr = &attrs[index];
        attr->index = index;
        const int ret = rknn_query(ctx, command, attr, sizeof(*attr));
        if (ret < 0)
        {
            const char *direction = output ? "输出" : "输入";
            if (verbose_outputs)
                printf("查询%s属性%u失败: %d\n", direction, index, ret);
            else
                printf("查询%s属性%d失败: %d\n", direction, (int)index, ret);
            return -1;
        }
        if (verbose_outputs && output)
            printf("输出 %u: %s\n", index, attr->name);
    }
    return 0;
}

static inline void yolo_close_resources(YOLOEngine *engine)
{
    if (engine->input_data)
    {
        free(engine->input_data);
        engine->input_data = NULL;
    }

    if (engine->input_attrs)
    {
        free(engine->input_attrs);
        engine->input_attrs = NULL;
    }

    if (engine->output_attrs)
    {
        free(engine->output_attrs);
        engine->output_attrs = NULL;
    }

    if (engine->inputs)
    {
        free(engine->inputs);
        engine->inputs = NULL;
    }

    if (engine->outputs)
    {
        free(engine->outputs);
        engine->outputs = NULL;
    }

    if (engine->ctx > 0)
    {
        rknn_destroy(engine->ctx);
        engine->ctx = 0;
    }
}

#endif
