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

#define SPI_DEVICE "/dev/spidev4.0"
#define SPI_SPEED 100000000
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

static int configure_spi_interface(int fd, SpiTestConfig *config);
static int perform_transfer(int fd, uint8_t *tx_buf, uint8_t *rx_buf, SpiTestConfig *config);
static int spi_write_3bytes(int fd, uint16_t addr, uint8_t data);
static int spi_read_addr_data(int fd, uint16_t addr, uint8_t *data);
static int spi_write_reliable(int fd, uint16_t addr, uint8_t data);
static int spi_read_reliable(int fd, uint16_t addr, uint8_t *data);

/* 按原顺序设置 SPI 模式、字宽和频率，配置失败时立即返回。 */
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

/* 按协议选择单线、双线或四线收发宽度并执行一次事务。 */
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
        tr.tx_nbits = tr.rx_nbits = 2;
        break;
    case SPI_MODE_QUAD:
        tr.tx_nbits = tr.rx_nbits = 4;
        break;
    default:
        tr.tx_nbits = tr.rx_nbits = 1;
    }

    return ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
}

/* 高位地址、低位地址和数据依次组成三字节写帧。 */
static int spi_write_3bytes(int fd, uint16_t addr, uint8_t data)
{
    uint8_t tx_buffer[3];

    tx_buffer[0] = (addr >> 8) & 0xFF;
    tx_buffer[1] = addr & 0xFF;
    tx_buffer[2] = data;

    printf("TX Buffer: [0x%02X] [0x%02X] [0x%02X]\n",
           tx_buffer[0], tx_buffer[1], tx_buffer[2]);

    g_config.spi_mode |= SPI_TX_QUAD;
    g_config.spi_mode &= ~SPI_RX_QUAD;

    if (configure_spi_interface(fd, &g_config) != 0)
    {
        return -1;
    }

    if (perform_transfer(fd, tx_buffer, NULL, &g_config) < 0)
    {
        perror("Write 3 bytes failed");
        return -1;
    }

    return 0;
}

/* 先发送地址的两个字节，再切为四线接收一字节；传输长度在退出前恢复。 */
static int spi_read_addr_data(int fd, uint16_t addr, uint8_t *data)
{
    uint8_t tx_buffer[2];
    uint8_t rx_data = 0;

    tx_buffer[0] = (addr >> 8) & 0xFF;
    tx_buffer[1] = addr & 0xFF;

    printf("TX Address: [0x%02X] [0x%02X]\n", tx_buffer[0], tx_buffer[1]);

    g_config.spi_mode |= SPI_TX_QUAD;
    g_config.spi_mode &= ~SPI_RX_QUAD;

    if (configure_spi_interface(fd, &g_config) != 0)
    {
        return -1;
    }

    uint32_t original_size = g_config.transfer_size;
    g_config.transfer_size = 2;

    if (perform_transfer(fd, tx_buffer, NULL, &g_config) < 0)
    {
        perror("Send address failed");
        g_config.transfer_size = original_size;
        return -1;
    }

    g_config.spi_mode = (g_config.spi_mode & ~SPI_TX_QUAD) | SPI_RX_QUAD;
    g_config.transfer_size = 1;

    if (configure_spi_interface(fd, &g_config) != 0)
    {
        g_config.transfer_size = original_size;
        return -1;
    }

    if (perform_transfer(fd, NULL, &rx_data, &g_config) < 0)
    {
        perror("Read data failed");
        g_config.transfer_size = original_size;
        return -1;
    }

    g_config.transfer_size = original_size;

    *data = rx_data;
    printf("RX Data: [0x%02X]\n", *data);

    return 0;
}

