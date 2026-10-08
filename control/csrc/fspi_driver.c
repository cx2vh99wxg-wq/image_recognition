/*
 * fspi_driver.c — FSPI 主机驱动实现（【人员 C · 控制与 FPGA】）
 *
 * 经 /dev/spidev4.0 以 QUAD SPI 与 FPGA 从机通信。协议/寄存器映射见
 * common/include/fpga_regmap.h，本文件不出现裸数字（SPI 参数、地址、
 * 编码全部取自 fpga_regmap.h）。
 *
 * 相对旧版 fspi_module.c 的改进：
 *   1. 删除 getopt/SpiTestConfig/TestResult/TOOL_VERSION 等测试工具死代码；
 *   2. 两段重复的「众数采样」收敛为单一 spi_majority_read()；
 *   3. 写/读路径拆成 spi_set_quad_tx/spi_set_quad_rx + spi_transfer，语义清晰；
 *   4. 所有日志走 log.h 宏（LOG_TAG=CONTROL），不再裸 printf。
 *
 * 行为保持不变：QUAD 4 线、SPI Mode3、100MHz、写帧 [0x00][addr][data]、
 * 读路径发送 [0x00][addr] 后收 1 字节（历史保留，FPGA 读命令 0x80 未被主机触发）、
 * 写重试 15 次 / 读众数采样 10 次、低有效编码（运动=0，停止=1）、
 * 急停写寄存器 4=0（mem4 下降沿触发 FPGA 停车脉冲）。
 */
#include "fspi_driver.h"
#include "fpga_regmap.h"

#define LOG_TAG "CONTROL"
#include "log.h"

#include <string.h>

#if defined(__linux__) && !defined(USE_STUB)
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

struct fspi_driver {
    int      fd;              /* /dev/spidev4.0 文件描述符，<0 表示未打开 */
    uint32_t spi_mode;        /* SPI_MODE_3 基础上叠加 SPI_TX_QUAD / SPI_RX_QUAD */
    uint8_t  bits_per_word;
    uint32_t speed_hz;
};

/* ---- 内部：SPI 方向切换（mode32 的 TX_QUAD/RX_QUAD 位） ---- */
static int spi_set_quad_tx(fspi_driver_t *d)
{
    d->spi_mode |=  (uint32_t)SPI_TX_QUAD;
    d->spi_mode &= ~(uint32_t)SPI_RX_QUAD;
    return ioctl(d->fd, SPI_IOC_WR_MODE32, &d->spi_mode);
}

static int spi_set_quad_rx(fspi_driver_t *d)
{
    d->spi_mode = (d->spi_mode & ~(uint32_t)SPI_TX_QUAD) | (uint32_t)SPI_RX_QUAD;
    return ioctl(d->fd, SPI_IOC_WR_MODE32, &d->spi_mode);
}

/* ---- 内部：一次 QUAD 传输 ---- */
static int spi_transfer(fspi_driver_t *d, const uint8_t *tx, uint8_t *rx, uint32_t len)
{
    struct spi_ioc_transfer tr;
    memset(&tr, 0, sizeof(tr));
    tr.tx_buf        = (unsigned long)tx;
    tr.rx_buf        = (unsigned long)rx;
    tr.len           = len;
    tr.delay_usecs   = FSPI_DELAY_US;
    tr.speed_hz      = d->speed_hz;
    tr.bits_per_word = d->bits_per_word;
    tr.tx_nbits      = 4;   /* QUAD */
    tr.rx_nbits      = 4;   /* QUAD */
    return ioctl(d->fd, SPI_IOC_MESSAGE(1), &tr);
}

/* ---- 内部：写 3 字节 [命令/地址高][地址低][数据] ---- */
static int spi_write_3b(fspi_driver_t *d, uint8_t addr, uint8_t data)
{
    uint8_t tx[FSPI_TRANSFER_SIZE];
    tx[0] = (uint8_t)FSPI_CMD_WRITE;   /* 0x00，兼作 16 位地址高字节 */
    tx[1] = addr;
    tx[2] = data;

    if (spi_set_quad_tx(d) != 0)
        return -1;
    if (spi_transfer(d, tx, NULL, FSPI_TRANSFER_SIZE) < 0)
        return -1;
    return 0;
}

/* ---- 内部：读一次（发送 [0x00][addr]，再收 1 字节；历史保留语义） ---- */
static int spi_read_addr_once(fspi_driver_t *d, uint8_t addr, uint8_t *out)
{
    uint8_t tx[2];
    uint8_t rx = 0;
    tx[0] = (uint8_t)FSPI_CMD_WRITE;   /* 历史如此：读路径首字节亦为 0x00 */
    tx[1] = addr;

    if (spi_set_quad_tx(d) != 0)
        return -1;
    if (spi_transfer(d, tx, NULL, 2) < 0)
        return -1;

    if (spi_set_quad_rx(d) != 0)
        return -1;
    if (spi_transfer(d, NULL, &rx, 1) < 0)
        return -1;

    *out = rx;
    return 0;
}

/* ---- 内部：可靠写（重试 FSPI_WRITE_RETRY 次，间隔 1ms） ---- */
static int spi_write_reliable(fspi_driver_t *d, uint8_t addr, uint8_t data)
{
    for (int i = 0; i < FSPI_WRITE_RETRY; i++) {
        if (spi_write_3b(d, addr, data) == 0)
            return 0;
        usleep(1000);
    }
    return -1;
}

