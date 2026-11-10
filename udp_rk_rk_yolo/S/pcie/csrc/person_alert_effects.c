#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "person_alert_effects.h"
#include "../../../common/image/frame_pixels.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <unistd.h>
#if defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h>
#endif

typedef struct { float *on, *off; } AlertMaskBuild;

static void accumulate_alert_mask(AlertMaskBuild *mask, int index, float intensity)
{
    if (intensity > 0.01f)
        mask->on[index] *= 1.0f - intensity;
    intensity *= 0.2f;
    if (intensity > 0.01f)
        mask->off[index] *= 1.0f - intensity;
}

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

    const int dx = x > 0 ? x : 0, dy = y > 0 ? y : 0;
    // Every consumer only distinguishes distances up to 60 pixels.
    if (dx > BOX_OUTER_GRADIENT_WIDTH || dy > BOX_OUTER_GRADIENT_WIDTH)
        return BOX_OUTER_GRADIENT_WIDTH + 1.0f;
    static float distances[61][61];
    static int ready;
    if (!ready)
    {
        for (int a = 0; a <= 60; ++a)
            for (int b = 0; b <= 60; ++b)
                distances[a][b] = sqrtf((float)(a * a + b * b));
        ready = 1;
    }
    return distances[dx][dy];
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
                                const BOX_RECT *box, CornerType corner, float blink,
                                AlertMaskBuild *mask)
{
    for (int x = left; x <= right; ++x)
    {
        float intensity = calculate_red_effect_intensity(x, y, box, corner);
        if (mask)
        {
            accumulate_alert_mask(mask, y * width + x, intensity);
            continue;
        }
        intensity *= blink;
        if (intensity <= 0.01f)
            continue;
        blend_red_pixel(rgb_data, (y * width + x) * 3, intensity);
    }
}

static void draw_corner_alert_span(uint8_t *rgb_data, int width, int y, int left, int right,
                                   CornerType corner, float blink, AlertMaskBuild *mask)
{
    for (int x = left; x <= right; ++x)
    {
        float dist_to_edge = calculate_distance_to_corner_edge(x, y, corner);
        if (dist_to_edge <= SCREEN_EDGE_GRADIENT_WIDTH)
        {
            float intensity = 0.4f * (1.0f - dist_to_edge / SCREEN_EDGE_GRADIENT_WIDTH);
            if (mask)
            {
                accumulate_alert_mask(mask, y * width + x, intensity);
                continue;
            }
            intensity *= blink;
            if (intensity <= 0.01f)
                continue;
            blend_red_pixel(rgb_data, (y * width + x) * 3, intensity);
        }
    }
}

/* 图像处理：对人员框周围及其最近角的边缘分别叠加红色渐变，亮暗状态控制强度。 */
static void draw_person_alert_impl(uint8_t *rgb_data, int width, int height,
                                   const BOX_RECT *person_box, AlertEffectState *state,
                                   AlertMaskBuild *mask)
{
    if ((!rgb_data && !mask) || !person_box || !state || !state->is_initialized)
        return;

    if (!mask)
        update_blink_state(state);

    float blink_multiplier = mask || state->blink_state ? 1.0f : 0.2f;

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
                                nearest_corner, blink_multiplier, mask);
            draw_box_alert_span(rgb_data, width, y, skip_right + 1, box_right, person_box,
                                nearest_corner, blink_multiplier, mask);
        }
        else
        {
            draw_box_alert_span(rgb_data, width, y, box_left, box_right, person_box, nearest_corner,
                                blink_multiplier, mask);
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
        draw_corner_alert_span(rgb_data, width, y, left, right, nearest_corner, blink_multiplier, mask);
    }
}

void draw_person_alert_effect(uint8_t *rgb_data, int width, int height, const BOX_RECT *person_box,
                              AlertEffectState *state)
{
    draw_person_alert_impl(rgb_data, width, height, person_box, state, NULL);
}

typedef struct
{
    int width, height, count, valid;
    BOX_RECT boxes[OBJ_NUMB_MAX_SIZE];
    float *on, *off;
    uint16_t *alpha_on, *alpha_off;
    uint8_t *blocks_on, *blocks_off;
} AlertMaskCache;
static AlertMaskCache alert_caches[2];

