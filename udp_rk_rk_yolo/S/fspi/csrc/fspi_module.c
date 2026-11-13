#define _DEFAULT_SOURCE
#define _BSD_SOURCE

#include "fspi_module.h"
#include "wheel_speed.h"
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
#include <sys/file.h>

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
static int g_spi_owner_lock = -1;
static int g_auto_brake_errno;
static int g_auto_brake_transfer_result = -1;
static uint8_t g_auto_brake_raw_status = 0xff;

static int perform_transfer(int fd, uint8_t *tx_buf, uint8_t *rx_buf, SpiTestConfig *config);
static int spi_write_register(int fd, uint16_t addr, uint8_t data);
static int spi_read_addr_data(int fd, uint16_t addr, uint8_t *data);
static int spi_read_direct(int fd, uint8_t *data);
static int spi_write_reliable(int fd, uint16_t addr, uint8_t data);
static int spi_read_reliable(int fd, uint16_t addr, uint8_t *data);
static int spi_read_direct_reliable(int fd, uint8_t *data);

/* 寄存器访问：按当前配置建立单次 SPI 事务，分别设置单线、双线或四线传输宽度。 */
static int perform_transfer(int fd, uint8_t *tx_buf, uint8_t *rx_buf, SpiTestConfig *config)
{
    return fspi_transfer(fd, tx_buf, rx_buf, config, 0);
}

/* 寄存器访问：本板SMDT控制器会在写事务前自动发送00命令。
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
        if (transferred >= 0)
            errno = EIO;
        perror("Write register failed");
        return -1;
    }

    return 0;
}

/* 地址0用来HMI选择码。SMDT在接收事务前自动发送80，直接接收即可。
 * 其他地址保留原有接口；该旧接口不代表真实方向寄存器读回。 */
static int spi_read_addr_data(int fd, uint16_t addr, uint8_t *data)
{
    if (!data)
    {
        errno = EINVAL;
        return -1;
    }
    if (addr == 0)
        return spi_read_direct(fd, data);
    return fspi_read_addressed(fd, addr, data, &g_config, 0, 0);
}

/* 寄存器访问：无需地址阶段，直接切换到四线接收并读取一字节。 */
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
        if (transferred >= 0)
            errno = EIO;
        perror("Direct read failed");
        g_config.transfer_size = original_size;
        return -1;
    }

    g_config.transfer_size = original_size;

    *data = rx_data;
    return 0;
}

/* 寄存器访问：最多尝试十五次写入，只有需要继续重试时才等待一毫秒。 */
static int spi_write_reliable(int fd, uint16_t addr, uint8_t data)
{
    return fspi_retry_write(spi_write_register, WRITE_RETRY_COUNT, fd, addr, data);
}

/* 寄存器访问：采样指定寄存器十次，每次后间隔半毫秒，再选取成功读值中的众数。 */
static int spi_read_reliable(int fd, uint16_t addr, uint8_t *data)
{
    return fspi_vote_read(spi_read_addr_data, READ_RETRY_COUNT, fd, addr, data);
}

/* 寄存器访问：无地址采样十次；失败样本不参与计数，并列众数选择较小值。 */
static int spi_read_direct_reliable(int fd, uint8_t *data)
{
    return fspi_vote_read(spi_read_addr_data, READ_RETRY_COUNT, fd, 0, data);
}

/* 寄存器访问：装载固定配置，打开 SPI 设备并设置总线参数；配置失败时关闭句柄。 */
int fspi_init(void)
{
    if (g_spi_fd >= 0)
        return 0;
    int owner =
        open("/tmp/steering-actuator.lock", O_CREAT | O_RDWR | O_CLOEXEC | O_NOFOLLOW, 0600);
    if (owner < 0)
        return -1;
    if (flock(owner, LOCK_EX | LOCK_NB))
    {
        int e = errno;
        close(owner);
        errno = e;
        return -1;
    }
    g_spi_owner_lock = owner;
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
        close(g_spi_owner_lock);
        g_spi_owner_lock = -1;
        return -1;
    }

    if (configure_spi_interface(g_spi_fd, &g_config) != 0)
    {
        close(g_spi_fd);
        g_spi_fd = -1;
        close(g_spi_owner_lock);
        g_spi_owner_lock = -1;
        return -1;
    }

    printf("FSPI Configuration:\n");
    printf("  Device: %s\n", g_config.device);
    printf("  Speed: %u Hz (%u KHz)\n", g_config.speed_hz, g_config.speed_hz / 1000);
    printf("FSPI initialized successfully\n");
    printf(
        "  SMDT transport: host write=ADDR_H ADDR_L DATA (3 bytes); controller supplies 00/80 opcodes\n");
    return 0;
}

