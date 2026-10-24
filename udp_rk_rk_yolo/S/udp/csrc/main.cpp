#include "shared_memory.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

/* 为了使用aligned_alloc，需要定义_ISOC11_SOURCE */
#ifndef _ISOC11_SOURCE
#define _ISOC11_SOURCE
#endif

//=========================================================================
// *作   者：科研轮转（C语言重构版：UDP网络接收后台程序）
// *完成时间：2025年10月14日
// *实现功能：480p视频流UDP行同步接收并写入共享内存（网络接收端）
// *性能优化：去除X11显示功能，专注于网络接收和共享内存写入
//=========================================================================

//=========================================================================
// UDP网络接收配置
//=========================================================================
#define UDP_PORT 8888
#define MAX_PACKET_SIZE 1400
#define FRAME_BUFFER_SIZE (IMAGE_WIDTH * IMAGE_HEIGHT * 2)  // RGB565格式
#define RECEIVE_TIMEOUT_SEC 5

#define IMAGE_WIDTH  640
#define IMAGE_HEIGHT 480
#define LEADING_PIXELS 120   /* 每行前导像素数量 */
#define EXTRA_SKIP_PIXELS 0  /* 额外需要跳过的前导像素（微调） */

/* 列重排配置：将第n列到第m列移动到行末 */
#define COLUMN_REARRANGE_START 0   /* 起始列n (0-based索引，0表示第一列) */
#define COLUMN_REARRANGE_END   0   /* 结束列m (0-based索引，包含此列) */
#define COLUMN_SHIFT_UP_ROWS   0   /* 移动到末尾的列向上移动k行 (0表示不移动) */

/* 控制是否显示前导像素 */
#define SHOW_LEADING_PIXELS 0  /* 0=不显示前导像素(默认), 1=显示前导像素 */

#if SHOW_LEADING_PIXELS
    #define DISPLAY_WIDTH  IMAGE_WIDTH   /* 显示前导像素时，保持显示宽度为640 */
    #define ACTUAL_WIDTH   IMAGE_WIDTH   /* 实际复制的像素宽度 */
    #define SKIP_PIXELS    0             /* 从前导像素开始 */
#else
    #define DISPLAY_WIDTH  IMAGE_WIDTH   /* 不显示前导像素，显示宽度为640 */
    #define ACTUAL_WIDTH   IMAGE_WIDTH   /* 实际复制的像素宽度 */
    #define SKIP_PIXELS    (LEADING_PIXELS + EXTRA_SKIP_PIXELS) /* 跳过前导像素+额外偏移 */
#endif

#define DISPLAY_HEIGHT IMAGE_HEIGHT  /* 直接使用原始图像尺寸 */

//=========================================================================
// 帧同步和行同步结构定义 - 增强版
//=========================================================================
#define FRAME_HEADER_MAGIC 0xFF00FF00  /* 帧头魔数 */
#define FRAME_TRAILER_MAGIC 0x00FF00FF /* 帧尾魔数 */
#define LINE_HEADER_MAGIC 0xAA55AA55   /* 行头魔数 */
#define SYNC_TIMEOUT_MS 1000           /* 同步超时(毫秒) - 改为1秒 */
#define MAX_RESYNC_ATTEMPTS 10         /* 最大重同步次数 */
#define LINE_DATA_MAX_SIZE (IMAGE_WIDTH * 2 + 100) /* 行数据最大大小 */

typedef struct {
    uint32_t magic;        /* 魔数: FRAME_HEADER_MAGIC */
    uint32_t frame_id;     /* 帧ID */
    uint32_t width;        /* 图像宽度 */
    uint32_t height;       /* 图像高度 */
    uint32_t data_size;    /* 帧数据大小 */
    uint32_t line_count;   /* 行数 */
    uint32_t checksum;     /* 帧校验和 */
    uint32_t timestamp;    /* 时间戳 */
    uint32_t curve_type;   /* 弯道类型: 0=无, 1=左弯, 2=右弯, 3=左右弯 */
    uint32_t curve_confidence; /* 弯道检测置信度 (0-100) */
    uint32_t curve_angle;  /* 弯道角度 (度数) */
    uint32_t reserved;     /* 预留字段 */
} FrameHeader;

