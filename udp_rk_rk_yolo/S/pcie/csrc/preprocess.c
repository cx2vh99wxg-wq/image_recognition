#include "preprocess.h"
#include <string.h>
#include <math.h>

void letterbox_simple(const uint8_t* src_rgb, uint8_t* dst_rgb, 
                     int src_w, int src_h, int dst_w, int dst_h, 
                     BOX_RECT* pads, float* scale) {
    if (!src_rgb || !dst_rgb || !pads || !scale) return;
    
    // 计算缩放比例
    float scale_w = (float)dst_w / src_w;
    float scale_h = (float)dst_h / src_h;
    *scale = (scale_w < scale_h) ? scale_w : scale_h;
    
    // 计算新的尺寸
    int new_w = (int)(src_w * (*scale));
    int new_h = (int)(src_h * (*scale));
    
    // 计算padding
    int pad_w = (dst_w - new_w) / 2;
    int pad_h = (dst_h - new_h) / 2;
    
    pads->left = pad_w;
    pads->right = dst_w - new_w - pad_w;
    pads->top = pad_h;
    pads->bottom = dst_h - new_h - pad_h;
    
    // 初始化目标图像为灰色(128, 128, 128)
    for (int i = 0; i < dst_w * dst_h * 3; i += 3) {
        dst_rgb[i] = 128;     // R
        dst_rgb[i + 1] = 128; // G
        dst_rgb[i + 2] = 128; // B
    }
    
    // 简单的最近邻插值缩放
    for (int y = 0; y < new_h; y++) {
        for (int x = 0; x < new_w; x++) {
            // 计算源图像坐标
            int src_x = (int)(x / (*scale));
            int src_y = (int)(y / (*scale));
            
            // 边界检查
            if (src_x >= src_w) src_x = src_w - 1;
            if (src_y >= src_h) src_y = src_h - 1;
            
            // 计算目标坐标
            int dst_x = x + pad_w;
            int dst_y = y + pad_h;
            
            // 复制像素
            int src_idx = (src_y * src_w + src_x) * 3;
            int dst_idx = (dst_y * dst_w + dst_x) * 3;
            
            dst_rgb[dst_idx] = src_rgb[src_idx];         // R
            dst_rgb[dst_idx + 1] = src_rgb[src_idx + 1]; // G
            dst_rgb[dst_idx + 2] = src_rgb[src_idx + 2]; // B
        }
    }
}