static int prepare_alert_cache(AlertMaskCache *c, int width, int height, const BOX_RECT *boxes, int count)
{
    const size_t n = (size_t)width * height;
    const size_t blocks = (n + 63) / 64;
    if (c->valid && c->width == width && c->height == height && c->count == count &&
        !memcmp(c->boxes, boxes, (size_t)count * sizeof(*boxes)))
        return 1;
    if (!c->on || c->width != width || c->height != height)
    {
        float *on = (float *)malloc(n * sizeof(float)), *off = (float *)malloc(n * sizeof(float));
        uint16_t *a = (uint16_t *)malloc(n * sizeof(uint16_t)), *b = (uint16_t *)malloc(n * sizeof(uint16_t));
        uint8_t *active_on = (uint8_t *)malloc(blocks), *active_off = (uint8_t *)malloc(blocks);
        if (!on || !off || !a || !b || !active_on || !active_off)
        {
            free(on); free(off); free(a); free(b);
            free(active_on); free(active_off);
            return 0;
        }
        free(c->on); free(c->off); free(c->alpha_on); free(c->alpha_off);
        free(c->blocks_on); free(c->blocks_off);
        c->on = on; c->off = off; c->alpha_on = a; c->alpha_off = b;
        c->blocks_on = active_on; c->blocks_off = active_off;
    }
    c->width = width; c->height = height; c->count = count; c->valid = 0;
    memcpy(c->boxes, boxes, (size_t)count * sizeof(*boxes));
    for (size_t i = 0; i < n; ++i)
        c->on[i] = c->off[i] = 1.0f;
    AlertMaskBuild mask = {c->on, c->off};
    AlertEffectState build_state = {0, 1, 1};
    for (int i = 0; i < count; ++i)
        draw_person_alert_impl(NULL, width, height, boxes + i, &build_state, &mask);
    memset(c->blocks_on, 0, blocks); memset(c->blocks_off, 0, blocks);
    for (size_t i = 0; i < n; ++i)
    {
        float a = (1.0f - c->on[i]) * 256.0f + 0.5f;
        float b = (1.0f - c->off[i]) * 256.0f + 0.5f;
        c->alpha_on[i] = (uint16_t)(a > 256.0f ? 256.0f : a);
        c->alpha_off[i] = (uint16_t)(b > 256.0f ? 256.0f : b);
        if (c->alpha_on[i]) c->blocks_on[i / 64] = 1;
        if (c->alpha_off[i]) c->blocks_off[i / 64] = 1;
    }
    c->valid = 1;
    return 1;
}

static void blend_alert_span(uint8_t *rgb, const uint16_t *alpha, int length)
{
    int i = 0;
#if defined(__ARM_NEON) || defined(__aarch64__)
    const int vector_end = length & ~7;
    for (; i < vector_end; i += 8)
    {
        uint8x8x3_t pixels = vld3_u8(rgb + i * 3);
        const uint16x8_t a = vld1q_u16(alpha + i), inv = vsubq_u16(vdupq_n_u16(256), a);
        pixels.val[0] = vshrn_n_u16(vmulq_u16(vmovl_u8(pixels.val[0]), inv), 8);
        pixels.val[1] = vshrn_n_u16(vmulq_u16(vmovl_u8(pixels.val[1]), inv), 8);
        pixels.val[2] = vshrn_n_u16(vmlaq_n_u16(vmulq_u16(vmovl_u8(pixels.val[2]), inv), a, 255), 8);
        vst3_u8(rgb + i * 3, pixels);
    }
#endif
    for (; i < length; ++i)
    {
        unsigned a = alpha[i], inv = 256 - a;
        rgb[i * 3] = (uint8_t)(rgb[i * 3] * inv >> 8);
        rgb[i * 3 + 1] = (uint8_t)(rgb[i * 3 + 1] * inv >> 8);
        rgb[i * 3 + 2] = (uint8_t)((rgb[i * 3 + 2] * inv + 255 * a) >> 8);
    }
}

static void blend_cached_alert(uint8_t *rgb, const uint16_t *alpha,
                                const uint8_t *active, size_t count)
{
    if (count > SCREEN_WIDTH * SCREEN_HEIGHT) return;
    for (size_t block = 0; block < (count + 63) / 64; ++block)
    {
        if (!active[block]) continue;
        const size_t first = block * 64;
        const int length = (int)(count - first < 64 ? count - first : 64);
        blend_alert_span(rgb + first * 3, alpha + first, length);
    }
}

