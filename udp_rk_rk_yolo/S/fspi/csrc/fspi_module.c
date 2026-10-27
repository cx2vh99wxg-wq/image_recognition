#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include "fspi_module.h"
#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

#define SPI_DEVICE "/dev/spidev4.0"
#define SPI_SPEED 24000000
#define SPI_MODE_FLAGS (SPI_CPOL | SPI_CPHA)
#define SPI_TRANSFER_SIZE 3
#define SPI_CYCLE_COUNT 1
#define SPI_BITS_PER_WORD 8
#define SPI_DELAY_US 0
#define SPI_PROTOCOL_MODE SPI_MODE_QUAD

#define TOOL_VERSION "2.0"
#define DEFAULT_DEVICE "/dev/spidev4.0"
#define MAX_DEVICE_PATH 64
#define MAX_BUFFER_SIZE (1024 * 1024)

#define WRITE_RETRY_COUNT 15
#define READ_RETRY_COUNT 10

typedef enum
{
    SPI_MODE_SINGLE = 1,
    SPI_MODE_DUAL,
    SPI_MODE_QUAD
} SpiTransferMode;

typedef struct
{
    char device[MAX_DEVICE_PATH];
    uint32_t speed_hz;
    uint32_t transfer_size;
    uint32_t cycle_count;
    uint32_t spi_mode;
    uint8_t bits_per_word;
    uint16_t delay_us;
    SpiTransferMode protocol_mode;
} SpiTestConfig;

typedef struct
{
    double tx_rate_mbs;
    double rx_rate_mbs;
    float error_percent;
    uint64_t tx_bytes;
    uint64_t rx_bytes;
    struct timeval time_cost;
} TestResult;

static SpiTestConfig g_config;
static int g_spi_fd = -1;
static int g_auto_brake_errno;
static int g_auto_brake_transfer_result = -1;
static uint8_t g_auto_brake_raw_status = 0xff;

static int configure_spi_interface(int fd, SpiTestConfig *config);
static int perform_transfer(int fd, uint8_t *tx_buf, uint8_t *rx_buf, SpiTestConfig *config);
static int spi_write_register(int fd, uint16_t addr, uint8_t data);
static int spi_read_addr_data(int fd, uint16_t addr, uint8_t *data);
static int spi_read_direct(int fd, uint8_t *data);
static int spi_write_reliable(int fd, uint16_t addr, uint8_t data);
static int spi_read_reliable(int fd, uint16_t addr, uint8_t *data);
static int spi_read_direct_reliable(int fd, uint8_t *data);

/* 依次写入 SPI 模式、每字位数及最大时钟；任一 ioctl 失败立即返回。 */
static int configure_spi_interface(int fd, SpiTestConfig *config)
{

    if (ioctl(fd, SPI_IOC_WR_MODE32, &config->spi_mode) == -1)
    {
        perror("Failed to set SPI mode");
        return -1;
    }

    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &config->bits_per_word) == -1)
    {
        perror("Failed to set bits per word");
        return -1;
    }

    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &config->speed_hz) == -1)
    {
        perror("Failed to set SPI speed");
        return -1;
    }

    return 0;
}

/* 按当前配置建立单次 SPI 事务，分别设置单线、双线或四线传输宽度。 */
static int perform_transfer(int fd, uint8_t *tx_buf, uint8_t *rx_buf, SpiTestConfig *config)
{
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = config->transfer_size,
        .delay_usecs = config->delay_us,
        .speed_hz = config->speed_hz,
        .bits_per_word = config->bits_per_word,
    };

    switch (config->protocol_mode)
    {
    case SPI_MODE_DUAL:
        tr.tx_nbits = tx_buf ? 2 : 0;
        tr.rx_nbits = rx_buf ? 2 : 0;
        break;
    case SPI_MODE_QUAD:
        tr.tx_nbits = tx_buf ? 4 : 0;
        tr.rx_nbits = rx_buf ? 4 : 0;
        break;
    default:
        tr.tx_nbits = tx_buf ? 1 : 0;
        tr.rx_nbits = rx_buf ? 1 : 0;
    }

    return ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
}

/* 本板SMDT控制器会在写事务前自动发送00命令。
 * 用户缓冲区只放地址高、地址低、数据，三字节对应线上四字节。
 * 重复添加00会让FPGA将地址低字节写入reg0，而非目标寄存器。 */
