#include "pcie_dma_read_test.h"
#include "fspi_module.h"
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
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XShm.h>  /* MIT-SHM共享内存扩展 */
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
// *作   者：辉哥大盗（C语言重构版：更好的函数封装）
// *完成时间：2025年10月1日  
// *实现功能：480p视频流UDP接收与实时渲染（网络接收端）
// *性能优化：移除PCIE采集，专注网络接收和显示刷新
//=========================================================================

//=========================================================================
// UDP网络接收配置
//=========================================================================
#define UDP_PORT 8888
#define MAX_PACKET_SIZE 1400
#define UDP_FRAME_BUFFER_SIZE (UDP_IMAGE_WIDTH * UDP_IMAGE_HEIGHT * 2)  // RGB565格式
#define COMBINED_FRAME_BUFFER_SIZE (COMBINED_IMAGE_WIDTH * COMBINED_IMAGE_HEIGHT * 2)  // 拼接后的图像大小
#define RECEIVE_TIMEOUT_SEC 5

#define UDP_IMAGE_WIDTH  640
#define UDP_IMAGE_HEIGHT 480
#define PCIE_IMAGE_WIDTH  640
#define PCIE_IMAGE_HEIGHT 480

// 为了兼容性，添加通用的IMAGE_WIDTH和IMAGE_HEIGHT定义
#define IMAGE_WIDTH  UDP_IMAGE_WIDTH
#define IMAGE_HEIGHT UDP_IMAGE_HEIGHT
#define COMBINED_IMAGE_WIDTH  (UDP_IMAGE_WIDTH + PCIE_IMAGE_WIDTH)  // 1280
#define COMBINED_IMAGE_HEIGHT UDP_IMAGE_HEIGHT  // 480
#define LEADING_PIXELS 120   /* 每行前导像素数量 */
#define EXTRA_SKIP_PIXELS 0  /* 额外需要跳过的前导像素（微调） */

/* 列重排配置：将第n列到第m列移动到行末 */
#define COLUMN_REARRANGE_START 0   /* 起始列n (0-based索引，0表示第一列) */
#define COLUMN_REARRANGE_END   0   /* 结束列m (0-based索引，包含此列) */
#define COLUMN_SHIFT_UP_ROWS   0   /* 移动到末尾的列向上移动k行 (0表示不移动) */

/* 控制是否显示前导像素 */
#define SHOW_LEADING_PIXELS 0  /* 0=不显示前导像素(默认), 1=显示前导像素 */

#if SHOW_LEADING_PIXELS
    #define DISPLAY_WIDTH  COMBINED_IMAGE_WIDTH   /* 显示前导像素时，保持显示宽度为1280 */
    #define ACTUAL_WIDTH   UDP_IMAGE_WIDTH       /* 实际复制的像素宽度为640 */
    #define SKIP_PIXELS    0                     /* 从前导像素开始 */
#else
    #define DISPLAY_WIDTH  COMBINED_IMAGE_WIDTH   /* 不显示前导像素，显示宽度为1280 */
    #define ACTUAL_WIDTH   UDP_IMAGE_WIDTH       /* 实际复制的像素宽度为640 */
    #define SKIP_PIXELS    (LEADING_PIXELS + EXTRA_SKIP_PIXELS) /* 跳过前导像素+额外偏移 */
#endif

#define DISPLAY_HEIGHT COMBINED_IMAGE_HEIGHT  /* 直接使用原始图像尺寸480 */

//=========================================================================
// 删除了行同步传输相关的定义，只保留分包传输
//=========================================================================

//=========================================================================
// 分包传输结构定义 - 新增
//=========================================================================
#define PACKET_MAGIC 0xAF12CD34        /* 分包传输魔数 */
#define PACKET_MAX_DATA_SIZE (MAX_PACKET_SIZE - sizeof(PacketHeader)) /* 每包最大数据量 */

/* 包类型定义 */
#define PACKET_TYPE_START 0x01         /* 帧开始标记 */
#define PACKET_TYPE_DATA  0x02         /* 数据包 */
#define PACKET_TYPE_END   0x03         /* 帧结束标记 */

/* 分包传输的包头结构 */
typedef struct {
    uint32_t magic;           /* 包魔数: PACKET_MAGIC */
    uint32_t frame_id;        /* 帧ID */
    uint16_t packet_id;       /* 包序号 */
    uint16_t total_packets;   /* 总包数 */
    uint16_t data_size;       /* 本包数据大小 */
    uint8_t  packet_type;     /* 包类型 */
    uint8_t  reserved;        /* 保留字节(对齐) */
    uint32_t image_width;     /* 图像宽度 */
    uint32_t image_height;    /* 图像高度 */
    uint32_t checksum;        /* 本包校验和 */
} PacketHeader;

/* 分包重组器 */
typedef struct {
    uint8_t* frame_buffer;    /* 帧重组缓冲区 */
    uint8_t* received_mask;   /* 包接收位图 */
    uint32_t frame_id;        /* 当前重组的帧ID */
    uint16_t total_packets;   /* 总包数 */
    uint16_t received_packets; /* 已接收包数 */
    uint32_t image_width;     /* 图像宽度 */
    uint32_t image_height;    /* 图像高度 */
    struct timeval start_time; /* 开始重组的时间 */
    int is_valid;            /* 是否有效 */
} PacketAssembler;

// 删除了行同步传输相关的结构体定义

//=========================================================================
// PCIE管理结构体和渲染引擎结构体的定义
//=========================================================================
typedef struct {
    int pci_driver_fd;
    COMMAND_OPERATION command_operation;
    DMA_OPERATION dma_operation;
} PCIEManager;

typedef struct {
    Display *display;
    Window window;
    GC gc;
    XImage *ximage;
    char* display_buffer;
    Pixmap pixmap;           /* 双缓冲pixmap */
    int use_fast_mode;       /* 是否使用快速模式 */
    int use_shm;             /* 是否使用共享内存 */
    XShmSegmentInfo shminfo; /* 共享内存信息 */
    XFontStruct *font;       /* 字体结构 */
    int is_init;
} RenderEngine;

typedef struct {
    int socket_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    uint8_t* frame_buffer;
    uint8_t* temp_buffer;      /* 临时接收缓冲区 */
    size_t buffer_size;
    
    /* 分包传输支持 */
    PacketAssembler assembler; /* 分包重组器 */
    uint8_t packet_buffer[MAX_PACKET_SIZE]; /* 包接收缓冲区 */
} NetworkReceiver;

//=========================================================================
// 函数声明
//=========================================================================
// PCIE管理函数
int pcie_manager_init(PCIEManager* manager);
void pcie_manager_get_device_info(PCIEManager* manager);
int pcie_manager_capture_frame(PCIEManager* manager, uint8_t* image_buffer, size_t buffer_size);
int pcie_manager_init_streaming(PCIEManager* manager);
void pcie_manager_cleanup_streaming(PCIEManager* manager);
void pcie_manager_cleanup(PCIEManager* manager);

// 渲染引擎函数
int render_engine_init(RenderEngine* engine);
int render_engine_render_frame(RenderEngine* engine, const uint8_t* rgb888_data);
int render_engine_check_exit_event(RenderEngine* engine);
void render_engine_cleanup(RenderEngine* engine);

// 网络接收函数
int network_receiver_init(NetworkReceiver* receiver);
int network_receive_with_sync(NetworkReceiver* receiver);
void network_receiver_cleanup(NetworkReceiver* receiver);

// 分包传输函数 - 新增
uint32_t calculate_checksum(const uint8_t* data, size_t size);
int init_packet_assembler(PacketAssembler* assembler, uint32_t frame_id, 
                         uint16_t total_packets, uint32_t width, uint32_t height);
int add_packet_to_assembler(PacketAssembler* assembler, const PacketHeader* header,
                           const uint8_t* data);
int check_assembler_complete(PacketAssembler* assembler);
void cleanup_packet_assembler(PacketAssembler* assembler);
int get_complete_frame(PacketAssembler* assembler, uint8_t* output_buffer, size_t buffer_size);
// 网络设置函数
int set_socket_timeout(int sockfd, int timeout_ms);

// 工具函数
int nano_delay(long delay);
void combine_images(const uint8_t* pcie_image, const uint8_t* udp_image, 
                   uint8_t* combined_image, int width, int height);
void rgb565_to_rgb888(const uint16_t* image565, uint8_t* image888, size_t num_pixels);
void rgb565_to_bgra_direct(const uint16_t* image565, uint32_t* bgra_data, size_t num_pixels);
int save_frame_to_ppm(const uint8_t* rgb888_data, const char* filename);
double get_time_diff(struct timeval start, struct timeval end);

// 图像处理函数
void rearrange_columns(uint16_t* row_buffer, int width, int start_col, int end_col);
void shift_columns_up(uint8_t* image_buffer, int width, int height, int start_col, int end_col, int shift_rows);

//=========================================================================
// 全局变量定义
//=========================================================================
int current_video_stream = 0;  // 网络接收端不需要视频流切换，设为固定值
int has_received_udp_ever = 0;  // 标志是否曾经接收到过UDP数据

//=========================================================================
// PCIE管理函数实现 - PCIE设备管理
//=========================================================================

int pcie_manager_init(PCIEManager* manager) {
    printf("正在初始化PCIE设备...\n");
    
    if (!manager) {
        printf("PCIE管理器指针无效\n");
        return -1;
    }
    
    // 初始化结构体
    manager->pci_driver_fd = -1;
    memset(&manager->command_operation, 0, sizeof(COMMAND_OPERATION));
    memset(&manager->dma_operation, 0, sizeof(DMA_OPERATION));
    
    // 打开PCIE设备
    manager->pci_driver_fd = open(PCIE_DRIVER_FILE_PATH, O_RDWR);
    if (manager->pci_driver_fd < 0) {
        perror("PCIE设备打开失败");
        return -1;
    }
    
    // 获取设备信息
    pcie_manager_get_device_info(manager);
    
    printf("PCIE设备初始化成功\n");
    return 0;
}

void pcie_manager_get_device_info(PCIEManager* manager) {
    unsigned int cnt = 0;
    char pci_info[20][20];
    char unit[5][10] = {"B", "KB", "MB", "GB", "TB"};
    unsigned long temp_bar_len;
    int value = 0;
    int i;

    if (!manager || manager->pci_driver_fd < 0) {
        printf("PCIE设备未初始化\n");
        return;
    }

    manager->command_operation.delay = 0;
    read(manager->pci_driver_fd, &manager->command_operation, sizeof(COMMAND_OPERATION));
    
    sprintf(pci_info[cnt++], "%04x", manager->command_operation.get_pci_dev_info.vendor_id);
    sprintf(pci_info[cnt++], "%04x", manager->command_operation.get_pci_dev_info.device_id);

    if(manager->command_operation.cap_info.cap_status == 1) {
        printf("PCIe链路连接成功\n");
    } else {
        printf("PCIe链路连接失败 !!!\n");
    }
    
    sprintf(pci_info[cnt++], "Gen%x", manager->command_operation.get_pci_dev_info.link_speed);
    sprintf(pci_info[cnt++], "x%x", manager->command_operation.get_pci_dev_info.link_width);

    for(i = 0; i <= 5; i++) {
        sprintf(pci_info[cnt++], "%08lx", manager->command_operation.get_pci_dev_info.bar[i].bar_base);
    }
    for(i = 0; i <= 5; i++) {
        value = 0;
        temp_bar_len = manager->command_operation.get_pci_dev_info.bar[i].bar_len;
        while(temp_bar_len >= 1024) {
            temp_bar_len = temp_bar_len / 1024;
            value++;
        }
        sprintf(pci_info[cnt++], "%lu%s", temp_bar_len, unit[value]);
    }
    printf("MPS=%d, MRRS=%d\n",
           manager->command_operation.get_pci_dev_info.mps,
           manager->command_operation.get_pci_dev_info.mrrs);
    printf("串联加载基地址为 [ Bar0 ]\n");
}

