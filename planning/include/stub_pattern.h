/*
 * stub_pattern.h — 合成"每板 3 路摄像头拼接图"（【人员 B】，联调/演示用）
 *
 * 目的：在没有接 OV5640、或 FPGA 码流还没烧的时候，也能把
 *   「每片 FPGA 把 3 路 640×480 降采样成 320×240 → 排进 2×2 四宫格 → 输出 640×480」
 * 这条拼接通路**端到端跑通并肉眼验证**：
 *
 *   ┌──────────┬──────────┐
 *   │ ch0 渐变A │ ch1 渐变B │   ← 3 路有效 + 右下 1 格"预留空槽"
 *   ├──────────┼──────────┤     （与 axi4_ctrl_3ch.v 的写地址映射一一对应：
 *   │ ch2 渐变C │ 预留棋盘格 │      cur_ch[1]→上/下半区，cur_ch[0]→左/右半区）
 *   └──────────┴──────────┘
 *
 * 两块板用**不同的渐变方向**，于是 S 端 1280×480 一屏上能同时看到
 * 左半屏（S 板自己的 3 路）与右半屏（M 板送来的 3 路），共 6 块互不相同的区域。
 * 每个有效格里还画了 1/2/3 个小方块作为序号标记，便于确认通道归属。
 *
 * 真实数据可用时不会走到这里：M 端优先读 shm_pcie_img，S 端优先读自己的采集。
 */
#ifndef STUB_PATTERN_H
#define STUB_PATTERN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 把一张 width×height 的 RGB565 图填成 2×2 拼接图案（原地覆盖）。
 *   board_id: 0 = M 端（渐变方向 A）；1 = S 端（渐变方向 B，与 A 明显不同）
 * width/height 需为偶数；函数按 width/2 × height/2 划分四个象限。 */
void stub_pattern_fill(uint8_t *rgb565, int width, int height, int board_id);

#ifdef __cplusplus
}
#endif

#endif /* STUB_PATTERN_H */
