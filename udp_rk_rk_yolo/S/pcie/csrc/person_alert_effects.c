#include "person_alert_effects.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

// 全局特效状态
AlertEffectState g_alert_state = {0, 0, 0};

// 特效参数
#define SCREEN_EDGE_GRADIENT_WIDTH 80   // 屏幕边缘渐变宽度
#define BOX_INNER_GRADIENT_WIDTH 40     // 框内渐变宽度
#define BOX_OUTER_GRADIENT_WIDTH 60     // 框外渐变宽度

// 屏幕分割参数 (640x480)
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define CORNER_WIDTH 320   // 角区域宽度
#define CORNER_HEIGHT 240  // 角区域高度

// 角区域枚举
typedef enum {
    CORNER_TOP_LEFT = 0,
    CORNER_TOP_RIGHT = 1,
    CORNER_BOTTOM_LEFT = 2,
    CORNER_BOTTOM_RIGHT = 3,
    CORNER_NONE = -1
} CornerType;

// 初始化特效系统
void alert_effects_init(void) {
    memset(&g_alert_state, 0, sizeof(AlertEffectState));
    g_alert_state.last_blink_time = get_current_time_ms();
    g_alert_state.blink_state = 1; // 开始时为亮状态
    g_alert_state.is_initialized = 1;
    printf("人员警示特效系统初始化完成\n");
}

// 获取当前时间戳 (毫秒)
uint64_t get_current_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
}

// 更新闪烁状态
void update_blink_state(AlertEffectState* state) {
    if (!state || !state->is_initialized) return;
    
    uint64_t current_time = get_current_time_ms();
    if (current_time - state->last_blink_time >= ALERT_BLINK_INTERVAL) {
        state->blink_state = !state->blink_state; // 切换闪烁状态
        state->last_blink_time = current_time;
    }
}

// 计算像素到检测框的最小距离
static float calculate_distance_to_box(int pixel_x, int pixel_y, const BOX_RECT* box) {
    if (!box) return 1000.0f;
    
    int box_left = box->left;
    int box_right = box->right;
    int box_top = box->top;
    int box_bottom = box->bottom;
    
    // 如果像素在边框内部，计算到边框的距离
    if (pixel_x >= box_left && pixel_x <= box_right && 
        pixel_y >= box_top && pixel_y <= box_bottom) {
        
        // 计算到四个边的距离
        float dist_left = pixel_x - box_left;
        float dist_right = box_right - pixel_x;
        float dist_top = pixel_y - box_top;
        float dist_bottom = box_bottom - pixel_y;
        
        // 返回最小距离（负值表示在框内）
        float min_dist = dist_left;
        if (dist_right < min_dist) min_dist = dist_right;
        if (dist_top < min_dist) min_dist = dist_top;
        if (dist_bottom < min_dist) min_dist = dist_bottom;
        
        return -min_dist;  // 负值表示在框内
    }
    
    // 如果像素在边框外部，计算到边框的欧几里得距离
    float dx = 0.0f, dy = 0.0f;
    
    if (pixel_x < box_left) {
        dx = box_left - pixel_x;
    } else if (pixel_x > box_right) {
        dx = pixel_x - box_right;
    }
    
    if (pixel_y < box_top) {
        dy = box_top - pixel_y;
    } else if (pixel_y > box_bottom) {
        dy = pixel_y - box_bottom;
    }
    
    return sqrtf(dx * dx + dy * dy);  // 正值表示在框外
}