/* 寄存器访问：公开的寄存器写入入口，未初始化时直接失败。 */
int fspi_write_data(uint16_t addr, uint8_t data)
{
    if (g_spi_fd < 0)
    {
        return -1;
    }

    return spi_write_reliable(g_spi_fd, addr, data);
}

int fspi_write_direction_once(uint16_t addr, uint8_t data)
{
    if (g_spi_fd < 0 || addr < 2 || addr > 3 || data > 1)
    {
        errno = EINVAL;
        return -1;
    }
    /* 触点有效的定时转向阶段不能调用 perror 或 stdio。 */
    g_config.spi_mode = (g_config.spi_mode | SPI_TX_QUAD) & ~SPI_RX_QUAD;
    if (ioctl(g_spi_fd, SPI_IOC_WR_MODE32, &g_config.spi_mode) < 0 ||
        ioctl(g_spi_fd, SPI_IOC_WR_BITS_PER_WORD, &g_config.bits_per_word) < 0 ||
        ioctl(g_spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &g_config.speed_hz) < 0)
        return -1;
    uint8_t bytes[3] = {(uint8_t)(addr >> 8), (uint8_t)addr, data};
    int n = perform_transfer(g_spi_fd, bytes, NULL, &g_config);
    if (n != 3)
    {
        if (n >= 0)
            errno = EIO;
        return -1;
    }
    return 0;
}

/* 寄存器访问：公开的按地址可靠读取入口；读值由重复采样决定。 */
int fspi_read_data(uint16_t addr, uint8_t *data)
{
    if (g_spi_fd < 0)
    {
        return -1;
    }

    return spi_read_reliable(g_spi_fd, addr, data);
}

/* 策略层快速读取仅立即采样三次，不执行
 * 10x500us 重试休眠；原显示及控制读取接口不变。 */
int fspi_read_hmi_snapshot(uint8_t *data)
{
    if (g_spi_fd < 0 || !data)
        return -1;
    uint8_t a = 0, b = 0, c = 0;
    if (spi_read_direct(g_spi_fd, &a) || spi_read_direct(g_spi_fd, &b) ||
        spi_read_direct(g_spi_fd, &c) || a != b || b != c)
        return -1;
    *data = a;
    return 0;
}

int fspi_read_speed_snapshot(SpeedSnapshot *data, uint8_t *raw_status)
{
    if (g_spi_fd < 0 || !data)
    {
        errno = !data ? EINVAL : ENODEV;
        return -1;
    }
    uint8_t bytes[4] = {0};
    g_config.spi_mode = (g_config.spi_mode | SPI_RX_QUAD) & ~SPI_TX_QUAD;
    if (ioctl(g_spi_fd, SPI_IOC_WR_MODE32, &g_config.spi_mode) < 0 ||
        ioctl(g_spi_fd, SPI_IOC_WR_BITS_PER_WORD, &g_config.bits_per_word) < 0 ||
        ioctl(g_spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &g_config.speed_hz) < 0)
        return -1;
    struct spi_ioc_transfer tr = {0};
    tr.rx_buf = (unsigned long)bytes;
    tr.len = sizeof(bytes);
    tr.rx_nbits = 4;
    tr.bits_per_word = g_config.bits_per_word;
    tr.speed_hz = g_config.speed_hz;
    int n = ioctl(g_spi_fd, SPI_IOC_MESSAGE(1), &tr);
    if (raw_status)
        *raw_status = bytes[2];
    if (n != (int)sizeof(bytes))
    {
        if (n >= 0)
            errno = EIO;
        return -1;
    }
    if (speed_decode_packet(bytes, data))
    {
        errno = EPROTO;
        return -1;
    }
    return 0;
}

