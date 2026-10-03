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

// SPI配置宏定义 - 对应原来的参数 -d /dev/spidev4.0 -s 100000000 -OH -m 3 -S 1 -c 1 (修改为3字节传输)
#define SPI_DEVICE "/dev/spidev4.0"
#define SPI_SPEED 100000000
#define SPI_MODE_FLAGS (SPI_CPOL | SPI_CPHA) // 对应 -OH 参数，即 SPI_MODE_3
#define SPI_TRANSFER_SIZE 3
#define SPI_CYCLE_COUNT 1
#define SPI_BITS_PER_WORD 8
#define SPI_DELAY_US 0
#define SPI_PROTOCOL_MODE SPI_MODE_QUAD // 对应 -m 3 参数

#define TOOL_VERSION "2.0"
#define DEFAULT_DEVICE "/dev/spidev4.0"
#define MAX_DEVICE_PATH 64
#define MAX_BUFFER_SIZE (1024 * 1024) // 1MB

// 读写重试次数宏定义
#define WRITE_RETRY_COUNT 15    // 写入重试次数
#define READ_RETRY_COUNT 10     // 读取重试次数

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

// 全局配置变量
static SpiTestConfig g_config;
static int g_spi_fd = -1;

// 内部函数声明
static int configure_spi_interface(int fd, SpiTestConfig *config);
static int perform_transfer(int fd, uint8_t *tx_buf, uint8_t *rx_buf, SpiTestConfig *config);
static int spi_write_3bytes(int fd, uint16_t addr, uint8_t data);
static int spi_read_addr_data(int fd, uint16_t addr, uint8_t *data);
static int spi_write_reliable(int fd, uint16_t addr, uint8_t data);
static int spi_read_reliable(int fd, uint16_t addr, uint8_t *data);

// 内部函数实现
static int configure_spi_interface(int fd, SpiTestConfig *config)
{
    // Set SPI mode
    if (ioctl(fd, SPI_IOC_WR_MODE32, &config->spi_mode) == -1)
    {
        perror("Failed to set SPI mode");
        return -1;
    }

    // Set bits per word
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &config->bits_per_word) == -1)
    {
        perror("Failed to set bits per word");
        return -1;
    }

    // Set max speed
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &config->speed_hz) == -1)
    {
        perror("Failed to set SPI speed");
        return -1;
    }

    return 0;
}

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

    // Configure transfer mode
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

// 写3个字节函数: [地址高8位] [地址低8位] [数据]
static int spi_write_3bytes(int fd, uint16_t addr, uint8_t data)
{
    uint8_t tx_buffer[3];
    
    // 组装3字节数据包
    tx_buffer[0] = (addr >> 8) & 0xFF;   // 地址高8位
    tx_buffer[1] = addr & 0xFF;          // 地址低8位
    tx_buffer[2] = data;                 // 数据
    
    printf("TX Buffer: [0x%02X] [0x%02X] [0x%02X]\n", 
           tx_buffer[0], tx_buffer[1], tx_buffer[2]);
    
    // 设置QUAD发送模式
    g_config.spi_mode |= SPI_TX_QUAD;
    g_config.spi_mode &= ~SPI_RX_QUAD;
    
    if (configure_spi_interface(fd, &g_config) != 0)
    {
        return -1;
    }

    // 执行3字节传输
    if (perform_transfer(fd, tx_buffer, NULL, &g_config) < 0)
    {
        perror("Write 3 bytes failed");
        return -1;
    }

    return 0;
}

