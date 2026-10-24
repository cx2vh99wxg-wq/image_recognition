#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "shared_memory.h"

//=========================================================================
// *作   者：科研轮转（UDP模块）
// *完成时间：2025年10月17日
// *实现功能：UDP网络传输模块，负责将图像数据发送到S端
// *传输格式：支持同步的帧传输，包含帧头、行头、行数据、帧尾
//=========================================================================

#define IMAGE_WIDTH  640
#define IMAGE_HEIGHT 480
#define UDP_PORT 8888
#define TARGET_IP "192.168.100.20"  // S端接收IP地址
#define MAX_PACKET_SIZE 1400

//=========================================================================
// 帧同步和行同步结构定义 - 640*480专用
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

typedef struct {
    int socket_fd;
    struct sockaddr_in target_addr;
} NetworkManager;

//=========================================================================
// 函数声明
//=========================================================================
int network_manager_init(NetworkManager* net_mgr, const char* target_ip);
int network_send_frame_with_sync(NetworkManager* net_mgr, const uint8_t* frame_data, uint32_t frame_id,
                                 uint32_t curve_type, uint32_t curve_confidence, uint32_t curve_angle);
void network_manager_cleanup(NetworkManager* net_mgr);
uint32_t calculate_checksum(const uint8_t* data, size_t size);

//=========================================================================
// 网络传输函数实现
//=========================================================================