int pcie_manager_capture_frame(PCIEManager* manager, uint8_t* image_buffer, size_t buffer_size) {
    int i, k;
    static uint32_t frame_start_offset = 0;  // 静态变量保存帧起始位置
    
    if (!manager || manager->pci_driver_fd < 0) {
        printf("PCIE设备未初始化\n");
        return -1;
    }
    
    if (buffer_size < (LEADING_PIXELS + IMAGE_WIDTH) * IMAGE_HEIGHT * 2) {
        printf("缓冲区大小不足\n");
        return -1;
    }
    
    /* 尝试查找帧开始标记（如果需要的话）- 这里可以根据实际硬件接口做调整 */
    manager->dma_operation.offset_addr = frame_start_offset;
    
    /* 逐行读取图像数据，从当前有效的帧起始地址开始 */
    for (i = 0; i < IMAGE_HEIGHT; i++) {
        /* 移除memset提高性能 - 数据会被覆盖 */
        
        /* 设置当前行的偏移地址，相对于帧起始位置 */
        manager->dma_operation.offset_addr = frame_start_offset + i * manager->dma_operation.current_len * 4;
        
        ioctl(manager->pci_driver_fd, PCI_DMA_WRITE_CMD, &manager->dma_operation);
        
        /* 完全移除延时 - ioctl会自动等待就绪 */
        
        ioctl(manager->pci_driver_fd, PCI_READ_FROM_KERNEL_CMD, &manager->dma_operation);
        
#if SHOW_LEADING_PIXELS
        /* 显示前导像素：从第0个像素开始拷贝ACTUAL_WIDTH个像素(ACTUAL_WIDTH*2字节)，截断后面的图像数据 */
        memcpy(image_buffer + i * ACTUAL_WIDTH * 2, manager->dma_operation.data.read_buf, ACTUAL_WIDTH * 2);
#else
        /* 不显示前导像素：跳过前SKIP_PIXELS个像素(SKIP_PIXELS*2字节)，从第SKIP_PIXELS个像素开始拷贝ACTUAL_WIDTH个像素(ACTUAL_WIDTH*2字节) */
        memcpy(image_buffer + i * ACTUAL_WIDTH * 2, manager->dma_operation.data.read_buf + SKIP_PIXELS * 2, ACTUAL_WIDTH * 2);
#endif
        
        /* 列重排：如果定义了有效的列范围，则进行列重排 */
        if (COLUMN_REARRANGE_START >= 0 && COLUMN_REARRANGE_END < ACTUAL_WIDTH && 
            COLUMN_REARRANGE_START <= COLUMN_REARRANGE_END && 
            !(COLUMN_REARRANGE_START == 0 && COLUMN_REARRANGE_END == 0)) {
            /* 对当前行进行列重排（image_buffer中的数据是RGB565格式，每个像素2字节） */
            rearrange_columns((uint16_t*)(image_buffer + i * ACTUAL_WIDTH * 2), 
                            ACTUAL_WIDTH, 
                            COLUMN_REARRANGE_START, 
                            COLUMN_REARRANGE_END);
        }
    }
    
    /* 在帧采集完成后，如果需要对移动到末尾的列进行垂直上移 */
    if (COLUMN_REARRANGE_START >= 0 && COLUMN_REARRANGE_END < ACTUAL_WIDTH && 
        COLUMN_REARRANGE_START <= COLUMN_REARRANGE_END && 
        !(COLUMN_REARRANGE_START == 0 && COLUMN_REARRANGE_END == 0) && 
        COLUMN_SHIFT_UP_ROWS > 0) {
        /* 计算移动到末尾的列的起始位置 */
        int moved_col_start = ACTUAL_WIDTH - (COLUMN_REARRANGE_END - COLUMN_REARRANGE_START + 1);
        int moved_col_end = ACTUAL_WIDTH - 1;
        
        /* 对移动到末尾的列进行垂直上移 */
        shift_columns_up(image_buffer, ACTUAL_WIDTH, IMAGE_HEIGHT, 
                        moved_col_start, moved_col_end, COLUMN_SHIFT_UP_ROWS);
    }
    
    /* 在帧结束后，更新下一帧的起始位置 - 重置为0，确保每次从帧头开始 */
    frame_start_offset = 0; // 固定从0开始读取下一帧
    
    return 0;
}

/* 新增：初始化DMA用于连续采集 */
int pcie_manager_init_streaming(PCIEManager* manager) {
    if (!manager || manager->pci_driver_fd < 0) {
        printf("PCIE设备未初始化\n");
        return -1;
    }
    
    printf("初始化视频流采集...\n");
    
    /* 设置DMA操作参数（只需要设置一次） */
    /* 每行需要读取: LEADING_PIXELS(前导) + IMAGE_WIDTH(有效) = (LEADING_PIXELS+IMAGE_WIDTH)个像素 * 2字节 = (LEADING_PIXELS+IMAGE_WIDTH)*2字节 / 4 = (LEADING_PIXELS+IMAGE_WIDTH)*2/4 个dword */
    manager->dma_operation.current_len = (LEADING_PIXELS + IMAGE_WIDTH)*2/4;
    manager->dma_operation.offset_addr = 0;
    memset(manager->dma_operation.data.write_buf, 0, DMA_MAX_PACKET_SIZE);
    
    printf("DMA参数配置: current_len=%d dword (%d字节), offset_addr=0x%lx\n", 
           manager->dma_operation.current_len, 
           manager->dma_operation.current_len * 4,
           manager->dma_operation.offset_addr);
    
    /* 映射地址（只需要映射一次） */
    printf("正在映射DMA地址...\n");
    ioctl(manager->pci_driver_fd, PCI_MAP_ADDR_CMD, &manager->dma_operation);
    printf("DMA地址映射完成\n");
    
    /* 重置DMA读取地址到帧起始位置 */
    printf("正在重置DMA读取地址...\n");
    manager->dma_operation.offset_addr = 0;  // 确保从地址0开始
    ioctl(manager->pci_driver_fd, PCI_DMA_WRITE_CMD, &manager->dma_operation);
    printf("DMA读取地址重置完成\n");
    
    printf("视频流采集初始化完成\n");
    return 0;
}

/* 新增：清理DMA流采集 */
void pcie_manager_cleanup_streaming(PCIEManager* manager) {
    if (manager && manager->pci_driver_fd >= 0) {
        /* 取消映射 */
        ioctl(manager->pci_driver_fd, PCI_UMAP_ADDR_CMD, &manager->dma_operation);
        printf("视频流采集已停止\n");
    }
}

void pcie_manager_cleanup(PCIEManager* manager) {
    if (manager && manager->pci_driver_fd >= 0) {
        close(manager->pci_driver_fd);
        manager->pci_driver_fd = -1;
        printf("PCIE设备已关闭\n");
    }
}

//=========================================================================
// 网络接收函数实现
//=========================================================================

/**************************************************************************
** 函数名称:    network_receiver_init - 网络接收端初始化 (同步增强版)
****************************************************************************/
int network_receiver_init(NetworkReceiver* receiver) {
    printf("正在初始化UDP网络接收端 (同步版)...\n");
    
    if (!receiver) {
        printf("网络接收器指针无效\n");
        return -1;
    }
    
    // 初始化结构体
    memset(receiver, 0, sizeof(NetworkReceiver));
    receiver->socket_fd = -1;
    
    // 分配帧缓冲区
    receiver->buffer_size = UDP_FRAME_BUFFER_SIZE;
    receiver->frame_buffer = (uint8_t*)malloc(receiver->buffer_size);
    if (!receiver->frame_buffer) {
        printf("帧缓冲区分配失败\n");
        return -1;
    }
    
    // 分配临时缓冲区
    receiver->temp_buffer = (uint8_t*)malloc(MAX_PACKET_SIZE);
    if (!receiver->temp_buffer) {
        printf("临时缓冲区分配失败\n");
        free(receiver->frame_buffer);
        return -1;
    }
    
    // 行缓冲区已删除（仅用于行同步模式）
    
    // 创建UDP socket
    receiver->socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (receiver->socket_fd < 0) {
        perror("UDP Socket创建失败");
        free(receiver->frame_buffer);
        free(receiver->temp_buffer);
        return -1;
    }
    
    // 设置socket选项
    int reuse = 1;
    if (setsockopt(receiver->socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        printf("警告: 设置socket重用失败\n");
    }
    
    // 设置接收缓冲区大小
    int buffer_size = 8 * 1024 * 1024; // 8MB增大缓冲区
    if (setsockopt(receiver->socket_fd, SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof(buffer_size)) < 0) {
        printf("警告: 设置接收缓冲区失败\n");
    }
    
    // 设置socket超时 (5秒)
    if (set_socket_timeout(receiver->socket_fd, 5000) != 0) {
        printf("警告: 设置socket超时失败\n");
    }
    
    // 配置服务器地址
    memset(&receiver->server_addr, 0, sizeof(receiver->server_addr));
    receiver->server_addr.sin_family = AF_INET;
    receiver->server_addr.sin_addr.s_addr = INADDR_ANY;
    receiver->server_addr.sin_port = htons(UDP_PORT);
    
    // 绑定地址
    if (bind(receiver->socket_fd, (struct sockaddr*)&receiver->server_addr, sizeof(receiver->server_addr)) < 0) {
        perror("UDP绑定失败");
        close(receiver->socket_fd);
        free(receiver->frame_buffer);
        free(receiver->temp_buffer);
        return -1;
    }
    
    receiver->client_len = sizeof(receiver->client_addr);
    
    // 初始化分包传输支持
    memset(&receiver->assembler, 0, sizeof(receiver->assembler));
    
    printf("UDP网络接收端初始化成功\n");
    printf("监听端口: %d\n", UDP_PORT);
    printf("帧缓冲区大小: %zu字节\n", receiver->buffer_size);
    printf("传输模式: 分包传输模式 (专用于M端)\n");
    
    // 网络诊断信息
    printf("\n=== 网络诊断信息 ===\n");
    printf("本机监听地址: 0.0.0.0:%d\n", UDP_PORT);
    printf("期望发送方: 192.168.100.10\n");
    printf("socket超时: 5秒\n");
    printf("接收缓冲区: 8MB\n");
    printf("等待M端连接和发送数据...\n");
    
    return 0;
}

//=========================================================================
// 同步函数实现 - 简化版专注640*480
//=========================================================================

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
** 函数名称:    set_socket_timeout - 设置socket超时
****************************************************************************/
int set_socket_timeout(int sockfd, int timeout_ms) {
    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("设置接收超时失败");
        return -1;
    }
    return 0;
}

// 删除了行同步相关的函数实现