// 读地址数据函数: 先发送[地址高8位] [地址低8位]，然后读取数据
static int spi_read_addr_data(int fd, uint16_t addr, uint8_t *data)
{
    uint8_t tx_buffer[2];
    uint8_t rx_data = 0;
    
    // 第1步: 发送地址 (TX模式)
    // 组装2字节地址包
    tx_buffer[0] = (addr >> 8) & 0xFF;   // 地址高8位
    tx_buffer[1] = addr & 0xFF;          // 地址低8位
    
    printf("TX Address: [0x%02X] [0x%02X]\n", tx_buffer[0], tx_buffer[1]);
    
    // 设置QUAD发送模式
    g_config.spi_mode |= SPI_TX_QUAD;
    g_config.spi_mode &= ~SPI_RX_QUAD;
    
    if (configure_spi_interface(fd, &g_config) != 0)
    {
        return -1;
    }

    // 临时修改传输大小为2字节发送地址
    uint32_t original_size = g_config.transfer_size;
    g_config.transfer_size = 2;

    // 执行2字节地址传输
    if (perform_transfer(fd, tx_buffer, NULL, &g_config) < 0)
    {
        perror("Send address failed");
        g_config.transfer_size = original_size;
        return -1;
    }

    // 第2步: 读取数据 (切换到RX模式)
    g_config.spi_mode = (g_config.spi_mode & ~SPI_TX_QUAD) | SPI_RX_QUAD;
    g_config.transfer_size = 1;  // 读取1字节数据
    
    if (configure_spi_interface(fd, &g_config) != 0)
    {
        g_config.transfer_size = original_size;
        return -1;
    }

    // 执行1字节数据接收
    if (perform_transfer(fd, NULL, &rx_data, &g_config) < 0)
    {
        perror("Read data failed");
        g_config.transfer_size = original_size;
        return -1;
    }

    // 恢复原始传输大小
    g_config.transfer_size = original_size;

    *data = rx_data;
    printf("RX Data: [0x%02X]\n", *data);
    
    return 0;
}

// 封装的可靠写入函数 - 多次重试写入
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
            return 0; // 写入成功，直接返回
        }
        
        // 如果不是最后一次尝试，继续重试
        if (retry_count < WRITE_RETRY_COUNT - 1)
        {
            usleep(1000); // 延时1ms后重试
        }
    }
    
    return -1; // 所有尝试都失败
}

// 封装的可靠读取函数 - 多次读取返回出现次数最多的值
static int spi_read_reliable(int fd, uint16_t addr, uint8_t *data)
{
    int retry_count = 0;
    int ret;
    uint8_t read_data = 0;
    
    // 用于统计每个数据值的出现次数 (0-255)
    int data_count[256] = {0};
    int successful_reads = 0;
    
    printf("    [READ]  地址: 0x%04X\n", addr);
    
    for (retry_count = 0; retry_count < READ_RETRY_COUNT; retry_count++)
    {
        ret = spi_read_addr_data(fd, addr, &read_data);
        
        if (ret == 0)  // 读取成功
        {
            data_count[read_data]++;  // 统计该数据值的出现次数
            successful_reads++;       // 成功读取次数加1
        }
        
        // 每次读取之间稍微延时
        usleep(500); // 延时0.5ms
    }
    
    if (successful_reads > 0)
    {
        // 找出出现次数最多的数据值
        uint8_t most_frequent_data = 0;
        int max_count = 0;
        
        for (int i = 0; i < 256; i++)
        {
            if (data_count[i] > max_count)
            {
                max_count = data_count[i];
                most_frequent_data = (uint8_t)i;
            }
        }
        
        printf("    [READ]  结果: 0x%02X\n", most_frequent_data);
        
        *data = most_frequent_data;
        return 0;
    }
    else
    {
        printf("    [READ]  读取失败\n");
        return -1;  // 所有读取都失败
    }
}

//=========================================================================
// 外部接口函数实现
//=========================================================================

