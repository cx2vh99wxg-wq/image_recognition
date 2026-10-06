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
 * mode 决定拼接方式（SPLIT / LOCAL_ONLY / REMOTE_ONLY / BLANK）。 */
int  render_lcd_draw(render_lcd_ctx_t *ctx,
                     const void *local565, const void *remote565,
                     DisplayMode mode, uint32_t frame_id);

void render_lcd_deinit(render_lcd_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* RENDER_LCD_H */
