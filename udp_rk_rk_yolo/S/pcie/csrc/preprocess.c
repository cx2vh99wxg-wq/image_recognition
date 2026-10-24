#include "preprocess.h"
#include <string.h>
#include <math.h>

/* 将 RGB888 输入按较小轴的比例缩放到目标画布，并居中补齐到目标尺寸。
 * 缩放使用截断式最近邻采样，灰色填充为每通道 128。保留原有
 * 浮点计算、坐标截断和边界收敛顺序，以维持模型输入的像素结果。 */
void letterbox_simple(const uint8_t *src_rgb, uint8_t *dst_rgb,
                      int src_w, int src_h, int dst_w, int dst_h,
                      BOX_RECT *pads, float *scale)
{
    if (!src_rgb || !dst_rgb || !pads || !scale) {
        return;
    }

    const float scale_w = (float)dst_w / src_w;
    const float scale_h = (float)dst_h / src_h;
    *scale = (scale_w < scale_h) ? scale_w : scale_h;

    const int new_w = (int)(src_w * (*scale));
    const int new_h = (int)(src_h * (*scale));
    const int pad_w = (dst_w - new_w) / 2;
    const int pad_h = (dst_h - new_h) / 2;

    pads->left = pad_w;
    pads->right = dst_w - new_w - pad_w;
    pads->top = pad_h;
    pads->bottom = dst_h - new_h - pad_h;

    /* 原循环对整张 RGB888 画布的每个字节都写入 128；一次填充得到相同结果。 */
    const int canvas_bytes = dst_w * dst_h * 3;
    if (canvas_bytes > 0) {
        memset(dst_rgb, 128, (size_t)canvas_bytes);
    }

    for (int y = 0; y < new_h; ++y) {
        for (int x = 0; x < new_w; ++x) {
            int src_x = (int)(x / (*scale));
            int src_y = (int)(y / (*scale));
            if (src_x >= src_w) src_x = src_w - 1;
            if (src_y >= src_h) src_y = src_h - 1;

            const int src_idx = (src_y * src_w + src_x) * 3;
            const int dst_idx = ((y + pad_h) * dst_w + (x + pad_w)) * 3;
            dst_rgb[dst_idx] = src_rgb[src_idx];
            dst_rgb[dst_idx + 1] = src_rgb[src_idx + 1];
            dst_rgb[dst_idx + 2] = src_rgb[src_idx + 2];
        }
    }
}