// 判断人物框最靠近哪个角
static CornerType get_nearest_corner(const BOX_RECT* box) {
    if (!box) return CORNER_NONE;
    
    // 计算人物框的中心点
    float box_center_x = (box->left + box->right) / 2.0f;
    float box_center_y = (box->top + box->bottom) / 2.0f;
    
    // 计算到四个角的距离
    float dist_top_left = sqrtf(box_center_x * box_center_x + box_center_y * box_center_y);
    float dist_top_right = sqrtf((SCREEN_WIDTH - box_center_x) * (SCREEN_WIDTH - box_center_x) + box_center_y * box_center_y);
    float dist_bottom_left = sqrtf(box_center_x * box_center_x + (SCREEN_HEIGHT - box_center_y) * (SCREEN_HEIGHT - box_center_y));
    float dist_bottom_right = sqrtf((SCREEN_WIDTH - box_center_x) * (SCREEN_WIDTH - box_center_x) + (SCREEN_HEIGHT - box_center_y) * (SCREEN_HEIGHT - box_center_y));
    
    // 找到最近的角
    float min_dist = dist_top_left;
    CornerType nearest_corner = CORNER_TOP_LEFT;
    
    if (dist_top_right < min_dist) {
        min_dist = dist_top_right;
        nearest_corner = CORNER_TOP_RIGHT;
    }
    if (dist_bottom_left < min_dist) {
        min_dist = dist_bottom_left;
        nearest_corner = CORNER_BOTTOM_LEFT;
    }
    if (dist_bottom_right < min_dist) {
        min_dist = dist_bottom_right;
        nearest_corner = CORNER_BOTTOM_RIGHT;
    }
    
    return nearest_corner;
}

// 判断像素是否在指定角的区域内
static int is_pixel_in_corner(int pixel_x, int pixel_y, CornerType corner) {
    switch (corner) {
        case CORNER_TOP_LEFT:
            return (pixel_x < CORNER_WIDTH && pixel_y < CORNER_HEIGHT);
        case CORNER_TOP_RIGHT:
            return (pixel_x >= (SCREEN_WIDTH - CORNER_WIDTH) && pixel_y < CORNER_HEIGHT);
        case CORNER_BOTTOM_LEFT:
            return (pixel_x < CORNER_WIDTH && pixel_y >= (SCREEN_HEIGHT - CORNER_HEIGHT));
        case CORNER_BOTTOM_RIGHT:
            return (pixel_x >= (SCREEN_WIDTH - CORNER_WIDTH) && pixel_y >= (SCREEN_HEIGHT - CORNER_HEIGHT));
        default:
            return 0;
    }
}

// 计算像素到指定角的屏幕边缘的最小距离
static float calculate_distance_to_corner_edge(int pixel_x, int pixel_y, CornerType corner) {
    float min_dist = 1000.0f;
    
    switch (corner) {
        case CORNER_TOP_LEFT:
            // 到左边缘和上边缘的距离
            min_dist = fminf(pixel_x, pixel_y);
            break;
        case CORNER_TOP_RIGHT:
            // 到右边缘和上边缘的距离
            min_dist = fminf(SCREEN_WIDTH - 1 - pixel_x, pixel_y);
            break;
        case CORNER_BOTTOM_LEFT:
            // 到左边缘和下边缘的距离
            min_dist = fminf(pixel_x, SCREEN_HEIGHT - 1 - pixel_y);
            break;
        case CORNER_BOTTOM_RIGHT:
            // 到右边缘和下边缘的距离
            min_dist = fminf(SCREEN_WIDTH - 1 - pixel_x, SCREEN_HEIGHT - 1 - pixel_y);
            break;
        default:
            break;
    }
    
    return min_dist;
}

