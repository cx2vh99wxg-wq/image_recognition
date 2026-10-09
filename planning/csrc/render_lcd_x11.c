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
    /* 叠加层状态（由 render_lcd_overlay 记录，render_lcd_draw 上屏前应用） */
    int      ov_cmd;
    int      ov_tl;
    int      ov_alarm;
    uint32_t ov_seq;
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

/* 从 src_w 宽的 RGB565 源图里取 (sx,sy,w,h) 子块，画到 canvas 的 (dx,dy)。
 * 3×2 上屏要靠它：源图固定是每板 640×480（2×2 四宫格），只取 3 个有效子块。
 * （原「整幅 blit」函数在改为 3×2 后已无调用点，一并删除——静态函数未使用
 *   会在板端 -Werror 下直接编译失败。） */
static void blit_sub565(const uint8_t *src565, int src_w,
                        int sx, int sy, int w, int h,
                        uint8_t *canvas, int canvas_w, int dx, int dy)
{
    for (int y = 0; y < h; y++) {
        const uint8_t *src = src565 +
            ((size_t)(sy + y) * (size_t)src_w + (size_t)sx) * 2u;
        uint8_t *dst = canvas +
            ((size_t)(dy + y) * (size_t)canvas_w + (size_t)dx) * 4u;
        rgb565_line_to_xrgb(src, dst, w);
    }
}

/* ---------- 叠加层绘制原语（canvas 为 32bpp BGRX，color 用 0xRRGGBB） ---------- */
static void c_fill_rect(uint8_t *canvas, int cw, int ch,
                        int x, int y, int w, int h, uint32_t rgb)
{
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > cw) w = cw - x;
    if (y + h > ch) h = ch - y;
    if (w <= 0 || h <= 0) return;
    const uint8_t b = (uint8_t)(rgb & 0xFFu);
    const uint8_t g = (uint8_t)((rgb >> 8) & 0xFFu);
    const uint8_t r = (uint8_t)((rgb >> 16) & 0xFFu);
    for (int yy = 0; yy < h; yy++) {
        uint8_t *p = canvas + ((size_t)(y + yy) * cw + x) * 4u;
        for (int xx = 0; xx < w; xx++) { p[0] = b; p[1] = g; p[2] = r; p[3] = 0; p += 4; }
    }
}

/* 等腰三角形（逐行/逐列扫描填充）：apex 在 (cx,cy)，底边距 apex len，半宽 half。
 * dir: 0=尖朝上 1=尖朝下 2=尖朝左 3=尖朝右 */
static void c_fill_tri(uint8_t *canvas, int cw, int ch,
                       int cx, int cy, int half, int len, int dir, uint32_t rgb)
{
    if (len <= 0) return;
    for (int i = 0; i <= len; i++) {
        const int hw = half * i / len;
        if (dir == 0)      c_fill_rect(canvas, cw, ch, cx - hw, cy + i, hw * 2 + 1, 1, rgb);
        else if (dir == 1) c_fill_rect(canvas, cw, ch, cx - hw, cy - i, hw * 2 + 1, 1, rgb);
        else if (dir == 2) c_fill_rect(canvas, cw, ch, cx + i, cy - hw, 1, hw * 2 + 1, rgb);
        else               c_fill_rect(canvas, cw, ch, cx - i, cy - hw, 1, hw * 2 + 1, rgb);
    }
}