/**************************************************************************
** 函数名称:    network_receive_with_sync - 分包传输接收函数 (专用于M端)
****************************************************************************/
int network_receive_with_sync(NetworkReceiver* receiver) {
    if (!receiver || !receiver->frame_buffer) {
        return -1;
    }
    
    // 接收分包数据
    ssize_t received = recvfrom(receiver->socket_fd, receiver->packet_buffer, 
                               MAX_PACKET_SIZE, 0,
                               (struct sockaddr*)&receiver->client_addr, 
                               &receiver->client_len);
    
    if (received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 超时时检查是否有正在重组的帧可以完成
            if (receiver->assembler.is_valid) {
                int status = check_assembler_complete(&receiver->assembler);
                if (status == 1) {
                    // 重组完成，返回完整帧
                    int size = get_complete_frame(&receiver->assembler, receiver->frame_buffer, 
                                               receiver->buffer_size);
                    // 不立即清理重组器，允许继续接收后续包
                    printf("超时重组完成: %d字节，重组器继续保持\n", size);
                    return size;
                } else if (status == -1) {
                    // 重组超时失败，但不立即清理，给一次机会
                    printf("帧重组超时，但保持重组器继续尝试\n");
                }
            }
            
            // 定期输出等待状态
            static int timeout_count = 0;
            if (++timeout_count % 50 == 0) {
                printf("UDP接收: 等待数据中... (超时次数: %d)\n", timeout_count);
            }
            return 0; // 超时，继续等待
        }
        printf("UDP接收错误: %s\n", strerror(errno));
        return 0; // 其他错误，继续等待
    }
    
    // 检查分包格式
    if (received < sizeof(PacketHeader)) {
        printf("收到数据包太小: %zd字节 (最小需要: %zu字节)\n", received, sizeof(PacketHeader));
        return 0; // 数据包太小，继续等待
    }
    
    PacketHeader* header = (PacketHeader*)receiver->packet_buffer;
    
    // 检查是否是分包传输数据
    if (header->magic != PACKET_MAGIC) {
        // 尝试作为简单UDP数据处理（兼容性处理）
        printf("收到非分包数据，尝试简单UDP处理: %zd字节\n", received);
        
        // 如果接收到的数据大小符合一个完整帧，直接使用
        if (received >= UDP_FRAME_BUFFER_SIZE) {
            memcpy(receiver->frame_buffer, receiver->packet_buffer, UDP_FRAME_BUFFER_SIZE);
            printf("简单UDP接收完成: %zd字节\n", received);
            return UDP_FRAME_BUFFER_SIZE;
        } else if (received >= 1024) { // 如果是较大的数据包，也尝试使用
            memcpy(receiver->frame_buffer, receiver->packet_buffer, 
                   received < UDP_FRAME_BUFFER_SIZE ? received : UDP_FRAME_BUFFER_SIZE);
            printf("部分UDP数据接收: %zd字节\n", received);
            return received;
        }
        
        return 0; // 不是有效格式，继续等待
    }
    
    // 减少调试输出频率，避免刷屏
    static int packet_count = 0;
    packet_count++;
    if (packet_count % 50 == 0 || header->packet_type != PACKET_TYPE_DATA) {
        printf("收到有效分包: %zd字节, 帧ID=%u, 包ID=%u, 类型=%u (第%d个包)\n", 
               received, header->frame_id, header->packet_id, header->packet_type, packet_count);
    }
    
    // 处理分包数据
    switch (header->packet_type) {
        case PACKET_TYPE_START:
            printf("接收到帧开始包: 帧ID=%u, 总包数=%u, 尺寸=%ux%u\n", 
                   header->frame_id, header->total_packets, header->image_width, header->image_height);
            
            // 清理旧的重组器
            if (receiver->assembler.is_valid) {
                cleanup_packet_assembler(&receiver->assembler);
            }
            
            if (init_packet_assembler(&receiver->assembler, header->frame_id, 
                                     header->total_packets,
                                     header->image_width, 
                                     header->image_height) != 0) {
                printf("重组器初始化失败\n");
            }
            break;
            
        case PACKET_TYPE_DATA:
            if (receiver->assembler.is_valid) {
                // 检查帧ID是否匹配
                if (receiver->assembler.frame_id != header->frame_id) {
                    // 只有在帧ID差距较大或当前重组进度很低时才切换
                    uint32_t frame_diff = (header->frame_id > receiver->assembler.frame_id) ? 
                                         (header->frame_id - receiver->assembler.frame_id) : 
                                         (receiver->assembler.frame_id - header->frame_id);
                    
                    double progress = (double)receiver->assembler.received_packets / receiver->assembler.total_packets;
                    
                    // 切换条件：帧差距大于10，或者当前进度小于10%且差距大于3
                    if (frame_diff > 10 || (frame_diff > 3 && progress < 0.1)) {
                        printf("帧ID切换 %u -> %u (差距:%u, 当前进度:%.1f%%)\n", 
                               receiver->assembler.frame_id, header->frame_id, frame_diff, progress * 100);
                        
                        // 如果当前帧有足够进度，先尝试完成它
                        if (progress >= 0.5 || receiver->assembler.received_packets >= 250) {
                            printf("尝试完成当前帧后再切换... (进度:%.1f%%, 包数:%u)\n", 
                                   progress * 100, receiver->assembler.received_packets);
                            int size = get_complete_frame(&receiver->assembler, receiver->frame_buffer, 
                                                       receiver->buffer_size);
                            cleanup_packet_assembler(&receiver->assembler);
                            if (size > 0) {
                                printf("切换前完成旧帧: %d字节\n", size);
                                return size; // 返回旧帧，下次循环处理新帧
                            }
                        } else {
                            // 进度太低，直接放弃旧帧
                            printf("进度太低，放弃当前帧 (进度:%.1f%%, 包数:%u)\n", 
                                   progress * 100, receiver->assembler.received_packets);
                            cleanup_packet_assembler(&receiver->assembler);
                        }
                        
                        // 根据数据包信息推测重组器参数
                        uint16_t estimated_total_packets = header->total_packets;
                        if (estimated_total_packets == 0) {
                            // 根据640x480 RGB565图像计算包数
                            estimated_total_packets = (UDP_IMAGE_WIDTH * UDP_IMAGE_HEIGHT * 2 + PACKET_MAX_DATA_SIZE - 1) / PACKET_MAX_DATA_SIZE;
                        }
                        
                        printf("初始化新重组器: 帧ID=%u, 估计包数=%u\n", 
                               header->frame_id, estimated_total_packets);
                        
                        if (init_packet_assembler(&receiver->assembler, header->frame_id, 
                                                 estimated_total_packets,
                                                 header->image_width > 0 ? header->image_width : UDP_IMAGE_WIDTH, 
                                                 header->image_height > 0 ? header->image_height : UDP_IMAGE_HEIGHT) != 0) {
                            printf("新重组器初始化失败\n");
                            break;
                        }
                    } else {
                        // 帧差距不大且有一定进度，忽略新帧，继续当前帧
                        static int ignore_count = 0;
                        static uint32_t last_reported_frame = 0;
                        
                        if (receiver->assembler.frame_id != last_reported_frame || ++ignore_count % 100 == 0) {
                            printf("继续当前帧 %u (进度:%.1f%%, 包数:%u), 忽略新帧 %u (差距:%u, 已忽略%d次)\n", 
                                   receiver->assembler.frame_id, progress * 100, receiver->assembler.received_packets,
                                   header->frame_id, frame_diff, ignore_count);
                            last_reported_frame = receiver->assembler.frame_id;
                        }
                        return 0; // 忽略这个包，继续当前帧
                    }
                }
                
                uint8_t* data_ptr = receiver->packet_buffer + sizeof(PacketHeader);
                if (add_packet_to_assembler(&receiver->assembler, header, data_ptr) == 0) {
                    // 显示进度(减少输出频率)
                    if (receiver->assembler.received_packets % 50 == 0 || 
                        receiver->assembler.received_packets == receiver->assembler.total_packets) {
                        printf("接收进度: %u/%u (%.1f%%)\n", 
                               receiver->assembler.received_packets, receiver->assembler.total_packets,
                               receiver->assembler.received_packets * 100.0 / receiver->assembler.total_packets);
                    }
                    
                    // 检查是否完成重组
                    int status = check_assembler_complete(&receiver->assembler);
                    if (status == 1) {
                        int size = get_complete_frame(&receiver->assembler, receiver->frame_buffer, 
                                                   receiver->buffer_size);
                        if (size > 0) {
                            printf("分包重组完成: %d字节\n", size);
                            
                            // 如果重组完成度很高，清理重组器准备下一帧
                            if (receiver->assembler.received_packets >= receiver->assembler.total_packets * 0.9) {
                                cleanup_packet_assembler(&receiver->assembler);
                                printf("高完成度，清理重组器准备下一帧\n");
                            } else {
                                printf("部分完成，重组器保持活跃状态\n");
                            }
                            
                            return size;
                        }
                    }
                    
                    // 强制重组条件调整：需要更多包才重组，避免条纹图像
                    if (receiver->assembler.received_packets >= 200 && 
                        receiver->assembler.received_packets % 100 == 0) {
                        printf("尝试强制重组: %u包 (%.1f%%)\n",
                               receiver->assembler.received_packets,
                               receiver->assembler.received_packets * 100.0 / receiver->assembler.total_packets);
                        
                        int size = get_complete_frame(&receiver->assembler, receiver->frame_buffer, 
                                                   receiver->buffer_size);
                        if (size > 0) {
                            printf("强制重组成功: %d字节\n", size);
                            // 不清理重组器，继续接收更多数据
                            return size;  
                        }
                    }
                    
                    // 如果收到很多包但还没完成，可能需要调整总包数估计
                    if (receiver->assembler.received_packets > receiver->assembler.total_packets) {
                        printf("收到包数超过预期，调整总包数: %u -> %u\n",
                               receiver->assembler.total_packets, receiver->assembler.received_packets + 50);
                        receiver->assembler.total_packets = receiver->assembler.received_packets + 50;
                    }
                }
            } else {
                printf("收到数据包但无活跃重组器，自动初始化: 帧ID=%u, 包ID=%u/%u\n",
                       header->frame_id, header->packet_id, header->total_packets);
                
                // 根据数据包信息自动初始化重组器
                uint16_t estimated_total_packets = header->total_packets;
                if (estimated_total_packets == 0) {
                    // 根据640x480 RGB565图像计算包数
                    estimated_total_packets = (UDP_IMAGE_WIDTH * UDP_IMAGE_HEIGHT * 2 + PACKET_MAX_DATA_SIZE - 1) / PACKET_MAX_DATA_SIZE;
                }
                
                if (init_packet_assembler(&receiver->assembler, header->frame_id, 
                                         estimated_total_packets,
                                         header->image_width > 0 ? header->image_width : UDP_IMAGE_WIDTH, 
                                         header->image_height > 0 ? header->image_height : UDP_IMAGE_HEIGHT) == 0) {
                    printf("自动重组器初始化成功: 帧ID=%u, 包数=%u\n", 
                           header->frame_id, estimated_total_packets);
                    
                    // 重新尝试添加数据包
                    uint8_t* data_ptr = receiver->packet_buffer + sizeof(PacketHeader);
                    add_packet_to_assembler(&receiver->assembler, header, data_ptr);
                }
            }
            break;
            
        case PACKET_TYPE_END:
            printf("接收到帧结束包: 帧ID=%u\n", header->frame_id);
            if (receiver->assembler.is_valid && receiver->assembler.frame_id == header->frame_id) {
                int status = check_assembler_complete(&receiver->assembler);
                printf("帧结束检查: 状态=%d, 已收包=%u/%u (%.1f%%)\n", status,
                       receiver->assembler.received_packets, receiver->assembler.total_packets,
                       receiver->assembler.received_packets * 100.0 / receiver->assembler.total_packets);
                
                // 如果完成度达到80%以上，也认为可以使用
                if (status == 1 || receiver->assembler.received_packets >= receiver->assembler.total_packets * 0.8) {
                    int size = get_complete_frame(&receiver->assembler, receiver->frame_buffer, 
                                               receiver->buffer_size);
                    cleanup_packet_assembler(&receiver->assembler);
                    if (size > 0) {
                        printf("帧接收完成: %d字节 (完成度: %.1f%%)\n", size,
                               receiver->assembler.received_packets * 100.0 / receiver->assembler.total_packets);
                        return size;
                    }
                } else {
                    printf("帧接收不完整，丢弃: %.1f%% (需要80%%以上)\n",
                           receiver->assembler.received_packets * 100.0 / receiver->assembler.total_packets);
                    cleanup_packet_assembler(&receiver->assembler);
                }
            } else {
                printf("收到帧结束包但无匹配的重组器\n");
            }
            break;
            
        default:
            printf("未知包类型: %u\n", header->packet_type);
            break;
    }
    
    return 0; // 继续接收下一个包
}

// 删除了传统行同步接收函数