// FSPI初始化函数
int fspi_init(void)
{
    printf("========== SPI可靠读写程序 ==========\n");
    
    // 使用宏定义配置
    g_config.speed_hz = SPI_SPEED;
    g_config.transfer_size = SPI_TRANSFER_SIZE;
    g_config.cycle_count = SPI_CYCLE_COUNT;
    g_config.spi_mode = SPI_MODE_FLAGS;
    g_config.bits_per_word = SPI_BITS_PER_WORD;
    g_config.delay_us = SPI_DELAY_US;
    g_config.protocol_mode = SPI_PROTOCOL_MODE;
    strcpy(g_config.device, SPI_DEVICE);

    // 打开SPI设备
    g_spi_fd = open(g_config.device, O_RDWR);
    if (g_spi_fd < 0)
    {
        perror("FSPI Device open failed");
        return -1;
    }

    // 配置SPI接口
    if (configure_spi_interface(g_spi_fd, &g_config) != 0)
    {
        close(g_spi_fd);
        g_spi_fd = -1;
        return -1;
    }

    // 只在初始化时打印一次配置信息
    printf("FSPI Configuration:\n");
    printf("  Device: %s\n", g_config.device);
    printf("  Mode: 0x%08X\n", g_config.spi_mode);
    printf("  Bits per word: %d\n", g_config.bits_per_word);
    printf("  Speed: %u Hz (%u KHz)\n", g_config.speed_hz, g_config.speed_hz / 1000);
    printf("FSPI initialized successfully\n");
    return 0;
}

// FSPI写数据函数
int fspi_write_data(uint16_t addr, uint8_t data)
{
    if (g_spi_fd < 0) {
        printf("FSPI not initialized\n");
        return -1;
    }
    
    return spi_write_reliable(g_spi_fd, addr, data);
}

// FSPI读数据函数
int fspi_read_data(uint16_t addr, uint8_t *data)
{
    if (g_spi_fd < 0) {
        printf("FSPI not initialized\n");
        return -1;
    }
    
    return spi_read_reliable(g_spi_fd, addr, data);
}

// 车辆控制函数 - 控制前进、后退、左转、右转（带重试机制）
int vehicle_control(int command, int enable)
{
    if (g_spi_fd < 0) {
        printf("FSPI not initialized\n");
        return -1;
    }
    
    // 参数检查：命令必须在0-3范围内，enable必须是0或1
    if (command < 0 || command > 3) {
        printf("错误：车辆控制命令必须在0-3范围内，当前值: %d\n", command);
        printf("0=前进, 1=后退, 2=左转, 3=右转\n");
        return -1;
    }
    
    if (enable != 0 && enable != 1) {
        printf("错误：enable值必须是0或1，当前值: %d\n", enable);
        return -1;
    }
    
    uint16_t addr = (uint16_t)command;    // 地址0-3分别对应前进、后退、左转、右转
    uint8_t control_value = (uint8_t)(enable ? 0 : 1);  // 修改逻辑：1=停止，0=运行
    
    const char* command_names[] = {"前进", "后退", "左转", "右转"};
    const char* enable_text = enable ? "运行" : "停止";
    
    printf("正在%s %s...\n", enable_text, command_names[command]);
    
    // 重试机制：写入失败后重试，直到读取和写入一致
    int max_retries = 10;  // 最大重试次数
    int retry_count = 0;
    
    while (retry_count < max_retries) {
        // 写入车辆控制命令
        int write_ret = fspi_write_data(addr, control_value);
        
        if (write_ret != 0) {
            printf("  第%d次写入失败，重试中...\n", retry_count + 1);
            retry_count++;
            usleep(10000);  // 延时10ms后重试
            continue;
        }
        
        // 写入成功，进行读回验证
        uint8_t read_data = 0;
        int read_ret = fspi_read_data(addr, &read_data);
        
        if (read_ret != 0) {
            printf("  第%d次读取验证失败，重试写入...\n", retry_count + 1);
            retry_count++;
            usleep(10000);  // 延时10ms后重试
            continue;
        }
        
        // 检查读取值是否与写入值一致
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
            usleep(10000);  // 延时10ms后重试
        }
    }
    
    // 所有重试都失败
    printf("车辆控制失败: %s %s (已重试%d次，仍未成功)\n", 
           command_names[command], enable_text, max_retries);
    return -1;
}

// 便捷的车辆控制函数
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

// FSPI清理函数
void fspi_cleanup(void)
{
    if (g_spi_fd >= 0) {
        close(g_spi_fd);
        g_spi_fd = -1;
        printf("FSPI resources cleaned up\n");
    }
}