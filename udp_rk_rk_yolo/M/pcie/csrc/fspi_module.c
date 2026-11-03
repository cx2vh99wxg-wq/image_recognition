
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

#include "../../../common/transport/include/fspi_register_io.h"

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

static int perform_transfer(int fd, uint8_t *tx_buf, uint8_t *rx_buf, SpiTestConfig *config);
static int spi_write_3bytes(int fd, uint16_t addr, uint8_t data);
static int spi_read_addr_data(int fd, uint16_t addr, uint8_t *data);
static int spi_write_reliable(int fd, uint16_t addr, uint8_t data);
static int spi_read_reliable(int fd, uint16_t addr, uint8_t *data);
static void run_rw_test_loop(int fd) __attribute__((unused));

/* 根据传输方向确定总线宽度，再完成一次 SPI 事务。 */
static int perform_transfer(int fd, uint8_t *tx_buf, uint8_t *rx_buf, SpiTestConfig *config)
{
    return fspi_transfer(fd, tx_buf, rx_buf, config, 1);
}

/* 16 位地址拆为高、低字节，与数据字节按原线协议一同发送。 */
static int spi_write_3bytes(int fd, uint16_t addr, uint8_t data)
{
    uint8_t tx_buffer[3];

    tx_buffer[0] = (addr >> 8) & 0xFF;
    tx_buffer[1] = addr & 0xFF;
    tx_buffer[2] = data;

    printf("TX Buffer: [0x%02X] [0x%02X] [0x%02X]\n", tx_buffer[0], tx_buffer[1], tx_buffer[2]);

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

/* 先发送两个地址字节，再切换接收模式读入一个数据字节。 */
static int spi_read_addr_data(int fd, uint16_t addr, uint8_t *data)
{
    return fspi_read_addressed(fd, addr, data, &g_config, 1, 1);
}

/* 写请求最多三轮，仍可继续重试的失败之间等待一毫秒。 */
static int spi_write_reliable(int fd, uint16_t addr, uint8_t data)
{
    printf("    [WRITE] 地址: 0x%04X, 数据: 0x%02X\n", addr, data);
    return fspi_retry_write(spi_write_3bytes, WRITE_RETRY_COUNT, fd, addr, data);
}

/* 按本端读重试次数采样，输出成功读值的众数。 */
static int spi_read_reliable(int fd, uint16_t addr, uint8_t *data)
{
    printf("    [READ]  地址: 0x%04X\n", addr);
    const int result = fspi_vote_read(spi_read_addr_data, READ_RETRY_COUNT, fd, addr, data);
    if (result == 0)
        printf("    [READ]  结果: 0x%02X\n", *data);
    else
        printf("    [READ]  读取失败\n");
    return result;
}

/* 保留原寄存器循环测试，写随机值、读回核对并统计；正式主流程不调用。 */
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
    for (int i = 0; i < REGISTER_COUNT; i++)
    {
        printf("0x%04X ", register_addrs[i]);
    }
    printf("\n使用随机数据进行测试\n\n");

    for (int reg_idx = 0; reg_idx < REGISTER_COUNT; reg_idx++)
    {
        uint16_t addr = register_addrs[reg_idx];
        uint8_t write_data = (uint8_t)(rand() % 256);
        uint8_t read_data = 0;

        printf("测试寄存器 %d: 地址=0x%04X, 随机数据=0x%02X\n", reg_idx + 1, addr, write_data);

        total_tests++;

        int write_ret = spi_write_reliable(fd, addr, write_data);
        if (write_ret == 0)
        {
            write_success++;
            printf("  写入: 成功\n");
        }
        else
        {
            printf("  写入: 失败\n");
        }

        usleep(5000);

        int read_ret = spi_read_reliable(fd, addr, &read_data);
        if (read_ret == 0)
        {
            read_success++;
            printf("  读取: 成功, 值=0x%02X\n", read_data);

            if (read_data == write_data)
            {
                data_match++;
                printf("  匹配: 成功 ✓\n");
            }
            else
            {
                printf("  匹配: 失败 ✗ (写入=0x%02X, 读取=0x%02X)\n", write_data, read_data);
            }
        }
        else
        {
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
    printf("写入成功率: %.2f%% (%d/%d)\n", (float)write_success / total_tests * 100, write_success,
           total_tests);
    printf("读取成功率: %.2f%% (%d/%d)\n", (float)read_success / total_tests * 100, read_success,
           total_tests);
    printf("数据正确率: %.2f%% (%d/%d)\n", (float)data_match / total_tests * 100, data_match,
           total_tests);

    float overall_success = (float)data_match / total_tests * 100;
    printf("\n========== 综合评估 ==========\n");
    if (overall_success >= 95.0)
    {
        printf("测试结果: 优秀 (%.2f%%)\n", overall_success);
    }
    else if (overall_success >= 90.0)
    {
        printf("测试结果: 良好 (%.2f%%)\n", overall_success);
    }
    else if (overall_success >= 80.0)
    {
        printf("测试结果: 一般 (%.2f%%)\n", overall_success);
    }
    else
    {
        printf("测试结果: 需要改进 (%.2f%%)\n", overall_success);
    }
}

/* 打开既定 SPI 设备并设置总线，配置失败则关闭已取得的句柄。 */
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

/* 对外写寄存器的统一入口，设备未就绪时返回失败。 */
int fspi_write_data(uint16_t addr, uint8_t data)
{
    if (g_spi_fd < 0)
    {
        printf("FSPI not initialized\n");
        return -1;
    }

    return spi_write_reliable(g_spi_fd, addr, data);
}

/* 对外读寄存器的统一入口，从多次成功采样中选择最常见值。 */
int fspi_read_data(uint16_t addr, uint8_t *data)
{
    if (g_spi_fd < 0)
    {
        printf("FSPI not initialized\n");
        return -1;
    }

    return spi_read_reliable(g_spi_fd, addr, data);
}

/* 仅接收流编号 1..6，写入地址零后照旧尝试读回验证。 */
int switch_video_stream(int stream_id)
{
    if (g_spi_fd < 0)
    {
        printf("FSPI not initialized\n");
        return -1;
    }

    if (stream_id < 1 || stream_id > 6)
    {
        printf("错误：视频流ID必须在1-6范围内，当前值: %d\n", stream_id);
        return -1;
    }

    uint16_t addr = 0x0000;
    uint8_t stream_value = (uint8_t)stream_id;

    printf("正在切换到视频流 %d...\n", stream_id);

    int write_ret = fspi_write_data(addr, stream_value);
    if (write_ret == 0)
    {
        printf("视频流切换成功: 流ID=%d\n", stream_id);

        uint8_t read_data = 0;
        int read_ret = fspi_read_data(addr, &read_data);
        if (read_ret == 0)
        {
            if (read_data == stream_value)
            {
                printf("视频流切换验证成功: 当前流ID=%d\n", read_data);
            }
            else
            {
                printf("警告：视频流切换验证失败 (写入=%d, 读取=%d)\n", stream_value, read_data);
            }
        }

        extern int current_video_stream;
        current_video_stream = stream_id;

        return 0;
    }
    else
    {
        printf("视频流切换失败: 流ID=%d\n", stream_id);
        return -1;
    }
}

/* 关闭 SPI 文件并恢复全局句柄的未打开状态。 */
void fspi_cleanup(void)
{
    if (g_spi_fd >= 0)
    {
        close(g_spi_fd);
        g_spi_fd = -1;
        printf("FSPI resources cleaned up\n");
    }
}