/**************************************************************************
** 函数名称:    network_receive_simple - 简单UDP接收（用于测试和兼容性）
****************************************************************************/
int network_receive_simple(NetworkReceiver* receiver) {
    if (!receiver || !receiver->frame_buffer) {
        return -1;
    }
    
    // 接收单个UDP数据包
    ssize_t received = recvfrom(receiver->socket_fd, receiver->temp_buffer, 
                               MAX_PACKET_SIZE, 0,
                               (struct sockaddr*)&receiver->client_addr, 
                               &receiver->client_len);
    
    if (received < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0; // 超时，继续等待
        }
        printf("简单UDP接收错误: %s\n", strerror(errno));
        return 0;
    }
    
    printf("简单UDP接收: %zd字节\n", received);
    
    // 如果接收到的数据大小合适，直接复制到帧缓冲区
    if (received >= UDP_FRAME_BUFFER_SIZE) {
        memcpy(receiver->frame_buffer, receiver->temp_buffer, UDP_FRAME_BUFFER_SIZE);
        return UDP_FRAME_BUFFER_SIZE;
    } else if (received >= 1024) { // 较大的数据包也尝试使用
        memcpy(receiver->frame_buffer, receiver->temp_buffer, received);
        // 如果数据不足，用黑色填充剩余部分
        if (received < UDP_FRAME_BUFFER_SIZE) {
            memset(receiver->frame_buffer + received, 0, UDP_FRAME_BUFFER_SIZE - received);
        }
        return UDP_FRAME_BUFFER_SIZE;
    }
    
    return 0;
}

/**************************************************************************
** 函数名称:    network_receive_frame - 接收一帧视频数据（简化版）
****************************************************************************/
int network_receive_frame(NetworkReceiver* receiver) {
    if (!receiver || !receiver->frame_buffer) {
        return -1;
    }
    
    size_t total_received = 0;
    size_t expected_size = UDP_FRAME_BUFFER_SIZE;
    
    // 清空缓冲区
    memset(receiver->frame_buffer, 0, receiver->buffer_size);
    
    // 设置接收超时
    fd_set read_fds;
    struct timeval timeout;
    
    // 尝试接收完整帧
    while (total_received < expected_size) {
        FD_ZERO(&read_fds);
        FD_SET(receiver->socket_fd, &read_fds);
        timeout.tv_sec = RECEIVE_TIMEOUT_SEC;
        timeout.tv_usec = 0;
        
        int ready = select(receiver->socket_fd + 1, &read_fds, NULL, NULL, &timeout);
        if (ready <= 0) {
            if (ready == 0) {
                printf("接收超时 (已接收 %zu/%zu 字节)\n", total_received, expected_size);
            } else {
                perror("select失败");
            }
            break; // 超时也返回已接收的数据
        }
        
        // 接收数据
        size_t remaining = expected_size - total_received;
        size_t receive_size = (remaining > MAX_PACKET_SIZE) ? MAX_PACKET_SIZE : remaining;
        
        ssize_t received = recvfrom(receiver->socket_fd, 
                                   receiver->frame_buffer + total_received, 
                                   receive_size, 0,
                                   (struct sockaddr*)&receiver->client_addr, 
                                   &receiver->client_len);
        
        if (received < 0) {
            perror("UDP接收失败");
            return -1;
        }
        
        if (received == 0) {
            printf("连接关闭\n");
            return -1;
        }
        
        total_received += received;
        
        // 显示接收进度（每接收10%显示一次）
        if ((total_received * 10 / expected_size) != ((total_received - received) * 10 / expected_size)) {
            printf("接收进度: %zu/%zu 字节 (%.1f%%)\n", 
                   total_received, expected_size, (double)total_received * 100.0 / expected_size);
        }
    }
    
    if (total_received >= expected_size) {
        printf("帧接收完成: %zu字节\n", total_received);
    }
    
    return total_received;
}

/**************************************************************************
** 函数名称:    network_receiver_cleanup - 清理网络接收资源
****************************************************************************/
void network_receiver_cleanup(NetworkReceiver* receiver) {
    if (receiver) {
        if (receiver->socket_fd >= 0) {
            close(receiver->socket_fd);
            receiver->socket_fd = -1;
        }
        
        if (receiver->frame_buffer) {
            free(receiver->frame_buffer);
            receiver->frame_buffer = NULL;
        }
        
        if (receiver->temp_buffer) {
            free(receiver->temp_buffer);
            receiver->temp_buffer = NULL;
        }
        
        // 清理分包传输相关资源
        cleanup_packet_assembler(&receiver->assembler);
        
        printf("UDP网络接收端已清理 (分包传输模式)\n");
    }
}

//=========================================================================
// 工具函数实现
//=========================================================================

/**************************************************************************
** 函数名称:    get_time_diff - 计算时间差（秒）
****************************************************************************/
double get_time_diff(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
}

/**************************************************************************
** 函数名称:    nano_delay - 纳秒级延时
****************************************************************************/
int nano_delay(long delay) {
    struct timespec req, rem;
    long nano_delay = delay;
    int ret = 0;
    while(nano_delay > 0) {
        rem.tv_sec = 0;
        rem.tv_nsec = 0;
        req.tv_sec = 0;
        req.tv_nsec = nano_delay;
        if((ret = (nanosleep(&req, &rem) == -1))) {
            printf("nanosleep failed !!!\n");
        }
        nano_delay = rem.tv_nsec;
    };
    return ret;
}

/**************************************************************************
** 函数名称:    rgb565_to_rgb888 - 图像格式转换（高度优化版+循环展开）
****************************************************************************/
void rgb565_to_rgb888(const uint16_t* image565, uint8_t* image888, size_t num_pixels) {
    size_t i;
    const uint16_t* src = image565;
    uint8_t* dst = image888;
    size_t unroll_count = num_pixels / 4;
    size_t remainder = num_pixels % 4;
    
    /* 循环展开4次，提高指令级并行度 */
    for (i = 0; i < unroll_count; ++i) {
        uint16_t p0 = src[0], p1 = src[1], p2 = src[2], p3 = src[3];
        
        /* 像素0 */
        dst[0] = ((p0 & 0x001F) << 3) | ((p0 & 0x001F) >> 2);
        dst[1] = ((p0 & 0x07E0) >> 3) | ((p0 & 0x07E0) >> 9);
        dst[2] = ((p0 & 0xF800) >> 8) | ((p0 & 0xF800) >> 13);
        
        /* 像素1 */
        dst[3] = ((p1 & 0x001F) << 3) | ((p1 & 0x001F) >> 2);
        dst[4] = ((p1 & 0x07E0) >> 3) | ((p1 & 0x07E0) >> 9);
        dst[5] = ((p1 & 0xF800) >> 8) | ((p1 & 0xF800) >> 13);
        
        /* 像素2 */
        dst[6] = ((p2 & 0x001F) << 3) | ((p2 & 0x001F) >> 2);
        dst[7] = ((p2 & 0x07E0) >> 3) | ((p2 & 0x07E0) >> 9);
        dst[8] = ((p2 & 0xF800) >> 8) | ((p2 & 0xF800) >> 13);
        
        /* 像素3 */
        dst[9] = ((p3 & 0x001F) << 3) | ((p3 & 0x001F) >> 2);
        dst[10] = ((p3 & 0x07E0) >> 3) | ((p3 & 0x07E0) >> 9);
        dst[11] = ((p3 & 0xF800) >> 8) | ((p3 & 0xF800) >> 13);
        
        src += 4;
        dst += 12;
    }
    
    /* 处理剩余像素 */
    for (i = 0; i < remainder; ++i) {
        uint16_t pixel = *src++;
        *dst++ = ((pixel & 0x001F) << 3) | ((pixel & 0x001F) >> 2);
        *dst++ = ((pixel & 0x07E0) >> 3) | ((pixel & 0x07E0) >> 9);
        *dst++ = ((pixel & 0xF800) >> 8) | ((pixel & 0xF800) >> 13);
    }
}

/**************************************************************************
** 函数名称:    rgb565_to_bgra_direct - 直接从RGB565转换到BGRA（跳过中间格式）
** 说明:       这是最快的转换方式，避免了RGB888中间步骤
****************************************************************************/
void rgb565_to_bgra_direct(const uint16_t* image565, uint32_t* bgra_data, size_t num_pixels) {
    size_t i;
    size_t unroll_count = num_pixels / 4;
    size_t remainder = num_pixels % 4;
    
    /* 循环展开4次处理 */
    for (i = 0; i < unroll_count; ++i) {
        uint16_t p0 = image565[0];
        uint16_t p1 = image565[1];
        uint16_t p2 = image565[2];
        uint16_t p3 = image565[3];
        
        /* 直接转换为BGRA格式: 0xAABBGGRR */
        bgra_data[0] = 0xFF000000 | 
                      ((((p0 & 0x001F) << 3) | ((p0 & 0x001F) >> 2)) << 16) |  /* R */
                      ((((p0 & 0x07E0) >> 3) | ((p0 & 0x07E0) >> 9)) << 8) |   /* G */
                      (((p0 & 0xF800) >> 8) | ((p0 & 0xF800) >> 13));          /* B */
        
        bgra_data[1] = 0xFF000000 | 
                      ((((p1 & 0x001F) << 3) | ((p1 & 0x001F) >> 2)) << 16) |
                      ((((p1 & 0x07E0) >> 3) | ((p1 & 0x07E0) >> 9)) << 8) |
                      (((p1 & 0xF800) >> 8) | ((p1 & 0xF800) >> 13));
        
        bgra_data[2] = 0xFF000000 | 
                      ((((p2 & 0x001F) << 3) | ((p2 & 0x001F) >> 2)) << 16) |
                      ((((p2 & 0x07E0) >> 3) | ((p2 & 0x07E0) >> 9)) << 8) |
                      (((p2 & 0xF800) >> 8) | ((p2 & 0xF800) >> 13));
        
        bgra_data[3] = 0xFF000000 | 
                      ((((p3 & 0x001F) << 3) | ((p3 & 0x001F) >> 2)) << 16) |
                      ((((p3 & 0x07E0) >> 3) | ((p3 & 0x07E0) >> 9)) << 8) |
                      (((p3 & 0xF800) >> 8) | ((p3 & 0xF800) >> 13));
        
        image565 += 4;
        bgra_data += 4;
    }
    
    /* 处理剩余像素 */
    for (i = 0; i < remainder; ++i) {
        uint16_t pixel = *image565++;
        *bgra_data++ = 0xFF000000 | 
                      ((((pixel & 0x001F) << 3) | ((pixel & 0x001F) >> 2)) << 16) |
                      ((((pixel & 0x07E0) >> 3) | ((pixel & 0x07E0) >> 9)) << 8) |
                      (((pixel & 0xF800) >> 8) | ((pixel & 0xF800) >> 13));
    }
}

/**************************************************************************
** 函数名称:    scale_image_simple - 简单的图像缩放（最近邻算法）
****************************************************************************/
void scale_image_simple(const uint8_t* src_rgb888, uint8_t* dst_rgb888, 
                       int src_width, int src_height, int dst_width, int dst_height) {
    int dst_x, dst_y;
    int src_x, src_y;
    int src_idx, dst_idx;
    
    for (dst_y = 0; dst_y < dst_height; dst_y++) {
        for (dst_x = 0; dst_x < dst_width; dst_x++) {
            /* 计算对应的源图像坐标 */
            src_x = (dst_x * src_width) / dst_width;
            src_y = (dst_y * src_height) / dst_height;
            
            /* 确保不越界 */
            if (src_x >= src_width) src_x = src_width - 1;
            if (src_y >= src_height) src_y = src_height - 1;
            
            /* 计算源和目标像素索引 */
            src_idx = (src_y * src_width + src_x) * 3;
            dst_idx = (dst_y * dst_width + dst_x) * 3;
            
            /* 复制RGB值 */
            dst_rgb888[dst_idx + 0] = src_rgb888[src_idx + 0];  /* R */
            dst_rgb888[dst_idx + 1] = src_rgb888[src_idx + 1];  /* G */
            dst_rgb888[dst_idx + 2] = src_rgb888[src_idx + 2];  /* B */
        }
    }
}

