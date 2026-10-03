#include "pcie_dma_read_test.h"
#include "yolo_integration.h"
#include "../../include/shared_memory.h"
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
#include <errno.h>


/* 为了使用aligned_alloc，需要定义_ISOC11_SOURCE */
#ifndef _ISOC11_SOURCE
#define _ISOC11_SOURCE
#endif

//=========================================================================
// *作   者：辉哥大盗（PCIE+YOLO整合版）
// *完成时间：2025年10月11日
// *实现功能：图像采集与YOLO实时目标检测（整合PCIE驱动和YOLO识别）
// *性能优化：在图像拼接完成后直接进行YOLO检测和标注绘制
//=========================================================================

// 性能优化配置
#define HIGH_PERFORMANCE_MODE 1  // 1=高性能模式, 0=调试模式

// 预设性能配置 - 选择其中一个
#define PERFORMANCE_PRESET 2     // 1=最高精度, 2=平衡模式, 3=最高帧率

#if PERFORMANCE_PRESET == 1
    // 最高精度模式：每帧检测
    #define YOLO_DETECT_INTERVAL 1
    #define FPS_REPORT_INTERVAL 100
#elif PERFORMANCE_PRESET == 2  
    // 平衡模式：每3帧检测
    #define YOLO_DETECT_INTERVAL 3
    #define FPS_REPORT_INTERVAL 200
#elif PERFORMANCE_PRESET == 3
    // 最高帧率模式：每5帧检测
    #define YOLO_DETECT_INTERVAL 5
    #define FPS_REPORT_INTERVAL 300
#else
    // 自定义模式
    #define YOLO_DETECT_INTERVAL 3   // 可自定义检测间隔
    #define FPS_REPORT_INTERVAL 200  // 可自定义FPS报告间隔
#endif

// 检测结果去重配置
#define ENABLE_DETECTION_DEDUP 1  // 1=启用去重, 0=禁用去重
#ifndef DEDUP_IOU_THRESHOLD
    #define DEDUP_IOU_THRESHOLD 0.8f  // IoU阈值，超过此值的同类别检测框会被合并
#endif

#if HIGH_PERFORMANCE_MODE
    #define PERF_PRINTF(...)  // 高性能模式下关闭调试输出
#else
    #define PERF_PRINTF printf // 调试模式下保留输出
#endif

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
// PCIE管理结构体和渲染引擎结构体的定义
//=========================================================================
typedef struct {
    int pci_driver_fd;
    COMMAND_OPERATION command_operation;
    DMA_OPERATION dma_operation;
} PCIEManager;

// 使用共享内存管理器替换渲染引擎
// typedef struct RenderEngine 已经被 SharedMemoryManager 替换



//=========================================================================
// 函数声明
//=========================================================================
// 错误处理函数
void emergency_stop(const char* error_msg, const char* component);
void cleanup_all_resources(PCIEManager* pcie_mgr, SharedMemoryManager* shm_mgr, YOLOEngine* yolo_eng);

// PCIE管理函数
int pcie_manager_init(PCIEManager* manager);
void pcie_manager_get_device_info(PCIEManager* manager);
int pcie_manager_capture_frame(PCIEManager* manager, uint8_t* image_buffer, size_t buffer_size);
int pcie_manager_init_streaming(PCIEManager* manager);
void pcie_manager_cleanup_streaming(PCIEManager* manager);
void pcie_manager_cleanup(PCIEManager* manager);

// 共享内存管理函数
int shm_manager_init(SharedMemoryManager* manager);
int shm_manager_send_frame_with_detections(SharedMemoryManager* manager, const uint8_t* rgb888_data);
void shm_manager_cleanup(SharedMemoryManager* manager);



// 工具函数
int nano_delay(long delay);
void rgb565_to_rgb888(const uint16_t* image565, uint8_t* image888, size_t num_pixels);
void rgb888_to_rgb565(const uint8_t* rgb888_data, uint16_t* rgb565_data, size_t num_pixels);
int save_frame_to_ppm(const uint8_t* rgb888_data, const char* filename);
double get_time_diff(struct timeval start, struct timeval end);
void rearrange_columns(uint16_t* row_buffer, int width, int start_col, int end_col);
void shift_columns_up(uint8_t* image_buffer, int width, int height, int start_col, int end_col, int shift_rows);