static int read_control_packet(ControlSnapshot *data, uint8_t *view, uint16_t *period)
{
    if (g_spi_fd < 0 || !data)
    {
        errno = !data ? EINVAL : ENODEV;
        return -1;
    }
    uint8_t bytes[13] = {0};
    const uint32_t length = period ? 13u : view ? 9u : 8u;
    g_config.spi_mode = (g_config.spi_mode | SPI_RX_QUAD) & ~SPI_TX_QUAD;
    if (configure_spi_interface(g_spi_fd, &g_config))
        return -1;
    struct spi_ioc_transfer tr = {0};
    tr.rx_buf = (unsigned long)bytes;
    tr.len = length;
    tr.rx_nbits = 4;
    tr.bits_per_word = g_config.bits_per_word;
    tr.speed_hz = g_config.speed_hz;
    int n = ioctl(g_spi_fd, SPI_IOC_MESSAGE(1), &tr);
    if (n != (int)length)
    {
        if (n >= 0)
            errno = EIO;
        return -1;
    }
    if (control_decode_packet(bytes, data))
    {
        errno = EPROTO;
        return -1;
    }
    if (view)
        *view = bytes[8];
    if (period)
        *period = wheel_decode_period(bytes + 9);
    return 0;
}

int fspi_read_control_snapshot(ControlSnapshot *data)
{
    return read_control_packet(data, NULL, NULL);
}

int fspi_read_control_snapshot_with_view(ControlSnapshot *data, uint8_t *view)
{
    if (!view)
    {
        errno = EINVAL;
        return -1;
    }
    if (read_control_packet(data, view, NULL) == 0)
        return 0;

    // A failed optional tail read must not take away the control status.
    *view = 0xff;
    return fspi_read_control_snapshot(data);
}

int fspi_read_control_snapshot_with_wheel(ControlSnapshot *data, uint8_t *view, uint16_t *period)
{
    if (!view || !period)
    {
        errno = EINVAL;
        return -1;
    }
    *period = WHEEL_PERIOD_UNKNOWN;
    if (read_control_packet(data, view, period) == 0)
        return 0;
    return fspi_read_control_snapshot_with_view(data, view);
}

/* 读取独立开关不能采用旧的两次片选路径。两个 transfer
 * 必须在同一 SPI_IOC_MESSAGE 内，
 * cs_change=0 保持片选；地址后留 1us 供
 * FPGA 锁存，再转四线接收。实板 smdt_spi 最低支持
 * 24MHz，1MHz 会返回 EINVAL，因此沿用
 * SPI_SPEED=24MHz。失败只记诊断，由主循环限频输出。 */