/**************************************************************************
** 函数名称:    rearrange_columns - 列重排：将第n列到第m列移动到行末
** 功能描述:    将指定范围的列移动到行的最后，其余列向左移动
** 输入参数:    row_buffer - 行数据缓冲区(RGB565格式，每个像素2字节)
**              width - 行宽度（像素数）
**              start_col - 起始列索引(0-based, 包含)
**              end_col - 结束列索引(0-based, 包含)
** 算法说明:    
**   原始: [0, 1, 2, ..., n, n+1, ..., m, m+1, ..., width-1]
**   结果: [0, 1, ..., n-1, m+1, ..., width-1, n, n+1, ..., m]
****************************************************************************/
void rearrange_columns(uint16_t* row_buffer, int width, int start_col, int end_col) {
    int move_count, remaining_count;
    uint16_t* temp_buffer;
    
    /* 参数检查 */
    if (!row_buffer || width <= 0 || start_col < 0 || end_col >= width || start_col > end_col) {
        return;  /* 无效参数，不进行处理 */
    }
    
    /* 如果起始列为0且结束列为width-1，说明要移动整行，无需处理 */
    if (start_col == 0 && end_col == width - 1) {
        return;
    }
    
    /* 计算要移动的列数和剩余列数 */
    move_count = end_col - start_col + 1;          /* 要移动到末尾的列数 */
    remaining_count = width - end_col - 1;         /* end_col之后的列数 */
    
    /* 如果没有剩余列，说明要移动的列已经在末尾，无需处理 */
    if (remaining_count == 0) {
        return;
    }
    
    /* 分配临时缓冲区保存要移动的列 */
    temp_buffer = (uint16_t*)malloc(move_count * sizeof(uint16_t));
    if (!temp_buffer) {
        printf("列重排失败：内存分配失败\n");
        return;
    }
    
    /* 步骤1: 保存第n列到第m列的数据 */
    memcpy(temp_buffer, row_buffer + start_col, move_count * sizeof(uint16_t));
    
    /* 步骤2: 将第m+1列到最后的数据向左移动到第n列位置 */
    if (start_col == 0) {
        /* 如果从第0列开始，直接移动后面的列到前面 */
        memmove(row_buffer, row_buffer + end_col + 1, remaining_count * sizeof(uint16_t));
    } else {
        /* 如果不从第0列开始，将m+1列之后的数据移动到start_col位置 */
        memmove(row_buffer + start_col, row_buffer + end_col + 1, remaining_count * sizeof(uint16_t));
    }
    
    /* 步骤3: 将保存的第n列到第m列数据追加到行末 */
    memcpy(row_buffer + width - move_count, temp_buffer, move_count * sizeof(uint16_t));
    
    /* 释放临时缓冲区 */
    free(temp_buffer);
}

/**************************************************************************
** 函数名称:    shift_columns_up - 列垂直上移：将指定列向上移动k行
** 功能描述:    将指定范围的列向上移动k行，底部k行用顶部k行的数据填充（循环移位）
** 输入参数:    image_buffer - 图像数据缓冲区(RGB565格式，每个像素2字节)
**              width - 图像宽度（像素数）
**              height - 图像高度（像素数）
**              start_col - 起始列索引(0-based, 包含)
**              end_col - 结束列索引(0-based, 包含)
**              shift_rows - 向上移动的行数
** 算法说明:    
**   对于指定列范围内的每一列：
**   - 保存顶部shift_rows行的数据
**   - 将下面的数据向上移动shift_rows行
**   - 将保存的数据放到底部
****************************************************************************/
void shift_columns_up(uint8_t* image_buffer, int width, int height, int start_col, int end_col, int shift_rows) {
    int col, row;
    uint16_t* temp_buffer;
    int col_count;
    
    /* 参数检查 */
    if (!image_buffer || width <= 0 || height <= 0 || 
        start_col < 0 || end_col >= width || start_col > end_col || 
        shift_rows <= 0 || shift_rows >= height) {
        return;  /* 无效参数，不进行处理 */
    }
    
    /* 确保shift_rows不超过图像高度 */
    if (shift_rows >= height) {
        shift_rows = height - 1;
    }
    
    col_count = end_col - start_col + 1;  /* 需要处理的列数 */
    
    /* 分配临时缓冲区保存顶部shift_rows行的数据 */
    temp_buffer = (uint16_t*)malloc(shift_rows * col_count * sizeof(uint16_t));
    if (!temp_buffer) {
        printf("列垂直移动失败：内存分配失败\n");
        return;
    }
    
    /* 步骤1: 保存顶部shift_rows行指定列的数据 */
    for (row = 0; row < shift_rows; row++) {
        for (col = start_col; col <= end_col; col++) {
            uint16_t* pixel_ptr = (uint16_t*)(image_buffer + (row * width + col) * 2);
            temp_buffer[row * col_count + (col - start_col)] = *pixel_ptr;
        }
    }
    
    /* 步骤2: 将下面的数据向上移动shift_rows行 */
    for (row = shift_rows; row < height; row++) {
        for (col = start_col; col <= end_col; col++) {
            uint16_t* src_pixel = (uint16_t*)(image_buffer + (row * width + col) * 2);
            uint16_t* dst_pixel = (uint16_t*)(image_buffer + ((row - shift_rows) * width + col) * 2);
            *dst_pixel = *src_pixel;
        }
    }
    
    /* 步骤3: 将保存的顶部数据放到底部 */
    for (row = 0; row < shift_rows; row++) {
        for (col = start_col; col <= end_col; col++) {
            uint16_t* dst_pixel = (uint16_t*)(image_buffer + ((height - shift_rows + row) * width + col) * 2);
            *dst_pixel = temp_buffer[row * col_count + (col - start_col)];
        }
    }
    
    /* 释放临时缓冲区 */
    free(temp_buffer);
}