//=========================================================================
// 全局变量定义
//=========================================================================
uint8_t* image_buf_temp;
uint8_t* image_buf_888;
YOLOEngine yolo_engine;

// 全局变量用于信号处理
static volatile int g_running = 1;
static PCIEManager* g_pcie_manager = NULL;
static SharedMemoryManager* g_shm_manager = NULL;
static YOLOEngine* g_yolo_engine = NULL;
static EmergencyBrakeSharedMemory* g_brake_shm = NULL;

//=========================================================================
// 信号处理函数
//=========================================================================
void signal_handler(int signum) {
    g_running = 0;
    printf("\n收到信号 %d，正在安全退出...\n", signum);
    
    if (signum == SIGINT) {
        emergency_stop("用户中断 (Ctrl+C)", "信号处理");
    } else if (signum == SIGTERM) {
        emergency_stop("程序终止信号", "信号处理");
    } else {
        emergency_stop("未知信号", "信号处理");
    }
    
    // 执行紧急清理
    cleanup_all_resources(g_pcie_manager, g_shm_manager, g_yolo_engine);
    
    exit(signum);
}

void setup_signal_handlers() {
    signal(SIGINT, signal_handler);   // Ctrl+C
    signal(SIGTERM, signal_handler);  // 终止信号
    signal(SIGQUIT, signal_handler);  // Ctrl+Backslash
}

//=========================================================================
// PCIE管理函数实现 - PCIE设备管理
//=========================================================================

int pcie_manager_init(PCIEManager* manager) {
    printf("正在初始化PCIE设备...\n");
    
    if (!manager) {
        printf("*** 致命错误：PCIE管理器指针无效 ***\n");
        printf("*** 程序立即停止 ***\n");
        return -1;
    }
    
    // 初始化结构体
    manager->pci_driver_fd = -1;
    memset(&manager->command_operation, 0, sizeof(COMMAND_OPERATION));
    memset(&manager->dma_operation, 0, sizeof(DMA_OPERATION));
    
    // 打开PCIE设备
    manager->pci_driver_fd = open(PCIE_DRIVER_FILE_PATH, O_RDWR);
    if (manager->pci_driver_fd < 0) {
        printf("*** 致命错误：PCIE设备打开失败 ***\n");
        printf("*** 错误原因：%s ***\n", strerror(errno));
        printf("*** 检查项目：***\n");
        printf("*** 1. PCIE驱动是否正确加载 ***\n");
        printf("*** 2. 设备文件路径是否正确：%s ***\n", PCIE_DRIVER_FILE_PATH);
        printf("*** 3. 是否有足够的权限访问设备 ***\n");
        printf("*** 程序立即停止 ***\n");
        return -1;
    }
    
    // 获取设备信息
    pcie_manager_get_device_info(manager);
    
    printf("PCIE设备初始化成功\n");
    return 0;
}

void pcie_manager_get_device_info(PCIEManager* manager) {
    if (!manager || manager->pci_driver_fd < 0) return;
    
    manager->command_operation.w_r = read_num;
    if (ioctl(manager->pci_driver_fd, PCI_READ_DATA_CMD, &manager->command_operation) == 0) {
        printf("PCIE设备信息获取成功\n");
        printf("设备ID: 0x%x, 厂商ID: 0x%x\n", 
               manager->command_operation.get_pci_dev_info.device_id,
               manager->command_operation.get_pci_dev_info.vendor_id);
    } else {
        printf("PCIE设备信息获取失败\n");
    }
}