static int spi_write_register(int fd, uint16_t addr, uint8_t data)
{
    uint8_t tx_buffer[SPI_TRANSFER_SIZE];

    tx_buffer[0] = (uint8_t)(addr >> 8);
    tx_buffer[1] = (uint8_t)addr;
    tx_buffer[2] = data;

    g_config.spi_mode |= SPI_TX_QUAD;
    g_config.spi_mode &= ~SPI_RX_QUAD;

    if (configure_spi_interface(fd, &g_config) != 0)
    {
        return -1;
    }

    int transferred = perform_transfer(fd, tx_buffer, NULL, &g_config);
    if (transferred != SPI_TRANSFER_SIZE)
    {
        if (transferred >= 0) errno = EIO;
        perror("Write register failed");
        return -1;
    }

    return 0;
}

/* 地址0用于HMI选择码。SMDT在接收事务前自动发送80，直接接收即可。
 * 其他地址保留原有接口；该旧接口不代表真实方向寄存器读回。 */
static int spi_read_addr_data(int fd, uint16_t addr, uint8_t *data)
{
    if (!data) {
        errno = EINVAL;
        return -1;
    }

    if (addr == 0) return spi_read_direct(fd, data);

    {
        uint8_t tx_buffer[2] = {(uint8_t)(addr >> 8), (uint8_t)addr};
        uint8_t rx_data = 0;
        g_config.spi_mode |= SPI_TX_QUAD;
        g_config.spi_mode &= ~SPI_RX_QUAD;
        if (configure_spi_interface(fd, &g_config) != 0) return -1;

        uint32_t original_size = g_config.transfer_size;
        g_config.transfer_size = 2;
        if (perform_transfer(fd, tx_buffer, NULL, &g_config) < 0) {
            perror("Send address failed");
            g_config.transfer_size = original_size;
            return -1;
        }

        g_config.spi_mode = (g_config.spi_mode & ~SPI_TX_QUAD) | SPI_RX_QUAD;
        g_config.transfer_size = 1;
        if (configure_spi_interface(fd, &g_config) != 0) {
            g_config.transfer_size = original_size;
            return -1;
        }
        if (perform_transfer(fd, NULL, &rx_data, &g_config) < 0) {
            perror("Read data failed");
            g_config.transfer_size = original_size;
            return -1;
        }
        g_config.transfer_size = original_size;
        *data = rx_data;
        return 0;
    }

}

/* 无需地址阶段，直接切换到四线接收并读取一字节。 */
static int spi_read_direct(int fd, uint8_t *data)
{
    uint8_t rx_data = 0;

    g_config.spi_mode = (g_config.spi_mode & ~SPI_TX_QUAD) | SPI_RX_QUAD;

    if (configure_spi_interface(fd, &g_config) != 0)
    {
        return -1;
    }

    uint32_t original_size = g_config.transfer_size;
    g_config.transfer_size = 1;

    int transferred = perform_transfer(fd, NULL, &rx_data, &g_config);
    if (transferred != 1)
    {
        if (transferred >= 0) errno = EIO;
        perror("Direct read failed");
        g_config.transfer_size = original_size;
        return -1;
    }

    g_config.transfer_size = original_size;

    *data = rx_data;
    return 0;
}

/* 最多尝试十五次写入，只有需要继续重试时才等待一毫秒。 */
static int spi_write_reliable(int fd, uint16_t addr, uint8_t data)
{
    int retry_count = 0;
    int ret;

    for (retry_count = 0; retry_count < WRITE_RETRY_COUNT; retry_count++)
    {
        ret = spi_write_register(fd, addr, data);

        if (ret == 0)
        {
            return 0;
        }

        if (retry_count < WRITE_RETRY_COUNT - 1)
        {
            usleep(1000);
        }
    }

    return -1;
}

/* 逐项扫描成功读值的计数；并列时沿用原有的最小字节值优先规则。 */
static uint8_t select_most_frequent_byte(const int counts[256])
{
    uint8_t selected = 0;
    int highest_count = 0;
    for (int value = 0; value < 256; ++value)
    {
        if (counts[value] > highest_count)
        {
            highest_count = counts[value];
            selected = (uint8_t)value;
        }
    }
    return selected;
}

/* 采样指定寄存器十次，每次后间隔半毫秒，再选取成功读值中的众数。 */
static int spi_read_reliable(int fd, uint16_t addr, uint8_t *data)
{
    int retry_count = 0;
    int ret;
    uint8_t read_data = 0;

    int data_count[256] = {0};
    int successful_reads = 0;

    for (retry_count = 0; retry_count < READ_RETRY_COUNT; retry_count++)
    {
        ret = spi_read_addr_data(fd, addr, &read_data);

        if (ret == 0)
        {
            data_count[read_data]++;
            successful_reads++;
        }

        usleep(500);
    }

    if (successful_reads > 0)
    {

        uint8_t most_frequent_data = select_most_frequent_byte(data_count);

        *data = most_frequent_data;
        return 0;
    }
    else
    {
        return -1;
    }
}