/* 把叠加层画到 canvas（上屏前调用）。含义见 render_lcd.h:render_lcd_overlay */
static void overlay_apply(render_lcd_ctx_t *ctx)
{
    uint8_t *cv = ctx->canvas;
    const int cw = ctx->win_w, ch = ctx->win_h;
    if (!cv) return;

    /* 1) 红绿灯指示灯：窗口右上角竖排三色，点亮者高亮、其余暗色。
     *    位置按画布宽算（3×2 布局下画布 960 宽），不写死 1280 的右半屏起点。 */
    {
        const int bx = cw - 60, by = 22, bw = 38, bh = 38, gap = 8;
        static const uint32_t on_rgb[3]  = { 0xFF2020u, 0xFFD400u, 0x20FF40u };
        static const uint32_t off_rgb[3] = { 0x400000u, 0x403300u, 0x00300Cu };
        for (int i = 0; i < 3; i++) {
            const int on = (ctx->ov_tl == (i + 1));   /* TL_RED=1 / YELLOW=2 / GREEN=3 */
            c_fill_rect(cv, cw, ch, bx, by + i * (bh + gap), bw, bh,
                        on ? on_rgb[i] : off_rgb[i]);
        }
    }

    /* 2) 行驶指示箭头：窗口右下角（黄=行驶方向，红方块=停车/刹车）。
     *    放在下排（S 板那一行）的右下，避开上排的 M 板画面。 */
    {
        const int ax = cw - 150, ay = ch - 130;
        const uint32_t YEL = 0xFFD400u, RED = 0xFF2020u;
        switch (ctx->ov_cmd) {
            case 3:   /* CMD_LEFT */
                c_fill_rect(cv, cw, ch, ax - 16, ay - 13, 56, 26, YEL);
                c_fill_tri (cv, cw, ch, ax - 56, ay, 36, 56, 2, YEL);
                break;
            case 4:   /* CMD_RIGHT */
                c_fill_rect(cv, cw, ch, ax - 40, ay - 13, 56, 26, YEL);
                c_fill_tri (cv, cw, ch, ax + 56, ay, 36, 56, 3, YEL);
                break;
            case 5:   /* CMD_BRAKE */
            case 6:   /* CMD_STOP */
                c_fill_rect(cv, cw, ch, ax - 42, ay - 42, 84, 84, RED);
                break;
            case 1:   /* CMD_GO：向上箭头 */
            default:
                c_fill_rect(cv, cw, ch, ax - 13, ay - 6, 26, 56, YEL);
                c_fill_tri (cv, cw, ch, ax, ay - 6, 36, 56, 0, YEL);
                break;
        }
    }

    /* 3) 报警：整屏红色边框闪烁（赛题三"通过输出图像叠加红色模拟报警"）。
     *    渲染是按帧触发的，用 frame 序号分频做亮灭，约 1s 一个周期。 */
    if (ctx->ov_alarm && ((ctx->ov_seq / 4u) % 2u) == 0u) {
        const int t = 12;
        c_fill_rect(cv, cw, ch, 0,      0,      cw, t,  0xFF0000u);
        c_fill_rect(cv, cw, ch, 0,      ch - t, cw, t,  0xFF0000u);
        c_fill_rect(cv, cw, ch, 0,      0,      t,  ch, 0xFF0000u);
        c_fill_rect(cv, cw, ch, cw - t, 0,      t,  ch, 0xFF0000u);
    }
}

int render_lcd_overlay(render_lcd_ctx_t *ctx, int cmd, int tl_state,
                       int alarm, uint32_t frame_id)
{
    if (!ctx) return -1;
    ctx->ov_cmd   = cmd;
    ctx->ov_tl    = tl_state;
    ctx->ov_alarm = alarm ? 1 : 0;
    ctx->ov_seq   = frame_id;
    return 0;
}

/* ---------- X11 真实现 ---------- */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>   /* XK_Left / XK_space 等（按键模拟用） */
#include <unistd.h>   /* getuid()：用于在报错里点明"是不是 root 跑的" */

/* Xlib 默认错误处理器**会直接 exit()**——任何一次异步 X 错误（例如窗口尚未
 * 可见时调 XSetInputFocus 触发的 BadMatch）都会让渲染进程莫名退出。改为忽略。 */
static int ignore_x_error(Display *d, XErrorEvent *e) { (void)d; (void)e; return 0; }

