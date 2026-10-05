/*
 * rknn_dummy.c — 仅供本机单元测试的 RKNN 桩实现（非生产代码）
 *
 * 板端链接真实的 librknnrt；本机没有该库，这里提供同名空实现，使
 * lane_detect.c 能被编译链接进测试程序。测试只调用 lane_model_extract /
 * lane_seg_alloc 等纯函数，不会触达这些桩函数。
 */
#include "rknn_api.h"

int rknn_init(rknn_context *context, void *model, uint32_t size, uint32_t flag, rknn_init_extend *extend)
{
    (void)model; (void)size; (void)flag; (void)extend;
    if (context) *context = 0;
    return -1;
}
int rknn_destroy(rknn_context context) { (void)context; return -1; }
int rknn_query(rknn_context context, rknn_query_cmd cmd, void *info, uint32_t size)
{
    (void)context; (void)cmd; (void)info; (void)size; return -1;
}
int rknn_inputs_set(rknn_context context, uint32_t n_inputs, rknn_input inputs[])
{
    (void)context; (void)n_inputs; (void)inputs; return -1;
}
int rknn_run(rknn_context context, rknn_run_extend *extend)
{
    (void)context; (void)extend; return -1;
}
int rknn_outputs_get(rknn_context context, uint32_t n_outputs, rknn_output outputs[], rknn_output_extend *extend)
{
    (void)context; (void)n_outputs; (void)outputs; (void)extend; return -1;
}
int rknn_outputs_release(rknn_context context, uint32_t n_ouputs, rknn_output outputs[])
{
    (void)context; (void)n_ouputs; (void)outputs; return -1;
}