int pcie_manager_capture_frame(PCIEManager* manager, uint8_t* image_buffer, size_t buffer_size) {
    int i;
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
        /* 设置当前行的偏移地址，相对于帧起始位置 */
        manager->dma_operation.offset_addr = frame_start_offset + i * manager->dma_operation.current_len * 4;
        
        ioctl(manager->pci_driver_fd, PCI_DMA_WRITE_CMD, &manager->dma_operation);
        
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

int pcie_manager_init_streaming(PCIEManager* manager) {
    if (!manager || manager->pci_driver_fd < 0) {
        printf("*** 致命错误：PCIE设备未初始化，无法启动数据流传输 ***\n");
        printf("*** 程序立即停止 ***\n");
        return -1;
    }
    
    printf("初始化视频采集...\n");
    
    /* 设置DMA操作参数（只需要设置一次） */
    /* 每行需要读取: LEADING_PIXELS(前导) + IMAGE_WIDTH(有效) = (LEADING_PIXELS+IMAGE_WIDTH)个像素 * 2字节 = (LEADING_PIXELS+IMAGE_WIDTH)*2字节 / 4 = (LEADING_PIXELS+IMAGE_WIDTH)*2/4 个dword */
    manager->dma_operation.current_len = (LEADING_PIXELS + IMAGE_WIDTH)*2/4;
    manager->dma_operation.offset_addr = 0;
    memset(manager->dma_operation.data.write_buf, 0, DMA_MAX_PACKET_SIZE);
    
    printf("DMA参数配置: current_len=%d dword (%d字节), offset_addr=0x%x\n", 
           manager->dma_operation.current_len, 
           manager->dma_operation.current_len * 4,
           manager->dma_operation.offset_addr);
    
    /* 映射地址（只需要映射一次） */
    printf("正在映射DMA地址...\n");
    if (ioctl(manager->pci_driver_fd, PCI_MAP_ADDR_CMD, &manager->dma_operation) != 0) {
        printf("*** 致命错误：DMA地址映射失败 ***\n");
        printf("*** 错误原因：%s ***\n", strerror(errno));
        printf("*** 检查项目：***\n");
        printf("*** 1. PCIE驱动是否支持DMA操作 ***\n");
        printf("*** 2. DMA缓冲区是否可用 ***\n");
        printf("*** 3. 系统内存是否充足 ***\n");
        printf("*** 程序立即停止 ***\n");
        return -1;
    }
    printf("DMA地址映射完成\n");
    
    /* 重置DMA读取地址到帧起始位置 */
    printf("正在重置DMA读取地址...\n");
    manager->dma_operation.offset_addr = 0;  // 确保从地址0开始
    if (ioctl(manager->pci_driver_fd, PCI_DMA_WRITE_CMD, &manager->dma_operation) != 0) {
        printf("*** 致命错误：DMA读取地址重置失败 ***\n");
        printf("*** 错误原因：%s ***\n", strerror(errno));
        printf("*** 检查项目：***\n");
        printf("*** 1. DMA控制器是否响应 ***\n");
        printf("*** 2. PCIE连接是否稳定 ***\n");
        printf("*** 程序立即停止 ***\n");
        return -1;
    }
    printf("DMA读取地址重置完成\n");
    
    printf("视频采集初始化完成\n");
    return 0;
}

void pcie_manager_cleanup_streaming(PCIEManager* manager) {
    if (manager && manager->pci_driver_fd >= 0) {
        /* 取消映射 */
        ioctl(manager->pci_driver_fd, PCI_UMAP_ADDR_CMD, &manager->dma_operation);
        printf("视频采集已停止\n");
    }
}

void pcie_manager_cleanup(PCIEManager* manager) {
    if (!manager) return;
    
    if (manager->pci_driver_fd >= 0) {
        close(manager->pci_driver_fd);
        manager->pci_driver_fd = -1;
    }
    
    printf("PCIE设备清理完成\n");
}

//=========================================================================
// 共享内存管理函数实现 - 替换X11显示管理
//=========================================================================

int shm_manager_init(SharedMemoryManager* manager) {
    if (!manager) {
        printf("*** 致命错误：共享内存管理器指针无效 ***\n");
        printf("*** 程序立即停止 ***\n");
        return -1;
    }
    
    printf("初始化共享内存管理器...\n");
    
    // 初始化PCIe共享内存管理器作为写入者
    if (shared_memory_init_pcie(manager, 1) != 0) {
        printf("*** 致命错误：共享内存初始化失败 ***\n");
        printf("*** 错误原因：可能的原因包括 ***\n");
        printf("*** 1. 系统共享内存资源不足 ***\n");
        printf("*** 2. 权限不足，无法创建共享内存 ***\n");
        printf("*** 3. 已有相同的共享内存存在且无法删除 ***\n");
        printf("*** 4. 系统不支持System V IPC机制 ***\n");
        printf("*** 建议操作：***\n");
        printf("*** 1. 检查系统共享内存使用情况：ipcs -m ***\n");
        printf("*** 2. 清理遗留的共享内存：ipcrm -m <shm_id> ***\n");
        printf("*** 3. 确保以足够权限运行程序 ***\n");
        printf("*** 程序立即停止 ***\n");
        return -1;
    }
    
    printf("PCIe共享内存管理器初始化成功 - 类型: %d, 作为写入者\n", manager->type);
    return 0;
}

int shm_manager_send_frame_with_detections(SharedMemoryManager* manager, const uint8_t* rgb888_data) {
    if (!manager || !rgb888_data) return -1;
    
    // 将RGB888转换为RGB565格式用于共享内存传输
    static uint16_t* rgb565_buffer = NULL;
    static int first_call = 1;
    
    if (first_call) {
        rgb565_buffer = (uint16_t*)malloc(PCIE_IMAGE_WIDTH * PCIE_IMAGE_HEIGHT * sizeof(uint16_t));
        if (!rgb565_buffer) {
            printf("分配RGB565缓冲区失败\n");
            return -1;
        }
        first_call = 0;
    }
    
    // RGB888转RGB565
    rgb888_to_rgb565(rgb888_data, rgb565_buffer, PCIE_IMAGE_WIDTH * PCIE_IMAGE_HEIGHT);
    
    // 写入共享内存
    if (shared_memory_write_image(manager, (uint8_t*)rgb565_buffer, 
                                 PCIE_IMAGE_WIDTH, PCIE_IMAGE_HEIGHT) != 0) {
        printf("PCIe共享内存写入失败\n");
        return -1;
    }
    
    // 添加调试信息：确认写入的是PCIe共享内存
    static int debug_count = 0;
    if (++debug_count % 100 == 0) {
        printf("PCIe程序写入帧%d到共享内存类型:%d (应该是1)\n", debug_count, manager->type);
    }
    
    return 0;
}

void shm_manager_cleanup(SharedMemoryManager* manager) {
    if (!manager) return;
    
    shared_memory_cleanup(manager);
    printf("共享内存管理器清理完成\n");
}

//=========================================================================
// 错误处理函数实现
//=========================================================================

void emergency_stop(const char* error_msg, const char* component) {
    printf("\n");
    printf("█████████████████████████████████████████████████████████\n");
    printf("██                 紧急停止                            ██\n");
    printf("█████████████████████████████████████████████████████████\n");
    printf("** 组件: %s\n", component ? component : "未知");
    printf("** 错误: %s\n", error_msg ? error_msg : "未指定错误");
    printf("** 时间: ");
    
    // 打印当前时间
    time_t now;
    time(&now);
    printf("%s", ctime(&now));
    
    printf("** 操作: 立即停止所有正在运行的程序\n");
    printf("█████████████████████████████████████████████████████████\n");
    printf("\n");
    
    // 立即刷新输出缓冲区
    fflush(stdout);
    fflush(stderr);
}

void cleanup_all_resources(PCIEManager* pcie_mgr, SharedMemoryManager* shm_mgr, YOLOEngine* yolo_eng) {
    printf("\n=== 紧急资源清理 ===\n");
    
    // 清理紧急制动共享内存
    if (g_brake_shm) {
        printf("清理紧急制动共享内存...\n");
        emergency_brake_shm_cleanup(g_brake_shm, 1);
        g_brake_shm = NULL;
    }
    
    // 清理共享内存管理器
    if (shm_mgr) {
        printf("清理共享内存管理器...\n");
        shm_manager_cleanup(shm_mgr);
    }
    
    // 清理PCIE流传输
    if (pcie_mgr) {
        printf("清理PCIE流传输...\n");
        pcie_manager_cleanup_streaming(pcie_mgr);
    }
    
    // 清理YOLO引擎
    if (yolo_eng) {
        printf("清理YOLO引擎...\n");
        yolo_engine_cleanup(yolo_eng);
    }
    
    // 清理PCIE管理器
    if (pcie_mgr) {
        printf("清理PCIE管理器...\n");
        pcie_manager_cleanup(pcie_mgr);
    }
    
    // 清理全局缓冲区
    if (image_buf_temp) {
        printf("释放临时图像缓冲区...\n");
        free(image_buf_temp);
        image_buf_temp = NULL;
    }
    if (image_buf_888) {
        printf("释放RGB888图像缓冲区...\n");
        free(image_buf_888);
        image_buf_888 = NULL;
    }
    
    printf("紧急资源清理完成\n");
}

//=========================================================================
// 工具函数实现
//=========================================================================

int nano_delay(long delay) {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = delay;
    return nanosleep(&ts, NULL);
}

void rgb565_to_rgb888(const uint16_t* image565, uint8_t* image888, size_t num_pixels) {
    if (!image565 || !image888) return;
    
    for (size_t i = 0; i < num_pixels; i++) {
        uint16_t pixel = image565[i];
        
        // 提取RGB分量 (5-6-5格式)
        uint8_t r = (pixel >> 11) & 0x1F;
        uint8_t g = (pixel >> 5) & 0x3F;
        uint8_t b = pixel & 0x1F;
        
        // 扩展到8位
        image888[i * 3] = (r << 3) | (r >> 2);         // R
        image888[i * 3 + 1] = (g << 2) | (g >> 4);     // G
        image888[i * 3 + 2] = (b << 3) | (b >> 2);     // B
    }
}

void rgb888_to_rgb565(const uint8_t* rgb888_data, uint16_t* rgb565_data, size_t num_pixels) {
    if (!rgb888_data || !rgb565_data) return;
    
    for (size_t i = 0; i < num_pixels; i++) {
        uint8_t r = rgb888_data[i * 3];
        uint8_t g = rgb888_data[i * 3 + 1];
        uint8_t b = rgb888_data[i * 3 + 2];
        
        // 压缩到5-6-5格式
        uint16_t r5 = (r >> 3) & 0x1F;        // 5位红色
        uint16_t g6 = (g >> 2) & 0x3F;        // 6位绿色  
        uint16_t b5 = (b >> 3) & 0x1F;        // 5位蓝色
        
        // 组合成RGB565格式
        rgb565_data[i] = (r5 << 11) | (g6 << 5) | b5;
    }
}



int save_frame_to_ppm(const uint8_t* rgb888_data, const char* filename) {
    if (!rgb888_data || !filename) return -1;
    
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        printf("无法创建文件: %s\n", filename);
        return -1;
    }
    
    fprintf(fp, "P6\n%d %d\n255\n", DISPLAY_WIDTH, DISPLAY_HEIGHT);
    fwrite(rgb888_data, 1, DISPLAY_WIDTH * DISPLAY_HEIGHT * 3, fp);
    fclose(fp);
    
    printf("帧已保存到: %s\n", filename);
    return 0;
}

