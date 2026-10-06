/* ===========================================================================
 * 文件：pcie_capture.h
 * 归属：人员 A（感知与预处理）· M 端（采集端）
 * 说明：PCIe 图像采集 + RGB565/RGB888 互转 + 前导像素剥离 的对外接口。
 *       显示（X11）、共享内存基础设施、UDP 均不属于 A，由 B 实现；本头只声明
 *       A 自己的采集/预处理能力，以及把感知结果交给 B 的“唯一出口”。
 * 接口约定：A 对外只产生 LaneResult（见 common/driving_types.h），
 *       由 main 调用 B 提供的 shm_write_lane() 写入 key=0x1234567E。
 * ======================================================================== */
#ifndef PCIE_CAPTURE_H_A
#define PCIE_CAPTURE_H_A

#include <stdint.h>
#include <stddef.h>
#include <sys/time.h>

/* 图像规格：640x480，每行前端附带 120 个前导像素，必须剥离 */
#define CAP_IMG_W        640
#define CAP_IMG_H        480
#define CAP_LEAD_PIX     120          /* 每行前导像素个数（文档 3.②）*/
#define CAP_LEAD_BYTES   (CAP_LEAD_PIX * 2)
#define CAP_ROW_RAW_BYTES ((CAP_LEAD_PIX + CAP_IMG_W) * 2)
#define CAP_FRAME_RAW    (CAP_IMG_H * CAP_ROW_RAW_BYTES)
#define CAP_FRAME_RGB888 (CAP_IMG_W * CAP_IMG_H * 3)

/* 列重排（仅在特定硬件接线下启用，默认关闭）*/
#define CAP_REARRANGE_FROM 0
#define CAP_REARRANGE_TO   0
#define CAP_REARRANGE_UP   0

/* A 的采集上下文：设备句柄 + DMA 参数。只保存 A 需要的东西。 */
typedef struct {
    int            dev_fd;            /* /dev/pango_pci_driver 句柄 */
    void          *cmd_op;            /* COMMAND_OPERATION（见 pcie_dma_read_test.h）*/
    void          *dma_op;            /* DMA_OPERATION */
    int            stream_id;         /* 当前视频流编号 */
} pcie_ctx_t;

/* ---- 采集生命周期 ---- */
int  pcie_capture_open(pcie_ctx_t *ctx);
int  pcie_capture_start_stream(pcie_ctx_t *ctx);
int  pcie_capture_grab(pcie_ctx_t *ctx,
                       uint8_t *out_rgb565, size_t out_bytes);
void pcie_capture_stop_stream(pcie_ctx_t *ctx);
void pcie_capture_close(pcie_ctx_t *ctx);

/* ---- 格式与工具（A 的预处理能力）---- */
void rgb565_to_rgb888(const uint16_t *src565, uint8_t *dst888, size_t pixels);
void rgb888_to_rgb565(const uint8_t *src888, uint16_t *dst565, size_t pixels);
int  save_rgb888_to_ppm(const uint8_t *rgb888, const char *path);
double timeval_diff_ms(struct timeval a, struct timeval b);

/* 把一行原始数据里的“前导像素”剥掉，只保留有效 640 宽。
 * raw_row 指向该行 CAP_ROW_RAW_BYTES 字节，dst_row 接收 CAP_IMG_W*2 字节。 */
void strip_leading_pixels(const uint8_t *raw_row, uint8_t *dst_row);

/* 列重排（默认空操作；仅当 CAP_REARRANGE_FROM<=TO 且非 0 区间时生效）。*/
void rearrange_row_columns(uint16_t *row, int width, int from, int to);

#endif /* PCIE_CAPTURE_H_A */
