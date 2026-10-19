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

/* 与原命令行参数相同的固定设备、时钟、模式及每次事务长度。 */
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

/* 写操作和读操作分别使用固定的尝试次数；读取失败的样本不参与投票。 */
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

/* 此实例只管理一个 SPI 设备；传输阶段会暂时改变模式和数据长度。 */
static SpiTestConfig g_config;
static int g_spi_fd = -1;

static int configure_spi_interface(int fd, SpiTestConfig *config);
static int perform_transfer(int fd, uint8_t *tx_buf, uint8_t *rx_buf, SpiTestConfig *config);
static int spi_write_3bytes(int fd, uint16_t addr, uint8_t data);
static int spi_read_addr_data(int fd, uint16_t addr, uint8_t *data);
static int spi_read_direct(int fd, uint8_t *data);
static int spi_write_reliable(int fd, uint16_t addr, uint8_t data);
static int spi_read_reliable(int fd, uint16_t addr, uint8_t *data);
static int spi_read_direct_reliable(int fd, uint8_t *data);

/* 依次设置 SPI 模式、每字位数和时钟上限；保持原有 ioctl 顺序，任一步失败即返回。 */
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

/* 用当前配置构造单次 SPI 事务，并根据协议模式设置收发数据线宽度。 */
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

/* 按高位地址、低位地址、数据的顺序组帧，切到四线发送后执行一次传输。 */
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

/* 先以四线发送两字节地址，再切换为四线接收一字节；所有退出路径恢复传输长度。 */
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

/* 直接切换到四线接收并读取一字节，不预先向从设备发送寄存器地址。 */
static int spi_read_direct(int fd, uint8_t *data)
{
    uint8_t rx_data = 0;

    printf("Direct Read...\n");

    g_config.spi_mode = (g_config.spi_mode & ~SPI_TX_QUAD) | SPI_RX_QUAD;

    if (configure_spi_interface(fd, &g_config) != 0)
    {
        return -1;
    }

    uint32_t original_size = g_config.transfer_size;
    g_config.transfer_size = 1;

    if (perform_transfer(fd, NULL, &rx_data, &g_config) < 0)
    {
        perror("Direct read failed");
        g_config.transfer_size = original_size;
        return -1;
    }

    g_config.transfer_size = original_size;

    *data = rx_data;
    printf("RX Data: [0x%02X]\n", *data);

    return 0;
}

/* 最多尝试 15 次写入；只有失败且仍有下一次机会时等待 1 ms。 */
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

/*
 * 从 0..255 的读值计数中选出出现次数最多的字节。相同次数时保留
 * 数值较小者：原实现按升序扫描并只在计数严格增大时更新结果。
 * 调用方仍负责判断是否至少有一次读取成功。
 */
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

/* 对指定寄存器采样十次，每次后等待 0.5 ms；以出现次数最多的成功读值为结果。 */
static int spi_read_reliable(int fd, uint16_t addr, uint8_t *data)
{
    int retry_count = 0;
    int ret;
    uint8_t read_data = 0;

    int data_count[256] = {0};
    int successful_reads = 0;

    printf("    [READ]  地址: 0x%04X", addr);
    if (addr == 0) {
        printf(" (小车视镜功能状态寄存器)");
    }
    printf("\n");

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

        printf("    [READ]  结果: 0x%02X", most_frequent_data);
        if (addr == 0) {

            switch (most_frequent_data) {
                case 0x00:
                    printf(" (前视镜模式)");
                    break;
                case 0x11:
                    printf(" (后视镜模式)");
                    break;
                default:
                    printf(" (异常状态)");
                    break;
            }
        }
        printf("\n");

        *data = most_frequent_data;
        return 0;
    }
    else
    {
        printf("    [READ]  读取失败 - 无法获取寄存器数据\n");
        return -1;
    }
}

/* 不发送地址地连续采样十次，忽略失败读数，再对成功读数作众数选择。 */
static int spi_read_direct_reliable(int fd, uint8_t *data)
{
    int retry_count = 0;
    int ret;
    uint8_t read_data = 0;

    int data_count[256] = {0};
    int successful_reads = 0;

    printf("    [DIRECT READ]\n");

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

        printf("    [DIRECT READ]  结果: 0x%02X\n", most_frequent_data);

        *data = most_frequent_data;
        return 0;
    }
    else
    {
        printf("    [DIRECT READ]  读取失败\n");
        return -1;
    }
}

/* 装载固定的设备和传输参数，打开 SPI 节点并配置总线；失败时关闭已打开的句柄。 */
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

/* 公开的寄存器写入入口：只有设备已初始化时才启动带重试的写事务。 */
int fspi_write_data(uint16_t addr, uint8_t data)
{
    if (g_spi_fd < 0) {
        printf("FSPI not initialized\n");
        return -1;
    }

    return spi_write_reliable(g_spi_fd, addr, data);
}

/* 公开的寄存器读取入口；零号寄存器的返回值还会被解释为视镜模式。 */
int fspi_read_data(uint16_t addr, uint8_t *data)
{
    if (g_spi_fd < 0) {
        printf("FSPI not initialized\n");
        return -1;
    }

    return spi_read_reliable(g_spi_fd, addr, data);
}

/* 公开的无地址读取入口；与按地址读取共用相同的设备初始化状态。 */
int fspi_read_direct(uint8_t *data)
{
    if (g_spi_fd < 0) {
        printf("FSPI not initialized\n");
        return -1;
    }

    return spi_read_direct_reliable(g_spi_fd, data);
}

/* 将方向 0..3 与启停状态写入对应寄存器；协议规定运行写 0、停止写 1。 */
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
    const char* enable_text = enable ? "运动" : "停止";

    printf("正在%s %s...\n", enable_text, command_names[command]);

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

/* 以下包装函数仅绑定方向编号，保留统一参数校验及相同的输出信息。 */
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

/* 向地址 4 写入 1 触发主动停止；此操作与方向寄存器的停止命令不同。 */
int vehicle_emergency_stop(void) {
    if (g_spi_fd < 0) {
        printf("FSPI not initialized\n");
        return -1;
    }

    uint16_t addr = 4;
    uint8_t stop_value = 1;

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

/* 若设备已经打开，则关闭句柄并重置全局状态；重复调用时不再关闭。 */
void fspi_cleanup(void)
{
    if (g_spi_fd >= 0) {
        close(g_spi_fd);
        g_spi_fd = -1;
        printf("FSPI resources cleaned up\n");
    }
}