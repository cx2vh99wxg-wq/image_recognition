/*
 * stub_pattern.h — 合成"每板 3 路摄像头拼接图"（【人员 B】，联调/演示用）
 *
 * 目的：在没有接 OV5640、或 FPGA 码流还没烧的时候，也能把
 *   「每片 FPGA 把 3 路 640×480 降采样成 320×240 → 排进 2×2 四宫格 → 输出 640×480」
 * 这条拼接通路**端到端跑通并肉眼验证**。单板 640×480 的内部布局（硬件契约）：
 *
 *   ┌──────────┬──────────┐
 *   │ ch0 渐变A │ ch1 渐变B │   ← 3 路有效（左上/右上/左下）
 *   ├──────────┼──────────┤     + 右下 1 格"预留空槽"（未接摄像头，填平色，不上屏）
 *   │ ch2 渐变C │  预留（平色）│
 *   └──────────┴──────────┘
 *
 * 最终上屏是 **3×2 六宫格**（由 render_lcd 合成）：
 *   上排 = M 板 3 路（经 UDP 送来），下排 = S 板 3 路（本板 PCIe 采集）。
 * 两板用**相反方向的渐变**，6 格互不相同；
 * 每格左上角的小方块数量 = 该格在本板内的通道序号（1/2/3）。
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
