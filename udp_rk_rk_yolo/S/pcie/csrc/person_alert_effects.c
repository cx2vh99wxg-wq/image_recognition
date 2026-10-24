#include "person_alert_effects.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

/* 全局闪烁状态由初始化入口设置，逐帧绘制时复用。 */
AlertEffectState g_alert_state = {0, 0, 0};

/* 检测框内外及屏幕边缘分别使用独立的渐变宽度。 */
#define SCREEN_EDGE_GRADIENT_WIDTH 80
#define BOX_INNER_GRADIENT_WIDTH 40
#define BOX_OUTER_GRADIENT_WIDTH 60

/* 四个 320×240 角区域基于 640×480 画布划分。 */
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define CORNER_WIDTH 320
#define CORNER_HEIGHT 240

typedef enum {
    CORNER_TOP_LEFT = 0,
    CORNER_TOP_RIGHT = 1,
    CORNER_BOTTOM_LEFT = 2,
    CORNER_BOTTOM_RIGHT = 3,
    CORNER_NONE = -1
} CornerType;

/* 清空旧状态，记录当前毫秒时间，并将闪烁初始状态设为亮。 */
void alert_effects_init(void) {
    memset(&g_alert_state, 0, sizeof(AlertEffectState));
    g_alert_state.last_blink_time = get_current_time_ms();
    g_alert_state.blink_state = 1;
    g_alert_state.is_initialized = 1;
    printf("人员警示特效系统初始化完成\n");
}

/* 将 gettimeofday 的秒和微秒部分换算为毫秒。 */
uint64_t get_current_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
}

/* 距离上次翻转达到间隔常量时切换亮暗状态；未初始化时不更新。 */
void update_blink_state(AlertEffectState* state) {
    if (!state || !state->is_initialized) return;

    uint64_t current_time = get_current_time_ms();
    if (current_time - state->last_blink_time >= ALERT_BLINK_INTERVAL) {
        state->blink_state = !state->blink_state;
        state->last_blink_time = current_time;
    }
}

/* 框内返回到最近边界的负距离，框外返回到矩形的欧氏距离。 */
static float calculate_distance_to_box(int pixel_x, int pixel_y, const BOX_RECT* box) {
    if (!box) return 1000.0f;

    int box_left = box->left;
    int box_right = box->right;
    int box_top = box->top;
    int box_bottom = box->bottom;

    if (pixel_x >= box_left && pixel_x <= box_right &&
        pixel_y >= box_top && pixel_y <= box_bottom) {

        float dist_left = pixel_x - box_left;
        float dist_right = box_right - pixel_x;
        float dist_top = pixel_y - box_top;
        float dist_bottom = box_bottom - pixel_y;

        float min_dist = dist_left;
        if (dist_right < min_dist) min_dist = dist_right;
        if (dist_top < min_dist) min_dist = dist_top;
        if (dist_bottom < min_dist) min_dist = dist_bottom;

        return -min_dist;
    }

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

    return sqrtf(dx * dx + dy * dy);
}