typedef struct {
    uint32_t magic;        /* 魔数: FRAME_TRAILER_MAGIC */
    uint32_t frame_id;     /* 帧ID */
    uint32_t checksum;     /* 帧校验和 */
    uint32_t total_lines;  /* 总行数 */
} FrameTrailer;

typedef struct {
    uint32_t magic;        /* 魔数: LINE_HEADER_MAGIC */
    uint16_t line_id;      /* 行号 */
    uint16_t line_size;    /* 行数据大小 */
    uint32_t line_checksum;/* 行校验和 */
} LineHeader;

//=========================================================================
// 同步状态枚举
//=========================================================================
typedef enum {
    SYNC_STATE_IDLE,           /* 空闲状态 */
    SYNC_STATE_WAIT_FRAME,     /* 等待帧头 */
    SYNC_STATE_RECV_LINES,     /* 接收行数据 */
    SYNC_STATE_WAIT_TRAILER,   /* 等待帧尾 */
    SYNC_STATE_ERROR           /* 错误状态 */
} SyncState;

//=========================================================================
// 同步控制结构
//=========================================================================
typedef struct {
    SyncState state;
    uint32_t current_frame_id;
    uint16_t expected_line_id;
    uint32_t received_lines;
    uint32_t frame_checksum;
    struct timeval sync_start_time;
    int resync_count;
    uint32_t lost_frames;
    uint32_t corrupted_frames;
    uint32_t recovered_frames;
    uint32_t total_frames;
} SyncController;

//=========================================================================
// 网络接收器结构体
//=========================================================================
typedef struct {
    int socket_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    uint8_t* frame_buffer;
    size_t buffer_size;
    SyncController sync_ctrl;   /* 同步控制器 */
} NetworkReceiver;

//=========================================================================
// 全局变量
//=========================================================================
static volatile int keep_running = 1;
static SharedMemoryManager shm_manager;
static int curve_detection_available = 0;

//=========================================================================
// 函数声明
//=========================================================================
// 网络接收函数
int network_receiver_init(NetworkReceiver* receiver);
int network_receive_with_sync(NetworkReceiver* receiver);
void network_receiver_cleanup(NetworkReceiver* receiver);

// 同步相关函数
uint32_t calculate_checksum(const uint8_t* data, size_t size);
void reset_sync_state(NetworkReceiver* receiver);
int set_socket_timeout(int sockfd, int timeout_ms);
void print_sync_statistics(const NetworkReceiver* receiver);

// 信号处理函数
void signal_handler(int sig);

// 工具函数
double get_time_diff(struct timeval start, struct timeval end);
void save_frame_to_ppm(const uint8_t* rgb565_data, const char* filename);
void rgb565_to_rgb888(const uint16_t* rgb565_data, uint8_t* rgb888_data, size_t pixel_count);

//=========================================================================
// 信号处理函数
//=========================================================================
void signal_handler(int sig) {
    keep_running = 0;
}

//=========================================================================
// 工具函数实现
//=========================================================================
double get_time_diff(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
}

uint32_t calculate_checksum(const uint8_t* data, size_t size) {
    uint32_t checksum = 0;
    for (size_t i = 0; i < size; ++i) {
        checksum += data[i];
    }
    return checksum;
}

void rgb565_to_rgb888(const uint16_t* rgb565_data, uint8_t* rgb888_data, size_t pixel_count) {
    for (size_t i = 0; i < pixel_count; i++) {
        uint16_t pixel = rgb565_data[i];
        
        // 提取RGB分量 (大端格式：RRRRRGGGGGGBBBBB)
        uint8_t r = (pixel >> 11) & 0x1F;
        uint8_t g = (pixel >> 5) & 0x3F;
        uint8_t b = pixel & 0x1F;
        
        // 扩展到8位
        rgb888_data[i * 3 + 0] = (r << 3) | (r >> 2);  // R
        rgb888_data[i * 3 + 1] = (g << 2) | (g >> 4);  // G
        rgb888_data[i * 3 + 2] = (b << 3) | (b >> 2);  // B
    }
}