int fspi_read_auto_brake_switch(int *enabled)
{
    g_auto_brake_errno = 0;
    g_auto_brake_transfer_result = -1;
    g_auto_brake_raw_status = 0xff;
    if (g_spi_fd < 0 || !enabled)
    {
        g_auto_brake_errno = !enabled ? EINVAL : ENODEV;
        return -1;
    }

    uint8_t command[3] = {0x81, 0x00, 0x05};
    uint8_t status = 0xff;
    const uint32_t status_speed_hz = SPI_SPEED;
    struct spi_ioc_transfer transfers[2] = {{0}, {0}};
    g_config.spi_mode |= SPI_TX_QUAD | SPI_RX_QUAD;
    /* 寄存器访问：保留已有模式/位宽/全局速度配置，但此入口不调用带perror的旧配置函数。 */
    if (ioctl(g_spi_fd, SPI_IOC_WR_MODE32, &g_config.spi_mode) == -1 ||
        ioctl(g_spi_fd, SPI_IOC_WR_BITS_PER_WORD, &g_config.bits_per_word) == -1 ||
        ioctl(g_spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &g_config.speed_hz) == -1)
    {
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
    if (result != 4)
    {
        g_auto_brake_errno = result < 0 ? error_number : EIO;
        return -1;
    }
    if (status != 0xa0 && status != 0xa1)
    {
        g_auto_brake_errno = EPROTO;
        return -1;
    }
    *enabled = status == 0xa1;
    return 0;
}

void fspi_get_auto_brake_diagnostic(int *error_number, int *transfer_result, uint8_t *raw_status)
{
    if (error_number)
        *error_number = g_auto_brake_errno;
    if (transfer_result)
        *transfer_result = g_auto_brake_transfer_result;
    if (raw_status)
        *raw_status = g_auto_brake_raw_status;
}

/* 寄存器访问：公开的无地址可靠读取入口，同样要求先初始化设备。 */
int fspi_read_direct(uint8_t *data)
{
    if (g_spi_fd < 0)
    {
        return -1;
    }

    return spi_read_direct_reliable(g_spi_fd, data);
}

/* 寄存器访问：按方向 0..3 写相应寄存器；运行时写零，停止时写一，不做读回验证。 */
int vehicle_control(int command, int enable)
{
    if (g_spi_fd < 0)
    {
        return -1;
    }

    if (command < 0 || command > 3)
    {
        printf("错误：车辆控制命令必须在0-3范围内，当前值: %d\n", command);
        printf("0=前进, 1=后退, 2=左转, 3=右转\n");
        return -1;
    }

    if (enable != 0 && enable != 1)
    {
        printf("错误：enable值必须是0或1，当前值: %d\n", enable);
        return -1;
    }

    uint16_t addr = (uint16_t)command;
    uint8_t control_value = (uint8_t)(enable ? 0 : 1);

    const char *command_names[] = {"前进", "后退", "左转", "右转"};
    const char *enable_text = enable ? "运动" : "停止";

    printf("正在%s %s...\n", enable_text, command_names[command]);
    printf("调试信息: 命令=%d, enable=%d, 地址=0x%04X, 发送值=%d\n", command, enable, addr,
           control_value);

    int write_ret = fspi_write_data(addr, control_value);

    if (write_ret == 0)
    {
        printf("车辆控制成功: %s %s (地址=0x%04X, 值=%d)\n", command_names[command], enable_text,
               addr, control_value);
        return 0;
    }
    else
    {
        printf("车辆控制失败: %s %s (写入失败)\n", command_names[command], enable_text);
        return -1;
    }
}

/* 寄存器访问：四个方向包装函数仅传入固定命令编号。 */
int vehicle_forward(int enable)
{
    return vehicle_control(0, enable);
}

int vehicle_backward(int enable)
{
    return vehicle_control(1, enable);
}

int vehicle_turn_left(int enable)
{
    return vehicle_control(2, enable);
}

int vehicle_turn_right(int enable)
{
    return vehicle_control(3, enable);
}

/* 寄存器访问：stop 按键低电平有效；向地址 4 写零并保持，直到明确写一解除。 */
int vehicle_emergency_stop(void)
{
    if (g_spi_fd < 0)
    {
        return -1;
    }

    uint16_t addr = 4;
    uint8_t stop_value = 0;

    printf("正在执行立即主动停止...\n");

    int write_ret = fspi_write_data(addr, stop_value);

    if (write_ret == 0)
    {
        printf("立即主动停止成功 (地址=0x%04X, 值=%d)\n", addr, stop_value);
        return 0;
    }
    else
    {
        printf("立即主动停止失败 (写入失败)\n");
        return -1;
    }
}

/* 寄存器访问：关闭已打开的 SPI 句柄并重置全局状态。 */
void fspi_cleanup(void)
{
    if (g_spi_fd >= 0)
    {
        close(g_spi_fd);
        g_spi_fd = -1;
        printf("FSPI resources cleaned up\n");
    }
    if (g_spi_owner_lock >= 0)
    {
        close(g_spi_owner_lock);
        g_spi_owner_lock = -1;
    }
}