static void blend_cached_alert565(uint16_t *pixels,const uint16_t *alpha,
                                  const uint8_t *active,size_t count)
{
    for(size_t block=0;block<(count+63)/64;++block){
        if(!active[block])continue;
        size_t first=block*64,end=count-first<64?count:first+64,i=first;
#if defined(__ARM_NEON) || defined(__aarch64__)
        for(;i+8<=end;i+=8){
            const uint16x8_t p=vld1q_u16(pixels+i),a=vld1q_u16(alpha+i),inv=vsubq_u16(vdupq_n_u16(256),a);
            uint16x8_t hi=vshrq_n_u16(p,11),mid=vandq_u16(vshrq_n_u16(p,5),vdupq_n_u16(63)),lo=vandq_u16(p,vdupq_n_u16(31));
            hi=vorrq_u16(vshlq_n_u16(hi,3),vshrq_n_u16(hi,2));mid=vorrq_u16(vshlq_n_u16(mid,2),vshrq_n_u16(mid,4));lo=vorrq_u16(vshlq_n_u16(lo,3),vshrq_n_u16(lo,2));
            hi=vshrq_n_u16(vmulq_u16(hi,inv),8);mid=vshrq_n_u16(vmulq_u16(mid,inv),8);
            lo=vshrq_n_u16(vmlaq_n_u16(vmulq_u16(lo,inv),a,255),8);
            vst1q_u16(pixels+i,vorrq_u16(vorrq_u16(vshlq_n_u16(vandq_u16(hi,vdupq_n_u16(248)),8),vshlq_n_u16(vandq_u16(mid,vdupq_n_u16(252)),3)),vshrq_n_u16(lo,3)));
        }
#endif
        for(;i<end;++i){
            const unsigned p=pixels[i],a=alpha[i],inv=256-a;
            const unsigned hi=frame_expand_5bit(p>>11),g=(p>>5)&63,mid=(g<<2)|(g>>4),lo=frame_expand_5bit(p&31);
            pixels[i]=(uint16_t)((((hi*inv>>8)&248)<<8)|(((mid*inv>>8)&252)<<3)|((lo*inv+255*a)>>11));
        }
    }
}

static void blend_alert_target(uint8_t *rgb,uint16_t *native,const AlertMaskCache *c,int width,int height)
{
    const int on=g_alert_state.blink_state;
    if(native)blend_cached_alert565(native,on?c->alpha_on:c->alpha_off,on?c->blocks_on:c->blocks_off,(size_t)width*height);
    else blend_cached_alert(rgb,on?c->alpha_on:c->alpha_off,on?c->blocks_on:c->blocks_off,(size_t)width*height);
}
static void draw_alert_fallback(uint8_t *rgb,uint16_t *native,uint8_t *scratch,int width,int height,const BOX_RECT *boxes,int count)
{
    uint8_t *out=native?scratch:rgb;
    if(native)frame_unpack_rgb565(native,out,(size_t)width*height);
    for(int i=0;i<count;++i)draw_person_alert_effect(out,width,height,boxes+i,&g_alert_state);
    if(native)frame_pack_rgb565(out,native,(size_t)width*height);
}

typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t changed;
    int started, failed, pending, building, requested;
    int active, width, height, count;
    BOX_RECT boxes[OBJ_NUMB_MAX_SIZE];
} AlertMaskWorker;
static AlertMaskWorker alert_worker = {
    .mutex = PTHREAD_MUTEX_INITIALIZER, .changed = PTHREAD_COND_INITIALIZER, .active = -1
};

static void *alert_mask_worker(void *unused)
{
    (void)unused;
#ifdef __linux__
    // Cosmetic work must not inherit the capture thread's high priority.
    setpriority(PRIO_PROCESS, (id_t)syscall(SYS_gettid), 0);
#endif
    for (;;)
    {
        BOX_RECT boxes[OBJ_NUMB_MAX_SIZE];
        pthread_mutex_lock(&alert_worker.mutex);
        while (!alert_worker.pending)
            pthread_cond_wait(&alert_worker.changed, &alert_worker.mutex);
        int width = alert_worker.width, height = alert_worker.height, count = alert_worker.count;
        int target = alert_worker.active == 0 ? 1 : 0;
        memcpy(boxes, alert_worker.boxes, (size_t)count * sizeof(*boxes));
        alert_worker.pending = 0; alert_worker.building = 1;
        pthread_mutex_unlock(&alert_worker.mutex);
        int ok = prepare_alert_cache(&alert_caches[target], width, height, boxes, count);
        pthread_mutex_lock(&alert_worker.mutex);
        if (ok)
            alert_worker.active = target;
        alert_worker.building = 0;
        pthread_cond_broadcast(&alert_worker.changed);
        pthread_mutex_unlock(&alert_worker.mutex);
    }
    return NULL;
}