int render_lcd_init(render_lcd_ctx_t **ctx, int win_w, int win_h)
{
    if (!ctx) return -1;
    render_lcd_ctx_t *c = (render_lcd_ctx_t *)calloc(1, sizeof(*c));
    if (!c) return -1;
    c->win_w = win_w > 0 ? win_w : DISP_WIN_W;
    c->win_h = win_h > 0 ? win_h : DISP_WIN_H;
    c->is_stub = 0;

    Display *d = XOpenDisplay(NULL);
    if (!d) {
        /* "进程明明在跑、就是没有画面"的头号原因：X11 连不上。
         * 典型场景：sudo 启动 → root 拿不到桌面会话的 X 授权 cookie
         * （XAUTHORITY 落到 /root/.Xauthority，通常不存在）。
         * 这里把真实环境值打出来，避免"启动成功但黑屏"无从下手。 */
        const char *disp = getenv("DISPLAY");
        const char *xaut = getenv("XAUTHORITY");
        const char *user = getenv("USER");
        fprintf(stderr,
            "[LCD] XOpenDisplay 失败：连不上 X 服务器，画面无法上屏（收帧/决策不受影响）。\n"
            "      DISPLAY=%s  XAUTHORITY=%s  当前用户=%s(uid=%d)\n"
            "      若 uid=0 说明是用 sudo 启动的——root 默认没有桌面会话的 X 授权。\n"
            "      正确做法：网络/驱动用 root，图形进程用「桌面用户」身份起，例如\n"
            "        sudo -u <桌面用户> env DISPLAY=:0 XAUTHORITY=/home/<桌面用户>/.Xauthority \\\n"
            "             LD_LIBRARY_PATH=<仓库>/lib ./bin/planning_main --model model/yolov5s-640-640.rknn\n"
            "      （scripts/start_s.sh 已自动完成这一步，无需手敲）\n",
            disp ? disp : "(未设置)", xaut ? xaut : "(未设置)",
            user ? user : "?", (int)getuid());
        free(c);
        return -1;
    }
    XSetErrorHandler(ignore_x_error);   /* 见函数上方说明：避免异步 X 错误杀进程 */

    int scr = DefaultScreen(d);
    Window w = XCreateSimpleWindow(d, RootWindow(d, scr), 0, 0,
                                   (unsigned)c->win_w, (unsigned)c->win_h, 1,
                                   BlackPixel(d, scr), WhitePixel(d, scr));
    XStoreName(d, w, "ADAS-Display");
    /* KeyPressMask：赛题三"按键模拟"必须——不选它窗口收不到任何按键 */
    XSelectInput(d, w, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(d, w);
    XRaiseWindow(d, w);
    /* 让窗口拿到输入焦点，否则按键会送给桌面/其它窗口。
     * 若此时窗口尚不可见而失败（BadMatch），由上面的容错处理器吞掉。 */
    XSetInputFocus(d, w, RevertToParent, CurrentTime);
    XFlush(d);

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

    /* 3×2 六宫格合成：上排 = M 板（远端 UDP）3 路，下排 = S 板（本地）3 路。
     * 每路从源图 640×480 里取一个 320×240 子块，子块位置与 FPGA 写地址映射
     * （axi4_ctrl_3ch.v）严格对应：ch0=左上(0,0)、ch1=右上(1,0)、ch2=左下(0,1)。
     * 第 4 格是硬件预留槽，不取用、不上屏。 */
    static const int qx[DISP_COLS] = { 0, 1, 0 };
    static const int qy[DISP_COLS] = { 0, 0, 1 };

    if ((mode == DISPLAY_MODE_SPLIT || mode == DISPLAY_MODE_REMOTE_ONLY) && rem) {
        for (int k = 0; k < DISP_COLS; k++) {
            blit_sub565(rem, IMG_WIDTH, qx[k] * DISP_CELL_W, qy[k] * DISP_CELL_H,
                        DISP_CELL_W, DISP_CELL_H, ctx->canvas, ctx->win_w,
                        k * DISP_CELL_W, 0);
        }
    }
    if ((mode == DISPLAY_MODE_SPLIT || mode == DISPLAY_MODE_LOCAL_ONLY) && loc) {
        for (int k = 0; k < DISP_COLS; k++) {
            blit_sub565(loc, IMG_WIDTH, qx[k] * DISP_CELL_W, qy[k] * DISP_CELL_H,
                        DISP_CELL_W, DISP_CELL_H, ctx->canvas, ctx->win_w,
                        k * DISP_CELL_W, DISP_CELL_H);
        }
    }
    /* BLANK：保持黑屏 */

    overlay_apply(ctx);   /* 叠加行驶箭头 / 红绿灯指示 / 报警红框（见 render_lcd_overlay） */

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

/* 非阻塞取键：返回 RENDER_KEY_*（见 render_lcd.h），无键立即返回 0。
 * 一次调用只消费一个有效按键，避免长按键把队列塞满后界面卡顿。 */
int render_lcd_poll_key(render_lcd_ctx_t *ctx)
{
    if (!ctx || ctx->is_stub || !ctx->disp) return RENDER_KEY_NONE;
    Display *d = (Display *)ctx->disp;
    int mapped = RENDER_KEY_NONE;

    while (XPending(d)) {
        XEvent ev;
        XNextEvent(d, &ev);
        if (ev.type != KeyPress) continue;
        const KeySym ks = XLookupKeysym(&ev.xkey, 0);
        switch (ks) {
            case XK_Left:  mapped = RENDER_KEY_LANE_LEFT;   break;
            case XK_Right: mapped = RENDER_KEY_LANE_RIGHT;  break;
            case XK_Up:    mapped = RENDER_KEY_FORWARD;     break;
            case XK_space: mapped = RENDER_KEY_STOP;        break;
            case XK_c: case XK_C: mapped = RENDER_KEY_LANE_CHANGE; break;
            case XK_a: case XK_A: mapped = RENDER_KEY_AUTO; break;
            default: break;
        }
        if (mapped != RENDER_KEY_NONE) break;
    }
    return mapped;
}

#else  /* !__linux__：桩实现 */

int render_lcd_init(render_lcd_ctx_t **ctx, int win_w, int win_h)
{
    if (!ctx) return -1;
    render_lcd_ctx_t *c = (render_lcd_ctx_t *)calloc(1, sizeof(*c));
    if (!c) return -1;
    c->win_w = win_w > 0 ? win_w : DISP_WIN_W;
    c->win_h = win_h > 0 ? win_h : DISP_WIN_H;
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

int render_lcd_overlay(render_lcd_ctx_t *ctx, int cmd, int tl_state,
                       int alarm, uint32_t frame_id)
{
    if (!ctx) return -1;
    ctx->ov_cmd   = cmd;
    ctx->ov_tl    = tl_state;
    ctx->ov_alarm = alarm ? 1 : 0;
    ctx->ov_seq   = frame_id;
    return 0;   /* 桩：只记录，不上屏 */
}

int render_lcd_poll_key(render_lcd_ctx_t *ctx)
{
    (void)ctx;
    return RENDER_KEY_NONE;   /* 本机无 X11：无按键输入 */
}

void render_lcd_deinit(render_lcd_ctx_t *ctx)
{
    if (!ctx) return;
    free(ctx->canvas);
    free(ctx);
}
#endif /* __linux__ */
