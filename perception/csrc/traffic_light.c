/*
 * traffic_light.c — 红绿灯（灯色）识别实现（【人员 A · 感知】）
 *
 * 见 traffic_light.h 的算法说明。颜色分类 + 三色连通块分析 + 最大面积选色。
 */
#include "traffic_light.h"

#include <stdlib.h>
#include <string.h>

/* 类别索引 */
enum { C_RED = 0, C_YELLOW = 1, C_GREEN = 2, C_NONE = 3 };

/*
 * 判定单像素（R/G/B）最可能属于哪一类灯色。
 * 依据：归一化 RGB（以最大通道为基准）+ 饱和度 + 亮度（发光）。
 * 返回 C_RED / C_YELLOW / C_GREEN / C_NONE。
 */
static int tl_classify(uint8_t r, uint8_t g, uint8_t b)
{
    int v = (r > g) ? (r > b ? r : b) : (g > b ? g : b);
    if (v < TL_MIN_BRIGHT) return C_NONE;            /* 不发光 */

    float rn = (float)r / (float)v;
    float gn = (float)g / (float)v;
    float bn = (float)b / (float)v;

    /* 饱和度：最暗通道占比越低越饱和 */
    int dmin = (r < g) ? (r < b ? r : b) : (g < b ? g : b);
    float sat = 1.0f - (float)dmin / (float)v;
    if (sat < TL_SAT_MIN) return C_NONE;             /* 灰白，非纯色灯 */

    if (rn > 0.55f && gn < 0.45f && bn < 0.45f) return C_RED;
    if (gn > 0.55f && rn < 0.45f && bn < 0.45f) return C_GREEN;
    if (rn > 0.50f && gn > 0.40f && bn < 0.35f && rn >= gn) return C_YELLOW;

    return C_NONE;
}

/*
 * 对二值掩码（0/1，尺寸 rw×rh）做连通块分析，返回最大连通块面积与包围盒。
 * 使用显式栈 BFS（4 邻域），避免递归栈溢出。包围盒坐标相对掩码左上角。
 */
static int max_blob(const uint8_t *mask, int rw, int rh,
                    int *bx, int *by, int *bw, int *bh)
{
    *bx = *by = *bw = *bh = 0;
    const int n = rw * rh;
    uint8_t *vis = (uint8_t *)calloc((size_t)n, 1);
    int *stack = (int *)malloc((size_t)n * sizeof(int));
    if (!vis || !stack) { free(vis); free(stack); return 0; }

    int best = 0, bbx = 0, bby = 0, bbw = 0, bbh = 0;

    for (int y = 0; y < rh; y++) {
        for (int x = 0; x < rw; x++) {
            int idx = y * rw + x;
            if (!mask[idx] || vis[idx]) continue;

            int area = 0, x0 = x, x1 = x, y0 = y, y1 = y;
            int sp = 0;
            stack[sp++] = idx; vis[idx] = 1;
            while (sp > 0) {
                int p = stack[--sp];
                int py = p / rw, px = p % rw;
                area++;
                if (px < x0) x0 = px;
                if (px > x1) x1 = px;
                if (py < y0) y0 = py;
                if (py > y1) y1 = py;

                if (px > 0)        { int q = p - 1; if (mask[q] && !vis[q]) { vis[q] = 1; stack[sp++] = q; } }
                if (px < rw - 1)    { int q = p + 1; if (mask[q] && !vis[q]) { vis[q] = 1; stack[sp++] = q; } }
                if (py > 0)        { int q = p - rw; if (mask[q] && !vis[q]) { vis[q] = 1; stack[sp++] = q; } }
                if (py < rh - 1)   { int q = p + rw; if (mask[q] && !vis[q]) { vis[q] = 1; stack[sp++] = q; } }
            }

            if (area > best) {
                best = area; bbx = x0; bby = y0;
                bbw = x1 - x0 + 1; bbh = y1 - y0 + 1;
            }
        }
    }

    free(stack); free(vis);
    *bx = bbx; *by = bby; *bw = bbw; *bh = bbh;
    return best;
}

int traffic_light_detect(const uint8_t *rgb888, int w, int h,
                         TrafficLightResult *out)
{
    if (!rgb888 || !out || w <= 0 || h <= 0) return -1;

    memset(out, 0, sizeof(*out));
    out->version = TL_VERSION;
    out->box_x = -1; out->box_y = -1; out->box_w = -1; out->box_h = -1;

    int y0 = (int)((float)h * TL_ROI_TOP_FRAC);
    int y1 = (int)((float)h * TL_ROI_BOT_FRAC);
    if (y0 < 0) y0 = 0;
    if (y1 > h) y1 = h;
    if (y1 <= y0) return 0;                 /* ROI 非法，无检出 */

    const int rw = w;
    const int rh = y1 - y0;

    /* 三色掩码（仅 ROI 区域） */
    uint8_t *masks[3];
    masks[0] = (uint8_t *)calloc((size_t)rw * rh, 1);
    masks[1] = (uint8_t *)calloc((size_t)rw * rh, 1);
    masks[2] = (uint8_t *)calloc((size_t)rw * rh, 1);
    if (!masks[0] || !masks[1] || !masks[2]) {
        free(masks[0]); free(masks[1]); free(masks[2]);
        return -1;
    }

    for (int y = y0; y < y1; y++) {
        for (int x = 0; x < rw; x++) {
            const uint8_t *p = rgb888 + ((size_t)y * (size_t)w + (size_t)x) * 3u;
            int cls = tl_classify(p[0], p[1], p[2]);
            int mi = y - y0;
            int off = mi * rw + x;
            if (cls == C_RED)    masks[0][off] = 1;
            else if (cls == C_YELLOW) masks[1][off] = 1;
            else if (cls == C_GREEN)  masks[2][off] = 1;
        }
    }

    int areas[3];
    int boxes[3][4];   /* x,y,w,h */
    for (int c = 0; c < 3; c++) {
        areas[c] = max_blob(masks[c], rw, rh,
                            &boxes[c][0], &boxes[c][1], &boxes[c][2], &boxes[c][3]);
    }
    out->red_area    = (uint32_t)areas[0];
    out->yellow_area = (uint32_t)areas[1];
    out->green_area  = (uint32_t)areas[2];

    /* 选面积最大且超过噪声阈值者为点亮灯 */
    int best_c = -1;
    int best_a = TL_MIN_AREA - 1;
    for (int c = 0; c < 3; c++) {
        if (areas[c] > best_a) { best_a = areas[c]; best_c = c; }
    }

    if (best_c < 0) {
        out->state = TL_UNKNOWN;
        out->detected = 0;
    } else {
        static const TrafficLightState map[3] = { TL_RED, TL_YELLOW, TL_GREEN };
        out->state = map[best_c];
        out->detected = 1;
        out->box_x = boxes[best_c][0];
        out->box_y = boxes[best_c][1] + y0;     /* 还原到全图坐标 */
        out->box_w = boxes[best_c][2];
        out->box_h = boxes[best_c][3];

        /* 真实置信度：点亮面积 / 参考面积，封顶 100（禁硬编码） */
        float conf = (float)best_a / (float)TL_REF_AREA * 100.0f;
        if (conf > 100.0f) conf = 100.0f;
        out->confidence = (uint32_t)(conf + 0.5f);
    }

    free(masks[0]); free(masks[1]); free(masks[2]);
    return 0;
}