/* ---- 内部：众数读（采样 FSPI_READ_RETRY 次，取出现最多者，间隔 0.5ms） ---- */
static int spi_majority_read(fspi_driver_t *d, uint8_t addr, uint8_t *out)
{
    uint8_t hist[256];
    int     ok = 0;
    memset(hist, 0, sizeof(hist));

    for (int i = 0; i < FSPI_READ_RETRY; i++) {
        uint8_t v = 0;
        if (spi_read_addr_once(d, addr, &v) == 0) {
            hist[v]++;
            ok++;
        }
        usleep(500);
    }

    if (ok == 0)
        return -1;

    uint8_t best = 0;
    int     best_n = 0;
    for (int i = 0; i < 256; i++) {
        if (hist[i] > best_n) {
            best_n = hist[i];
            best = (uint8_t)i;
        }
    }
    *out = best;
    return 0;
}

/* ---- 公开接口 ---- */
int fspi_driver_init(fspi_driver_t **ctx)
{
    if (!ctx)
        return -1;

    fspi_driver_t *d = (fspi_driver_t *)calloc(1, sizeof(*d));
    if (!d)
        return -1;

    d->fd            = -1;
    d->spi_mode      = (uint32_t)FSPI_SPI_MODE;   /* SPI_MODE_3 */
    d->bits_per_word = FSPI_BITS_PER_WORD;
    d->speed_hz      = FSPI_SPEED_HZ;

    d->fd = open(FSPI_DEVICE, O_RDWR);
    if (d->fd < 0) {
        LOGE("open %s 失败: %s\n", FSPI_DEVICE, strerror(errno));
        free(d);
        return -1;
    }

    if (ioctl(d->fd, SPI_IOC_WR_MODE32, &d->spi_mode) == -1) {
        LOGE("设置 SPI mode 失败: %s\n", strerror(errno));
        goto fail;
    }
    if (ioctl(d->fd, SPI_IOC_WR_BITS_PER_WORD, &d->bits_per_word) == -1) {
        LOGE("设置 bits_per_word 失败: %s\n", strerror(errno));
        goto fail;
    }
    if (ioctl(d->fd, SPI_IOC_WR_MAX_SPEED_HZ, &d->speed_hz) == -1) {
        LOGE("设置 speed 失败: %s\n", strerror(errno));
        goto fail;
    }

    LOGI("FSPI 初始化成功: %s @ %u Hz, mode=%u, quad\n",
         FSPI_DEVICE, (unsigned)d->speed_hz, (unsigned)d->spi_mode);
    *ctx = d;
    return 0;

fail:
    close(d->fd);
    free(d);
    return -1;
}

void fspi_driver_deinit(fspi_driver_t *ctx)
{
    if (!ctx)
        return;
    if (ctx->fd >= 0)
        close(ctx->fd);
    free(ctx);
    LOGI("FSPI 资源已释放\n");
}

int fspi_driver_write_reg(fspi_driver_t *ctx, uint8_t addr, uint8_t data)
{
    if (!ctx || ctx->fd < 0)
        return -1;
    return spi_write_reliable(ctx, addr, data);
}

int fspi_driver_read_reg(fspi_driver_t *ctx, uint8_t addr, uint8_t *data)
{
    if (!ctx || ctx->fd < 0 || !data)
        return -1;
    return spi_majority_read(ctx, addr, data);
}

int fspi_driver_move(fspi_driver_t *ctx, fspi_move_t move, uint8_t enable)
{
    if (!ctx || ctx->fd < 0)
        return -1;
    if ((int)move < FSPI_MOVE_FORWARD || (int)move > FSPI_MOVE_RIGHT)
        return -1;
    if (enable > 1)
        return -1;

    return fspi_driver_write_reg(ctx, (uint8_t)move, fspi_encode_enable(enable));
}

int fspi_driver_emergency_stop(fspi_driver_t *ctx)
{
    if (!ctx || ctx->fd < 0)
        return -1;
    /* 写 0：mem4_out[0] 由 1→0 下降沿触发 FPGA 1s 停车脉冲 */
    return fspi_driver_write_reg(ctx, (uint8_t)FSPI_REG_STOP,
                                 (uint8_t)FSPI_VAL_ACTIVE);
}

int fspi_driver_emergency_release(fspi_driver_t *ctx)
{
    if (!ctx || ctx->fd < 0)
        return -1;
    /* 写 1：复位到正常（重新武装，供下次下降沿触发） */
    return fspi_driver_write_reg(ctx, (uint8_t)FSPI_REG_STOP,
                                 (uint8_t)FSPI_VAL_IDLE);
}

#else /* !__linux__ 或 USE_STUB：桩实现，仅供语法/链接自检与无硬件联调 */

struct fspi_driver { int fd; };

int fspi_driver_init(fspi_driver_t **ctx)
{
    (void)ctx;
    LOGW("非 Linux 或 USE_STUB，FSPI 驱动为桩实现\n");
    return -1;
}

void fspi_driver_deinit(fspi_driver_t *ctx) { (void)ctx; }

int fspi_driver_write_reg(fspi_driver_t *ctx, uint8_t addr, uint8_t data)
{
    (void)ctx; (void)addr; (void)data;
    return -1;
}

int fspi_driver_read_reg(fspi_driver_t *ctx, uint8_t addr, uint8_t *data)
{
    (void)ctx; (void)addr; (void)data;
    return -1;
}

int fspi_driver_move(fspi_driver_t *ctx, fspi_move_t move, uint8_t enable)
{
    (void)ctx; (void)move; (void)enable;
    return -1;
}

int fspi_driver_emergency_stop(fspi_driver_t *ctx)
{
    (void)ctx;
    return -1;
}

int fspi_driver_emergency_release(fspi_driver_t *ctx)
{
    (void)ctx;
    return -1;
}

#endif /* __linux__ && !USE_STUB */