static int start_alert_worker(void)
{
    if (alert_worker.started) return 1;
    if (alert_worker.failed) return 0;
    // Initialize the distance table before it can be read by two threads.
    const BOX_RECT warm = {0, 0, 0, 0};
    (void)calculate_distance_to_box(1, 1, &warm);
    pthread_t thread;
    if (pthread_create(&thread, NULL, alert_mask_worker, NULL))
    {
        alert_worker.failed = 1;
        return 0;
    }
    pthread_detach(thread);
    alert_worker.started = 1;
    return 1;
}

#ifdef PERSON_ALERT_CACHE_TEST
int person_alert_cache_wait(unsigned timeout_ms)
{
    if (!alert_worker.started) return 1;
    struct timespec until; clock_gettime(CLOCK_REALTIME, &until);
    until.tv_sec += timeout_ms / 1000;
    until.tv_nsec += (long)(timeout_ms % 1000) * 1000000L;
    if (until.tv_nsec >= 1000000000L) { ++until.tv_sec; until.tv_nsec -= 1000000000L; }
    pthread_mutex_lock(&alert_worker.mutex);
    int ok = 1;
    while (alert_worker.pending || alert_worker.building)
        if (pthread_cond_timedwait(&alert_worker.changed, &alert_worker.mutex, &until) == ETIMEDOUT)
        { ok = 0; break; }
    pthread_mutex_unlock(&alert_worker.mutex);
    return ok;
}
#endif

/* 为全局状态后遍历检测结果，仅对名称为 person 的框绘制特效。建立初始状态。 */
static void apply_person_alert_impl(uint8_t *rgb_data,uint16_t *native,uint8_t *scratch,
                                    int width,int height,const detect_result_group_t *results)
{
    if ((!rgb_data&&!native) || !results)
        return;

    if (!g_alert_state.is_initialized)
    {
        alert_effects_init();
    }

    BOX_RECT boxes[OBJ_NUMB_MAX_SIZE];
    int count = 0;
    for (int i = 0; i < results->count && i < OBJ_NUMB_MAX_SIZE; i++)
    {
        const detect_result_t *result = &results->results[i];
        if (strcmp(result->name, "person") == 0)
            boxes[count++] = result->box;
    }
    if (!count)
        return;
    update_blink_state(&g_alert_state);
    // With many overlapping people, retain the legacy sequential byte
    // rounding instead of accumulating a visible composition difference.
    if (count > 8 || width != SCREEN_WIDTH || height != SCREEN_HEIGHT)
    {
        draw_alert_fallback(rgb_data,native,scratch,width,height,boxes,count);
        return;
    }
    if (!start_alert_worker())
    {
        AlertMaskCache *cache = &alert_caches[0];
        if (prepare_alert_cache(cache, width, height, boxes, count))
            blend_alert_target(rgb_data,native,cache,width,height);
        else
            draw_alert_fallback(rgb_data,native,scratch,width,height,boxes,count);
        return;
    }
    pthread_mutex_lock(&alert_worker.mutex);
    if (!alert_worker.requested || alert_worker.width != width || alert_worker.height != height ||
        alert_worker.count != count || memcmp(alert_worker.boxes, boxes, (size_t)count * sizeof(*boxes)))
    {
        alert_worker.width = width; alert_worker.height = height; alert_worker.count = count;
        memcpy(alert_worker.boxes, boxes, (size_t)count * sizeof(*boxes));
        alert_worker.requested = alert_worker.pending = 1;
        pthread_cond_signal(&alert_worker.changed);
    }
    // The capture thread only blends a finished mask. The worker owns the
    // other buffer; it never touches camera pixels or braking metadata.
    if (alert_worker.active >= 0)
    {
        AlertMaskCache *cache = &alert_caches[alert_worker.active];
        blend_alert_target(rgb_data,native,cache,width,height);
    }
    pthread_mutex_unlock(&alert_worker.mutex);
}

void apply_person_alert_effects(uint8_t *rgb,int width,int height,const detect_result_group_t *results)
{
    apply_person_alert_impl(rgb,NULL,NULL,width,height,results);
}
int apply_person_alert_effects565(uint16_t *pixels,int width,int height,const detect_result_group_t *results,uint8_t *scratch)
{
    if(!pixels||!scratch||!results||width<=0||height<=0)return -1;
    apply_person_alert_impl(NULL,pixels,scratch,width,height,results);return 0;
}
