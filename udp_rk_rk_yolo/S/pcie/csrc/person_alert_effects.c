#include "person_alert_effects.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

/* 图像处理：全局闪烁状态由初始化入口设置，逐帧绘制时复用。 */
AlertEffectState g_alert_state = {0, 0, 0};

/* 图像处理：检测框内外及屏幕边缘分别使用独立的渐变宽度。 */
#define SCREEN_EDGE_GRADIENT_WIDTH 80
#define BOX_INNER_GRADIENT_WIDTH 40
#define BOX_OUTER_GRADIENT_WIDTH 60

/* 图像处理：四个 320×240 角区域基于 640×480 画布划分。 */
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define CORNER_WIDTH 320
#define CORNER_HEIGHT 240

typedef enum
{
    CORNER_TOP_LEFT = 0,
    CORNER_TOP_RIGHT = 1,
    CORNER_BOTTOM_LEFT = 2,
    CORNER_BOTTOM_RIGHT = 3,
    CORNER_NONE = -1
} CornerType;

/* 图像处理：清空旧状态，记录当前毫秒时间，并将闪烁初始状态设为亮。 */
void alert_effects_init(void)
{
    memset(&g_alert_state, 0, sizeof(AlertEffectState));
    g_alert_state.last_blink_time = get_current_time_ms();
    g_alert_state.blink_state = 1;
    g_alert_state.is_initialized = 1;
    printf("人员警示特效系统初始化完成\n");
}

/* 图像处理：将 gettimeofday 的秒和微秒部分换算为毫秒。 */
uint64_t get_current_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
}

/* 图像处理：距离上次翻转达到间隔常量时切换亮暗状态；未初始化时不更新。 */
void update_blink_state(AlertEffectState *state)
{
    if (!state || !state->is_initialized)
        return;

    uint64_t current_time = get_current_time_ms();
    if (current_time - state->last_blink_time >= ALERT_BLINK_INTERVAL)
    {
        state->blink_state = !state->blink_state;
        state->last_blink_time = current_time;
    }
}

// Negative within the interval, positive outside. Keep the low-edge check
// first for reversed bounds, as in the original box distance calculation.
static int axis_distance(int pixel, int low, int high)
{
    if (pixel < low)
        return low - pixel;
    if (pixel > high)
        return pixel - high;
    const int from_low = pixel - low;
    const int from_high = high - pixel;
    return -(from_low < from_high ? from_low : from_high);
}

static float calculate_distance_to_box(int pixel_x, int pixel_y, const BOX_RECT *box)
{
    if (!box)
        return 1000.0f;

    const int x = axis_distance(pixel_x, box->left, box->right);
    const int y = axis_distance(pixel_y, box->top, box->bottom);
    if (x <= 0 && y <= 0)
        return -(float)(x > y ? -x : -y);

    const float dx = x > 0 ? (float)x : 0.0f;
    const float dy = y > 0 ? (float)y : 0.0f;
    return sqrtf(dx * dx + dy * dy);
}

static int corner_is_left(CornerType corner)
{
    return (corner & 1) == 0;
}

static int corner_is_top(CornerType corner)
{
    return (corner & 2) == 0;
}

static int corner_edge_x(int x, CornerType corner)
{
    return corner_is_left(corner) ? x : SCREEN_WIDTH - 1 - x;
}

static int corner_edge_y(int y, CornerType corner)
{
    return corner_is_top(corner) ? y : SCREEN_HEIGHT - 1 - y;
}

static CornerType get_nearest_corner(const BOX_RECT *box)
{
    if (!box)
        return CORNER_NONE;

    const float center_x = (box->left + box->right) / 2.0f;
    const float center_y = (box->top + box->bottom) / 2.0f;
    CornerType nearest = CORNER_TOP_LEFT;
    float shortest = sqrtf(center_x * center_x + center_y * center_y);
    // Strict comparison preserves the original tie order.
    for (int i = CORNER_TOP_RIGHT; i <= CORNER_BOTTOM_RIGHT; ++i)
    {
        const CornerType corner = (CornerType)i;
        const float dx = corner_is_left(corner) ? center_x : SCREEN_WIDTH - center_x;
        const float dy = corner_is_top(corner) ? center_y : SCREEN_HEIGHT - center_y;
        const float distance = sqrtf(dx * dx + dy * dy);
        if (distance < shortest)
        {
            shortest = distance;
            nearest = corner;
        }
    }
    return nearest;
}