/* 以检测框中心到四个画布角的距离决定特效作用区域；并列时保留先遇到的角。 */
static CornerType get_nearest_corner(const BOX_RECT* box) {
    if (!box) return CORNER_NONE;

    float box_center_x = (box->left + box->right) / 2.0f;
    float box_center_y = (box->top + box->bottom) / 2.0f;

    float dist_top_left = sqrtf(box_center_x * box_center_x + box_center_y * box_center_y);
    float dist_top_right = sqrtf((SCREEN_WIDTH - box_center_x) * (SCREEN_WIDTH - box_center_x) + box_center_y * box_center_y);
    float dist_bottom_left = sqrtf(box_center_x * box_center_x + (SCREEN_HEIGHT - box_center_y) * (SCREEN_HEIGHT - box_center_y));
    float dist_bottom_right = sqrtf((SCREEN_WIDTH - box_center_x) * (SCREEN_WIDTH - box_center_x) + (SCREEN_HEIGHT - box_center_y) * (SCREEN_HEIGHT - box_center_y));

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

/* 判定像素是否处于所选的 320×240 角区域。 */
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

/* 返回像素到该角邻接的两条屏幕边中较近的一条的距离。 */
static float calculate_distance_to_corner_edge(int pixel_x, int pixel_y, CornerType corner) {
    float min_dist = 1000.0f;

    switch (corner) {
        case CORNER_TOP_LEFT:

            min_dist = fminf(pixel_x, pixel_y);
            break;
        case CORNER_TOP_RIGHT:

            min_dist = fminf(SCREEN_WIDTH - 1 - pixel_x, pixel_y);
            break;
        case CORNER_BOTTOM_LEFT:

            min_dist = fminf(pixel_x, SCREEN_HEIGHT - 1 - pixel_y);
            break;
        case CORNER_BOTTOM_RIGHT:

            min_dist = fminf(SCREEN_WIDTH - 1 - pixel_x, SCREEN_HEIGHT - 1 - pixel_y);
            break;
        default:
            break;
    }

    return min_dist;
}

/* 合成框内、框外及角边缘的红色强度，并夹在原有的 0..0.8 范围内。 */
static float calculate_red_effect_intensity(int pixel_x, int pixel_y, int width, int height, const BOX_RECT* box) {
    if (!box) return 0.0f;

    CornerType nearest_corner = get_nearest_corner(box);
    if (nearest_corner == CORNER_NONE) return 0.0f;

    float dist_to_box = calculate_distance_to_box(pixel_x, pixel_y, box);

    float intensity = 0.0f;

    if (dist_to_box <= 0) {
        float inner_dist = -dist_to_box;
        if (inner_dist <= BOX_INNER_GRADIENT_WIDTH) {

            intensity = 0.7f * (1.0f - inner_dist / BOX_INNER_GRADIENT_WIDTH);
        }
    }

    else if (dist_to_box <= BOX_OUTER_GRADIENT_WIDTH) {

        intensity = 0.5f * (1.0f - dist_to_box / BOX_OUTER_GRADIENT_WIDTH);
    }

    if (is_pixel_in_corner(pixel_x, pixel_y, nearest_corner)) {
        float dist_to_corner_edge = calculate_distance_to_corner_edge(pixel_x, pixel_y, nearest_corner);

        if (dist_to_corner_edge <= SCREEN_EDGE_GRADIENT_WIDTH) {
            float edge_intensity = 0.4f * (1.0f - dist_to_corner_edge / SCREEN_EDGE_GRADIENT_WIDTH);
            intensity = fmaxf(intensity, edge_intensity);

            if (dist_to_box <= BOX_OUTER_GRADIENT_WIDTH) {
                intensity *= 1.3f;
            }
        }
    }

    if (intensity > 0.8f) intensity = 0.8f;
    if (intensity < 0.0f) intensity = 0.0f;

    return intensity;
}

/* 兼容旧接口：给出到检测框的距离比例、红色及限制后的透明度。 */
void calculate_fade_info(int pixel_x, int pixel_y, const BOX_RECT* box, FadeInfo* fade_info) {
    if (!fade_info) return;

    fade_info->distance_ratio = 1.0f;
    fade_info->alpha = 0.0f;
    fade_info->color.r = 255;
    fade_info->color.g = 0;
    fade_info->color.b = 0;

    if (!box) return;

    float dist_to_box = calculate_distance_to_box(pixel_x, pixel_y, box);

    if (dist_to_box <= ALERT_MAX_FADE_DISTANCE) {
        fade_info->distance_ratio = fabsf(dist_to_box) / (float)ALERT_MAX_FADE_DISTANCE;

        if (dist_to_box <= 0) {
            fade_info->alpha = 0.6f * (1.0f + dist_to_box / BOX_INNER_GRADIENT_WIDTH);
        } else {
            fade_info->alpha = 0.4f * (1.0f - dist_to_box / ALERT_MAX_FADE_DISTANCE);
        }

        if (fade_info->alpha < 0.0f) fade_info->alpha = 0.0f;
        if (fade_info->alpha > 0.8f) fade_info->alpha = 0.8f;
    }
}

/* 将 alpha 限制到 0..1，再按通道线性插值得到混合颜色。 */
Color blend_colors(Color original, Color effect, float alpha) {
    Color result;

    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;

    result.r = (uint8_t)((1.0f - alpha) * original.r + alpha * effect.r);
    result.g = (uint8_t)((1.0f - alpha) * original.g + alpha * effect.g);
    result.b = (uint8_t)((1.0f - alpha) * original.b + alpha * effect.b);

    return result;
}

/* 图像缓冲区按 B、G、R 顺序存放；先读原像素，按同一 alpha 混合，再按原顺序写回。 */
static void blend_red_pixel(uint8_t *rgb_data, int pixel_idx, float intensity) {
    Color original = {
        rgb_data[pixel_idx + 2],
        rgb_data[pixel_idx + 1],
        rgb_data[pixel_idx]
    };
    Color effect_color = {255, 0, 0};
    Color blended = blend_colors(original, effect_color, intensity);
    rgb_data[pixel_idx] = blended.b;
    rgb_data[pixel_idx + 1] = blended.g;
    rgb_data[pixel_idx + 2] = blended.r;
}

/* 对人员框周围及其最近角的边缘分别叠加红色渐变，亮暗状态控制强度。 */
void draw_person_alert_effect(uint8_t* rgb_data, int width, int height,
                             const BOX_RECT* person_box, AlertEffectState* state) {
    if (!rgb_data || !person_box || !state || !state->is_initialized) return;

    update_blink_state(state);

    float blink_multiplier = state->blink_state ? 1.0f : 0.2f;

    CornerType nearest_corner = get_nearest_corner(person_box);
    if (nearest_corner == CORNER_NONE) return;

    int box_left = person_box->left - BOX_OUTER_GRADIENT_WIDTH;
    int box_right = person_box->right + BOX_OUTER_GRADIENT_WIDTH;
    int box_top = person_box->top - BOX_OUTER_GRADIENT_WIDTH;
    int box_bottom = person_box->bottom + BOX_OUTER_GRADIENT_WIDTH;

    if (box_left < 0) box_left = 0;
    if (box_right >= width) box_right = width - 1;
    if (box_top < 0) box_top = 0;
    if (box_bottom >= height) box_bottom = height - 1;

    for (int y = box_top; y <= box_bottom; y++) {
        for (int x = box_left; x <= box_right; x++) {
            float intensity = calculate_red_effect_intensity(x, y, width, height, person_box);
            intensity *= blink_multiplier;

            if (intensity <= 0.01f) continue;

            blend_red_pixel(rgb_data, (y * width + x) * 3, intensity);
        }
    }

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

    for (int y = corner_top; y <= corner_bottom; y++) {
        for (int x = corner_left; x <= corner_right; x++) {
            float dist_to_edge = calculate_distance_to_corner_edge(x, y, nearest_corner);

            if (dist_to_edge <= SCREEN_EDGE_GRADIENT_WIDTH) {
                float intensity = 0.4f * (1.0f - dist_to_edge / SCREEN_EDGE_GRADIENT_WIDTH);
                intensity *= blink_multiplier;

                if (intensity <= 0.01f) continue;

                blend_red_pixel(rgb_data, (y * width + x) * 3, intensity);
            }
        }
    }
}

/* 初始化全局状态后遍历检测结果，仅对名称为 person 的框绘制特效。 */
void apply_person_alert_effects(uint8_t* rgb_data, int width, int height,
                               const detect_result_group_t* results) {
    if (!rgb_data || !results) return;

    if (!g_alert_state.is_initialized) {
        alert_effects_init();
    }

    for (int i = 0; i < results->count; i++) {
        const detect_result_t* result = &results->results[i];

        if (strcmp(result->name, "person") == 0) {

            draw_person_alert_effect(rgb_data, width, height, &result->box, &g_alert_state);
        }
    }
}