/*
 * render_lcd_x11.c — LCD 屏（Linux/X11）渲染实现（【人员 B】）
 *
 * 双路拼接 1280x480：
 *   左半 640x480 = 本地 PCIe 图（S 端 shm_pcie_img，含行人框）；
 *   右半 640x480 = 远端 UDP 图（S 端 shm_udp_img，M 端车道图）。
 * RGB565 → RGB888 后经 XImage 上屏。
 *
 * 非 Linux 环境编译为桩（printf 帧号），保证本机 make 全绿。
 */
#include "render_lcd.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct render_lcd_ctx {
    int win_w;
    int win_h;
    int is_stub;
    uint8_t *canvas;      /* RGB888 画布 win_w*win_h*3 */
#if defined(__linux__)
    void *disp;           /* Display* */
    void *win;            /* Window（存储为整型句柄） */
    void *ximg;           /* XImage* */
    void *gc;
#endif
};

#if defined(__linux__)
/* RGB565 → X11 32bpp ZPixmap 像素（BGRX，每像素 4 字节）。
 * canvas 分配为 win_w*win_h*4，XCreateImage 的 bytes_per_line 也是 win_w*4，
 * 即 24bpp 按 32bit/像素存放。小端（RK3568 ARM）下 0x00RRGGBB 的内存顺序为
 * B,G,R,0。旧实现只写 3 字节且按 R,G,B 顺序，既造成行 stride 错位（3≠4），
 * 又把红蓝通道写反，导致上屏画面撕裂 + 颜色错乱。 */
static void rgb565_line_to_xrgb(const uint8_t *src, uint8_t *dst, int n)
{
    for (int i = 0; i < n; i++) {
        uint16_t p = (uint16_t)((src[0]) | ((uint16_t)src[1] << 8));
        uint8_t r5 = (uint8_t)((p >> 11) & 0x1F);
        uint8_t g6 = (uint8_t)((p >> 5) & 0x3F);
        uint8_t b5 = (uint8_t)(p & 0x1F);
        dst[0] = (uint8_t)((b5 << 3) | (b5 >> 2));   /* B */
        dst[1] = (uint8_t)((g6 << 2) | (g6 >> 4));   /* G */
        dst[2] = (uint8_t)((r5 << 3) | (r5 >> 2));   /* R */
        dst[3] = 0;                                   /* X（未用填充字节） */
        src += 2; dst += 4;
    }
}

/* 把一幅 640x480 RGB565 画到 canvas 的 (dx, dy) 处（32bpp，每像素 4 字节） */
static void blit_565(const uint8_t *src565, uint8_t *canvas,
                     int canvas_w, int dx, int dy, int w, int h)
{
    for (int y = 0; y < h; y++) {
        uint8_t *dst = canvas + ((size_t)(dy + y) * canvas_w + dx) * 4u;
        const uint8_t *src = src565 + (size_t)y * w * 2u;
        rgb565_line_to_xrgb(src, dst, w);
    }
}

/* ---------- X11 真实现 ---------- */
#include <X11/Xlib.h>
#include <X11/Xutil.h>

int render_lcd_init(render_lcd_ctx_t **ctx, int win_w, int win_h)
{
    if (!ctx) return -1;
    render_lcd_ctx_t *c = (render_lcd_ctx_t *)calloc(1, sizeof(*c));
    if (!c) return -1;
    c->win_w = win_w > 0 ? win_w : IMG_WIDTH * 2;
    c->win_h = win_h > 0 ? win_h : IMG_HEIGHT;
    c->is_stub = 0;

    Display *d = XOpenDisplay(NULL);
    if (!d) { free(c); return -1; }
    int scr = DefaultScreen(d);
    Window w = XCreateSimpleWindow(d, RootWindow(d, scr), 0, 0,
                                   (unsigned)c->win_w, (unsigned)c->win_h, 1,
                                   BlackPixel(d, scr), WhitePixel(d, scr));
    XStoreName(d, w, "ADAS-Display");
    XMapWindow(d, w);

    c->canvas = (uint8_t *)calloc(1, (size_t)c->win_w * c->win_h * 4u);  /* 24bpp + 1 填充字节/像素 */
    if (!c->canvas) { XCloseDisplay(d); free(c); return -1; }

    XImage *img = XCreateImage(d, DefaultVisual(d, scr), 24, ZPixmap, 0,
                               (char *)c->canvas, (unsigned)c->win_w,
                               (unsigned)c->win_h, 32, c->win_w * 4);
    if (!img) { free(c->canvas); XCloseDisplay(d); free(c); return -1; }

    GC gc = XCreateGC(d, w, 0, NULL);
    c->disp = d; c->win = (void *)w; c->ximg = img; c->gc = gc;
    *ctx = c;
    return 0;
}