/* 无地址采样十次；失败样本不参与计数，并列众数选择较小值。 */
static int spi_read_direct_reliable(int fd, uint8_t *data)
{
    int retry_count = 0;
    int ret;
    uint8_t read_data = 0;

    int data_count[256] = {0};
    int successful_reads = 0;

    for (retry_count = 0; retry_count < READ_RETRY_COUNT; retry_count++)
    {
        ret = spi_read_direct(fd, &read_data);

        if (ret == 0)
        {
            data_count[read_data]++;
            successful_reads++;
        }

        usleep(500);
    }

    if (successful_reads > 0)
    {

        uint8_t most_frequent_data = select_most_frequent_byte(data_count);

        *data = most_frequent_data;
        return 0;
    }
    else
    {
        return -1;
    }
}

/* 装载固定配置，打开 SPI 设备并设置总线参数；配置失败时关闭句柄。 */
int fspi_init(void)
{

    g_config.speed_hz = SPI_SPEED;
    g_config.transfer_size = SPI_TRANSFER_SIZE;
    g_config.cycle_count = SPI_CYCLE_COUNT;
    g_config.spi_mode = SPI_MODE_FLAGS;
    g_config.bits_per_word = SPI_BITS_PER_WORD;
    g_config.delay_us = SPI_DELAY_US;
    g_config.protocol_mode = SPI_PROTOCOL_MODE;
    strcpy(g_config.device, SPI_DEVICE);

    g_spi_fd = open(g_config.device, O_RDWR);
    if (g_spi_fd < 0)
    {
        perror("FSPI Device open failed");
        return -1;
    }

    if (configure_spi_interface(g_spi_fd, &g_config) != 0)
    {
        close(g_spi_fd);
        g_spi_fd = -1;
        return -1;
    }

    printf("FSPI Configuration:\n");
    printf("  Device: %s\n", g_config.device);
    printf("  Speed: %u Hz (%u KHz)\n", g_config.speed_hz, g_config.speed_hz / 1000);
    printf("FSPI initialized successfully\n");
    printf("  SMDT transport: host write=ADDR_H ADDR_L DATA (3 bytes); controller supplies 00/80 opcodes\n");
    return 0;
}

/* 公开的寄存器写入入口，未初始化时直接失败。 */
int fspi_write_data(uint16_t addr, uint8_t data)
{
    if (g_spi_fd < 0) {
        return -1;
    }

    return spi_write_reliable(g_spi_fd, addr, data);
}

/* 公开的按地址可靠读取入口；读值由重复采样决定。 */
int fspi_read_data(uint16_t addr, uint8_t *data)
{
    if (g_spi_fd < 0) {
        return -1;
    }

    return spi_read_reliable(g_spi_fd, addr, data);
}

/* Nonblocking-in-the-policy sense: three immediate samples, no 10x500us
 * retry sleep. The existing display/control read API is unchanged. */
int fspi_read_hmi_snapshot(uint8_t *data)
{
    if (g_spi_fd < 0 || !data) return -1;
    uint8_t a=0,b=0,c=0;
    if (spi_read_direct(g_spi_fd,&a) || spi_read_direct(g_spi_fd,&b) ||
        spi_read_direct(g_spi_fd,&c) || a!=b || b!=c) return -1;
    *data=a;return 0;
}

/* 独立开关读事务不能使用旧的分两次片选的模式读取路径。
 * 两个 transfer 属于同一个 SPI_IOC_MESSAGE；cs_change=0 保持片选。
 * 地址阶段后保留 1us，让 FPGA 发送端锁存状态，再切换四线接收。
 * S端实机smdt_spi仅支持固定速率（最低24MHz），1MHz会返回EINVAL。
 * 因此状态通道使用与已有控制路径相同的SPI_SPEED=24MHz。
 * 读取失败只记录诊断，由车控主循环节流打印，不在每次轮询刷屏。 */
