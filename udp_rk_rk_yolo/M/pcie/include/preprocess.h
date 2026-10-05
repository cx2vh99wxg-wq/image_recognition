/* ===========================================================================
 * 文件：preprocess.h
 * 归属：人员 A（感知与预处理）
 * 说明：把 640x480 的 RGB888 帧缩放/填充成 YOLOPv2 模型需要的输入尺寸。
 *       采用 letterbox（等比例缩放 + 灰边填充），保持横纵比不畸变。
 *       本预处理只服务于 A 的车道线模型，不依赖任何检测框结构。
 * ======================================================================== */
#ifndef PERCEPTION_PREPROCESS_H_A
#define PERCEPTION_PREPROCESS_H_A

#include <stdint.h>

/* letterbox 结果：描述源图在目标画布里的位置与缩放比，供后处理反算坐标 */
typedef struct {
    int  pad_left;
    int  pad_top;
    int  pad_right;
    int  pad_bottom;
    float scale;        /* 实际采用的等比缩放系数 */
} letterbox_t;

/* 把 src(宽sw,高sh) 等比缩放并灰边填充到 dst(宽dw,高dh)，输出 box 描述填充信息 */
void perception_letterbox(const uint8_t *src, int sw, int sh,
                          uint8_t *dst, int dw, int dh,
                          letterbox_t *box);

#endif /* PERCEPTION_PREPROCESS_H_A */