int render_lcd_draw(render_lcd_ctx_t *ctx,
                    const void *local565, const void *remote565,
                    DisplayMode mode, uint32_t frame_id)
{
    (void)frame_id;
    if (!ctx || ctx->is_stub) return -1;
    Display *d = (Display *)ctx->disp;
    Window w = (Window)ctx->win;
    XImage *img = (XImage *)ctx->ximg;
    GC gc = (GC)ctx->gc;

    memset(ctx->canvas, 0, (size_t)ctx->win_w * ctx->win_h * 4u);

    const uint8_t *loc = (const uint8_t *)local565;
    const uint8_t *rem = (const uint8_t *)remote565;

    if (mode == DISPLAY_MODE_SPLIT || mode == DISPLAY_MODE_LOCAL_ONLY) {
        if (loc) blit_565(loc, ctx->canvas, ctx->win_w, 0, 0, IMG_WIDTH, IMG_HEIGHT);
    }
    if (mode == DISPLAY_MODE_SPLIT || mode == DISPLAY_MODE_REMOTE_ONLY) {
        if (rem) blit_565(rem, ctx->canvas, ctx->win_w, IMG_WIDTH, 0, IMG_WIDTH, IMG_HEIGHT);
    }
    /* BLANK：保持黑屏 */

    XPutImage(d, w, gc, img, 0, 0, 0, 0, (unsigned)ctx->win_w, (unsigned)ctx->win_h);
    XFlush(d);
    return 0;
}

void render_lcd_deinit(render_lcd_ctx_t *ctx)
{
    if (!ctx) return;
    if (!ctx->is_stub && ctx->disp) {
        Display *d = (Display *)ctx->disp;
        if (ctx->gc) XFreeGC(d, (GC)ctx->gc);
        if (ctx->ximg) {
            /* XDestroyImage 会释放 image 结构及 data（canvas） */
            XDestroyImage((XImage *)ctx->ximg);
            ctx->canvas = NULL;
        }
        XCloseDisplay(d);
    }
    free(ctx->canvas);
    free(ctx);
}

#else  /* !__linux__：桩实现 */

int render_lcd_init(render_lcd_ctx_t **ctx, int win_w, int win_h)
{
    if (!ctx) return -1;
    render_lcd_ctx_t *c = (render_lcd_ctx_t *)calloc(1, sizeof(*c));
    if (!c) return -1;
    c->win_w = win_w > 0 ? win_w : IMG_WIDTH * 2;
    c->win_h = win_h > 0 ? win_h : IMG_HEIGHT;
    c->is_stub = 1;
    c->canvas = NULL;
    *ctx = c;
    return 0;
}

int render_lcd_draw(render_lcd_ctx_t *ctx,
                    const void *local565, const void *remote565,
                    DisplayMode mode, uint32_t frame_id)
{
    if (!ctx) return -1;
    (void)local565; (void)remote565;
    printf("[LCD-STUB] frame=%u mode=%d (本机无 X11，仅打印)\n", frame_id, (int)mode);
    return 0;
}

void render_lcd_deinit(render_lcd_ctx_t *ctx)
{
    if (!ctx) return;
    free(ctx->canvas);
    free(ctx);
}
#endif /* __linux__ */