static int is_pixel_in_corner(int x, int y, CornerType corner)
{
    return corner != CORNER_NONE &&
           corner_edge_x(x, corner) < CORNER_WIDTH && corner_edge_y(y, corner) < CORNER_HEIGHT;
}

static float calculate_distance_to_corner_edge(int x, int y, CornerType corner)
{
    if (corner == CORNER_NONE)
        return 1000.0f;
    const int dx = corner_edge_x(x, corner);
    const int dy = corner_edge_y(y, corner);
    return (float)(dx < dy ? dx : dy);
}

/* 图像处理：合成框内、框外及角边缘的红色强度，并夹在原有的 0..0.8 范围内。 */
static float calculate_red_effect_intensity(int pixel_x, int pixel_y, const BOX_RECT *box,
                                            CornerType nearest_corner)
{
    if (!box)
        return 0.0f;

    if (nearest_corner == CORNER_NONE)
        return 0.0f;

    float dist_to_box = calculate_distance_to_box(pixel_x, pixel_y, box);

    float intensity = 0.0f;

    if (dist_to_box <= 0)
    {
        float inner_dist = -dist_to_box;
        if (inner_dist <= BOX_INNER_GRADIENT_WIDTH)
        {

            intensity = 0.7f * (1.0f - inner_dist / BOX_INNER_GRADIENT_WIDTH);
        }
    }

    else if (dist_to_box <= BOX_OUTER_GRADIENT_WIDTH)
    {

        intensity = 0.5f * (1.0f - dist_to_box / BOX_OUTER_GRADIENT_WIDTH);
    }

    if (is_pixel_in_corner(pixel_x, pixel_y, nearest_corner))
    {
        float dist_to_corner_edge =
            calculate_distance_to_corner_edge(pixel_x, pixel_y, nearest_corner);

        if (dist_to_corner_edge <= SCREEN_EDGE_GRADIENT_WIDTH)
        {
            float edge_intensity = 0.4f * (1.0f - dist_to_corner_edge / SCREEN_EDGE_GRADIENT_WIDTH);
            intensity = fmaxf(intensity, edge_intensity);

            if (dist_to_box <= BOX_OUTER_GRADIENT_WIDTH)
            {
                intensity *= 1.3f;
            }
        }
    }

    if (intensity > 0.8f)
        intensity = 0.8f;
    if (intensity < 0.0f)
        intensity = 0.0f;

    return intensity;
}

/* 图像处理：兼容旧接口：给出到检测框的距离比例、红色及限制后的透明度。 */
void calculate_fade_info(int pixel_x, int pixel_y, const BOX_RECT *box, FadeInfo *fade_info)
{
    if (!fade_info)
        return;

    fade_info->distance_ratio = 1.0f;
    fade_info->alpha = 0.0f;
    fade_info->color.r = 255;
    fade_info->color.g = 0;
    fade_info->color.b = 0;

    if (!box)
        return;

    float dist_to_box = calculate_distance_to_box(pixel_x, pixel_y, box);

    if (dist_to_box <= ALERT_MAX_FADE_DISTANCE)
    {
        fade_info->distance_ratio = fabsf(dist_to_box) / (float)ALERT_MAX_FADE_DISTANCE;

        if (dist_to_box <= 0)
        {
            fade_info->alpha = 0.6f * (1.0f + dist_to_box / BOX_INNER_GRADIENT_WIDTH);
        }
        else
        {
            fade_info->alpha = 0.4f * (1.0f - dist_to_box / ALERT_MAX_FADE_DISTANCE);
        }

        if (fade_info->alpha < 0.0f)
            fade_info->alpha = 0.0f;
        if (fade_info->alpha > 0.8f)
            fade_info->alpha = 0.8f;
    }
}

