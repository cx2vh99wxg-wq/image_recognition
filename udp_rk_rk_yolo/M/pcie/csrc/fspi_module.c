// 为usleep函数定义_GNU_SOURCE
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
#define WRITE_RETRY_COUNT 3    // 写入重试次数
#define READ_RETRY_COUNT 5     // 读取重试次数

// 寄存器数量宏定义
#define REGISTER_COUNT 5       // 寄存器数量

// 循环测试宏定义
#define TEST_LOOP_COUNT 100    // 循环测试次数

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
static void run_rw_test_loop(int fd) __attribute__((unused));

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

// 循环测试函数 - 测试读写正确性并统计正确率
static void run_rw_test_loop(int fd)
{
    // 定义5个寄存器地址
    uint16_t register_addrs[REGISTER_COUNT] = {0x0000, 0x0001, 0x0002, 0x0003, 0x0004};
    
    int total_tests = 0;
    int write_success = 0;
    int read_success = 0;
    int data_match = 0;
    
    // 初始化随机数种子
    srand((unsigned int)time(NULL));
    
    printf("\n========== 开始寄存器测试 ==========\n");
    printf("测试寄存器地址: ");
    for (int i = 0; i < REGISTER_COUNT; i++) {
        printf("0x%04X ", register_addrs[i]);
    }
    printf("\n使用随机数据进行测试\n\n");
    
    // 测试5个寄存器
    for (int reg_idx = 0; reg_idx < REGISTER_COUNT; reg_idx++)
    {
        uint16_t addr = register_addrs[reg_idx];
        uint8_t write_data = (uint8_t)(rand() % 256);  // 生成0-255的随机数
        uint8_t read_data = 0;
        
        printf("测试寄存器 %d: 地址=0x%04X, 随机数据=0x%02X\n", 
               reg_idx + 1, addr, write_data);
        
        total_tests++;
        
        // 写入测试
        int write_ret = spi_write_reliable(fd, addr, write_data);
        if (write_ret == 0) {
            write_success++;
            printf("  写入: 成功\n");
        } else {
            printf("  写入: 失败\n");
        }
        
        // 短暂延时，确保写入完成
        usleep(5000); // 5ms
        
        // 读取测试
        int read_ret = spi_read_reliable(fd, addr, &read_data);
        if (read_ret == 0) {
            read_success++;
            printf("  读取: 成功, 值=0x%02X\n", read_data);
            
            // 数据匹配性检查
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
        
        // 寄存器间隔延时
        usleep(2000); // 2ms
    }
    
    // 统计结果
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
    
    // 综合评估
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

// 切换视频流函数 - 向地址0写入1-6的值来切换视频流
int switch_video_stream(int stream_id)
{
    if (g_spi_fd < 0) {
        printf("FSPI not initialized\n");
        return -1;
    }
    
    // 参数检查：视频流ID必须在1-6范围内
    if (stream_id < 1 || stream_id > 6) {
        printf("错误：视频流ID必须在1-6范围内，当前值: %d\n", stream_id);
        return -1;
    }
    
    uint16_t addr = 0x0000;                    // 视频流切换寄存器地址
    uint8_t stream_value = (uint8_t)stream_id;  // 视频流值(1-6)
    
    printf("正在切换到视频流 %d...\n", stream_id);
    
    // 写入视频流切换命令
    int write_ret = fspi_write_data(addr, stream_value);
    if (write_ret == 0) {
        printf("视频流切换成功: 流ID=%d\n", stream_id);
        
        // 可选：读回验证
        uint8_t read_data = 0;
        int read_ret = fspi_read_data(addr, &read_data);
        if (read_ret == 0) {
            if (read_data == stream_value) {
                printf("视频流切换验证成功: 当前流ID=%d\n", read_data);
            } else {
                printf("警告：视频流切换验证失败 (写入=%d, 读取=%d)\n", stream_value, read_data);
            }
        }
        
        // 直接更新全局视频流跟踪变量
        extern int current_video_stream;
        current_video_stream = stream_id;
        
        return 0;
    } else {
        printf("视频流切换失败: 流ID=%d\n", stream_id);
        return -1;
    }
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