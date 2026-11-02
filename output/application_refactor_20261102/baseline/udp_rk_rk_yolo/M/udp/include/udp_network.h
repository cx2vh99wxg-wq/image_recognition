#ifndef UDP_NETWORK_H
#define UDP_NETWORK_H

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>

//=========================================================================
// UDP网络传输模块头文件
//=========================================================================

#define IMAGE_WIDTH  640
#define IMAGE_HEIGHT 480
#define UDP_PORT 8888
#define MAX_PACKET_SIZE 1400

//=========================================================================
// 帧同步和行同步结构定义
//=========================================================================
#define FRAME_HEADER_MAGIC 0xFF00FF00  /* 帧头魔数 */
#define FRAME_TRAILER_MAGIC 0x00FF00FF /* 帧尾魔数 */
#define LINE_HEADER_MAGIC 0xAA55AA55   /* 行头魔数 */

typedef struct {
    uint32_t magic;        /* 魔数: FRAME_HEADER_MAGIC */
    uint32_t frame_id;     /* 帧ID */
    uint32_t width;        /* 图像宽度 */
    uint32_t height;       /* 图像高度 */
    uint32_t data_size;    /* 帧数据大小 */
    uint32_t line_count;   /* 行数 */
    uint32_t checksum;     /* 帧校验和 */
    uint32_t timestamp;    /* 时间戳 */
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

typedef struct {
    int socket_fd;
    struct sockaddr_in target_addr;
} NetworkManager;

//=========================================================================
// 函数声明
//=========================================================================

/**
 * 初始化UDP网络传输
 * @param net_mgr 网络管理器
 * @param target_ip 目标IP地址
 * @return 0=成功, -1=失败
 */
int network_manager_init(NetworkManager* net_mgr, const char* target_ip);

/**
 * 发送带同步的帧数据
 * @param net_mgr 网络管理器
 * @param frame_data RGB888格式的图像数据
 * @param frame_id 帧ID
 * @return 发送的字节数, -1=失败
 */
int network_send_frame_with_sync(NetworkManager* net_mgr, const uint8_t* frame_data, uint32_t frame_id);

/**
 * 清理网络资源
 * @param net_mgr 网络管理器
 */
void network_manager_cleanup(NetworkManager* net_mgr);

/**
 * 计算简单校验和
 * @param data 数据指针
 * @param size 数据大小
 * @return 校验和
 */
uint32_t calculate_checksum(const uint8_t* data, size_t size);

/**
 * RGB888转RGB565格式
 * @param rgb888_data RGB888数据
 * @param rgb565_data RGB565数据输出
 * @param num_pixels 像素数量
 */
void rgb888_to_rgb565(const uint8_t* rgb888_data, uint16_t* rgb565_data, size_t num_pixels);

#endif // UDP_NETWORK_H