/* 图像处理：将 alpha 限制到 0..1，再按通道线性插值得到混合颜色。 */
Color blend_colors(Color original, Color effect, float alpha)
{
    Color result;

    if (alpha < 0.0f)
        alpha = 0.0f;
    if (alpha > 1.0f)
        alpha = 1.0f;

    result.r = (uint8_t)((1.0f - alpha) * original.r + alpha * effect.r);
    result.g = (uint8_t)((1.0f - alpha) * original.g + alpha * effect.g);
    result.b = (uint8_t)((1.0f - alpha) * original.b + alpha * effect.b);

    return result;
}

/* 图像处理：图像缓冲区按 B、G、R 顺序存放；先读原像素，按同一 alpha 混合，再按原顺序写回。 */
static void blend_red_pixel(uint8_t *rgb_data, int pixel_idx, float intensity)
{
    Color original = {rgb_data[pixel_idx + 2], rgb_data[pixel_idx + 1], rgb_data[pixel_idx]};
    Color effect_color = {255, 0, 0};
    Color blended = blend_colors(original, effect_color, intensity);
    rgb_data[pixel_idx] = blended.b;
    rgb_data[pixel_idx + 1] = blended.g;
    rgb_data[pixel_idx + 2] = blended.r;
}

/* 保留浮点表达式和混合次序，
 * 只跳过框体及圆角两项贡献都严格为零的区段。 */
static void draw_box_alert_span(uint8_t *rgb_data, int width, int y, int left, int right,
                                const BOX_RECT *box, CornerType corner, float blink)
{
    for (int x = left; x <= right; ++x)
    {
        float intensity = calculate_red_effect_intensity(x, y, box, corner);
        intensity *= blink;
        if (intensity <= 0.01f)
            continue;
        blend_red_pixel(rgb_data, (y * width + x) * 3, intensity);
    }
}

static void draw_corner_alert_span(uint8_t *rgb_data, int width, int y, int left, int right,
                                   CornerType corner, float blink)
{
    for (int x = left; x <= right; ++x)
    {
        float dist_to_edge = calculate_distance_to_corner_edge(x, y, corner);
        if (dist_to_edge <= SCREEN_EDGE_GRADIENT_WIDTH)
        {
            float intensity = 0.4f * (1.0f - dist_to_edge / SCREEN_EDGE_GRADIENT_WIDTH);
            intensity *= blink;
            if (intensity <= 0.01f)
                continue;
            blend_red_pixel(rgb_data, (y * width + x) * 3, intensity);
        }
    }
}

