#ifndef CAPTURE_SUPPORT_H
#define CAPTURE_SUPPORT_H

#include "pango_dma_abi.h"
#include <errno.h>
#include <stdint.h>

// Map the DMA buffer and reset the FPGA read address to the start of a frame.
static inline int capture_start_dma(int fd, DMA_OPERATION *operation)
{
    printf("DMA参数配置: current_len=%d dword (%d字节), offset_addr=0x%x\n",
           operation->current_len, operation->current_len * 4,
           operation->offset_addr);

    /* 映射地址（只需映射一次） */
    printf("正在映射DMA地址...\n");
    if (ioctl(fd, PCI_MAP_ADDR_CMD, operation) != 0)
    {
        printf("*** 致命错误：DMA地址映射失败 ***\n");
        printf("*** 错误原因：%s ***\n", strerror(errno));
        printf("*** 检查项目：***\n");
        printf("*** 1. PCIE驱动是否支持DMA操作 ***\n");
        printf("*** 2. DMA缓冲区是否可用 ***\n");
        printf("*** 3. 系统内存是否充足 ***\n");
        printf("*** 程序立即停止 ***\n");
        return -1;
    }
    printf("DMA地址映射完成\n");

    /* 图像处理：重置DMA读取地址到帧起始位置 */
    printf("正在重置DMA读取地址...\n");
    operation->offset_addr = 0; // 保证从地址0开始
    if (ioctl(fd, PCI_DMA_WRITE_CMD, operation) != 0)
    {
        printf("*** 致命错误：DMA读取地址重置失败 ***\n");
        printf("*** 错误原因：%s ***\n", strerror(errno));
        printf("*** 检查项目：***\n");
        printf("*** 1. DMA控制器是否响应 ***\n");
        printf("*** 2. PCIE连接是否稳定 ***\n");
        printf("*** 程序立即停止 ***\n");
        return -1;
    }
    printf("DMA读取地址重置完成\n");
    return 0;
}

static inline int capture_save_ppm(const uint8_t *rgb888_data, const char *filename,
                                   int width, int height)
{
    if (!rgb888_data || !filename)
        return -1;

    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        printf("无法创建文件: %s\n", filename);
        return -1;
    }

    fprintf(fp, "P6\n%d %d\n255\n", width, height);
    fwrite(rgb888_data, 1, width * height * 3, fp);
    fclose(fp);

    printf("帧已保存到: %s\n", filename);
    return 0;
}

static inline void capture_report_stop(const char *error_msg, const char *component)
{
    printf("\n");
    printf("█████████████████████████████████████████████████████████\n");
    printf("██                 紧急停止                            ██\n");
    printf("█████████████████████████████████████████████████████████\n");
    printf("** 组件: %s\n", component ? component : "未知");
    printf("** 错误: %s\n", error_msg ? error_msg : "未指定错误");
    printf("** 时间: ");

    // 在日志中输出当前时间。
    time_t now;
    time(&now);
    printf("%s", ctime(&now));

    printf("** 操作: 立即停止所有正在运行的程序\n");
    printf("█████████████████████████████████████████████████████████\n");
    printf("\n");

    // 立即刷新输出缓冲区
    fflush(stdout);
    fflush(stderr);
}

#endif