void save_frame_to_ppm(const uint8_t* rgb565_data, const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        perror("无法创建PPM文件");
        return;
    }
    
    // 转换为RGB888格式
    uint8_t* rgb888_buffer = (uint8_t*)malloc(IMAGE_WIDTH * IMAGE_HEIGHT * 3);
    if (!rgb888_buffer) {
        fclose(fp);
        return;
    }
    
    rgb565_to_rgb888((const uint16_t*)rgb565_data, rgb888_buffer, IMAGE_WIDTH * IMAGE_HEIGHT);
    
    // 写入PPM头
    fprintf(fp, "P6\n%d %d\n255\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    
    // 写入像素数据
    fwrite(rgb888_buffer, 3, IMAGE_WIDTH * IMAGE_HEIGHT, fp);
    
    fclose(fp);
    free(rgb888_buffer);
}

int set_socket_timeout(int sockfd, int timeout_ms) {
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("设置socket超时失败");
        return -1;
    }
    
    return 0;
}

//=========================================================================
// 网络接收器函数实现
//=========================================================================
int network_receiver_init(NetworkReceiver* receiver) {
    if (!receiver) {
        return -1;
    }
    
    memset(receiver, 0, sizeof(NetworkReceiver));
    receiver->socket_fd = -1;
    
    // 分配缓冲区
    receiver->buffer_size = FRAME_BUFFER_SIZE;
    receiver->frame_buffer = (uint8_t*)malloc(receiver->buffer_size);
    
    if (!receiver->frame_buffer) {
        network_receiver_cleanup(receiver);
        return -1;
    }
    
    memset(receiver->frame_buffer, 0, receiver->buffer_size);
    
    // 初始化同步控制器
    reset_sync_state(receiver);
    
    // 创建UDP socket
    receiver->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (receiver->socket_fd < 0) {
        perror("UDP Socket创建失败");
        network_receiver_cleanup(receiver);
        return -1;
    }
    
    // 设置socket选项
    int reuse = 1;
    if (setsockopt(receiver->socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("设置SO_REUSEADDR失败");
    }
    
    // 设置接收缓冲区大小
    int recv_buf_size = 8 * 1024 * 1024;  // 8MB
    if (setsockopt(receiver->socket_fd, SOL_SOCKET, SO_RCVBUF, &recv_buf_size, sizeof(recv_buf_size)) < 0) {
        perror("设置接收缓冲区大小失败");
    }
    
    // 使用阻塞接收与超时，等待每个数据报到达。
    // 绑定到端口
    receiver->server_addr.sin_family = AF_INET;
    receiver->server_addr.sin_addr.s_addr = INADDR_ANY;
    receiver->server_addr.sin_port = htons(UDP_PORT);
    
    if (bind(receiver->socket_fd, (struct sockaddr*)&receiver->server_addr, sizeof(receiver->server_addr)) < 0) {
        perror("UDP绑定失败");
        network_receiver_cleanup(receiver);
        return -1;
    }
    
    // 设置超时
    if (set_socket_timeout(receiver->socket_fd, SYNC_TIMEOUT_MS) != 0) {
        // 非致命错误，继续执行
    }
    
    receiver->client_len = sizeof(receiver->client_addr);
    
    return 0;
}

void reset_sync_state(NetworkReceiver* receiver) {
    if (!receiver) {
        return;
    }
    
    receiver->sync_ctrl.state = SYNC_STATE_WAIT_FRAME;
    receiver->sync_ctrl.current_frame_id = 0;
    receiver->sync_ctrl.expected_line_id = 0;
    receiver->sync_ctrl.received_lines = 0;
    receiver->sync_ctrl.frame_checksum = 0;
    receiver->sync_ctrl.resync_count = 0;
    
    gettimeofday(&receiver->sync_ctrl.sync_start_time, NULL);
}

void print_sync_statistics(const NetworkReceiver* receiver) {
    if (!receiver) return;
    
    // 统计信息已移除，减少输出
}

//=========================================================================
// 按数据报接收完整帧，并在包丢失时重新同步到下一帧头
//=========================================================================
// 读取一个UDP数据报。超时返回0，让主循环及时处理停止信号。
static ssize_t receive_packet(NetworkReceiver* receiver, uint8_t* packet, size_t capacity) {
    while (keep_running) {
        receiver->client_len = sizeof(receiver->client_addr);
        ssize_t size = recvfrom(receiver->socket_fd, packet, capacity, 0,
                                (struct sockaddr*)&receiver->client_addr, &receiver->client_len);
        if (size >= 0) return size;
        if (errno == EINTR) continue;
        if (errno == EAGAIN || errno == EWOULDBLOCK) return 0;
        perror("UDP接收失败");
        return -1;
    }
    return 0;
}

int network_receive_with_sync(NetworkReceiver* receiver) {
    if (!receiver || !receiver->frame_buffer) return -1;

    uint8_t packet[MAX_PACKET_SIZE];
    FrameHeader header;
    // 启动在半帧或发生丢包时，扫描到下一个有效帧头。
    while (keep_running) {
        ssize_t size = receive_packet(receiver, packet, sizeof(packet));
        if (size <= 0) return (int)size;
        if (size != (ssize_t)sizeof(header)) continue;
        memcpy(&header, packet, sizeof(header));
        if (header.magic == FRAME_HEADER_MAGIC &&
            header.width == IMAGE_WIDTH && header.height == IMAGE_HEIGHT &&
            header.data_size == FRAME_BUFFER_SIZE && header.line_count == IMAGE_HEIGHT) {
            break;
        }
    }
    if (!keep_running) return 0;

    const struct sockaddr_in source = receiver->client_addr;
    uint32_t checksum = 0;
    for (uint16_t line = 0; line < IMAGE_HEIGHT; ++line) {
        ssize_t size = receive_packet(receiver, packet, sizeof(packet));
        LineHeader line_header;
        if (size != (ssize_t)sizeof(line_header) ||
            receiver->client_addr.sin_addr.s_addr != source.sin_addr.s_addr ||
            receiver->client_addr.sin_port != source.sin_port) {
            receiver->sync_ctrl.corrupted_frames++;
            return 0;
        }
        memcpy(&line_header, packet, sizeof(line_header));
        if (line_header.magic != LINE_HEADER_MAGIC ||
            line_header.line_id != line || line_header.line_size != IMAGE_WIDTH * 2) {
            receiver->sync_ctrl.corrupted_frames++;
            return 0;
        }

        size = receive_packet(receiver, packet, sizeof(packet));
        if (size != line_header.line_size ||
            receiver->client_addr.sin_addr.s_addr != source.sin_addr.s_addr ||
            receiver->client_addr.sin_port != source.sin_port ||
            calculate_checksum(packet, line_header.line_size) != line_header.line_checksum) {
            receiver->sync_ctrl.corrupted_frames++;
            return 0;
        }
        memcpy(receiver->frame_buffer + (size_t)line * IMAGE_WIDTH * 2,
               packet, line_header.line_size);
        checksum += line_header.line_checksum;
    }

    ssize_t size = receive_packet(receiver, packet, sizeof(packet));
    FrameTrailer trailer;
    if (size != (ssize_t)sizeof(trailer) ||
        receiver->client_addr.sin_addr.s_addr != source.sin_addr.s_addr ||
        receiver->client_addr.sin_port != source.sin_port) {
        receiver->sync_ctrl.corrupted_frames++;
        return 0;
    }
    memcpy(&trailer, packet, sizeof(trailer));
    // 旧版M端没有初始化帧头校验字段；帧尾与逐行校验足以验证完整数据。
    if (trailer.magic != FRAME_TRAILER_MAGIC ||
        trailer.frame_id != header.frame_id ||
        trailer.total_lines != IMAGE_HEIGHT ||
        trailer.checksum != checksum) {
        receiver->sync_ctrl.corrupted_frames++;
        return 0;
    }

    receiver->sync_ctrl.total_frames++;
    if (curve_detection_available &&
        curve_detection_update(header.frame_id, (CurveType)header.curve_type,
                               header.curve_confidence, header.curve_angle) != 0) {
        fprintf(stderr, "警告: 弯道信息写入共享内存失败\n");
    }
    if (receiver->sync_ctrl.total_frames % 30 == 1) {
        fprintf(stderr, "UDP帧%u接收完成，累计%u帧，丢弃%u帧\n",
                header.frame_id, receiver->sync_ctrl.total_frames,
                receiver->sync_ctrl.corrupted_frames);
    }
    return FRAME_BUFFER_SIZE;
}

void network_receiver_cleanup(NetworkReceiver* receiver) {
    if (!receiver) {
        return;
    }
    
    if (receiver->socket_fd >= 0) {
        close(receiver->socket_fd);
        receiver->socket_fd = -1;
    }
    
    if (receiver->frame_buffer) {
        free(receiver->frame_buffer);
        receiver->frame_buffer = NULL;
    }
    

}

//=========================================================================
// 主函数
//=========================================================================
int main() {
    NetworkReceiver network_receiver;
    struct timeval stream_start, current_time;
    int frame_count = 0;
    double total_time = 0.0;
    
    // 设置信号处理
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // 初始化UDP共享内存
    if (shared_memory_init_udp(&shm_manager, 1) != 0) {  // 1表示写入者
        return -1;
    }

    // 初始化弯道检测共享内存（S端UDP作为写入者）
    if (curve_detection_shm_init(1) == 0) {
        curve_detection_available = 1;
        printf("弯道检测共享内存初始化成功\n");
    } else {
        fprintf(stderr, "弯道检测共享内存不可用，继续接收图像\n");
    }

    // 初始化网络接收器
    if (network_receiver_init(&network_receiver) != 0) {
        if (curve_detection_available) curve_detection_shm_cleanup();
        shared_memory_cleanup(&shm_manager);
        return -1;
    }
    
    gettimeofday(&stream_start, NULL);
    
    // 主接收循环
    while (keep_running) {
        int received_size = network_receive_with_sync(&network_receiver);
        
        if (received_size <= 0) {
            // 接收失败但不是严重错误，直接继续
            continue;
        }
        
        // 写入共享内存 (直接写入RGB565数据)
        if (shared_memory_write_image(&shm_manager,
                                     network_receiver.frame_buffer,
                                     IMAGE_WIDTH, IMAGE_HEIGHT) != 0) {
            printf("UDP共享内存写入失败\n");
            break;
        }
        
        // 添加调试信息：确认写入的是UDP共享内存
        static int udp_debug_count = 0;
        if (++udp_debug_count % 100 == 0) {
            printf("UDP程序写入帧%d到共享内存类型:%d (应该是2)\n", udp_debug_count, shm_manager.type);
        }
        
        frame_count++;
        
        // 实时FPS计算和性能分析 - 已移除输出
    }
    
    // 性能统计
    gettimeofday(&current_time, NULL);
    total_time = get_time_diff(stream_start, current_time);
    
    print_sync_statistics(&network_receiver);
    
    // 保存最后一帧为PPM文件
    if (frame_count > 0) {
        save_frame_to_ppm(network_receiver.frame_buffer, "udp_last_frame.ppm");
    }
    
    // 系统清理
    network_receiver_cleanup(&network_receiver);
    shared_memory_cleanup(&shm_manager);
    if (curve_detection_available) curve_detection_shm_cleanup();


    return 0;
}