/**************************************************************************
** 函数名称:    network_manager_init - 初始化UDP网络传输
****************************************************************************/
int network_manager_init(NetworkManager* net_mgr, const char* target_ip) {
    if (!net_mgr || !target_ip) {
        printf("网络管理器参数无效\n");
        return -1;
    }
    
    printf("正在初始化UDP网络传输...\n");
    
    // 创建UDP socket
    net_mgr->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (net_mgr->socket_fd < 0) {
        perror("UDP Socket创建失败");
        return -1;
    }
    
    // 设置socket选项
    int broadcast = 1;
    if (setsockopt(net_mgr->socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
        perror("设置socket广播选项失败");
        close(net_mgr->socket_fd);
        return -1;
    }
    
    // 设置发送缓冲区大小
    int send_buf_size = 2 * 1024 * 1024; // 2MB
    if (setsockopt(net_mgr->socket_fd, SOL_SOCKET, SO_SNDBUF, &send_buf_size, sizeof(send_buf_size)) < 0) {
        perror("设置发送缓冲区大小失败");
    }
    
    // 配置目标地址
    memset(&net_mgr->target_addr, 0, sizeof(net_mgr->target_addr));
    net_mgr->target_addr.sin_family = AF_INET;
    net_mgr->target_addr.sin_port = htons(UDP_PORT);
    
    if (inet_pton(AF_INET, target_ip, &net_mgr->target_addr.sin_addr) <= 0) {
        printf("目标IP地址格式无效: %s\n", target_ip);
        close(net_mgr->socket_fd);
        return -1;
    }
    
    printf("UDP网络传输初始化成功\n");
    printf("目标地址: %s:%d\n", target_ip, UDP_PORT);
    printf("发送缓冲区: %d字节\n", send_buf_size);
    
    return 0;
}

/**************************************************************************
** 函数名称:    calculate_checksum - 计算简单校验和
****************************************************************************/
uint32_t calculate_checksum(const uint8_t* data, size_t size) {
    uint32_t checksum = 0;
    for (size_t i = 0; i < size; i++) {
        checksum += data[i];
    }
    return checksum;
}

/**************************************************************************
** 函数名称:    network_send_frame_with_sync - 发送带同步的640*480帧
****************************************************************************/
int network_send_frame_with_sync(NetworkManager* net_mgr, const uint8_t* frame_data, uint32_t frame_id,
                                 uint32_t curve_type, uint32_t curve_confidence, uint32_t curve_angle) {
    if (!net_mgr || !frame_data) return -1;

    // PCIe共享内存已经是RGB565，直接传输原始像素，不再重复转换颜色。
    const uint32_t frame_checksum = calculate_checksum(frame_data, PCIE_FRAME_SIZE);
    FrameHeader header = {};
    header.magic = FRAME_HEADER_MAGIC;
    header.frame_id = frame_id;
    header.width = IMAGE_WIDTH;
    header.height = IMAGE_HEIGHT;
    header.data_size = PCIE_FRAME_SIZE;
    header.line_count = IMAGE_HEIGHT;
    header.checksum = frame_checksum;
    header.timestamp = (uint32_t)time(NULL);
    header.curve_type = curve_type;
    header.curve_confidence = curve_confidence;
    header.curve_angle = curve_angle;

    const struct sockaddr* target = (const struct sockaddr*)&net_mgr->target_addr;
    const socklen_t target_len = sizeof(net_mgr->target_addr);
    if (sendto(net_mgr->socket_fd, &header, sizeof(header), 0,
               target, target_len) != (ssize_t)sizeof(header)) {
        perror("帧头发送失败");
        return -1;
    }

    for (uint16_t line = 0; line < IMAGE_HEIGHT; ++line) {
        const uint8_t* line_data = frame_data + (size_t)line * IMAGE_WIDTH * 2;
        LineHeader line_header = {};
        line_header.magic = LINE_HEADER_MAGIC;
        line_header.line_id = line;
        line_header.line_size = IMAGE_WIDTH * 2;
        line_header.line_checksum = calculate_checksum(line_data, line_header.line_size);

        if (sendto(net_mgr->socket_fd, &line_header, sizeof(line_header), 0,
                   target, target_len) != (ssize_t)sizeof(line_header) ||
            sendto(net_mgr->socket_fd, line_data, line_header.line_size, 0,
                   target, target_len) != (ssize_t)line_header.line_size) {
            perror("图像行发送失败");
            return -1;
        }
        // 每帧约615KB，在100Mb/s链路上适当限流，避免队列溢出。
        usleep(100);
    }

    FrameTrailer trailer = {};
    trailer.magic = FRAME_TRAILER_MAGIC;
    trailer.frame_id = frame_id;
    trailer.checksum = frame_checksum;
    trailer.total_lines = IMAGE_HEIGHT;
    if (sendto(net_mgr->socket_fd, &trailer, sizeof(trailer), 0,
               target, target_len) != (ssize_t)sizeof(trailer)) {
        perror("帧尾发送失败");
        return -1;
    }
    return PCIE_FRAME_SIZE;
}

/**************************************************************************
** 函数名称:    network_manager_cleanup - 清理网络资源
****************************************************************************/
void network_manager_cleanup(NetworkManager* net_mgr) {
    if (net_mgr && net_mgr->socket_fd >= 0) {
        close(net_mgr->socket_fd);
        net_mgr->socket_fd = -1;
        printf("UDP网络连接已关闭\n");
    }
}

//=========================================================================
// 主函数 - UDP传输测试
//=========================================================================

int main(int argc, char* argv[]) {
    NetworkManager network_manager;
    SharedMemoryManager pcie_shm_manager;
    const char* target_ip = TARGET_IP;

    printf("\n==============================\n");
    printf("UDP网络传输模块\n");
    printf("==============================\n");
    printf("编译时间: %s %s\n", __DATE__, __TIME__);
    printf("功能: 从PCIe共享内存读取图像并发送到S端\n");
    printf("==============================\n\n");

    // 如果提供了命令行参数，使用它作为目标IP
    if (argc > 1) {
        target_ip = argv[1];
    }

    // 初始化PCIe共享内存（作为读取者）
    printf("=== 初始化PCIe共享内存 ===\n");
    if (shared_memory_init(&pcie_shm_manager, SHM_TYPE_PCIE, 0) != 0) {
        printf("PCIe共享内存初始化失败！\n");
        return -1;
    }
    printf("PCIe共享内存连接成功\n");

    // 初始化弯道检测共享内存（作为读取者）- 可选功能
    printf("\n=== 初始化弯道检测共享内存 ===\n");
    int curve_detection_available = 0;  // 弯道检测是否可用
    if (curve_detection_shm_init(0) != 0) {
        printf("警告: 弯道检测共享内存初始化失败，将不发送弯道信息\n");
        printf("提示: 确保PCIe程序正在运行并已创建弯道检测共享内存\n");
        curve_detection_available = 0;
    } else {
        printf("弯道检测共享内存连接成功\n");
        curve_detection_available = 1;
    }

    // 初始化网络管理器
    printf("\n=== 初始化网络传输 ===\n");
    if (network_manager_init(&network_manager, target_ip) != 0) {
        printf("网络传输初始化失败！\n");
        if (curve_detection_available) {
            curve_detection_shm_cleanup();
        }
        shared_memory_cleanup(&pcie_shm_manager);
        return -1;
    }
    printf("网络传输初始化成功\n");
    
    // 分配图像缓冲区
    uint8_t* image_buffer = (uint8_t*)malloc(PCIE_FRAME_SIZE);
    if (!image_buffer) {
        printf("图像缓冲区分配失败！\n");
        network_manager_cleanup(&network_manager);
        if (curve_detection_available) {
            curve_detection_shm_cleanup();
        }
        shared_memory_cleanup(&pcie_shm_manager);
        return -1;
    }
    
    printf("\n=== 开始UDP传输 ===\n");
    printf("图像尺寸: %dx%d\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    printf("目标地址: %s:%d\n", target_ip, UDP_PORT);
    printf("数据源: PCIe共享内存\n");
    printf("按Ctrl+C停止传输\n\n");
    
    // 主传输循环
    uint32_t frame_id = 0;
    uint32_t last_frame_id = 0;
    uint32_t width, height;
    int no_new_frame_count = 0;
    int read_error_count = 0;
    int consecutive_errors = 0;

    // 弯道检测变量
    uint32_t curve_frame_id = 0;
    CurveType curve_type = CURVE_NONE;
    uint32_t curve_confidence = 0;
    uint32_t curve_angle = 0;

    while (1) {
        // 从PCIe共享内存读取图像帧
        int read_result = pcie_shared_memory_read_frame(&pcie_shm_manager,
                                                       image_buffer,
                                                       &width, &height,
                                                       &frame_id);

        if (read_result == 0 && frame_id != last_frame_id) {
            // 成功读取到新帧
            no_new_frame_count = 0;
            consecutive_errors = 0;  // 重置连续错误计数
            last_frame_id = frame_id;

            // 从共享内存读取弯道检测结果
            if (curve_detection_available) {
                int curve_read_result = curve_detection_read(&curve_frame_id, &curve_type,
                                                             &curve_confidence, &curve_angle);
                if (curve_read_result != 0) {
                    // 如果读取失败，使用默认值
                    curve_type = CURVE_NONE;
                    curve_confidence = 0;
                    curve_angle = 0;
                }
            } else {
                // 弯道检测不可用，使用默认值
                curve_type = CURVE_NONE;
                curve_confidence = 0;
                curve_angle = 0;
            }

            // 发送图像帧(包含弯道检测信息)
            if (width != IMAGE_WIDTH || height != IMAGE_HEIGHT) {
                fprintf(stderr, "跳过尺寸不匹配的帧: %ux%u\n", width, height);
                continue;
            }
            int send_result = network_send_frame_with_sync(&network_manager, image_buffer, frame_id,
                                                          (uint32_t)curve_type, curve_confidence, curve_angle);

            if (send_result > 0) {
                // 每1000帧打印一次发送成功信息，避免刷屏
                if (frame_id % 1000 == 0) {
                    const char* curve_name = "未知";
                    switch(curve_type) {
                        case CURVE_NONE: curve_name = "直道"; break;
                        case CURVE_LEFT: curve_name = "左弯"; break;
                        case CURVE_RIGHT: curve_name = "右弯"; break;
                        case CURVE_BOTH: curve_name = "左右弯"; break;
                    }
                    printf("帧%d发送成功: %d字节 (尺寸: %dx%d, 弯道: %s, 置信度: %u%%)\n",
                           frame_id, send_result, width, height, curve_name, curve_confidence);
                }
            } else {
                printf("帧%d发送失败\n", frame_id);
            }

            // 每100帧输出一次统计
            if (frame_id % 100 == 0) {
                printf("累计发送: %d帧, 读取错误: %d次\n", frame_id, read_error_count);
            }
            
        } else if (read_result == 1) {
            // 没有新帧，稍微等待
            no_new_frame_count++;
            consecutive_errors = 0;  // 重置连续错误计数
            if (no_new_frame_count % 1000 == 0) {
                printf("等待新帧中... (计数: %d)\n", no_new_frame_count);
            }
            usleep(1000); // 1ms延时
        } else {
            // 读取失败
            read_error_count++;
            consecutive_errors++;
            
            // 只在连续错误较少时输出详细信息
            if (consecutive_errors <= 3) {
                printf("从共享内存读取帧失败 (错误计数: %d)\n", read_error_count);
            } else if (consecutive_errors % 100 == 0) {
                printf("持续读取失败中... (连续错误: %d次)\n", consecutive_errors);
            }
            
            // 根据连续错误次数调整延时
            if (consecutive_errors < 10) {
                usleep(1000);   // 1ms延时
            } else if (consecutive_errors < 100) {
                usleep(5000);   // 5ms延时
            } else {
                usleep(10000);  // 10ms延时
            }
        }
    }
    
    // 清理资源
    printf("\n=== 清理资源 ===\n");
    free(image_buffer);
    network_manager_cleanup(&network_manager);
    if (curve_detection_available) {
        curve_detection_shm_cleanup();
    }
    shared_memory_cleanup(&pcie_shm_manager);

    printf("\nUDP传输结束\n");
    return 0;
}