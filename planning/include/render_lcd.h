/*
 * render_lcd.h — LCD 屏（Linux/X11/HDMI）3×2 六宫格渲染（【人员 B】）
 *
 * 硬件：S 端板卡的 HDMI/屏接口，运行 X11 桌面环境。
 * 注意与 C 负责的"串口屏（FPGA/UART/陶晶驰 HMI）"区分——本模块是 Linux 显示。
 *
 * 画面：960×480 六宫格（常量见 driving_config.h 的「上屏布局」段）——
 *      ┌────────┬────────┬────────┐
 *      │ M ch0  │ M ch1  │ M ch2  │   上排 = M 板 3 路（远端 UDP 图）
 *      ├────────┼────────┼────────┤
 *      │ S ch0  │ S ch1  │ S ch2  │   下排 = S 板 3 路（本地 PCIe 图）
 *      └────────┴────────┴────────┘
 * 每路是源图 640×480 里的一个 320×240 子块（单板 3 路有效 + 1 格硬件预留，
 * 预留格不上屏；子块位置与 axi4_ctrl_3ch.v 的写地址映射一致）。
 * 合成模式由调用者传入（当前主循环固定 SPLIT，尚未接入 HMI 视角切换）。
 */
#ifndef RENDER_LCD_H
#define RENDER_LCD_H

#include <stdint.h>
#include "driving_config.h"   /* DisplayMode / IMG_* / DISP_* */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct render_lcd_ctx render_lcd_ctx_t;

/* 创建窗口并初始化。win_w/win_h 为输出画布尺寸（默认 DISP_WIN_W×DISP_WIN_H）。
 * 非 Linux 环境为桩（仅打印帧号）。成功 0，失败 -1。 */
int  render_lcd_init(render_lcd_ctx_t **ctx, int win_w, int win_h);

/* 六宫格绘制：local565 / remote565 均为 640x480 RGB565；任一可为 NULL（缺路）。
 * mode 决定合成方式（SPLIT=上下两排都画 / LOCAL_ONLY=只下排 / REMOTE_ONLY=只上排 / BLANK）。
 * 若此前调用过 render_lcd_overlay()，本函数会在上屏前把叠加层一并画上。 */
int  render_lcd_draw(render_lcd_ctx_t *ctx,
                     const void *local565, const void *remote565,
                     DisplayMode mode, uint32_t frame_id);

/* 设置叠加层状态（在 render_lcd_draw 之前调用即可生效；只记录、不立即绘制）。
 * 对应赛题三测评要求的"通过输出图像叠加的方式指示车辆行驶行为"与"叠加红色报警"：
 *   cmd      : ControlCommand 取值（1=前进 3=左转 4=右转 5=刹车 6=急停 0=无）
 *              → 画布右下角叠加对应指向的箭头 / 红方块
 *   tl_state : TrafficLightState（0未知 1红 2黄 3绿）→ 画布右上角三色指示灯
 *   alarm    : 非 0 时整屏红色边框闪烁（模拟报警）
 *   frame_id : 闪烁相位来源（渲染按帧触发，约 1s 一个亮灭周期）
 * 非 Linux 桩实现只记录、不绘制。成功 0。 */
int  render_lcd_overlay(render_lcd_ctx_t *ctx, int cmd, int tl_state,
                        int alarm, uint32_t frame_id);

/* ---- 按键输入（赛题三"通过按键模拟…"的落地通道）----
 * 在显示窗口上监听键盘，非阻塞取键；返回下列 RENDER_KEY_* 之一，无键返回 0。
 * 映射：←/→ 模拟选择左/右车道；↑ 模拟行驶；空格 急停；
 *       c 强制变道（实线时触发报警）；a 交还自动决策。
 * 桩实现恒返回 0。注意：窗口需获得输入焦点（init 已 XSetInputFocus 并安装
 * 容错错误处理器，失败不会终止进程）。 */
#define RENDER_KEY_NONE         0
#define RENDER_KEY_LANE_LEFT    1   /* 模拟：切到左车道 / 左转向 */
#define RENDER_KEY_LANE_RIGHT   2   /* 模拟：切到右车道 / 右转向 */
#define RENDER_KEY_FORWARD      3   /* 模拟：行驶 */
#define RENDER_KEY_STOP         4   /* 模拟：停止（急停） */
#define RENDER_KEY_LANE_CHANGE  5   /* 模拟：强制变道（用于实线变道报警演示） */
#define RENDER_KEY_AUTO         6   /* 交还自动决策 */

int  render_lcd_poll_key(render_lcd_ctx_t *ctx);

void render_lcd_deinit(render_lcd_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RENDER_LCD_H */