double get_time_diff(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
}

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

//=========================================================================
// 网络传输函数实现
//=========================================================================









//=========================================================================
// MAIN函数 - 整合PCIE视频流和YOLO检测
//=========================================================================

int main() {
    printf("=== PCIE图像采集 + YOLO目标检测 + 紧急制动整合系统 ===\n");
    
    PCIEManager pcie_manager;
    SharedMemoryManager shm_manager;  // 使用共享内存管理器替换渲染引擎
    detect_result_group_t detection_results;
    
    struct timeval stream_start, current_time;
    int frame_count = 0;
    double total_time;
    
    // 设置全局指针用于信号处理
    g_pcie_manager = &pcie_manager;
    g_shm_manager = &shm_manager;
    g_yolo_engine = &yolo_engine;
    
    // 设置信号处理程序
    setup_signal_handlers();
    
    /////////////////////////////////////////////////////////////////////////////
    // 系统初始化区域
    /////////////////////////////////////////////////////////////////////////////
    
    /* 1. 分配图像缓冲区 */
    printf("\n=== 内存分配 ===\n");
    size_t temp_buffer_size = (LEADING_PIXELS + IMAGE_WIDTH) * IMAGE_HEIGHT * 2;
    size_t rgb888_buffer_size = IMAGE_WIDTH * IMAGE_HEIGHT * 3;
    
    image_buf_temp = (uint8_t*)aligned_alloc(32, temp_buffer_size);
    if (!image_buf_temp) {
        printf("临时图像缓冲区分配失败！\n");
        goto cleanup;
    }
    
    image_buf_888 = (uint8_t*)aligned_alloc(32, rgb888_buffer_size);
    if (!image_buf_888) {
        printf("RGB888图像缓冲区分配失败！\n");
        goto cleanup;
    }
    
    printf("缓冲区分配成功: 临时=%zu字节, RGB888=%zu字节\n", 
           temp_buffer_size, rgb888_buffer_size);
    
    /* 3. 初始化PCIE管理器 */
    printf("\n=== PCIE设备初始化 ===\n");
    if (pcie_manager_init(&pcie_manager) != 0) {
        emergency_stop("PCIE设备初始化失败", "PCIE管理器");
        printf("!!! 这是系统启动的第一个关键步骤，必须成功 !!!\n");
        printf("!!! 请检查PCIE驱动和硬件连接 !!!\n");
        cleanup_all_resources(&pcie_manager, NULL, NULL);
        return -1;
    }
    printf("✓ PCIE设备初始化成功\n");
    
    /* 4. 初始化YOLO引擎 */
    printf("\n=== YOLO引擎初始化 ===\n");
    if (yolo_engine_init(&yolo_engine, YOLO_MODEL_PATH, YOLO_LABELS_PATH) != 0) {
        emergency_stop("YOLO引擎初始化失败", "YOLO引擎");
        printf("!!! 检查模型文件和标签文件是否存在 !!!\n");
        printf("!!! 模型路径: %s !!!\n", YOLO_MODEL_PATH);
        printf("!!! 标签路径: %s !!!\n", YOLO_LABELS_PATH);
        cleanup_all_resources(&pcie_manager, NULL, &yolo_engine);
        return -1;
    }
    printf("✓ YOLO引擎初始化成功\n");
    
    /* 5. 初始化PCIE图像传输 */
    printf("\n=== 图像传输初始化 ===\n");
    if (pcie_manager_init_streaming(&pcie_manager) != 0) {
        emergency_stop("PCIE图像传输初始化失败", "PCIE流传输");
        printf("!!! DMA操作或地址映射出现问题 !!!\n");
        printf("!!! 请检查PCIE连接和驱动状态 !!!\n");
        cleanup_all_resources(&pcie_manager, NULL, &yolo_engine);
        return -1;
    }
    printf("✓ 图像传输初始化成功\n");
    
    /* 6. 初始化共享内存管理器 */
    printf("\n=== 共享内存管理器初始化 ===\n");
    if (shm_manager_init(&shm_manager) != 0) {
        emergency_stop("共享内存管理器初始化失败", "共享内存管理器");
        printf("!!! 这通常是由于前面的PCIE初始化问题导致的连锁反应 !!!\n");
        printf("!!! 或者系统共享内存资源不足 !!!\n");
        cleanup_all_resources(&pcie_manager, &shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ 共享内存管理器初始化成功\n");
    
    /* 7. 初始化紧急制动共享内存 */
    printf("\n=== 紧急制动共享内存初始化 ===\n");
    if (emergency_brake_shm_init(&g_brake_shm, 1) != 0) {
        emergency_stop("紧急制动共享内存初始化失败", "紧急制动系统");
        printf("!!! 无法创建人员检测与FSPI制动通信通道 !!!\n");
        cleanup_all_resources(&pcie_manager, &shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ 紧急制动共享内存初始化成功\n");
    
    printf("\n=== 系统初始化完成 ===\n");
    printf("图像尺寸: %dx%d\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    printf("共享内存尺寸: %dx%d\n", PCIE_IMAGE_WIDTH, PCIE_IMAGE_HEIGHT);
    printf("YOLO模型: %s\n", YOLO_MODEL_PATH);
    printf("紧急制动系统: 已启用，检测到人员时将通知FSPI程序紧急制动\n");
    printf("使用Ctrl+C退出程序...\n\n");
    
    gettimeofday(&stream_start, NULL);
    
    /////////////////////////////////////////////////////////////////////////////
    // 主处理循环 - PCIE视频流 + YOLO检测
    /////////////////////////////////////////////////////////////////////////////
    
    printf("=== 开始图像处理和目标检测 ===\n");
    
    // 性能配置信息
    printf("=== 性能配置 ===\n");
    #if PERFORMANCE_PRESET == 1
        printf("性能模式: 最高精度 (每帧检测)\n");
    #elif PERFORMANCE_PRESET == 2
        printf("性能模式: 平衡模式 (每%d帧检测)\n", YOLO_DETECT_INTERVAL);
    #elif PERFORMANCE_PRESET == 3
        printf("性能模式: 最高帧率 (每%d帧检测)\n", YOLO_DETECT_INTERVAL);
    #else
        printf("性能模式: 自定义 (每%d帧检测)\n", YOLO_DETECT_INTERVAL);
    #endif
    printf("高性能模式: %s\n", HIGH_PERFORMANCE_MODE ? "启用" : "禁用");
    printf("FPS报告间隔: %d帧\n", FPS_REPORT_INTERVAL);
    printf("检测去重: %s (IoU阈值: %.2f)\n", 
           ENABLE_DETECTION_DEDUP ? "启用" : "禁用", DEDUP_IOU_THRESHOLD);
    
    // 测试文字渲染支持
    printf("字体支持: 数字0-9, 字母A-Z, 符号. %% space\n");
    printf("常见COCO类别: bottle, person, car, chair, cup, book等\n");
    
    while (g_running) {
        /* 步骤1: 从PCIE采集一帧数据 */
        if (pcie_manager_capture_frame(&pcie_manager, image_buf_temp, temp_buffer_size) != 0) {
            emergency_stop("PCIE帧采集失败", "PCIE数据采集");
            printf("!!! 可能的原因: !!!\n");
            printf("!!! 1. PCIE连接中断 !!!\n");
            printf("!!! 2. DMA传输错误 !!!\n");
            printf("!!! 3. 硬件设备异常 !!!\n");
            cleanup_all_resources(&pcie_manager, &shm_manager, &yolo_engine);
            return -1;
        }
        
        /* 步骤2: 将RGB565转换为RGB888格式 */
        rgb565_to_rgb888((uint16_t*)image_buf_temp, image_buf_888, IMAGE_WIDTH * IMAGE_HEIGHT);
        
        /* 步骤3: 使用YOLO进行目标检测 (通过宏控制检测间隔) */
        static detect_result_group_t cached_results = {0};
        
        if (frame_count % YOLO_DETECT_INTERVAL == 0) {
            // 按宏定义的间隔执行检测
            memset(&detection_results, 0, sizeof(detection_results));
            if (yolo_engine_detect(&yolo_engine, image_buf_888, IMAGE_WIDTH, IMAGE_HEIGHT, 
                                  &detection_results) == 0) {
                // 缓存检测结果
                cached_results = detection_results;
                
                #if !HIGH_PERFORMANCE_MODE
                if (detection_results.count > 0) {
                    printf("[帧%d] 检测到 %d 个目标 (间隔%d帧)\n", 
                           frame_count, detection_results.count, YOLO_DETECT_INTERVAL);
                }
                #endif
            } else {
                printf("警告: YOLO检测失败，使用上一次检测结果\n");
                detection_results = cached_results;
            }
        } else {
            // 使用缓存的检测结果
            detection_results = cached_results;
        }
        
        // 检查是否有人员检测并更新紧急制动状态
        int person_detected = 0;
        int person_count = 0;
        int person_box_left = 0, person_box_top = 0, person_box_right = 0, person_box_bottom = 0;
        static int last_person_detected = -1;  // 记录上次检测状态
        static int consecutive_person_frames = 0;  // 连续检测到人的帧数

        if (detection_results.count > 0) {
            // 检查检测结果中是否包含"person"
            for (int i = 0; i < detection_results.count; i++) {
                if (strcmp(detection_results.results[i].name, "person") == 0) {
                    person_detected = 1;
                    person_count++;
                    // 保存第一个检测到的人的框位置
                    if (person_count == 1) {
                        person_box_left = detection_results.results[i].box.left;
                        person_box_top = detection_results.results[i].box.top;
                        person_box_right = detection_results.results[i].box.right;
                        person_box_bottom = detection_results.results[i].box.bottom;
                    }
                }
            }

            // 绘制检测结果
            draw_detection_results(image_buf_888, IMAGE_WIDTH, IMAGE_HEIGHT, &detection_results);
        }
        
        // 人员检测状态变化时的详细打印
        if (person_detected != last_person_detected) {
            // 获取当前时间
            time_t current_time = time(NULL);
            struct tm* tm_info = localtime(&current_time);
            char time_str[64];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
            
            if (person_detected) {
                printf("\n🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨\n");
                printf("🚨                 人员检测警报                   🚨\n");
                printf("🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨\n");
                printf("⏰ 检测时间: %s\n", time_str);
                printf("👥 检测到人员数量: %d 人\n", person_count);
                printf("📍 检测帧编号: %d\n", frame_count);
                printf("🎯 YOLO目标总数: %d 个\n", detection_results.count);
                printf("🚨 紧急制动状态: 已通知FSPI程序触发制动\n");
                printf("⚠️  安全提醒: 检测区域内发现人员，车辆将自动制动！\n");
                printf("🔄 监控状态: 持续监控人员位置...\n");
                printf("🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨🚨\n\n");
                consecutive_person_frames = 1;
            } else {
                printf("\n✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅\n");
                printf("✅               人员检测解除                   ✅\n");
                printf("✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅\n");
                printf("⏰ 解除时间: %s\n", time_str);
                printf("👤 人员状态: 已离开检测区域\n");
                printf("📍 解除帧编号: %d\n", frame_count);
                printf("⏱️  持续检测帧数: %d 帧\n", consecutive_person_frames);
                printf("✅ 紧急制动状态: 已通知FSPI程序解除制动\n");
                printf("🚗 车辆状态: 可以恢复正常运行\n");
                printf("🔄 监控状态: 继续监控区域安全...\n");
                printf("✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅✅\n\n");
                consecutive_person_frames = 0;
            }
        } else if (person_detected) {
            // 如果连续检测到人员，增加帧计数
            consecutive_person_frames++;
            // 每隔50帧打印一次提醒
            if (consecutive_person_frames % 50 == 0) {
                printf("⚠️  持续人员检测: 已连续检测到人员 %d 帧 (约%.1f秒)\n", 
                       consecutive_person_frames, consecutive_person_frames * YOLO_DETECT_INTERVAL / 30.0);
            }
        }
        
        // 更新紧急制动共享内存状态
        if (g_brake_shm) {
            emergency_brake_update_status(g_brake_shm, person_detected,
                                        person_box_left, person_box_top,
                                        person_box_right, person_box_bottom);
        }
        
        // 更新上次检测状态
        last_person_detected = person_detected;
        
        /* 步骤4: 通过共享内存发送带有检测结果的图像 */
        if (shm_manager_send_frame_with_detections(&shm_manager, image_buf_888) != 0) {
            emergency_stop("共享内存发送失败", "共享内存传输");
            printf("!!! 可能的原因: !!!\n");
            printf("!!! 1. 共享内存被其他进程破坏 !!!\n");
            printf("!!! 2. 系统内存不足 !!!\n");
            printf("!!! 3. 权限被更改 !!!\n");
            cleanup_all_resources(&pcie_manager, &shm_manager, &yolo_engine);
            return -1;
        }
        

        
        /* 步骤6: 简单的程序运行控制（可以添加信号处理或其他退出条件） */
        // 由于不使用X11，这里可以添加其他退出条件，例如运行指定帧数后退出
        // 目前保持无限运行，可以通过Ctrl+C终止
        
        frame_count++;
        
        /* 实时FPS计算和性能分析 (通过宏控制报告频率) */
        if (frame_count % FPS_REPORT_INTERVAL == 0) {
            gettimeofday(&current_time, NULL);
            total_time = get_time_diff(stream_start, current_time);
            double current_fps = frame_count / total_time;
            
            printf("[%d帧] FPS: %.1f | 检测数: %d | 人员检测: %s\n", 
                   frame_count, current_fps, detection_results.count, 
                   person_detected ? "是" : "否");
        }
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
    
    /* 保存最后一帧为PPM文件 */
    printf("\n=== 保存最后一帧 ===\n");
    save_frame_to_ppm(image_buf_888, "last_frame_with_detections.ppm");
    
    /////////////////////////////////////////////////////////////////////////////
    // 系统清理区域
    /////////////////////////////////////////////////////////////////////////////
    printf("\n=== 系统清理 ===\n");
    
    // 清理紧急制动共享内存
    if (g_brake_shm) {
        emergency_brake_shm_cleanup(g_brake_shm, 1);
        g_brake_shm = NULL;
    }
    
    shm_manager_cleanup(&shm_manager);
    pcie_manager_cleanup_streaming(&pcie_manager);
    yolo_engine_cleanup(&yolo_engine);
    pcie_manager_cleanup(&pcie_manager);
    
cleanup:
    /* 释放全局缓冲区 */
    if (image_buf_temp) {
        free(image_buf_temp);
        image_buf_temp = NULL;
    }
    if (image_buf_888) {
        free(image_buf_888);
        image_buf_888 = NULL;
    }
    
    printf("\n=== PCIE+YOLO+紧急制动整合系统执行完成 ===\n");
    return 0;
}