// 计算红色渐变特效的强度
static float calculate_red_effect_intensity(int pixel_x, int pixel_y, int width, int height, const BOX_RECT* box) {
    if (!box) return 0.0f;
    
    // 确定人物框最靠近哪个角
    CornerType nearest_corner = get_nearest_corner(box);
    if (nearest_corner == CORNER_NONE) return 0.0f;
    
    // 计算到检测框的距离
    float dist_to_box = calculate_distance_to_box(pixel_x, pixel_y, box);
    
    float intensity = 0.0f;
    
    // 1. 检测框内部的渐变效果
    if (dist_to_box <= 0) {  // 在框内
        float inner_dist = -dist_to_box;  // 转为正值
        if (inner_dist <= BOX_INNER_GRADIENT_WIDTH) {
            // 距离边框越近，效果越强
            intensity = 0.7f * (1.0f - inner_dist / BOX_INNER_GRADIENT_WIDTH);
        }
    }
    // 2. 检测框外部的渐变效果
    else if (dist_to_box <= BOX_OUTER_GRADIENT_WIDTH) {
        // 距离边框越近，效果越强
        intensity = 0.5f * (1.0f - dist_to_box / BOX_OUTER_GRADIENT_WIDTH);
    }
    
    // 3. 屏幕边缘的渐变效果（只在人物框最靠近的角区域内）
    if (is_pixel_in_corner(pixel_x, pixel_y, nearest_corner)) {
        float dist_to_corner_edge = calculate_distance_to_corner_edge(pixel_x, pixel_y, nearest_corner);
        
        if (dist_to_corner_edge <= SCREEN_EDGE_GRADIENT_WIDTH) {
            float edge_intensity = 0.4f * (1.0f - dist_to_corner_edge / SCREEN_EDGE_GRADIENT_WIDTH);
            intensity = fmaxf(intensity, edge_intensity);  // 取较大值
            
            // 4. 如果同时靠近框和边缘，增强效果
            if (dist_to_box <= BOX_OUTER_GRADIENT_WIDTH) {
                intensity *= 1.3f;  // 增强30%
            }
        }
    }
    
    // 限制强度范围
    if (intensity > 0.8f) intensity = 0.8f;
    if (intensity < 0.0f) intensity = 0.0f;
    
    return intensity;
}

// 计算渐变信息
void calculate_fade_info(int pixel_x, int pixel_y, const BOX_RECT* box, FadeInfo* fade_info) {
    if (!fade_info) return;
    
    // 初始化默认值 - 红色特效
    fade_info->distance_ratio = 1.0f;
    fade_info->alpha = 0.0f;
    fade_info->color.r = 255;  // 红色
    fade_info->color.g = 0;    // 绿色分量为0
    fade_info->color.b = 0;    // 蓝色分量为0
    
    if (!box) return;
    
    // 这个函数保持兼容性，但现在只是简单的包装
    float dist_to_box = calculate_distance_to_box(pixel_x, pixel_y, box);
    
    if (dist_to_box <= ALERT_MAX_FADE_DISTANCE) {
        fade_info->distance_ratio = fabsf(dist_to_box) / (float)ALERT_MAX_FADE_DISTANCE;
        
        if (dist_to_box <= 0) {  // 在框内
            fade_info->alpha = 0.6f * (1.0f + dist_to_box / BOX_INNER_GRADIENT_WIDTH);
        } else {  // 在框外
            fade_info->alpha = 0.4f * (1.0f - dist_to_box / ALERT_MAX_FADE_DISTANCE);
        }
        
        if (fade_info->alpha < 0.0f) fade_info->alpha = 0.0f;
        if (fade_info->alpha > 0.8f) fade_info->alpha = 0.8f;
    }
}

// 混合颜色 (基于alpha值混合原色和特效色)
Color blend_colors(Color original, Color effect, float alpha) {
    Color result;
    
    // 限制alpha范围
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    
    // 线性混合
    result.r = (uint8_t)((1.0f - alpha) * original.r + alpha * effect.r);
    result.g = (uint8_t)((1.0f - alpha) * original.g + alpha * effect.g);
    result.b = (uint8_t)((1.0f - alpha) * original.b + alpha * effect.b);
    
    return result;
}