int fspi_read_auto_brake_switch(int *enabled)
{
    g_auto_brake_errno = 0;
    g_auto_brake_transfer_result = -1;
    g_auto_brake_raw_status = 0xff;
    if (g_spi_fd < 0 || !enabled) {
        g_auto_brake_errno = !enabled ? EINVAL : ENODEV;
        return -1;
    }

    uint8_t command[3] = {0x81, 0x00, 0x05};
    uint8_t status = 0xff;
    const uint32_t status_speed_hz = SPI_SPEED;
    struct spi_ioc_transfer transfers[2] = {{0}, {0}};
    g_config.spi_mode |= SPI_TX_QUAD | SPI_RX_QUAD;
    /* 保留已有模式/位宽/全局速度配置，但此入口不调用带perror的旧配置函数。 */
    if (ioctl(g_spi_fd, SPI_IOC_WR_MODE32, &g_config.spi_mode) == -1 ||
        ioctl(g_spi_fd, SPI_IOC_WR_BITS_PER_WORD, &g_config.bits_per_word) == -1 ||
        ioctl(g_spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &g_config.speed_hz) == -1) {
        g_auto_brake_errno = errno;
        return -1;
    }

    transfers[0].tx_buf = (unsigned long)command;
    transfers[0].len = sizeof(command);
    transfers[0].speed_hz = status_speed_hz;
    transfers[0].bits_per_word = g_config.bits_per_word;
    transfers[0].tx_nbits = 4;
    transfers[0].delay_usecs = 1;
    transfers[0].cs_change = 0;
    transfers[1].rx_buf = (unsigned long)&status;
    transfers[1].len = 1;
    transfers[1].speed_hz = status_speed_hz;
    transfers[1].bits_per_word = g_config.bits_per_word;
    transfers[1].rx_nbits = 4;
    transfers[1].cs_change = 0;

    int result = ioctl(g_spi_fd, SPI_IOC_MESSAGE(2), transfers);
    int error_number = result < 0 ? errno : 0;
    g_auto_brake_transfer_result = result;
    g_auto_brake_raw_status = status;
    if (result != 4) {
        g_auto_brake_errno = result < 0 ? error_number : EIO;
        return -1;
    }
    if (status != 0xa0 && status != 0xa1) {
        g_auto_brake_errno = EPROTO;
        return -1;
    }
    *enabled = status == 0xa1;
    return 0;
}

void fspi_get_auto_brake_diagnostic(int *error_number, int *transfer_result, uint8_t *raw_status)
{
    if (error_number) *error_number = g_auto_brake_errno;
    if (transfer_result) *transfer_result = g_auto_brake_transfer_result;
    if (raw_status) *raw_status = g_auto_brake_raw_status;
}

/* 公开的无地址可靠读取入口，同样要求先初始化设备。 */
int fspi_read_direct(uint8_t *data)
{
    if (g_spi_fd < 0) {
        return -1;
    }

    return spi_read_direct_reliable(g_spi_fd, data);
}

/* 按方向 0..3 写相应寄存器；运行时写零，停止时写一，不做读回验证。 */
int vehicle_control(int command, int enable)
{
    if (g_spi_fd < 0) {
        return -1;
    }

    if (command < 0 || command > 3) {
        printf("错误：车辆控制命令必须在0-3范围内，当前值: %d\n", command);
        printf("0=前进, 1=后退, 2=左转, 3=右转\n");
        return -1;
    }

    if (enable != 0 && enable != 1) {
        printf("错误：enable值必须是0或1，当前值: %d\n", enable);
        return -1;
    }

    uint16_t addr = (uint16_t)command;
    uint8_t control_value = (uint8_t)(enable ? 0 : 1);

    const char* command_names[] = {"前进", "后退", "左转", "右转"};
    const char* enable_text = enable ? "运动" : "停止";

    printf("正在%s %s...\n", enable_text, command_names[command]);
    printf("调试信息: 命令=%d, enable=%d, 地址=0x%04X, 发送值=%d\n",
           command, enable, addr, control_value);

    int write_ret = fspi_write_data(addr, control_value);

    if (write_ret == 0) {
        printf("车辆控制成功: %s %s (地址=0x%04X, 值=%d)\n",
               command_names[command], enable_text, addr, control_value);
        return 0;
    } else {
        printf("车辆控制失败: %s %s (写入失败)\n",
               command_names[command], enable_text);
        return -1;
    }
}

/* 四个方向包装函数仅传入固定命令编号。 */
int vehicle_forward(int enable) {
    return vehicle_control(0, enable);
}

int vehicle_backward(int enable) {
    return vehicle_control(1, enable);
}

int vehicle_turn_left(int enable) {
    return vehicle_control(2, enable);
}

int vehicle_turn_right(int enable) {
    return vehicle_control(3, enable);
}

/* stop 按键低电平有效；向地址 4 写零并保持，直到明确写一解除。 */
int vehicle_emergency_stop(void) {
    if (g_spi_fd < 0) {
        return -1;
    }

    uint16_t addr = 4;
    uint8_t stop_value = 0;

    printf("正在执行立即主动停止...\n");

    int write_ret = fspi_write_data(addr, stop_value);

    if (write_ret == 0) {
        printf("立即主动停止成功 (地址=0x%04X, 值=%d)\n", addr, stop_value);
        return 0;
    } else {
        printf("立即主动停止失败 (写入失败)\n");
        return -1;
    }
}

/* 关闭已打开的 SPI 句柄并重置全局状态。 */
void fspi_cleanup(void)
{
    if (g_spi_fd >= 0) {
        close(g_spi_fd);
        g_spi_fd = -1;
        printf("FSPI resources cleaned up\n");
    }
}