/* 图像处理：对人员框周围及其最近角的边缘分别叠加红色渐变，亮暗状态控制强度。 */
void draw_person_alert_effect(uint8_t *rgb_data, int width, int height, const BOX_RECT *person_box,
                              AlertEffectState *state)
{
    if (!rgb_data || !person_box || !state || !state->is_initialized)
        return;

    update_blink_state(state);

    float blink_multiplier = state->blink_state ? 1.0f : 0.2f;

    CornerType nearest_corner = get_nearest_corner(person_box);
    if (nearest_corner == CORNER_NONE)
        return;

    int box_left = person_box->left - BOX_OUTER_GRADIENT_WIDTH;
    int box_right = person_box->right + BOX_OUTER_GRADIENT_WIDTH;
    int box_top = person_box->top - BOX_OUTER_GRADIENT_WIDTH;
    int box_bottom = person_box->bottom + BOX_OUTER_GRADIENT_WIDTH;

    if (box_left < 0)
        box_left = 0;
    if (box_right >= width)
        box_right = width - 1;
    if (box_top < 0)
        box_top = 0;
    if (box_bottom >= height)
        box_bottom = height - 1;

    const int left_corner = corner_is_left(nearest_corner);
    const int top_corner = corner_is_top(nearest_corner);
    const int inner_left = person_box->left + BOX_INNER_GRADIENT_WIDTH;
    const int inner_right = person_box->right - BOX_INNER_GRADIENT_WIDTH;
    for (int y = box_top; y <= box_bottom; ++y)
    {
        int skip_left = inner_left, skip_right = inner_right;
        if (y < person_box->top + BOX_INNER_GRADIENT_WIDTH ||
            y > person_box->bottom - BOX_INNER_GRADIENT_WIDTH)
        {
            skip_left = 1;
            skip_right = 0;
        }
        else
        {
            /* 框内距离达到 40px 后框体亮度为零，
             * 但仍保留可能受到指定圆角 80px 范围梯度影响的部分。 */
            int in_corner_row = top_corner ? y < CORNER_HEIGHT : y >= SCREEN_HEIGHT - CORNER_HEIGHT;
            if (in_corner_row)
            {
                int edge_y = corner_edge_y(y, nearest_corner);
                if (left_corner)
                {
                    int last_edge_x = edge_y < SCREEN_EDGE_GRADIENT_WIDTH
                                          ? CORNER_WIDTH - 1
                                          : SCREEN_EDGE_GRADIENT_WIDTH - 1;
                    if (skip_left <= last_edge_x)
                        skip_left = last_edge_x + 1;
                }
                else
                {
                    int first_edge_x = edge_y < SCREEN_EDGE_GRADIENT_WIDTH
                                           ? SCREEN_WIDTH - CORNER_WIDTH
                                           : SCREEN_WIDTH - SCREEN_EDGE_GRADIENT_WIDTH;
                    if (skip_right >= first_edge_x)
                        skip_right = first_edge_x - 1;
                }
            }
        }
        if (skip_left < box_left)
            skip_left = box_left;
        if (skip_right > box_right)
            skip_right = box_right;
        if (skip_left <= skip_right)
        {
            draw_box_alert_span(rgb_data, width, y, box_left, skip_left - 1, person_box,
                                nearest_corner, blink_multiplier);
            draw_box_alert_span(rgb_data, width, y, skip_right + 1, box_right, person_box,
                                nearest_corner, blink_multiplier);
        }
        else
        {
            draw_box_alert_span(rgb_data, width, y, box_left, box_right, person_box, nearest_corner,
                                blink_multiplier);
        }
    }

    const int corner_left = left_corner ? 0 : width - CORNER_WIDTH;
    const int corner_top = top_corner ? 0 : height - CORNER_HEIGHT;
    const int corner_right = corner_left + CORNER_WIDTH - 1;
    const int corner_bottom = corner_top + CORNER_HEIGHT - 1;

    for (int y = corner_top; y <= corner_bottom; ++y)
    {
        int left = corner_left, right = corner_right;
        int edge_y = corner_edge_y(y, nearest_corner);
        if (edge_y >= SCREEN_EDGE_GRADIENT_WIDTH)
        {
            /* 距离达到 80 或更大时，原发光强度严格为零。 */
            if (left_corner)
            {
                if (right >= SCREEN_EDGE_GRADIENT_WIDTH)
                    right = SCREEN_EDGE_GRADIENT_WIDTH - 1;
            }
            else
            {
                int edge_start = SCREEN_WIDTH - SCREEN_EDGE_GRADIENT_WIDTH;
                if (left < edge_start)
                    left = edge_start;
            }
        }
        draw_corner_alert_span(rgb_data, width, y, left, right, nearest_corner, blink_multiplier);
    }
}

/* 为全局状态后遍历检测结果，仅对名称为 person 的框绘制特效。建立初始状态。 */
void apply_person_alert_effects(uint8_t *rgb_data, int width, int height,
                                const detect_result_group_t *results)
{
    if (!rgb_data || !results)
        return;

    if (!g_alert_state.is_initialized)
    {
        alert_effects_init();
    }

    for (int i = 0; i < results->count; i++)
    {
        const detect_result_t *result = &results->results[i];

        if (strcmp(result->name, "person") == 0)
        {

            draw_person_alert_effect(rgb_data, width, height, &result->box, &g_alert_state);
        }
    }
}