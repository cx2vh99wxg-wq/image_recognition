
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

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
#include <fcntl.h>
#include <unistd.h>
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

#define WRITE_RETRY_COUNT 3
#define READ_RETRY_COUNT 5

#define REGISTER_COUNT 5

#define TEST_LOOP_COUNT 100

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
static void run_rw_test_loop(int fd) __attribute__((unused));

/* 依次设置 SPI 模式、字宽及频率；配置失败即停止本次操作。 */
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

/* 根据配置选择数据线宽度并执行一笔 SPI 事务。 */
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

/* 将地址拆成高、低字节，连同一个数据字节按原协议发送。 */
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

/* 先发送两字节地址，再切换到接收模式获取一字节数据。 */
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

/* 最多写入三次，失败且可继续时等待一毫秒。 */
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

/* 按升序扫描字节计数。只有出现更高次数才更新结果，因此并列时选较小值。 */
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

/* 对寄存器采样五次，忽略失败读数，从成功样本中选出众数。 */
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

/* 保留原有的寄存器循环测试：写随机值、读取校验并统计结果；当前不由主流程调用。 */
static void run_rw_test_loop(int fd)
{

    uint16_t register_addrs[REGISTER_COUNT] = {0x0000, 0x0001, 0x0002, 0x0003, 0x0004};

    int total_tests = 0;
    int write_success = 0;
    int read_success = 0;
    int data_match = 0;

    srand((unsigned int)time(NULL));

    printf("\n========== 开始寄存器测试 ==========\n");
    printf("测试寄存器地址: ");
    for (int i = 0; i < REGISTER_COUNT; i++) {
        printf("0x%04X ", register_addrs[i]);
    }
    printf("\n使用随机数据进行测试\n\n");

    for (int reg_idx = 0; reg_idx < REGISTER_COUNT; reg_idx++)
    {
        uint16_t addr = register_addrs[reg_idx];
        uint8_t write_data = (uint8_t)(rand() % 256);
        uint8_t read_data = 0;

        printf("测试寄存器 %d: 地址=0x%04X, 随机数据=0x%02X\n",
               reg_idx + 1, addr, write_data);

        total_tests++;

        int write_ret = spi_write_reliable(fd, addr, write_data);
        if (write_ret == 0) {
            write_success++;
            printf("  写入: 成功\n");
        } else {
            printf("  写入: 失败\n");
        }

        usleep(5000);

        int read_ret = spi_read_reliable(fd, addr, &read_data);
        if (read_ret == 0) {
            read_success++;
            printf("  读取: 成功, 值=0x%02X\n", read_data);

            if (read_data == write_data) {
                data_match++;
                printf("  匹配: 成功 ✓\n");
            } else {
                printf("  匹配: 失败 ✗ (写入=0x%02X, 读取=0x%02X)\n",
                       write_data, read_data);
            }
        } else {
            printf("  读取: 失败\n");
        }

        printf("\n");

        usleep(2000);
    }

    printf("========== 测试结果统计 ==========\n");
    printf("总测试寄存器数: %d\n", total_tests);
    printf("写入成功数: %d\n", write_success);
    printf("读取成功数: %d\n", read_success);
    printf("数据匹配数: %d\n", data_match);
    printf("\n========== 正确率统计 ==========\n");
    printf("写入成功率: %.2f%% (%d/%d)\n",
           (float)write_success / total_tests * 100, write_success, total_tests);
    printf("读取成功率: %.2f%% (%d/%d)\n",
           (float)read_success / total_tests * 100, read_success, total_tests);
    printf("数据正确率: %.2f%% (%d/%d)\n",
           (float)data_match / total_tests * 100, data_match, total_tests);

    float overall_success = (float)data_match / total_tests * 100;
    printf("\n========== 综合评估 ==========\n");
    if (overall_success >= 95.0) {
        printf("测试结果: 优秀 (%.2f%%)\n", overall_success);
    } else if (overall_success >= 90.0) {
        printf("测试结果: 良好 (%.2f%%)\n", overall_success);
    } else if (overall_success >= 80.0) {
        printf("测试结果: 一般 (%.2f%%)\n", overall_success);
    } else {
        printf("测试结果: 需要改进 (%.2f%%)\n", overall_success);
    }
}

/* 打开固定 SPI 设备并设置总线参数；失败时清理已经打开的句柄。 */
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

/* 公开的寄存器写入口，设备未初始化时直接返回失败。 */
int fspi_write_data(uint16_t addr, uint8_t data)
{
    if (g_spi_fd < 0) {
        printf("FSPI not initialized\n");
        return -1;
    }

    return spi_write_reliable(g_spi_fd, addr, data);
}

/* 公开的寄存器读入口，返回五次采样中最常见的成功读值。 */
int fspi_read_data(uint16_t addr, uint8_t *data)
{
    if (g_spi_fd < 0) {
        printf("FSPI not initialized\n");
        return -1;
    }

    return spi_read_reliable(g_spi_fd, addr, data);
}

/* 只允许编号 1..6；向地址零写入编号，并按原逻辑更新全局流编号。 */
int switch_video_stream(int stream_id)
{
    if (g_spi_fd < 0) {
        printf("FSPI not initialized\n");
        return -1;
    }

    if (stream_id < 1 || stream_id > 6) {
        printf("错误：视频流ID必须在1-6范围内，当前值: %d\n", stream_id);
        return -1;
    }

    uint16_t addr = 0x0000;
    uint8_t stream_value = (uint8_t)stream_id;

    printf("正在切换到视频流 %d...\n", stream_id);

    int write_ret = fspi_write_data(addr, stream_value);
    if (write_ret == 0) {
        printf("视频流切换成功: 流ID=%d\n", stream_id);

        uint8_t read_data = 0;
        int read_ret = fspi_read_data(addr, &read_data);
        if (read_ret == 0) {
            if (read_data == stream_value) {
                printf("视频流切换验证成功: 当前流ID=%d\n", read_data);
            } else {
                printf("警告：视频流切换验证失败 (写入=%d, 读取=%d)\n", stream_value, read_data);
            }
        }

        extern int current_video_stream;
        current_video_stream = stream_id;

        return 0;
    } else {
        printf("视频流切换失败: 流ID=%d\n", stream_id);
        return -1;
    }
}

/* 关闭 SPI 设备并重置全局句柄状态。 */
void fspi_cleanup(void)
{
    if (g_spi_fd >= 0) {
        close(g_spi_fd);
        g_spi_fd = -1;
        printf("FSPI resources cleaned up\n");
    }
}