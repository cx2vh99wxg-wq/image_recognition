/*
 * isp_params.c — ISP / 图像参数字典实现（【人员 A · 感知】定义语义与范围）
 *
 * 仅登记“参数元信息”。真正的寄存器读写由【人员 C】在 fpga_regmap.h 的
 * ISP 配置块（0x10+）落地；串口屏（HMI）调参帧的 sel 字节与此表一一对应。
 */
#include "isp_params.h"

#include <stddef.h>   /* NULL */

static const isp_param_desc_t DESC_TABLE[ISP_PARAM_COUNT] = {
    [ISP_PARAM_MODE]        = { ISP_PARAM_MODE,        "mode",         0, 3,   1,  "工作模式（0=关闭 1=正常 2=夜视 3=调试）" },
    [ISP_PARAM_BLC]         = { ISP_PARAM_BLC,         "blc",          0, 255, 16, "黑电平校正" },
    [ISP_PARAM_AWB]         = { ISP_PARAM_AWB,         "awb",          0, 255, 128,"自动白平衡基准" },
    [ISP_PARAM_CNN_LEVEL]   = { ISP_PARAM_CNN_LEVEL,   "cnn_level",    0, 4,   2, "低照度增强 CNN 等级（越高越强）" },
    [ISP_PARAM_SATURATION]  = { ISP_PARAM_SATURATION,  "saturation",   0, 255, 128,"饱和度" },
    [ISP_PARAM_BRIGHTNESS]  = { ISP_PARAM_BRIGHTNESS,  "brightness",   0, 255, 128,"亮度" },
    [ISP_PARAM_AWB_EN]      = { ISP_PARAM_AWB_EN,      "awb_en",       0, 1,   1, "自动白平衡使能" },
    [ISP_PARAM_BINARIZATION]= { ISP_PARAM_BINARIZATION,"binarization", 0, 255, 128,"二值化阈值" },
    [ISP_PARAM_SOBEL]       = { ISP_PARAM_SOBEL,       "sobel",        0, 255, 64, "边缘(Sobel)强度" },
    [ISP_PARAM_ISP_JUDGE]   = { ISP_PARAM_ISP_JUDGE,   "isp_judge",    0, 1,   0, "ISP 判定源（0=CNN 1=Gamma）" },
    [ISP_PARAM_CB_MIN]      = { ISP_PARAM_CB_MIN,      "cb_min",       0, 255, 16, "色度 Cb 下限" },
    [ISP_PARAM_CB_MAX]      = { ISP_PARAM_CB_MAX,      "cb_max",       0, 255, 240,"色度 Cb 上限" },
    [ISP_PARAM_CR_MIN]      = { ISP_PARAM_CR_MIN,      "cr_min",       0, 255, 16, "色度 Cr 下限" },
    [ISP_PARAM_CR_MAX]      = { ISP_PARAM_CR_MAX,      "cr_max",       0, 255, 240,"色度 Cr 上限" },
};

/* 参数总表：以 sel 字节为下标（与串口屏协议一一对应），未定义项为 NULL。
 * 注意：DESC_TABLE 用指定初始化器 [ISP_PARAM_X] 以 id 值（1 基）直接索引，
 * 故此处引用必须为 &DESC_TABLE[ISP_PARAM_X]，不能减 1（否则整体错位一格）。 */
const isp_param_desc_t *isp_param_table[ISP_PARAM_COUNT] = {
    [ISP_PARAM_MODE]        = &DESC_TABLE[ISP_PARAM_MODE],
    [ISP_PARAM_BLC]         = &DESC_TABLE[ISP_PARAM_BLC],
    [ISP_PARAM_AWB]         = &DESC_TABLE[ISP_PARAM_AWB],
    [ISP_PARAM_CNN_LEVEL]   = &DESC_TABLE[ISP_PARAM_CNN_LEVEL],
    [ISP_PARAM_SATURATION]  = &DESC_TABLE[ISP_PARAM_SATURATION],
    [ISP_PARAM_BRIGHTNESS]  = &DESC_TABLE[ISP_PARAM_BRIGHTNESS],
    [ISP_PARAM_AWB_EN]      = &DESC_TABLE[ISP_PARAM_AWB_EN],
    [ISP_PARAM_BINARIZATION]= &DESC_TABLE[ISP_PARAM_BINARIZATION],
    [ISP_PARAM_SOBEL]       = &DESC_TABLE[ISP_PARAM_SOBEL],
    [ISP_PARAM_ISP_JUDGE]   = &DESC_TABLE[ISP_PARAM_ISP_JUDGE],
    [ISP_PARAM_CB_MIN]      = &DESC_TABLE[ISP_PARAM_CB_MIN],
    [ISP_PARAM_CB_MAX]      = &DESC_TABLE[ISP_PARAM_CB_MAX],
    [ISP_PARAM_CR_MIN]      = &DESC_TABLE[ISP_PARAM_CR_MIN],
    [ISP_PARAM_CR_MAX]      = &DESC_TABLE[ISP_PARAM_CR_MAX],
};

const isp_param_desc_t *isp_param_lookup(isp_param_id_t id)
{
    /* 有效 id 为 1..ISP_PARAM_COUNT-1（0x01..0x0E）；0x00 与 0x0F 均无效。
     * DESC_TABLE 以 id 值（1 基）直接下标，故直接取下标，不减 1。 */
    if (id < 1 || id >= ISP_PARAM_COUNT) return NULL;
    return &DESC_TABLE[id];
}

int isp_param_clamp(isp_param_id_t id, int *value)
{
    const isp_param_desc_t *d = isp_param_lookup(id);
    if (!d || !value) return 0;
    if (*value < d->min_val) { *value = d->min_val; return 0; }
    if (*value > d->max_val) { *value = d->max_val; return 0; }
    return 1; /* 合法 */
}
