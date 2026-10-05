/*
 * isp_params.h — ISP / 图像参数字典（【人员 A · 感知】定义语义与合法范围）
 *
 * 这些参数会被串口屏（陶晶驰 HMI）调参复用，但【人员 A】只定义“语义与
 * 合法范围”，由【人员 C】落地到 FPGA 寄存器（见 fpga_regmap.h 的 ISP 配置
 * 块 0x10+）。任何新增调参项必须在此登记，保证三端认知一致。
 *
 * 选择字节（sel）与串口屏 UART 协议一一对应（屏发 0x30 0x90 + sel + data）：
 *   0x01=mode 0x02=blc 0x03=awb 0x04=cnn_level 0x05=saturation
 *   0x06=brightness 0x07=awb_en 0x08=binarization 0x09=sobel
 *   0x0A=isp_judge(0=cnn/1=gamma) 0x0B=cb_min 0x0C=cb_max
 *   0x0D=cr_min 0x0E=cr_max
 */
#ifndef ISP_PARAMS_H
#define ISP_PARAMS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 参数标识（与串口屏选择字节对应） */
typedef enum {
    ISP_PARAM_MODE        = 0x01,
    ISP_PARAM_BLC         = 0x02,
    ISP_PARAM_AWB         = 0x03,
    ISP_PARAM_CNN_LEVEL   = 0x04,
    ISP_PARAM_SATURATION  = 0x05,
    ISP_PARAM_BRIGHTNESS  = 0x06,
    ISP_PARAM_AWB_EN      = 0x07,
    ISP_PARAM_BINARIZATION= 0x08,
    ISP_PARAM_SOBEL       = 0x09,
    ISP_PARAM_ISP_JUDGE   = 0x0A,
    ISP_PARAM_CB_MIN      = 0x0B,
    ISP_PARAM_CB_MAX      = 0x0C,
    ISP_PARAM_CR_MIN      = 0x0D,
    ISP_PARAM_CR_MAX      = 0x0E,
    ISP_PARAM_COUNT       = 0x0F   /* 数组上界（id 最大 0x0E，索引 0..14） */
} isp_param_id_t;

/* 单个参数描述：语义 + 合法取值范围 + 出厂默认 */
typedef struct {
    isp_param_id_t id;
    const char    *name;     /* 人类可读名称 */
    int            min_val;  /* 含 */
    int            max_val;  /* 含 */
    int            def_val;  /* 默认值 */
    const char    *desc;     /* 含义说明 */
} isp_param_desc_t;

/* 参数总表（以 sel 字节为下标，未定义项为 NULL） */
extern const isp_param_desc_t *isp_param_table[ISP_PARAM_COUNT];

/* 按 id 查描述；找不到返回 NULL */
const isp_param_desc_t *isp_param_lookup(isp_param_id_t id);

/* 校验某值是否落在合法区间，合法返回 1，否则裁剪到边界并返回 0 */
int isp_param_clamp(isp_param_id_t id, int *value);

#ifdef __cplusplus
}
#endif

#endif /* ISP_PARAMS_H */