// 绘制单个人员的警示特效（新版本 - 红色渐变+对应角的屏幕边缘）
void draw_person_alert_effect(uint8_t* rgb_data, int width, int height, 
                             const BOX_RECT* person_box, AlertEffectState* state) {
    if (!rgb_data || !person_box || !state || !state->is_initialized) return;
    
    // 更新闪烁状态
    update_blink_state(state);
    
    // 如果当前处于闪烁的暗状态，减弱特效强度
    float blink_multiplier = state->blink_state ? 1.0f : 0.2f;
    
    // 确定人物框最靠近哪个角
    CornerType nearest_corner = get_nearest_corner(person_box);
    if (nearest_corner == CORNER_NONE) return;
    
    // 1. 绘制人物框周围的渐变效果
    int box_left = person_box->left - BOX_OUTER_GRADIENT_WIDTH;
    int box_right = person_box->right + BOX_OUTER_GRADIENT_WIDTH;
    int box_top = person_box->top - BOX_OUTER_GRADIENT_WIDTH;
    int box_bottom = person_box->bottom + BOX_OUTER_GRADIENT_WIDTH;
    
    // 边界检查
    if (box_left < 0) box_left = 0;
    if (box_right >= width) box_right = width - 1;
    if (box_top < 0) box_top = 0;
    if (box_bottom >= height) box_bottom = height - 1;
    
    for (int y = box_top; y <= box_bottom; y++) {
        for (int x = box_left; x <= box_right; x++) {
            float intensity = calculate_red_effect_intensity(x, y, width, height, person_box);
            intensity *= blink_multiplier;
            
            if (intensity <= 0.01f) continue;
            
            int pixel_idx = (y * width + x) * 3;
            Color original = {
                rgb_data[pixel_idx + 2], // R
                rgb_data[pixel_idx + 1], // G
                rgb_data[pixel_idx]      // B
            };
            
            Color effect_color = {255, 0, 0};  // 纯红色
            Color blended = blend_colors(original, effect_color, intensity);
            
            rgb_data[pixel_idx] = blended.b;     // B
            rgb_data[pixel_idx + 1] = blended.g; // G  
            rgb_data[pixel_idx + 2] = blended.r; // R
        }
    }
    
    // 2. 绘制最靠近角的屏幕边缘渐变效果
    int corner_left, corner_right, corner_top, corner_bottom;
    
    switch (nearest_corner) {
        case CORNER_TOP_LEFT:
            corner_left = 0; corner_right = CORNER_WIDTH - 1;
            corner_top = 0; corner_bottom = CORNER_HEIGHT - 1;
            break;
        case CORNER_TOP_RIGHT:
            corner_left = width - CORNER_WIDTH; corner_right = width - 1;
            corner_top = 0; corner_bottom = CORNER_HEIGHT - 1;
            break;
        case CORNER_BOTTOM_LEFT:
            corner_left = 0; corner_right = CORNER_WIDTH - 1;
            corner_top = height - CORNER_HEIGHT; corner_bottom = height - 1;
            break;
        case CORNER_BOTTOM_RIGHT:
            corner_left = width - CORNER_WIDTH; corner_right = width - 1;
            corner_top = height - CORNER_HEIGHT; corner_bottom = height - 1;
            break;
        default:
            return;
    }
    
    // 只处理边缘渐变区域
    for (int y = corner_top; y <= corner_bottom; y++) {
        for (int x = corner_left; x <= corner_right; x++) {
            float dist_to_edge = calculate_distance_to_corner_edge(x, y, nearest_corner);
            
            if (dist_to_edge <= SCREEN_EDGE_GRADIENT_WIDTH) {
                float intensity = 0.4f * (1.0f - dist_to_edge / SCREEN_EDGE_GRADIENT_WIDTH);
                intensity *= blink_multiplier;
                
                if (intensity <= 0.01f) continue;
                
                int pixel_idx = (y * width + x) * 3;
                Color original = {
                    rgb_data[pixel_idx + 2], // R
                    rgb_data[pixel_idx + 1], // G
                    rgb_data[pixel_idx]      // B
                };
                
                Color effect_color = {255, 0, 0};  // 纯红色
                Color blended = blend_colors(original, effect_color, intensity);
                
                rgb_data[pixel_idx] = blended.b;     // B
                rgb_data[pixel_idx + 1] = blended.g; // G  
                rgb_data[pixel_idx + 2] = blended.r; // R
            }
        }
    }
}

// 应用人员警示特效到所有检测到的人员
void apply_person_alert_effects(uint8_t* rgb_data, int width, int height, 
                               const detect_result_group_t* results) {
    if (!rgb_data || !results) return;
    
    // 确保特效系统已初始化
    if (!g_alert_state.is_initialized) {
        alert_effects_init();
    }
    
    // 遍历所有检测结果，只处理"person"类别
    for (int i = 0; i < results->count; i++) {
        const detect_result_t* result = &results->results[i];
        
        // 检查是否是人员检测
        if (strcmp(result->name, "person") == 0) {
            // 对每个检测到的人员应用警示特效
            draw_person_alert_effect(rgb_data, width, height, &result->box, &g_alert_state);
        }
    }
}