/* 失败时最多重试十五次写入，两次尝试间隔一毫秒。 */
static int spi_write_reliable(int fd, uint16_t addr, uint8_t data)
{
    int retry_count = 0;
    int ret;

    printf("    [WRITE] 地址: 0x%04X, 数据: 0x%02X\n", addr, data);

    for (retry_count = 0; retry_count < WRITE_RETRY_COUNT; retry_count++)
    {
        ret = spi_write_3bytes(fd, addr, data);

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

/* 选择成功采样中的众数；并列时保留数值较小的字节。 */
static uint8_t select_most_frequent_byte(const int counts[256])
{
    uint8_t result = 0;
    int highest_count = 0;
    for (int value = 0; value < 256; ++value)
    {
        if (counts[value] > highest_count)
        {
            highest_count = counts[value];
            result = (uint8_t)value;
        }
    }
    return result;
}

/* 连续读取十次并按成功样本投票，每次采样后等待半毫秒。 */
static int spi_read_reliable(int fd, uint16_t addr, uint8_t *data)
{
    int retry_count = 0;
    int ret;
    uint8_t read_data = 0;

    int data_count[256] = {0};
    int successful_reads = 0;

    printf("    [READ]  地址: 0x%04X\n", addr);

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

        printf("    [READ]  结果: 0x%02X\n", most_frequent_data);

        *data = most_frequent_data;
        return 0;
    }
    else
    {
        printf("    [READ]  读取失败\n");
        return -1;
    }
}

/* 打开固定 SPI 设备，初始化模式、位宽与时钟；配置失败时关闭句柄。 */
int fspi_init(void)
{
    printf("========== SPI可靠读写程序 ==========\n");

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
    printf("  Mode: 0x%08X\n", g_config.spi_mode);
    printf("  Bits per word: %d\n", g_config.bits_per_word);
    printf("  Speed: %u Hz (%u KHz)\n", g_config.speed_hz, g_config.speed_hz / 1000);
    printf("FSPI initialized successfully\n");
    return 0;
}

/* 公开的寄存器写入入口；设备未初始化时直接失败。 */
int fspi_write_data(uint16_t addr, uint8_t data)
{
    if (g_spi_fd < 0) {
        printf("FSPI not initialized\n");
        return -1;
    }

    return spi_write_reliable(g_spi_fd, addr, data);
}

/* 公开的寄存器读取入口，通过重复采样选择稳定读值。 */
int fspi_read_data(uint16_t addr, uint8_t *data)
{
    if (g_spi_fd < 0) {
        printf("FSPI not initialized\n");
        return -1;
    }

    return spi_read_reliable(g_spi_fd, addr, data);
}

/* 方向 0..3 对应四个寄存器；写入后读回验证，最多重试十次。 */
int vehicle_control(int command, int enable)
{
    if (g_spi_fd < 0) {
        printf("FSPI not initialized\n");
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
    const char* enable_text = enable ? "运行" : "停止";

    printf("正在%s %s...\n", enable_text, command_names[command]);

    int max_retries = 10;
    int retry_count = 0;

    while (retry_count < max_retries) {

        int write_ret = fspi_write_data(addr, control_value);

        if (write_ret != 0) {
            printf("  第%d次写入失败，重试中...\n", retry_count + 1);
            retry_count++;
            usleep(10000);
            continue;
        }

        uint8_t read_data = 0;
        int read_ret = fspi_read_data(addr, &read_data);

        if (read_ret != 0) {
            printf("  第%d次读取验证失败，重试写入...\n", retry_count + 1);
            retry_count++;
            usleep(10000);
            continue;
        }

        if (read_data == control_value) {
            printf("车辆控制成功: %s %s (地址=0x%04X, 值=%d, 重试%d次)\n",
                   command_names[command], enable_text, addr, control_value, retry_count);
            printf("验证成功: %s当前状态=%s\n",
                   command_names[command], read_data ? "停止" : "运行");
            return 0;
        } else {
            printf("  第%d次验证失败 (写入=%d, 读取=%d)，重试写入...\n",
                   retry_count + 1, control_value, read_data);
            retry_count++;
            usleep(10000);
        }
    }

    printf("车辆控制失败: %s %s (已重试%d次，仍未成功)\n",
           command_names[command], enable_text, max_retries);
    return -1;
}

/* 四个方向包装函数复用相同的写入和读回验证流程。 */
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

/* 关闭已打开的 SPI 设备并重置句柄。 */
void fspi_cleanup(void)
{
    if (g_spi_fd >= 0) {
        close(g_spi_fd);
        g_spi_fd = -1;
        printf("FSPI resources cleaned up\n");
    }
}