/**************************************************************************
** 函数名称:    save_frame_to_ppm - 保存帧到PPM文件
****************************************************************************/
int save_frame_to_ppm(const uint8_t* rgb888_data, const char* filename) {
    FILE* f;
    size_t written;
    
    if (!rgb888_data || !filename) {
        printf("保存PPM文件失败：无效参数\n");
        return -1;
    }
    
    printf("正在保存PPM文件: %s\n", filename);
    
    f = fopen(filename, "wb");
    if (!f) {
        printf("无法创建文件: %s\n", filename);
        return -1;
    }
    
    /* 写入PPM头部 */
    fprintf(f, "P6\n%d %d\n255\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    
    /* 写入像素数据 */
    written = fwrite(rgb888_data, 1, IMAGE_WIDTH * IMAGE_HEIGHT * 3, f);
    fclose(f);
    
    if (written == IMAGE_WIDTH * IMAGE_HEIGHT * 3) {
        printf("PPM文件保存成功: %s\n", filename);
        return 0;
    } else {
        printf("PPM文件写入不完整\n");
        return -1;
    }
}

//=========================================================================
// 渲染引擎函数实现 - X11渲染引擎
//=========================================================================

int render_engine_init(RenderEngine* engine) {
    int screen;
    Visual *visual;
    int depth;
    
    printf("正在初始化X11渲染引擎...\n");
    
    if (!engine) {
        printf("渲染引擎指针无效\n");
        return -1;
    }
    
    /* 初始化结构体 */
    engine->display = NULL;
    engine->window = 0;
    engine->gc = 0;
    engine->ximage = NULL;
    engine->display_buffer = NULL;
    engine->pixmap = 0;
    engine->use_fast_mode = 1;  /* 使用优化显示模式 */
    engine->use_shm = 0;        /* 初始化SHM标志 */
    memset(&engine->shminfo, 0, sizeof(XShmSegmentInfo));
    engine->is_init = 0;
    
    /* 打开显示器连接 */
    engine->display = XOpenDisplay(NULL);
    if (engine->display == NULL) {
        printf("无法打开X11显示器\n");
        return -1;
    }
    
    screen = DefaultScreen(engine->display);
    visual = DefaultVisual(engine->display, screen);
    depth = DefaultDepth(engine->display, screen);
    
    /* 检查是否支持MIT-SHM扩展 */
    if (XShmQueryExtension(engine->display)) {
        printf("检测到MIT-SHM扩展，使用共享内存加速\n");
        engine->use_shm = 1;
    } else {
        printf("不支持MIT-SHM扩展，使用标准模式\n");
        engine->use_shm = 0;
    }
    
    /* 创建窗口 */
    engine->window = XCreateSimpleWindow(engine->display,
                                RootWindow(engine->display, screen),
                                0, 0,
                                DISPLAY_WIDTH, DISPLAY_HEIGHT,
                                1,
                                BlackPixel(engine->display, screen),
                                WhitePixel(engine->display, screen));
    
    /* 设置窗口属性 */
    XStoreName(engine->display, engine->window, "FPGA_PCIE_IMAGE_PROCESS");
    XSelectInput(engine->display, engine->window, ExposureMask | KeyPressMask | ButtonPressMask);
    XMapWindow(engine->display, engine->window);
    
    /* 创建图形上下文 */
    engine->gc = XCreateGC(engine->display, engine->window, 0, NULL);
    
    /* 加载字体 */
    engine->font = XLoadQueryFont(engine->display, "fixed");
    if (!engine->font) {
        engine->font = XLoadQueryFont(engine->display, "*");
    }
    if (engine->font) {
        XSetFont(engine->display, engine->gc, engine->font->fid);
    }
    
    /* 根据是否支持SHM选择不同的缓冲区分配方式 */
    if (engine->use_shm) {
        /* 使用共享内存 */
        engine->ximage = XShmCreateImage(engine->display, visual, depth, ZPixmap, NULL,
                                        &engine->shminfo, DISPLAY_WIDTH, DISPLAY_HEIGHT);
        if (!engine->ximage) {
            printf("无法创建SHM XImage\n");
            engine->use_shm = 0;
            goto fallback_mode;
        }
        
        /* 分配共享内存段 */
        engine->shminfo.shmid = shmget(IPC_PRIVATE, 
                                       engine->ximage->bytes_per_line * engine->ximage->height,
                                       IPC_CREAT | 0777);
        if (engine->shminfo.shmid < 0) {
            printf("共享内存分配失败\n");
            XDestroyImage(engine->ximage);
            engine->ximage = NULL;
            engine->use_shm = 0;
            goto fallback_mode;
        }
        
        /* 附加共享内存 */
        engine->shminfo.shmaddr = engine->ximage->data = (char*)shmat(engine->shminfo.shmid, 0, 0);
        engine->display_buffer = engine->shminfo.shmaddr;
        
        if (engine->shminfo.shmaddr == (char*)-1) {
            printf("共享内存附加失败\n");
            shmctl(engine->shminfo.shmid, IPC_RMID, 0);
            XDestroyImage(engine->ximage);
            engine->ximage = NULL;
            engine->use_shm = 0;
            goto fallback_mode;
        }
        
        engine->shminfo.readOnly = False;
        
        /* 附加到X服务器 */
        if (!XShmAttach(engine->display, &engine->shminfo)) {
            printf("X服务器共享内存附加失败\n");
            shmdt(engine->shminfo.shmaddr);
            shmctl(engine->shminfo.shmid, IPC_RMID, 0);
            XDestroyImage(engine->ximage);
            engine->ximage = NULL;
            engine->use_shm = 0;
            goto fallback_mode;
        }
        
        /* 标记共享内存段以便在进程退出时自动删除 */
        shmctl(engine->shminfo.shmid, IPC_RMID, 0);
        
        printf("共享内存模式初始化成功\n");
        engine->is_init = 1;
        return 0;
    }
    
fallback_mode:
    /* 标准模式：分配显示缓冲区 - 使用对齐内存，使用拼接图像尺寸 */
    engine->display_buffer = (char*)aligned_alloc(32, DISPLAY_WIDTH * DISPLAY_HEIGHT * 4);
    if (!engine->display_buffer) {
        engine->display_buffer = (char*)malloc(DISPLAY_WIDTH * DISPLAY_HEIGHT * 4);
    }
    if (!engine->display_buffer) {
        printf("显示缓冲区分配失败\n");
        render_engine_cleanup(engine);
        return -1;
    }
    
    /* 创建XImage，使用原始图像尺寸 */
    engine->ximage = XCreateImage(engine->display, visual, depth, ZPixmap, 0,
                         engine->display_buffer, DISPLAY_WIDTH, DISPLAY_HEIGHT, 32, 0);
    
    if (!engine->ximage) {
        printf("无法创建XImage\n");
        render_engine_cleanup(engine);
        return -1;
    }
    
    /* 如果使用快速模式，创建双缓冲pixmap */
    if (engine->use_fast_mode) {
        engine->pixmap = XCreatePixmap(engine->display, engine->window, 
                                     DISPLAY_WIDTH, DISPLAY_HEIGHT, depth);
        if (!engine->pixmap) {
            printf("警告：无法创建pixmap，回退到普通模式\n");
            engine->use_fast_mode = 0;
        }
    }
    
    engine->is_init = 1;
    printf("X11渲染引擎初始化成功\n");
    return 0;
}

int render_engine_render_frame(RenderEngine* engine, const uint8_t* rgb888_data) {
    int i;
    uint32_t* dst_pixels = (uint32_t*)engine->display_buffer;
    const uint8_t* src = rgb888_data;
    
    if (!engine || !engine->is_init || !rgb888_data) {
        printf("渲染引擎未初始化或数据无效\n");
        return -1;
    }
    
    /* 高度优化的RGB888到BGRA转换 - 循环展开4次 */
    size_t total_pixels = DISPLAY_WIDTH * DISPLAY_HEIGHT;
    size_t unroll_count = total_pixels / 4;
    size_t remainder = total_pixels % 4;
    
    for (i = 0; i < unroll_count; i++) {
        /* 一次处理4个像素，提高指令级并行 */
        dst_pixels[0] = 0xFF000000 | (src[0] << 16) | (src[1] << 8) | src[2];
        dst_pixels[1] = 0xFF000000 | (src[3] << 16) | (src[4] << 8) | src[5];
        dst_pixels[2] = 0xFF000000 | (src[6] << 16) | (src[7] << 8) | src[8];
        dst_pixels[3] = 0xFF000000 | (src[9] << 16) | (src[10] << 8) | src[11];
        
        dst_pixels += 4;
        src += 12;
    }
    
    /* 处理剩余像素 */
    for (i = 0; i < remainder; i++) {
        *dst_pixels++ = 0xFF000000 | (src[0] << 16) | (src[1] << 8) | src[2];
        src += 3;
    }
    
    /* 根据模式选择不同的显示方法 */
    if (engine->use_shm) {
        /* 共享内存模式：零拷贝直接渲染 */
        XShmPutImage(engine->display, engine->window, engine->gc, engine->ximage,
                    0, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, False);
    } else if (engine->use_fast_mode && engine->pixmap) {
        /* 双缓冲模式：先写入pixmap，再一次性复制到窗口 */
        XPutImage(engine->display, engine->pixmap, engine->gc, engine->ximage, 0, 0, 0, 0,
                 DISPLAY_WIDTH, DISPLAY_HEIGHT);
        XCopyArea(engine->display, engine->pixmap, engine->window, engine->gc, 
                 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, 0);
    } else {
        /* 直接模式：直接写入窗口 */
        XPutImage(engine->display, engine->window, engine->gc, engine->ximage, 0, 0, 0, 0,
                 DISPLAY_WIDTH, DISPLAY_HEIGHT);
    }
    
    /* 每帧都刷新，但不同步等待，确保30帧都能显示 */
    XFlush(engine->display);
    
    return 0;
}

/* 新增：直接从RGB565渲染（跳过RGB888中间格式，最快） */
int render_engine_render_frame_direct(RenderEngine* engine, const uint16_t* rgb565_data) {
    if (!engine || !engine->is_init || !rgb565_data) {
        printf("渲染引擎未初始化或数据无效\n");
        return -1;
    }
    
    /* 直接从RGB565转换到BGRA - 跳过中间步骤 */
    rgb565_to_bgra_direct(rgb565_data, (uint32_t*)engine->display_buffer, DISPLAY_WIDTH * DISPLAY_HEIGHT);
    
    /* 根据模式选择不同的显示方法 */
    if (engine->use_shm) {
        /* 共享内存模式：零拷贝直接渲染 */
        XShmPutImage(engine->display, engine->window, engine->gc, engine->ximage,
                    0, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, False);
    } else if (engine->use_fast_mode && engine->pixmap) {
        /* 双缓冲模式：先写入pixmap，再一次性复制到窗口 */
        XPutImage(engine->display, engine->pixmap, engine->gc, engine->ximage, 0, 0, 0, 0,
                 DISPLAY_WIDTH, DISPLAY_HEIGHT);
        XCopyArea(engine->display, engine->pixmap, engine->window, engine->gc, 
                 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, 0);
    } else {
        /* 直接模式：直接写入窗口 */
        XPutImage(engine->display, engine->window, engine->gc, engine->ximage, 0, 0, 0, 0,
                 DISPLAY_WIDTH, DISPLAY_HEIGHT);
    }
    
    /* 网络接收端不需要显示视频流数字 */
    
    /* 每帧都刷新，但不同步等待，确保30帧都能显示 */
    XFlush(engine->display);
    
    return 0;
}

/* 新增：检查是否有退出事件的函数 */
int render_engine_check_exit_event(RenderEngine* engine) {
    XEvent event;
    
    if (!engine || !engine->is_init) {
        return 1;  /* 如果引擎未初始化，返回退出 */
    }
    
    /* 非阻塞地检查事件 */
    while (XPending(engine->display)) {
        XNextEvent(engine->display, &event);
        switch (event.type) {
            case KeyPress:
            case ButtonPress:
                return 1;  /* 有退出事件 */
            case Expose:
                /* 重绘窗口 */
                XPutImage(engine->display, engine->window, engine->gc, engine->ximage, 0, 0, 0, 0,
                         DISPLAY_WIDTH, DISPLAY_HEIGHT);
                break;
        }
    }
    
    return 0;  /* 没有退出事件 */
}

void render_engine_cleanup(RenderEngine* engine) {
    if (!engine) return;
    
    /* 清理共享内存 */
    if (engine->use_shm && engine->display) {
        if (engine->shminfo.shmaddr) {
            XShmDetach(engine->display, &engine->shminfo);
            shmdt(engine->shminfo.shmaddr);
            engine->shminfo.shmaddr = NULL;
        }
    }
    
    if (engine->font) {
        XFreeFont(engine->display, engine->font);
        engine->font = NULL;
    }
    
    if (engine->pixmap) {
        XFreePixmap(engine->display, engine->pixmap);
        engine->pixmap = 0;
    }
    
    if (engine->ximage) {
        XDestroyImage(engine->ximage);  /* 这会自动释放display_buffer */
        engine->ximage = NULL;
        engine->display_buffer = NULL;  /* 已被XDestroyImage释放 */
    }
    
    if (engine->display_buffer && !engine->use_shm) {
        free(engine->display_buffer);
        engine->display_buffer = NULL;
    }
    
    if (engine->gc) {
        XFreeGC(engine->display, engine->gc);
        engine->gc = 0;
    }
    
    if (engine->window) {
        XDestroyWindow(engine->display, engine->window);
        engine->window = 0;
    }
    
    if (engine->display) {
        XCloseDisplay(engine->display);
        engine->display = NULL;
    }
    
    engine->is_init = 0;
    printf("X11渲染引擎已清理\n");
}

//=========================================================================
// 主函数 - 视频流实时采集与显示
//=========================================================================
int main(void) {
    NetworkReceiver network_receiver;  // UDP网络接收器
    SharedMemoryManager shm_manager;   // 共享内存管理器
    RenderEngine render_engine;
    uint8_t* udp_image_buf;           // UDP接收的图像缓冲区
    uint8_t* pcie_image_buf;          // PCIe共享内存读取的图像缓冲区  
    uint8_t* combined_image_buf;       // 拼接后的图像缓冲区
    uint8_t* display_buf_888;         // 用于显示的RGB888缓冲区
    int frame_count = 0;
    struct timeval stream_start, current_time;
    double total_time;
    uint32_t last_pcie_frame_id = 0;
    int udp_protocol_mode = 0;        // 0=分包传输模式, 1=简单UDP模式
    int no_udp_count = 0;             // UDP无数据计数器
    
    printf("=== FPGA 视频流双通道显示系统 ===\n");
    printf("版本: v3.0 (UDP+PCIe拼接显示版)\n");
    printf("功能: %dp双通道视频流实时显示 (%dx%d)\n", COMBINED_IMAGE_HEIGHT, COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT);
    printf("模式: UDP接收 + PCIe共享内存 + 拼接显示\n");
    printf("监听端口: %d\n", UDP_PORT);
    printf("显示布局: 左侧PCIe(%dx%d) + 右侧UDP(%dx%d)\n", 
           PCIE_IMAGE_WIDTH, PCIE_IMAGE_HEIGHT, UDP_IMAGE_WIDTH, UDP_IMAGE_HEIGHT);
    printf("操作: 按任意键或点击窗口退出\n\n");
    
    /////////////////////////////////////////////////////////////////////////////
    // 内存分配区域
    /////////////////////////////////////////////////////////////////////////////
    /* 分配各种缓冲区 */
    udp_image_buf = (uint8_t*)aligned_alloc(32, UDP_FRAME_BUFFER_SIZE);
    pcie_image_buf = (uint8_t*)aligned_alloc(32, UDP_FRAME_BUFFER_SIZE);  // 与UDP同大小
    combined_image_buf = (uint8_t*)aligned_alloc(32, COMBINED_FRAME_BUFFER_SIZE);
    display_buf_888 = (uint8_t*)aligned_alloc(32, COMBINED_IMAGE_WIDTH * COMBINED_IMAGE_HEIGHT * 3);
    
    /* 如果aligned_alloc失败，回退到普通malloc */
    if (!udp_image_buf) udp_image_buf = (uint8_t*)malloc(UDP_FRAME_BUFFER_SIZE);
    if (!pcie_image_buf) pcie_image_buf = (uint8_t*)malloc(UDP_FRAME_BUFFER_SIZE);
    if (!combined_image_buf) combined_image_buf = (uint8_t*)malloc(COMBINED_FRAME_BUFFER_SIZE);
    if (!display_buf_888) display_buf_888 = (uint8_t*)malloc(COMBINED_IMAGE_WIDTH * COMBINED_IMAGE_HEIGHT * 3);
    
    if (!udp_image_buf || !pcie_image_buf || !combined_image_buf || !display_buf_888) {
        printf("内存分配失败！\n");
        goto cleanup;
    }
    
    /* 初始化缓冲区为黑色 */
    memset(udp_image_buf, 0, UDP_FRAME_BUFFER_SIZE);
    memset(pcie_image_buf, 0, UDP_FRAME_BUFFER_SIZE);
    memset(combined_image_buf, 0, COMBINED_FRAME_BUFFER_SIZE);
    
    /////////////////////////////////////////////////////////////////////////////
    // UDP网络接收初始化区域
    /////////////////////////////////////////////////////////////////////////////
    /* 1. UDP网络接收初始化 */
    printf("=== 第一步：UDP网络接收初始化 ===\n");
    if (network_receiver_init(&network_receiver) != 0) {
        printf("UDP网络接收初始化失败！\n");
        goto cleanup;
    }
    
    /////////////////////////////////////////////////////////////////////////////
    // 共享内存初始化区域
    /////////////////////////////////////////////////////////////////////////////
    /* 2. 共享内存初始化 */
    printf("\n=== 第二步：共享内存初始化 ===\n");
    if (shared_memory_init(&shm_manager, 0) != 0) {  // 0表示读取者
        printf("共享内存初始化失败！继续执行，但PCIe图像将显示为黑色\n");
        memset(&shm_manager, 0, sizeof(shm_manager));  // 清零，后续检查时跳过
    } else {
        printf("共享内存初始化成功，作为PCIe图像数据读取者\n");
    }
    
    /////////////////////////////////////////////////////////////////////////////
    // 图像渲染引擎初始化区域
    /////////////////////////////////////////////////////////////////////////////
    /* 3. 初始化渲染引擎 */
    printf("\n=== 第三步：渲染引擎初始化 ===\n");
    if (render_engine_init(&render_engine) != 0) {
        printf("渲染引擎初始化失败！\n");
        goto cleanup;
    }
    
    /////////////////////////////////////////////////////////////////////////////
    // 双通道视频显示主循环区域
    /////////////////////////////////////////////////////////////////////////////
    printf("\n=== 开始双通道视频显示 ===\n");
    printf("窗口尺寸: %dx%d (左侧PCIe + 右侧UDP)\n", DISPLAY_WIDTH, DISPLAY_HEIGHT);
    printf("UDP监听端口: %d\n", UDP_PORT);
    printf("共享内存键值: 0x%08X\n", SHM_PCIE_IMAGE_KEY);
    printf("支持协议: 分包传输协议 + 简单UDP协议 (自动切换)\n");
    printf("按任意键或点击窗口退出...\n\n");
    
    // 网络状态检查
    printf("=== 网络状态检查 ===\n");
    printf("Socket FD: %d\n", network_receiver.socket_fd);
    printf("帧缓冲区: %p (%zu字节)\n", network_receiver.frame_buffer, network_receiver.buffer_size);
    printf("包缓冲区: %p (%zu字节)\n", network_receiver.packet_buffer, sizeof(network_receiver.packet_buffer));
    printf("重组器状态: %s\n", network_receiver.assembler.is_valid ? "已初始化" : "未初始化");
    printf("开始监听UDP数据...\n\n");
    
    gettimeofday(&stream_start, NULL);
    
    /* 4. 双通道视频主循环 */
    while (1) {
        int has_new_udp_frame = 0;
        int has_new_pcie_frame = 0;
        
        /* 接收UDP图像数据 - 自适应协议模式 */
        int received_size = 0;
        
        if (udp_protocol_mode == 0) {
            // 尝试分包传输模式
            received_size = network_receive_with_sync(&network_receiver);
            
            // 如果长时间没有数据，尝试切换到简单UDP模式
            if (received_size <= 0) {
                no_udp_count++;
                if (no_udp_count >= 200 && !has_received_udp_ever) {  // 约4秒无数据
                    printf("\n=== 分包传输模式无数据，切换到简单UDP模式 ===\n");
                    udp_protocol_mode = 1;
                    no_udp_count = 0;
                }
            } else {
                no_udp_count = 0;  // 重置计数器
            }
        } else {
            // 简单UDP模式
            received_size = network_receive_simple(&network_receiver);
            
            // 如果简单模式也长时间无数据，切换回分包模式重试
            if (received_size <= 0) {
                no_udp_count++;
                if (no_udp_count >= 200) {  // 约4秒无数据
                    printf("\n=== 简单UDP模式无数据，切换回分包传输模式 ===\n");
                    udp_protocol_mode = 0;
                    no_udp_count = 0;
                }
            } else {
                no_udp_count = 0;  // 重置计数器
            }
        }
        
        if (received_size > 0) {
            // 成功接收到UDP图像，复制到UDP缓冲区
            memcpy(udp_image_buf, network_receiver.frame_buffer, UDP_FRAME_BUFFER_SIZE);
            has_new_udp_frame = 1;
            has_received_udp_ever = 1;  // 标记已接收过UDP数据
            
            if (frame_count % 20 == 0) {  // 减少输出频率
                const char* mode_name = (udp_protocol_mode == 0) ? "分包传输" : "简单UDP";
                printf("UDP接收成功[%s]: %d字节 (帧%d)\n", mode_name, received_size, frame_count);
            }
        } else if (received_size < 0) {
            printf("UDP接收严重错误，等待重连\n");
            usleep(10000);  // 等待10ms后继续
        }
        
        /* 读取PCIe共享内存图像数据 */
        if (shm_manager.shm_ptr != NULL) {  // 检查共享内存是否可用
            if (shared_memory_has_new_frame(&shm_manager, last_pcie_frame_id) > 0) {
                uint32_t width, height, frame_id;
                int read_size = shared_memory_read_image(&shm_manager, pcie_image_buf, 
                                                       UDP_FRAME_BUFFER_SIZE,
                                                       &width, &height, &frame_id);
                if (read_size > 0) {
                    last_pcie_frame_id = frame_id;
                    has_new_pcie_frame = 1;
                    
                    if (frame_count % 20 == 0) {  // 减少输出频率
                        printf("PCIe读取成功: %dx%d, %d字节 (帧ID %d)\n", width, height, read_size, frame_id);
                    }
                }
            }
        }
        
        /* 生成测试图像（如果没有UDP数据） */
        if (!has_received_udp_ever && frame_count > 100) {  // 启动后2秒还没有UDP数据
            static int test_pattern_generated = 0;
            if (!test_pattern_generated) {
                printf("\n=== 生成UDP测试图像 ===\n");
                // 生成彩色测试图案 (RGB565格式)
                uint16_t* test_image = (uint16_t*)udp_image_buf;
                for (int y = 0; y < UDP_IMAGE_HEIGHT; y++) {
                    for (int x = 0; x < UDP_IMAGE_WIDTH; x++) {
                        uint16_t color;
                        if (y < UDP_IMAGE_HEIGHT / 3) {
                            // 红色区域
                            color = 0xF800; // 红色 RGB565
                        } else if (y < 2 * UDP_IMAGE_HEIGHT / 3) {
                            // 绿色区域  
                            color = 0x07E0; // 绿色 RGB565
                        } else {
                            // 蓝色区域
                            color = 0x001F; // 蓝色 RGB565
                        }
                        test_image[y * UDP_IMAGE_WIDTH + x] = color;
                    }
                }
                has_received_udp_ever = 1;  // 标记为已有UDP数据
                test_pattern_generated = 1;
                printf("UDP测试图像生成完成\n");
            }
        }
        
        /* 拼接图像并显示 - 强制每帧都渲染以提高帧率 */
        static int force_render_count = 0;
        if (has_new_udp_frame || has_new_pcie_frame || frame_count == 0 || (++force_render_count % 2 == 0)) {
            // 拼接图像：左侧PCIe，右侧UDP（如果从未接收过UDP数据则为NULL，显示黑色）
            combine_images(pcie_image_buf, has_received_udp_ever ? udp_image_buf : NULL, combined_image_buf, 
                          UDP_IMAGE_WIDTH, UDP_IMAGE_HEIGHT);
            
            // 转换为RGB888格式用于显示
            rgb565_to_rgb888((const uint16_t*)combined_image_buf, display_buf_888, 
                           COMBINED_IMAGE_WIDTH * COMBINED_IMAGE_HEIGHT);
            
            /* 渲染显示拼接后的图像 */
            if (render_engine_render_frame(&render_engine, display_buf_888) != 0) {
                printf("渲染失败！\n");
                break;
            }
        }
        
        /* 检查退出事件 */
        if (render_engine_check_exit_event(&render_engine)) {
            printf("检测到退出事件\n");
            break;
        }
        
        frame_count++;
        
        /* 实时FPS计算和性能分析 */
        if (frame_count % 50 == 0) {
            gettimeofday(&current_time, NULL);
            total_time = get_time_diff(stream_start, current_time);
            double current_fps = frame_count / total_time;
            
            const char* display_mode = has_received_udp_ever ? "双通道" : "单通道(仅PCIe)";
            printf("[%s显示] [%d帧] FPS: %.1f | UDP: %s | PCIe: %s\n", 
                   display_mode, frame_count, current_fps, 
                   has_new_udp_frame ? "活跃" : (has_received_udp_ever ? "缓存" : "无数据"),
                   (shm_manager.shm_ptr != NULL) ? "连接" : "断开");
        }
        
        // 添加小延时避免CPU占用过高 - 减少延时提高帧率
        nano_delay(1000000); // 1ms
    }
    
    /////////////////////////////////////////////////////////////////////////////
    // 性能统计和结果保存区域
    /////////////////////////////////////////////////////////////////////////////
    gettimeofday(&current_time, NULL);
    total_time = get_time_diff(stream_start, current_time);
    printf("\n=== 性能统计结果 ===\n");
    printf("总帧数: %d\n", frame_count);
    printf("总时间: %.2f秒\n", total_time);
    printf("平均FPS: %.1f\n", frame_count / total_time);
    printf("双通道显示: UDP + PCIe\n");
    
    /* 保存最后一帧为PPM文件 */
    printf("\n=== 保存最后一帧 ===\n");
    save_frame_to_ppm(display_buf_888, "last_combined_frame.ppm");
    
    /////////////////////////////////////////////////////////////////////////////
    // 系统清理区域
    /////////////////////////////////////////////////////////////////////////////
    /* 5. 清理资源 */
    printf("\n=== 系统清理 ===\n");
    
    /* 清理共享内存 */
    if (shm_manager.shm_ptr != NULL) {
        shared_memory_cleanup(&shm_manager);
    }
    
    render_engine_cleanup(&render_engine);
    network_receiver_cleanup(&network_receiver);
    
cleanup:
    /////////////////////////////////////////////////////////////////////////////
    // 最终清理区域
    /////////////////////////////////////////////////////////////////////////////
    /* 释放所有缓冲区 */
    if (udp_image_buf) {
        free(udp_image_buf);
        udp_image_buf = NULL;
    }
    if (pcie_image_buf) {
        free(pcie_image_buf);
        pcie_image_buf = NULL;
    }
    if (combined_image_buf) {
        free(combined_image_buf);
        combined_image_buf = NULL;
    }
    if (display_buf_888) {
        free(display_buf_888);
        display_buf_888 = NULL;
    }
    
    printf("\n=== 双通道视频显示程序执行完成 ===\n");
    return 0;
}

//=========================================================================
// 图像处理函数实现
//=========================================================================

/**
 * 拼接两个图像：左侧PCIe图像，右侧UDP图像
 * @param pcie_image PCIe图像数据 (640*480 RGB565)
 * @param udp_image UDP图像数据 (640*480 RGB565)
 * @param combined_image 输出拼接后的图像数据 (1280*480 RGB565)
 * @param width 单个图像宽度 (640)
 * @param height 图像高度 (480)
 */
void combine_images(const uint8_t* pcie_image, const uint8_t* udp_image, 
                   uint8_t* combined_image, int width, int height) {
    if (!pcie_image || !combined_image) {
        return;
    }
    
    int single_line_size = width * 2;  // 单个图像每行字节数 (RGB565)
    int combined_line_size = width * 2 * 2;  // 拼接后每行字节数
    
    for (int y = 0; y < height; y++) {
        uint8_t* dst_line = combined_image + y * combined_line_size;
        
        // 左侧放置PCIe图像
        const uint8_t* pcie_line = pcie_image + y * single_line_size;
        memcpy(dst_line, pcie_line, single_line_size);
        
        // 右侧放置UDP图像或黑屏
        if (udp_image) {
            // 有UDP数据时显示UDP图像
            const uint8_t* udp_line = udp_image + y * single_line_size;
            memcpy(dst_line + single_line_size, udp_line, single_line_size);
        } else {
            // 无UDP数据时右侧显示黑色
            memset(dst_line + single_line_size, 0, single_line_size);
        }
    }
}

//=========================================================================
// 分包传输功能实现 - 新增
//=========================================================================

/**
 * 初始化分包重组器
 */
int init_packet_assembler(PacketAssembler* assembler, uint32_t frame_id, 
                         uint16_t total_packets, uint32_t width, uint32_t height) {
    if (!assembler) {
        return -1;
    }
    
    // 清理现有资源
    if (assembler->is_valid) {
        if (assembler->frame_buffer)
            free(assembler->frame_buffer);
        if (assembler->received_mask)
            free(assembler->received_mask);
        assembler->is_valid = 0;
    }
    
    // 分配新资源
    size_t frame_size = width * height * 2; // RGB565
    assembler->frame_buffer = (uint8_t*)malloc(frame_size);
    if (!assembler->frame_buffer) {
        printf("分包重组器: 帧缓冲区分配失败\n");
        return -1;
    }
    
    assembler->received_mask = (uint8_t*)calloc(total_packets, 1);
    if (!assembler->received_mask) {
        printf("分包重组器: 包标记位图分配失败\n");
        free(assembler->frame_buffer);
        assembler->frame_buffer = NULL;
        return -1;
    }
    
    // 初始化参数
    assembler->frame_id = frame_id;
    assembler->total_packets = total_packets;
    assembler->received_packets = 0;
    assembler->image_width = width;
    assembler->image_height = height;
    gettimeofday(&assembler->start_time, NULL);
    assembler->is_valid = 1;
    
    // 清空帧缓冲区
    memset(assembler->frame_buffer, 0, frame_size);
    
    printf("分包重组器初始化: 帧ID=%u, 包数=%u, 尺寸=%ux%u\n", 
           frame_id, total_packets, width, height);
    
    return 0;
}

/**
 * 添加数据包到重组器
 */
int add_packet_to_assembler(PacketAssembler* assembler, const PacketHeader* header,
                           const uint8_t* data) {
    if (!assembler || !assembler->is_valid || !header || !data) {
        return -1;
    }
    
    // 检查帧ID (减少错误输出)
    if (header->frame_id != assembler->frame_id) {
        static int mismatch_count = 0;
        if (++mismatch_count % 100 == 0) {  // 每100次才输出一次
            printf("分包重组器: 包帧ID不匹配: 期望%u, 收到%u (已发生%d次)\n", 
                   assembler->frame_id, header->frame_id, mismatch_count);
        }
        return -1;
    }
    
    // 检查包ID
    if (header->packet_id >= assembler->total_packets) {
        printf("分包重组器: 包ID超出范围: %u >= %u\n", 
               header->packet_id, assembler->total_packets);
        return -1;
    }
    
    // 检查是否已接收
    if (assembler->received_mask[header->packet_id]) {
        printf("分包重组器: 重复包: ID=%u\n", header->packet_id);
        return 0; // 不是错误，只是重复
    }
    
    // 计算偏移地址
    size_t offset = header->packet_id * PACKET_MAX_DATA_SIZE;
    
    // 验证校验和
    uint32_t calc_checksum = calculate_checksum(data, header->data_size);
    if (calc_checksum != header->checksum) {
        printf("分包重组器: 包校验和错误: ID=%u (计算:0x%08X, 期望:0x%08X)\n", 
               header->packet_id, calc_checksum, header->checksum);
        return -1;
    }
    
    // 复制数据
    size_t frame_size = assembler->image_width * assembler->image_height * 2;
    if (offset + header->data_size <= frame_size) {
        memcpy(assembler->frame_buffer + offset, data, header->data_size);
        
        // 更新接收状态
        assembler->received_mask[header->packet_id] = 1;
        assembler->received_packets++;
        
        return 0;
    } else {
        printf("分包重组器: 数据超出范围: offset=%zu, size=%u, frame_size=%zu\n",
               offset, header->data_size, frame_size);
        return -1;
    }
}

/**
 * 检查重组器是否完成
 * 返回: 1=完成, 0=未完成, -1=超时
 */
int check_assembler_complete(PacketAssembler* assembler) {
    if (!assembler || !assembler->is_valid) {
        return -1;
    }
    
    // 检查接收完整性
    if (assembler->received_packets >= assembler->total_packets) {
        return 1;
    }
    
    // 检查超时
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    double elapsed = (current_time.tv_sec - assembler->start_time.tv_sec) + 
                    (current_time.tv_usec - assembler->start_time.tv_usec) / 1000000.0;
    
    // 提高完成度要求，减少条纹图像
    if (elapsed > 2.0) { // 2秒超时（增加等待时间）
        // 如果接收率超过70%，认为基本完成（提高阈值）
        if (assembler->received_packets >= assembler->total_packets * 0.7) {
            printf("超时重组完成: %.1f秒, %u/%u包 (%.1f%%)\n", 
                   elapsed, assembler->received_packets, assembler->total_packets,
                   assembler->received_packets * 100.0 / assembler->total_packets);
            return 1;
        } else if (assembler->received_packets >= 300) {
            // 需要更多绝对包数才重组
            printf("最小包数重组: %.1f秒, %u包\n", elapsed, assembler->received_packets);
            return 1;
        } else {
            return -1; // 真正超时
        }
    }
    
    // 如果接收率达到90%，立即认为完成（提高阈值）
    if (assembler->received_packets >= assembler->total_packets * 0.9) {
        printf("快速重组完成，接收率达到%.1f%%\n", 
               assembler->received_packets * 100.0 / assembler->total_packets);
        return 1;
    }
    
    // 如果收到足够多的绝对包数，也认为可以重组（提高阈值）
    if (assembler->received_packets >= 350) {
        printf("绝对包数重组完成: %u包\n", assembler->received_packets);
        return 1;
    }
    
    return 0; // 还在接收中
}

/**
 * 清理重组器资源
 */
void cleanup_packet_assembler(PacketAssembler* assembler) {
    if (!assembler) {
        return;
    }
    
    if (assembler->frame_buffer) {
        free(assembler->frame_buffer);
        assembler->frame_buffer = NULL;
    }
    
    if (assembler->received_mask) {
        free(assembler->received_mask);
        assembler->received_mask = NULL;
    }
    
    assembler->is_valid = 0;
}

/**
 * 从重组器中获取完整帧
 */
int get_complete_frame(PacketAssembler* assembler, uint8_t* output_buffer, size_t buffer_size) {
    if (!assembler || !assembler->is_valid || !output_buffer) {
        return -1;
    }
    
    // 计算需要的缓冲区大小
    size_t frame_size = assembler->image_width * assembler->image_height * 2;
    
    if (buffer_size < frame_size) {
        printf("分包重组器: 输出缓冲区太小: %zu < %zu\n", buffer_size, frame_size);
        return -1;
    }
    
    // 检查数据完整性 - 计算有效数据的比例
    double valid_ratio = (double)assembler->received_packets / assembler->total_packets;
    if (valid_ratio < 0.3) {
        printf("数据完整性不足，拒绝输出: %.1f%% (%u/%u包)\n", 
               valid_ratio * 100, assembler->received_packets, assembler->total_packets);
        return -1;
    }
    
    // 复制完整帧
    memcpy(output_buffer, assembler->frame_buffer, frame_size);
    
    printf("帧输出成功: %.1f%% 完整性 (%u/%u包)\n", 
           valid_ratio * 100, assembler->received_packets, assembler->total_packets);
    
    return frame_size;
}

/**
 * 使用分包接收协议接收图像数据
 */
int network_receive_with_packets(NetworkReceiver* receiver) {
    PacketHeader* header = (PacketHeader*)receiver->packet_buffer;
    ssize_t received;
    
    if (!receiver || !receiver->frame_buffer) {
        return -1;
    }
    
    // 接收一个UDP包
    received = recvfrom(receiver->socket_fd, receiver->packet_buffer, sizeof(receiver->packet_buffer), 0,
                       (struct sockaddr*)&receiver->client_addr, &receiver->client_len);
    
    if (received < sizeof(PacketHeader)) {
        if (received < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            // 超时检查当前重组状态
            if (receiver->assembler.is_valid) {
                int status = check_assembler_complete(&receiver->assembler);
                if (status == 1) {
                    // 重组完成或接近完成
                    int size = get_complete_frame(&receiver->assembler, receiver->frame_buffer, 
                                               receiver->buffer_size);
                    cleanup_packet_assembler(&receiver->assembler);
                    if (size > 0) {
                        printf("分包传输: 帧重组完成: %d字节\n", size);
                        return size;
                    }
                } else if (status == -1) {
                    // 重组超时
                    cleanup_packet_assembler(&receiver->assembler);
                }
            }
            return 0; // 正常超时，继续接收
        }
        printf("分包传输: 接收错误: %zd\n", received);
        return 0;
    }
    
    // 验证包头魔数
    if (header->magic != PACKET_MAGIC) {
        printf("分包传输: 包魔数错误: 0x%08X\n", header->magic);
        return 0;
    }
    
    // 处理不同类型的包
    switch (header->packet_type) {
        case PACKET_TYPE_START:
            // 帧开始包 - 初始化重组器
            printf("分包传输: 接收到帧开始包: 帧ID=%u, 总包数=%u\n", 
                   header->frame_id, header->total_packets);
            
            if (init_packet_assembler(&receiver->assembler, header->frame_id, 
                                     header->total_packets,
                                     header->image_width, 
                                     header->image_height) != 0) {
                printf("分包传输: 重组器初始化失败\n");
            }
            break;
            
        case PACKET_TYPE_DATA:
            // 数据包 - 添加到重组器
            if (receiver->assembler.is_valid) {
                if (add_packet_to_assembler(&receiver->assembler, header, 
                                          receiver->packet_buffer + sizeof(PacketHeader)) == 0) {
                    // 显示进度(间隔显示)
                    if (receiver->assembler.received_packets % 30 == 0 || 
                        receiver->assembler.received_packets == receiver->assembler.total_packets) {
                        printf("分包传输: 接收进度: %u/%u (%.1f%%)\n", 
                               receiver->assembler.received_packets, receiver->assembler.total_packets,
                               receiver->assembler.received_packets * 100.0 / receiver->assembler.total_packets);
                    }
                    
                    // 检查是否已完成
                    if (check_assembler_complete(&receiver->assembler) == 1) {
                        int size = get_complete_frame(&receiver->assembler, receiver->frame_buffer, 
                                                   receiver->buffer_size);
                        cleanup_packet_assembler(&receiver->assembler);
                        if (size > 0) {
                            printf("分包传输: 帧重组完成: %d字节\n", size);
                            return size;
                        }
                    }
                }
            } else {
                // 没有活跃的重组器，可能是乱序包或第一个包丢失
                printf("分包传输: 收到数据包但无活跃重组器: 帧ID=%u, 包ID=%u/%u\n",
                       header->frame_id, header->packet_id, header->total_packets);
            }
            break;
            
        case PACKET_TYPE_END:
            // 帧结束包 - 验证帧完整性
            printf("分包传输: 接收到帧结束包: 帧ID=%u\n", header->frame_id);
            
            if (receiver->assembler.is_valid && receiver->assembler.frame_id == header->frame_id) {
                int status = check_assembler_complete(&receiver->assembler);
                if (status == 1) {
                    int size = get_complete_frame(&receiver->assembler, receiver->frame_buffer, 
                                               receiver->buffer_size);
                    cleanup_packet_assembler(&receiver->assembler);
                    if (size > 0) {
                        printf("分包传输: 帧接收完成: %d字节\n", size);
                        return size;
                    }
                } else {
                    printf("分包传输: 帧结束但重组未完成: %u/%u包 (%.1f%%)\n",
                           receiver->assembler.received_packets, receiver->assembler.total_packets,
                           receiver->assembler.received_packets * 100.0 / receiver->assembler.total_packets);
                    
                    // 如果接收率超过80%，也可以认为基本完成
                    if (receiver->assembler.received_packets >= receiver->assembler.total_packets * 0.8) {
                        int size = get_complete_frame(&receiver->assembler, receiver->frame_buffer, 
                                                   receiver->buffer_size);
                        cleanup_packet_assembler(&receiver->assembler);
                        if (size > 0) {
                            printf("分包传输: 帧基本完成: %d字节 (%.1f%%)\n", 
                                   size, receiver->assembler.received_packets * 100.0 / receiver->assembler.total_packets);
                            return size;
                        }
                    }
                }
            }
            break;
            
        default:
            printf("分包传输: 未知包类型: %d\n", header->packet_type);
            break;
    }
    
    return 0; // 继续接收
}
