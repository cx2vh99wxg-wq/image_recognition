/*
 * zebra_detect.c — 斑马线（人行横道）识别实现（【人员 A · 感知】）
 *
 * 见 zebra_detect.h 的算法说明。逐行灰度剖面 + 自适应二值化 + 跳变/段数统计。
 */
#include "zebra_detect.h"

#include <stdlib.h>
#include <string.h>

/* 定点近似灰度：0.299R + 0.587G + 0.114B */
static int zb_gray(const uint8_t *rgb, int w, int x, int y)
{
    const uint8_t *p = rgb + ((size_t)y * (size_t)w + (size_t)x) * 3u;
    return ((int)p[0] * 77 + (int)p[1] * 150 + (int)p[2] * 29) >> 8;
}

int zebra_detect_rows(const uint8_t *rgb888, int w, int h, ZebraResult *out,
                      uint16_t *stripe_rows,uint32_t *stripe_count)
{
    if(stripe_count)*stripe_count=0;
    if (!rgb888 || !out || w <= 0 || h <= 0) return -1;

    memset(out, 0, sizeof(*out));
    out->version  = ZEBRA_VERSION;
    out->center_y = -1;

    int y0 = (int)((float)h * ZEBRA_ROI_TOP_FRAC);
    int y1 = (int)((float)h * ZEBRA_ROI_BOT_FRAC);
    int x0 = (int)((float)w * ZEBRA_ROI_L_FRAC);
    int x1 = (int)((float)w * ZEBRA_ROI_R_FRAC);
    if (y0 < 0) y0 = 0;
    if (y1 > h) y1 = h;
    if (x0 < 0) x0 = 0;
    if (x1 > w) x1 = w;

    const int rows = y1 - y0;
    const int cols = x1 - x0;
    if (rows < 8 || cols < 8) return -1;

    int *prof = (int *)malloc((size_t)rows * sizeof(int));
    if (!prof) return -1;

    /* 1) 逐行求水平灰度均值，构成沿 y 的亮度剖面 */
    int pmax = 0, pmin = 255;
    for (int i = 0; i < rows; i++) {
        const int y = y0 + i;
        long sum = 0;
        for (int x = x0; x < x1; x++) sum += zb_gray(rgb888, w, x, y);
        int v = (int)(sum / cols);
        prof[i] = v;
        if (v > pmax) pmax = v;
        if (v < pmin) pmin = v;
    }

    const int contrast = pmax - pmin;
    out->contrast = (uint32_t)contrast;

    if (contrast < ZEBRA_MIN_CONTRAST) {   /* 无明显明暗条纹 */
        free(prof);
        return 0;
    }

    /* 2) 自适应阈值二值化 + 统计亮条纹段数与跳变 */
    const int thr = (pmax + pmin) / 2;
    int stripes = 0;          /* 亮条纹段数（暗→亮的上升沿数） */
    int bright = 0;           /* 亮行总数 */
    long long sum_bright_y = 0;
    int prev = prof[0] > thr;
    if (prev) { stripes = 1; bright = 1; sum_bright_y = 0; }
    for (int i = 1; i < rows; i++) {
        const int cur = prof[i] > thr;
        if (cur && !prev) stripes++;    /* 上升沿 → 新的一条亮条纹 */
        if (cur) { bright++; sum_bright_y += i; }
        prev = cur;
    }

    out->stripe_count = (uint32_t)stripes;
    if (bright > 0) out->center_y = y0 + (int)(sum_bright_y / bright);

    /* 3) 判定：亮条纹段数落在合理区间 → 斑马线 */
    if (stripes >= ZEBRA_MIN_STRIPES && stripes <= ZEBRA_MAX_STRIPES) {
        out->detected = 1;
        float sc = (float)stripes / (float)ZEBRA_MIN_STRIPES;   /* ≥1 */
        if (sc > 2.0f) sc = 2.0f;
        float conf = (sc / 2.0f) * 60.0f + ((float)contrast / 255.0f) * 40.0f;
        if (conf > 100.0f) conf = 100.0f;
        out->confidence = (uint32_t)(conf + 0.5f);
    }

    if(out->detected && stripe_rows && stripe_count) {
        int begin=-1;
        for(int i=0;i<=rows;i++) {
            int on=i<rows&&prof[i]>thr;
            if(on&&begin<0)begin=i;
            if(!on&&begin>=0){
                if(*stripe_count<16)stripe_rows[(*stripe_count)++]=(uint16_t)(y0+(begin+i-1)/2);
                begin=-1;
            }
        }
    }
    free(prof);
    return 0;
}
int zebra_detect(const uint8_t *rgb,int w,int h,ZebraResult *out)
{
    return zebra_detect_rows(rgb,w,h,out,NULL,NULL);
}
