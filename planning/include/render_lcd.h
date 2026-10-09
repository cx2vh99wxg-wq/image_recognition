/*
 * render_lcd.h — LCD 屏（Linux/X11/HDMI）双路拼接渲染（【人员 B】）
 *
 * 硬件：S 端 RK3568 的 HDMI/屏接口，运行 X11 桌面环境。
 * 注意与 C 负责的"串口屏（FPGA/UART/陶晶驰 HMI）"区分——本模块是 Linux 显示。
 *
 * 画面：1280x480 双路拼接（左=本地 PCIe 图含行人框，右=远端 UDP 车道图），
 * 拼接模式由 shm_display（DisplayMode）控制。
 */
#ifndef RENDER_LCD_H
#define RENDER_LCD_H

#include <stdint.h>
#include "driving_config.h"   /* DisplayMode / IMG_* */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct render_lcd_ctx render_lcd_ctx_t;

/* 创建窗口并初始化。win_w/win_h 为输出画布尺寸（默认 1280x480 拼接）。
 * 非 Linux 环境为桩（仅打印帧号）。成功 0，失败 -1。 */
int  render_lcd_init(render_lcd_ctx_t **ctx, int win_w, int win_h);

/* 双路绘制：local565 / remote565 均为 640x480 RGB565；任一可为 NULL（缺路）。
 * mode 决定拼接方式（SPLIT / LOCAL_ONLY / REMOTE_ONLY / BLANK）。
 * 若此前调用过 render_lcd_overlay()，本函数会在上屏前把叠加层一并画上。 */
int  render_lcd_draw(render_lcd_ctx_t *ctx,
                     const void *local565, const void *remote565,
                     DisplayMode mode, uint32_t frame_id);

/* 设置叠加层状态（在 render_lcd_draw 之前调用即可生效；只记录、不立即绘制）。
 * 对应赛题三测评要求的"通过输出图像叠加的方式指示车辆行驶行为"与"叠加红色报警"：
 *   cmd      : ControlCommand 取值（1=前进 3=左转 4=右转 5=刹车 6=急停 0=无）
 *              → 右半屏叠加对应指向的箭头 / 红方块
 *   tl_state : TrafficLightState（0未知 1红 2黄 3绿）→ 右上角三色指示灯
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
