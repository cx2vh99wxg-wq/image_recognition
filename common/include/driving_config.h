/*
 * driving_config.h — 全局配置（B 唯一维护，A/C 只读）
 *
 * 本重写项目于第 1 周由三人共同冻结。所有魔法数字（尺寸、端口、key、
 * 阈值）集中在此，代码内禁止出现裸数字。A 仅在本文件参与冻结，不擅自修改。
 */
#ifndef DRIVING_CONFIG_H
#define DRIVING_CONFIG_H

/* 图像规格 */
#define IMG_WIDTH   640
#define IMG_HEIGHT  480
#define IMG_CH      3
#define IMG_BPP_565 2                 /* RGB565 每像素字节数 */

/* PCIe 采集 */
#define PCIE_DEVICE        "/dev/pango_pci_driver"
#define PCIE_LEAD_PIXELS   120        /* 每行前导像素，必须剥离 */
#define PCIE_LINE_PIXELS   (IMG_WIDTH + PCIE_LEAD_PIXELS)   /* 原始行宽（含前导） */
#define PCIE_FRAME_BYTES   ((size_t)PCIE_LINE_PIXELS * IMG_HEIGHT * IMG_BPP_565)
#define IMG_FRAME_BYTES    ((size_t)IMG_WIDTH * IMG_HEIGHT * IMG_BPP_565)

/* 共享内存 key（沿用原编号，语义重定义） */
#define SHM_KEY_PCIE_IMG  0x12345679  /* A 写：640x480 RGB565 */
#define SHM_KEY_UDP_IMG   0x1234567A  /* B 写：640x480 RGB565 */
#define SHM_KEY_DISPLAY   0x1234567B  /* B 写：DisplayMode */
#define SHM_KEY_CMD       0x1234567C  /* B 写 / C 读：ControlCommandMsg */
#define SHM_KEY_PERSON    0x1234567D  /* B 写 / B 读：行人状态 */
#define SHM_KEY_LANE      0x1234567E  /* A 写 / B 读：LaneResult */

/* 10-05 新增感知结果（A 写 / B 读）——key 取自保留段，需与 B 复核确认 */
#define SHM_KEY_TRAFFIC_LIGHT 0x1234567F  /* A 写 / B 读：TrafficLightResult（红绿灯灯色） */
#define SHM_KEY_ZEBRA         0x12345680  /* A 写 / B 读：ZebraResult（斑马线） */
#define SHM_KEY_LANE_MARK     0x12345681  /* A 写 / B 读：LaneMarkResult（虚实线/压线/变道） */

/* 共享内存尺寸（取自结构体，保证契约一致，禁止手填数字） */
#include "driving_types.h"
#define SHM_LANE_SIZE     sizeof(LaneResult)
#define SHM_CMD_SIZE      sizeof(ControlCommandMsg)
#define SHM_IMG_SIZE      IMG_FRAME_BYTES
#define SHM_TL_SIZE       sizeof(TrafficLightResult)
#define SHM_ZEBRA_SIZE    sizeof(ZebraResult)
#define SHM_LANE_MARK_SIZE sizeof(LaneMarkResult)

/* 车道线判定阈值（A 算法用，集中管理） */
#define LANE_ROI_TOP_FRAC  0.45f   /* ROI 自顶向下的起始比例 */
#define LANE_STRAIGHT_TH   24      /* 偏移绝对值小于该值判定为直道（像素） */
#define LANE_OFFSET_MAX    200     /* curve_offset 饱和上限（像素） */
#define LANE_PIXEL_REF     4000    /* 置信度覆盖率的参考像素数 */
#define LANE_STD_MAX       90      /* 车道线横向标准差上限（像素） */

/* 交叉编译前缀（板端 aarch64） */
#ifndef CROSS_PREFIX
#define CROSS_PREFIX "aarch64-linux-gnu-"
#endif

#endif /* DRIVING_CONFIG_H */
