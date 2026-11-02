/* RGB888 等比例补边由本文件统一实现。
 * BOX_RECT 仍由调用方的公开预处理头文件提供；
 * M 使用逐像素最近邻路径，S
 * 保留等比例为 1 时的逐行复制分支，
 * 两端入口分别包含本实现。 */
#ifndef PERCEPTION_LETTERBOX_UNIT_SCALE_COPY
#error "Include this implementation only from a configured preprocess.c"
#endif
#include <string.h>
#include <math.h>

/* 以宽、高缩放比例的较小者适配 RGB888 画布，
 * 剩余区域居中补齐。最近邻坐标按原顺序截断并限界；
 * 灰色背景的三个通道均为 128，浮点运算顺序保持既有模型输入结果。 */
void letterbox_simple(const uint8_t *src_rgb, uint8_t *dst_rgb, int src_w, int src_h, int dst_w,
                      int dst_h, BOX_RECT *pads, float *scale)
{
    if (!src_rgb || !dst_rgb || !pads || !scale)
    {
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

    /* 整幅 RGB888 目标缓冲区统一填为 128，与逐字节填充的结果相同。 */
    const int canvas_bytes = dst_w * dst_h * 3;
    if (canvas_bytes > 0)
    {
        memset(dst_rgb, 128, (size_t)canvas_bytes);
    }

#if PERCEPTION_LETTERBOX_UNIT_SCALE_COPY
    /* 实际 640x480 到 640x640
     * 的输入无需缩放。此分支逐行复制源数据并保留补边字节，
     * 省去每个像素的浮点除法及坐标重建。 */
    if (*scale == 1.0f && new_w == src_w && new_h == src_h)
    {
        for (int y = 0; y < src_h; ++y)
        {
            memcpy(dst_rgb + ((y + pad_h) * dst_w + pad_w) * 3, src_rgb + y * src_w * 3,
                   (size_t)src_w * 3);
        }
        return;
    }

#endif

    for (int y = 0; y < new_h; ++y)
    {
        for (int x = 0; x < new_w; ++x)
        {
            int src_x = (int)(x / (*scale));
            int src_y = (int)(y / (*scale));
            if (src_x >= src_w)
                src_x = src_w - 1;
            if (src_y >= src_h)
                src_y = src_h - 1;

            const int src_idx = (src_y * src_w + src_x) * 3;
            const int dst_idx = ((y + pad_h) * dst_w + (x + pad_w)) * 3;
            dst_rgb[dst_idx] = src_rgb[src_idx];
            dst_rgb[dst_idx + 1] = src_rgb[src_idx + 1];
            dst_rgb[dst_idx + 2] = src_rgb[src_idx + 2];
        }
    }
}
