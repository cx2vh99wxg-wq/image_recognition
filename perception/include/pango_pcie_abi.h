/*
 * pango_pcie_abi.h — 紫光同创 PCIe 驱动 ABI（硬件契约，字节级不可改）
 *
 * 该头文件是对 FPGA 端 PCIe 驱动 ioctl 接口的“最小必要镜像”，仅抽取
 * 感知采集用到的命令与结构体。命令号、DMA 结构体字段布局必须与驱动
 * 一侧逐字节一致，否则硬件失联——因此本文件刻意不做任何语义重排，
 * 只重命名以贴合本工程命名风格。
 */
#ifndef PANGO_PCIE_ABI_H
#define PANGO_PCIE_ABI_H

#include <stdint.h>

#if defined(__linux__)
#include <sys/ioctl.h>   /* 板端 Linux：使用内核真实 ioctl 宏 */
#else
/*
 * 本机（Windows / 非 Linux）没有 <sys/ioctl.h>，但 ABI 命令号是“数据契约”，
 * 必须和板端 Linux 上的真实值逐位一致，否则硬件失联。下面用与 Linux
 * <asm-generic/ioctl.h> 完全相同的算法给出占位宏，仅用于让头文件在本机
 * 通过编译；这些命令号在本机不会被真正下发（硬件函数已被桩禁用）。
 */
#ifndef _IOC
#define _IOC_DIRSHIFT   30
#define _IOC_TYPESHIFT  8
#define _IOC_NRSHIFT    0
#define _IOC_SIZESHIFT  16
#define _IOC_NONE       0U
#define _IOC_WRITE      1U
#define _IOC_READ       2U
#define _IOC(dir, type, nr, size)                                   \
    (((dir)  << _IOC_DIRSHIFT)  | ((type) << _IOC_TYPESHIFT) |       \
     ((nr)   << _IOC_NRSHIFT)   | ((size) << _IOC_SIZESHIFT))
#define _IOWR(type, nr, size) \
    _IOC(_IOC_READ | _IOC_WRITE, (type), (nr), (size))
#endif
#endif

/* 驱动设备节点（由内核模块 pango_pci_driver 创建） */
#define PANGO_PCIE_DEV "/dev/pango_pci_driver"

/* ioctl 命令类型码与原始驱动保持一致（'S' 类型） */
#define PANGO_TYPE          'S'
#define PANGO_IO_RD_DATA    _IOWR(PANGO_TYPE, 0, int)  /* 读配置/设备信息 */
#define PANGO_IO_WR_DATA    _IOWR(PANGO_TYPE, 1, int)  /* 写配置 */
#define PANGO_IO_MAP        _IOWR(PANGO_TYPE, 2, int)  /* 映射 DMA 地址 */
#define PANGO_IO_WR_KRNL    _IOWR(PANGO_TYPE, 3, int)  /* 写内核缓冲 */
#define PANGO_IO_DMA_RD     _IOWR(PANGO_TYPE, 4, int)  /* 发起 DMA 读 */
#define PANGO_IO_DMA_WR     _IOWR(PANGO_TYPE, 5, int)  /* 设置 DMA 读地址 */
#define PANGO_IO_RD_KRNL    _IOWR(PANGO_TYPE, 6, int)  /* 从内核缓冲取数 */
#define PANGO_IO_UNMAP      _IOWR(PANGO_TYPE, 7, int)  /* 取消 DMA 映射 */

#define PANGO_DMA_PACKET   4096   /* 单次 DMA 包上限（字节） */

/*
 * DMA 传输描述符：字段顺序/类型必须与驱动定义完全一致。
 *   current_len : 每次读写的 dword 数（= 每行字节数 / 4）
 *   offset_addr : 帧内行偏移（字节地址，由驱动换算）
 *   cmd         : 预留命令字
 *   data        : 收发缓冲（读写各一页）
 */
typedef struct {
    uint32_t current_len;
    uint32_t offset_addr;
    uint32_t cmd;
    uint8_t  read_buf[PANGO_DMA_PACKET];
    uint8_t  write_buf[PANGO_DMA_PACKET];
} pango_dma_xfer;

#endif /* PANGO_PCIE_ABI_H */
