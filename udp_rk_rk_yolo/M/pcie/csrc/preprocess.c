/* ===========================================================================
 * 文件：preprocess.c
 * 归属：人员 A（感知与预处理）
 * 说明：letterbox 实现 —— 最近邻等比例缩放 + 灰边填充。
 *       原理与原实现一致（保持横纵比、灰底 128），但去掉了对检测框(BOX_RECT)
 *       的依赖，接口更纯粹，只给 A 的车道线模型用。
 * ======================================================================== */
#include "preprocess.h"
#include <string.h>

void perception_letterbox(const uint8_t *src, int sw, int sh,
                          uint8_t *dst, int dw, int dh,
                          letterbox_t *box)
{
    if (!src || !dst || !box || sw <= 0 || sh <= 0 || dw <= 0 || dh <= 0) {
        if (box) {
            box->pad_left = box->pad_top = box->pad_right = box->pad_bottom = 0;
            box->scale = 1.0f;
        }
        return;
    }

    /* 取较小的缩放比，保证整张图都能放进目标画布（不裁切） */
    float scale_w = (float)dw / (float)sw;
    float scale_h = (float)dh / (float)sh;
    float scale   = (scale_w < scale_h) ? scale_w : scale_h;

    int new_w = (int)(sw * scale);
    int new_h = (int)(sh * scale);

    /* 居中放置：左右/上下灰边对称分布 */
    int pad_l = (dw - new_w) / 2;
    int pad_t = (dh - new_h) / 2;
    int pad_r = dw - new_w - pad_l;
    int pad_b = dh - new_h - pad_t;

    if (box) {
        box->pad_left   = pad_l;
        box->pad_top    = pad_t;
        box->pad_right  = pad_r;
        box->pad_bottom = pad_b;
        box->scale      = scale;
    }

    /* 灰底填充（128,128,128） */
    for (int i = 0; i < dw * dh; i++) {
        dst[i * 3 + 0] = 128;
        dst[i * 3 + 1] = 128;
        dst[i * 3 + 2] = 128;
    }

    /* 最近邻采样，把源图贴到中央有效区 */
    for (int y = 0; y < new_h; y++) {
        int sy = (int)((y + pad_t) / scale);
        if (sy >= sh) sy = sh - 1;
        for (int x = 0; x < new_w; x++) {
            int sx = (int)((x + pad_l) / scale);
            if (sx >= sw) sx = sw - 1;

            int dx = x + pad_l;
            int dy = y + pad_t;

            const uint8_t *s = src + (sy * sw + sx) * 3;
            uint8_t       *d = dst + (dy * dw + dx) * 3;
            d[0] = s[0];
            d[1] = s[1];
            d[2] = s[2];
        }
    }
}
