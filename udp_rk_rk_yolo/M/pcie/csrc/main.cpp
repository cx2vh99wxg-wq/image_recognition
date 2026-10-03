#include "pcie_dma_read_test.h"
#include "fspi_module.h"
#include "yolo_integration.h"
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
#include <errno.h>

// 可选的渲染功能 - 通过宏控制
#define ENABLE_RENDERING 1  // 1=启用渲染, 0=禁用渲染（仅PCIe采集）

#if ENABLE_RENDERING
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

/* 为了使用aligned_alloc，需要定义_ISOC11_SOURCE */
#ifndef _ISOC11_SOURCE
#define _ISOC11_SOURCE
#endif

//=========================================================================
// *作   者：辉哥大盗（PCIE模块）
// *完成时间：2025年10月17日
// *实现功能：PCIe视频流采集与YOLO实时目标检测（可选渲染功能）
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

#if ENABLE_RENDERING
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
#endif

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

#if ENABLE_RENDERING
// 渲染引擎函数
int render_engine_init(RenderEngine* engine);
int render_engine_render_frame_with_detections(RenderEngine* engine, const uint8_t* rgb888_data);
int render_engine_check_exit_event(RenderEngine* engine);
void render_engine_cleanup(RenderEngine* engine);
#endif

// 工具函数
int nano_delay(long delay);
void rgb565_to_rgb888(const uint16_t* image565, uint8_t* image888, size_t num_pixels);
void rgb888_to_rgb565(const uint8_t* image888, uint16_t* image565, size_t num_pixels);
int save_frame_to_ppm(const uint8_t* rgb888_data, const char* filename);
double get_time_diff(struct timeval start, struct timeval end);
void rearrange_columns(uint16_t* row_buffer, int width, int start_col, int end_col);
void shift_columns_up(uint8_t* image_buffer, int width, int height, int start_col, int end_col, int shift_rows);

//=========================================================================
// 全局变量定义
//=========================================================================
uint8_t* image_buf_temp;    // RGB565原始数据缓冲区
uint8_t* image_buf_888;     // RGB888处理缓冲区
uint8_t* image_buf_565_out; // RGB565输出缓冲区（用于共享内存）
YOLOEngine yolo_engine;

// 当前视频流跟踪变量
int current_video_stream = 5;  // 固定视频流5

// 视频流切换相关变量
int available_streams[] = {2,5,6};        
int stream_count = 3;                 
int current_stream_index = 0;         
struct timeval last_switch_time;

// 全局变量用于信号处理
static volatile int g_running = 1;
static PCIEManager* g_pcie_manager = NULL;
static SharedMemoryManager* g_shm_manager = NULL;
static YOLOEngine* g_yolo_engine = NULL;
#if ENABLE_RENDERING
static RenderEngine* g_render_engine = NULL;
#endif

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
    signal(SIGQUIT, signal_handler);  // Ctrl+反斜杠
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
    
    printf("初始化视频流采集...\n");
    
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
    
    printf("视频流采集初始化完成\n");
    return 0;
}

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

#if ENABLE_RENDERING
//=========================================================================
// 渲染引擎函数实现 - X11图形渲染
//=========================================================================

int render_engine_init(RenderEngine* engine) {
    if (!engine) return -1;
    
    printf("正在初始化渲染引擎...\n");
    
    // 初始化结构体
    memset(engine, 0, sizeof(RenderEngine));
    
    // 检查DISPLAY环境变量
    char* display_env = getenv("DISPLAY");
    printf("DISPLAY环境变量: %s\n", display_env ? display_env : "未设置");
    
    if (!display_env) {
        printf("警告: DISPLAY环境变量未设置，尝试使用默认值 :0\n");
        setenv("DISPLAY", ":0", 0);
    }
    
    // 连接X服务器
    printf("尝试连接X服务器...\n");
    engine->display = XOpenDisplay(NULL);
    if (!engine->display) {
        printf("✗ 错误: 无法连接到X服务器\n");
        printf("可能的原因:\n");
        printf("1. X11服务未运行 (检查: ps aux | grep Xorg)\n");
        printf("2. DISPLAY变量错误 (当前: %s)\n", getenv("DISPLAY"));
        printf("3. SSH连接缺少X11转发 (使用: ssh -X)\n");
        printf("4. 权限问题 (尝试: xhost +local:)\n");
        printf("建议修复:\n");
        printf("- 本地运行: export DISPLAY=:0\n");
        printf("- SSH连接: ssh -X user@host\n");
        printf("- 虚拟显示: Xvfb :1 -screen 0 1024x768x24 &; export DISPLAY=:1\n");
        return -1;
    }
    printf("✓ X服务器连接成功\n");
    
    // 获取默认屏幕
    int screen = DefaultScreen(engine->display);
    int screen_width = DisplayWidth(engine->display, screen);
    int screen_height = DisplayHeight(engine->display, screen);
    int depth = DefaultDepth(engine->display, screen);
    
    printf("屏幕信息: %dx%d, 深度: %d位, 屏幕: %d\n", 
           screen_width, screen_height, depth, screen);
    
    // 创建窗口
    printf("创建窗口: %dx%d...\n", DISPLAY_WIDTH, DISPLAY_HEIGHT);
    engine->window = XCreateSimpleWindow(
        engine->display,
        RootWindow(engine->display, screen),
        100, 100, DISPLAY_WIDTH, DISPLAY_HEIGHT, 2,
        BlackPixel(engine->display, screen),
        WhitePixel(engine->display, screen)
    );
    
    if (!engine->window) {
        printf("✗ 错误: 窗口创建失败\n");
        XCloseDisplay(engine->display);
        return -1;
    }
    printf("✓ 窗口创建成功\n");
    
    // 设置窗口属性
    printf("设置窗口属性...\n");
    XSelectInput(engine->display, engine->window, 
                ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);
    
    printf("映射窗口到屏幕...\n");
    XMapWindow(engine->display, engine->window);
    XStoreName(engine->display, engine->window, "PCIE+YOLO 目标检测系统");
    XFlush(engine->display);
    
    // 等待窗口映射完成
    printf("等待窗口显示...\n");
    XEvent event;
    while (1) {
        XNextEvent(engine->display, &event);
        if (event.type == MapNotify) {
            printf("✓ 窗口已成功显示\n");
            break;
        }
        if (event.type == Expose) {
            printf("✓ 窗口准备就绪\n");
            break;
        }
    }
    
    // 创建图形上下文
    engine->gc = XCreateGC(engine->display, engine->window, 0, NULL);
    
    // 创建双缓冲用的Pixmap
    engine->pixmap = XCreatePixmap(engine->display, engine->window, 
                                  DISPLAY_WIDTH, DISPLAY_HEIGHT, 
                                  DefaultDepth(engine->display, screen));
    
    // 分配显示缓冲区
    engine->display_buffer = (char*)malloc(DISPLAY_WIDTH * DISPLAY_HEIGHT * 4);
    if (!engine->display_buffer) {
        printf("渲染缓冲区分配失败\n");
        render_engine_cleanup(engine);
        return -1;
    }
    
    // 创建XImage
    engine->ximage = XCreateImage(
        engine->display,
        DefaultVisual(engine->display, screen),
        DefaultDepth(engine->display, screen),
        ZPixmap, 0,
        engine->display_buffer,
        DISPLAY_WIDTH, DISPLAY_HEIGHT,
        32, DISPLAY_WIDTH * 4
    );
    
    if (!engine->ximage) {
        printf("XImage创建失败\n");
        render_engine_cleanup(engine);
        return -1;
    }
    
    // 加载字体
    engine->font = XLoadQueryFont(engine->display, "fixed");
    if (engine->font) {
        XSetFont(engine->display, engine->gc, engine->font->fid);
    }
    
    engine->is_init = 1;
    printf("渲染引擎初始化成功\n");
    return 0;
}

int render_engine_render_frame_with_detections(RenderEngine* engine, const uint8_t* rgb888_data) {
    if (!engine || !engine->is_init || !rgb888_data) return -1;
    
    // 将RGB888数据转换为X11显示格式（RGBA）
    for (int i = 0; i < DISPLAY_WIDTH * DISPLAY_HEIGHT; i++) {
        engine->display_buffer[i * 4 + 0] = rgb888_data[i * 3 + 0]; // R 红色通道
        engine->display_buffer[i * 4 + 1] = rgb888_data[i * 3 + 1]; // G 绿色通道  
        engine->display_buffer[i * 4 + 2] = rgb888_data[i * 3 + 2]; // B 蓝色通道
        engine->display_buffer[i * 4 + 3] = 255; // A 透明度通道
    }
    
    // 将图像数据绘制到pixmap
    XPutImage(engine->display, engine->pixmap, engine->gc, engine->ximage,
              0, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    
    // 将pixmap复制到窗口
    XCopyArea(engine->display, engine->pixmap, engine->window, engine->gc,
              0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0, 0);
    
    XFlush(engine->display);
    return 0;
}

int render_engine_check_exit_event(RenderEngine* engine) {
    if (!engine || !engine->is_init) return 1;
    
    XEvent event;
    while (XPending(engine->display)) {
        XNextEvent(engine->display, &event);
        
        switch (event.type) {
            case KeyPress:
                return 1; // 任意键退出
            case ButtonPress:
                return 1; // 任意鼠标按键退出
            case ClientMessage:
                return 1; // 窗口关闭事件
        }
    }
    
    return 0; // 继续运行
}

void render_engine_cleanup(RenderEngine* engine) {
    if (!engine) return;
    
    if (engine->is_init) {
        if (engine->font) {
            XFreeFont(engine->display, engine->font);
        }
        if (engine->ximage) {
            engine->ximage->data = NULL; // 防止XDestroyImage释放我们的缓冲区
            XDestroyImage(engine->ximage);
        }
        if (engine->display_buffer) {
            free(engine->display_buffer);
        }
        if (engine->pixmap) {
            XFreePixmap(engine->display, engine->pixmap);
        }
        if (engine->gc) {
            XFreeGC(engine->display, engine->gc);
        }
        if (engine->window) {
            XDestroyWindow(engine->display, engine->window);
        }
        if (engine->display) {
            XCloseDisplay(engine->display);
        }
    }
    
    engine->is_init = 0;
    printf("渲染引擎清理完成\n");
}
#endif

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
    
#if ENABLE_RENDERING
    // 清理渲染引擎
    if (g_render_engine) {
        printf("清理渲染引擎...\n");
        render_engine_cleanup(g_render_engine);
    }
#endif
    
    // 清理PCIE流传输
    if (pcie_mgr) {
        printf("清理PCIE流传输...\n");
        pcie_manager_cleanup_streaming(pcie_mgr);
    }
    
    // 清理共享内存管理器
    if (shm_mgr) {
        printf("清理共享内存管理器...\n");
        shared_memory_cleanup(shm_mgr);
    }

    // 清理弯道检测共享内存
    printf("清理弯道检测共享内存...\n");
    curve_detection_shm_cleanup();

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
    if (image_buf_565_out) {
        printf("释放RGB565输出缓冲区...\n");
        free(image_buf_565_out);
        image_buf_565_out = NULL;
    }
    
    // 清理FSPI资源
    printf("清理FSPI资源...\n");
    fspi_cleanup();
    
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

void rgb888_to_rgb565(const uint8_t* image888, uint16_t* image565, size_t num_pixels) {
    if (!image888 || !image565) return;
    
    for (size_t i = 0; i < num_pixels; i++) {
        // 获取RGB888分量
        uint8_t r = image888[i * 3];     // R
        uint8_t g = image888[i * 3 + 1]; // G
        uint8_t b = image888[i * 3 + 2]; // B
        
        // 转换到5-6-5格式
        uint16_t r565 = (r >> 3) & 0x1F;   // 5位红色
        uint16_t g565 = (g >> 2) & 0x3F;   // 6位绿色
        uint16_t b565 = (b >> 3) & 0x1F;   // 5位蓝色
        
        // 组合成RGB565格式
        image565[i] = (r565 << 11) | (g565 << 5) | b565;
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
    int row, col;
    uint8_t* temp_buffer;
    int col_count = end_col - start_col + 1;
    
    if (!image_buffer || width <= 0 || height <= 0 || 
        start_col < 0 || end_col >= width || start_col > end_col || 
        shift_rows <= 0 || shift_rows >= height) {
        return;
    }
    
    // 分配临时缓冲区保存要移动的像素数据（RGB888格式，每个像素3字节）
    temp_buffer = (uint8_t*)malloc(col_count * shift_rows * 3);
    if (!temp_buffer) {
        printf("列垂直移动失败：内存分配失败\n");
        return;
    }
    
    // 保存顶部shift_rows行中指定列的数据
    for (row = 0; row < shift_rows; row++) {
        for (col = start_col; col <= end_col; col++) {
            int src_offset = (row * width + col) * 3;
            int temp_offset = (row * col_count + (col - start_col)) * 3;
            memcpy(temp_buffer + temp_offset, image_buffer + src_offset, 3);
        }
    }
    
    // 将下面的行向上移动
    for (row = shift_rows; row < height; row++) {
        for (col = start_col; col <= end_col; col++) {
            int src_offset = (row * width + col) * 3;
            int dst_offset = ((row - shift_rows) * width + col) * 3;
            memcpy(image_buffer + dst_offset, image_buffer + src_offset, 3);
        }
    }
    
    // 将保存的数据填充到底部
    for (row = height - shift_rows; row < height; row++) {
        for (col = start_col; col <= end_col; col++) {
            int temp_offset = ((row - (height - shift_rows)) * col_count + (col - start_col)) * 3;
            int dst_offset = (row * width + col) * 3;
            memcpy(image_buffer + dst_offset, temp_buffer + temp_offset, 3);
        }
    }
    
    free(temp_buffer);
}

//=========================================================================
// 主函数 - PCIe采集与可选渲染
//=========================================================================

int main() {
    PCIEManager pcie_manager;
#if ENABLE_RENDERING
    RenderEngine render_engine;
#endif
    yolopv2_result_t yolopv2_results;
    SharedMemoryManager pcie_shm_manager;  // PCIe共享内存管理器
    
    int frame_count = 0;
    struct timeval stream_start, current_time;
    double total_time;
    size_t temp_buffer_size = (LEADING_PIXELS + IMAGE_WIDTH) * IMAGE_HEIGHT * 2;
    
    // 声明路径变量，避免goto跨越初始化
    const char* model_path = "model/yolopv2_Nx3x480x640_rk3568.rknn";
    const char* labels_path = NULL;  // YOLOPv2不需要标签文件
    
    // 设置全局指针用于信号处理
    g_pcie_manager = &pcie_manager;
    g_shm_manager = &pcie_shm_manager;
    g_yolo_engine = &yolo_engine;
#if ENABLE_RENDERING
    g_render_engine = &render_engine;
#endif
    
    // 设置信号处理程序
    setup_signal_handlers();
    
    printf("\n==============================\n");
    printf("PCIE+YOLOPv2车道线检测系统启动\n");
    printf("==============================\n");
    printf("编译时间: %s %s\n", __DATE__, __TIME__);
    printf("渲染功能: %s\n", ENABLE_RENDERING ? "启用" : "禁用");
    printf("==============================\n\n");
    
    /////////////////////////////////////////////////////////////////////////////
    // 系统初始化区域
    /////////////////////////////////////////////////////////////////////////////
    
    /* 1. 分配图像缓冲区 */
    printf("=== 缓冲区分配 ===\n");
    image_buf_temp = (uint8_t*)aligned_alloc(32, temp_buffer_size);
    image_buf_888 = (uint8_t*)aligned_alloc(32, IMAGE_WIDTH * IMAGE_HEIGHT * 3);
    image_buf_565_out = (uint8_t*)aligned_alloc(32, IMAGE_WIDTH * IMAGE_HEIGHT * 2);
    
    if (!image_buf_temp || !image_buf_888 || !image_buf_565_out) {
        emergency_stop("图像缓冲区分配失败", "内存分配");
        printf("!!! 系统内存不足，无法分配图像处理缓冲区 !!!\n");
        cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
        return -1;
    }
    
    printf("RGB565原始缓冲区: %zu bytes\n", temp_buffer_size);
    printf("RGB888处理缓冲区: %d bytes\n", IMAGE_WIDTH * IMAGE_HEIGHT * 3);
    printf("RGB565输出缓冲区: %d bytes\n", IMAGE_WIDTH * IMAGE_HEIGHT * 2);
    
    /* 2. 初始化FSPI模块 */
    printf("\n=== FSPI模块初始化 ===\n");
    if (fspi_init() != 0) {
        emergency_stop("FSPI模块初始化失败", "FSPI模块");
        printf("!!! 检查FSPI硬件连接和驱动 !!!\n");
        cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ FSPI模块初始化成功\n");
    
    /* 3. 初始化PCIE管理器 */
    printf("\n=== PCIE设备初始化 ===\n");
    if (pcie_manager_init(&pcie_manager) != 0) {
        emergency_stop("PCIE设备初始化失败", "PCIE管理器");
        printf("!!! 这是系统启动的关键步骤，必须成功 !!!\n");
        printf("!!! 请检查PCIE驱动和硬件连接 !!!\n");
        cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ PCIE设备初始化成功\n");
    
    /* 4. 初始化YOLOPv2引擎 */
    printf("\n=== YOLOPv2引擎初始化 ===\n");

    if (yolo_engine_init(&yolo_engine, model_path, labels_path) != 0) {
        emergency_stop("YOLOPv2引擎初始化失败", "YOLOPv2引擎");
        printf("!!! 检查模型文件是否存在 !!!\n");
        printf("!!! 模型路径: %s !!!\n", model_path);
        cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ YOLOPv2引擎初始化成功\n");
    
    /* 5. 初始化PCIe共享内存 */
    printf("\n=== PCIe共享内存初始化 ===\n");
    if (shared_memory_init(&pcie_shm_manager, SHM_TYPE_PCIE, 1) != 0) {
        emergency_stop("PCIe共享内存初始化失败", "共享内存管理器");
        printf("!!! 系统共享内存资源不足或权限问题 !!!\n");
        printf("!!! 建议操作：ipcs -m 查看共享内存使用情况 !!!\n");
        cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ PCIe共享内存初始化成功\n");

    /* 5.5. 初始化弯道检测共享内存 */
    printf("\n=== 弯道检测共享内存初始化 ===\n");
    if (curve_detection_shm_init(1) != 0) {  // 1 = 作为写入者创建
        emergency_stop("弯道检测共享内存初始化失败", "弯道检测共享内存");
        printf("!!! 系统共享内存资源不足或权限问题 !!!\n");
        cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ 弯道检测共享内存初始化成功\n");

    /* 6. 初始化视频流采集 */
    printf("\n=== 视频流采集初始化 ===\n");
    if (pcie_manager_init_streaming(&pcie_manager) != 0) {
        emergency_stop("视频流采集初始化失败", "PCIE流传输");
        printf("!!! DMA操作或地址映射出现问题 !!!\n");
        printf("!!! 请检查PCIE连接和驱动状态 !!!\n");
        cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ 视频流采集初始化成功\n");

#if ENABLE_RENDERING
    /* 7. 初始化渲染引擎 */
    printf("\n=== 渲染引擎初始化 ===\n");
    if (render_engine_init(&render_engine) != 0) {
        emergency_stop("渲染引擎初始化失败", "X11渲染引擎");
        printf("!!! X11显示系统无法正常工作 !!!\n");
        printf("!!! 程序将继续运行但无法显示图像 !!!\n");
        // 注意：渲染失败不应该导致整个程序退出，因为核心功能仍可工作
        cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ 渲染引擎初始化成功\n");
#endif
    
    printf("=== 系统初始化完成 ===\n");
    printf("图像尺寸: %dx%d\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    printf("窗口尺寸: %dx%d\n", DISPLAY_WIDTH, DISPLAY_HEIGHT);
    printf("YOLOPv2模型: %s\n", model_path);
#if ENABLE_RENDERING
    printf("按任意键或点击窗口退出...\n\n");
#else
    printf("按Ctrl+C退出...\n\n");
#endif
    
    gettimeofday(&stream_start, NULL);
    last_switch_time = stream_start;
    
    /////////////////////////////////////////////////////////////////////////////
    // 主处理循环 - PCIE视频流 + YOLOPv2车道线和可行驶区域检测
    /////////////////////////////////////////////////////////////////////////////

    printf("=== 开始视频流处理和车道线检测 ===\n");
    
    // 在处理循环开始前切换到当前视频流
    printf("切换到视频流 %d...\n", current_video_stream);
    if (switch_video_stream(current_video_stream) == 0) {
        printf("视频流切换成功: 当前流%d\n", current_video_stream);
    } else {
        printf("视频流切换失败，使用默认视频流\n");
    }
    
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
    printf("检测目标: 车道线和可行驶区域\n");
    
    while (g_running) {
        // printf("[DEBUG] ===== 帧 %d 开始 =====\n", frame_count);
        /* 步骤1: 从PCIE采集一帧数据 */
        if (pcie_manager_capture_frame(&pcie_manager, image_buf_temp, temp_buffer_size) != 0) {
            emergency_stop("PCIE帧采集失败", "PCIE数据采集");
            printf("!!! 可能的原因: !!!\n");
            printf("!!! 1. PCIE连接中断 !!!\n");
            printf("!!! 2. DMA传输错误 !!!\n");
            printf("!!! 3. 硬件设备异常 !!!\n");
            cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
            return -1;
        }

        /* 步骤2: 将RGB565转换为RGB888格式 */
        // printf("[DEBUG] RGB565转RGB888开始\n");
        rgb565_to_rgb888((uint16_t*)image_buf_temp, image_buf_888, IMAGE_WIDTH * IMAGE_HEIGHT);
        // printf("[DEBUG] RGB565转RGB888完成\n");

        /* 步骤3: 使用YOLOPv2进行车道线和可行驶区域检测 (通过宏控制检测间隔) */
        static int last_detect_frame = -YOLO_DETECT_INTERVAL; // 上次检测的帧号

        if (frame_count - last_detect_frame >= YOLO_DETECT_INTERVAL) {
            // printf("[DEBUG] 进入检测逻辑: frame_count=%d\n", frame_count);
            // 按宏定义的间隔执行检测
            memset(&yolopv2_results, 0, sizeof(yolopv2_results));
            // printf("[DEBUG] 调用yolo_engine_detect...\n");
            if (yolo_engine_detect(&yolo_engine, image_buf_888, IMAGE_WIDTH, IMAGE_HEIGHT,
                                  &yolopv2_results) == 0) {

                #if !HIGH_PERFORMANCE_MODE
                printf("[帧%d] YOLOPv2检测完成 (间隔%d帧)\n",
                       frame_count, YOLO_DETECT_INTERVAL);
                #endif

                // 立即绘制车道线和可行驶区域到RGB888图像上
                // printf("[DEBUG] 检查结果指针: lane=%p, drivable=%p\n",
                //        yolopv2_results.lane_line_data, yolopv2_results.drivable_area_data);
                if (yolopv2_results.lane_line_data && yolopv2_results.drivable_area_data) {
                    // printf("[DEBUG] 调用draw_lane_and_drivable...\n");
                    draw_lane_and_drivable(image_buf_888, IMAGE_WIDTH, IMAGE_HEIGHT, &yolopv2_results);
                    // printf("[DEBUG] draw_lane_and_drivable完成\n");
                }

                last_detect_frame = frame_count;
            } else {
                printf("警告: YOLOPv2检测失败\n");
            }
            // printf("[DEBUG] 检测逻辑完成\n");
        }
        // 注意：如果不在检测间隔，直接使用原始图像，不绘制车道线

        /* 步骤3.5: 将带有车道线和可行驶区域的RGB888数据转换为RGB565格式并写入PCIe共享内存供UDP传输 */
        // 转换RGB888回RGB565格式到专用的输出缓冲区（避免与原始数据缓冲区冲突）
        // printf("[DEBUG] RGB888转RGB565开始\n");
        rgb888_to_rgb565(image_buf_888, (uint16_t*)image_buf_565_out, IMAGE_WIDTH * IMAGE_HEIGHT);
        // printf("[DEBUG] RGB888转RGB565完成\n");

        // 写入RGB565格式的带检测结果的图像数据到共享内存
        // printf("[DEBUG] 写入共享内存开始\n");
        if (pcie_shared_memory_write_frame(&pcie_shm_manager,
                                          image_buf_565_out,
                                          IMAGE_WIDTH, IMAGE_HEIGHT) != 0) {
            printf("警告: 写入PCIe共享内存失败，但继续处理下一帧\n");
        }
        // printf("[DEBUG] 写入共享内存完成\n");

#if ENABLE_RENDERING
        /* 步骤4: 渲染带有检测结果的图像 */
        // printf("[DEBUG] 渲染开始\n");
        if (render_engine_render_frame_with_detections(&render_engine, image_buf_888) != 0) {
            printf("警告: 渲染失败，但继续处理下一帧\n");
        }
        // printf("[DEBUG] 渲染完成\n");

        /* 步骤5: 检查退出事件 */
        // printf("[DEBUG] 检查退出事件\n");
        if (render_engine_check_exit_event(&render_engine)) {
            printf("检测到退出事件\n");
            break;
        }
        // printf("[DEBUG] 退出事件检查完成\n");
#endif

        frame_count++;
        // printf("[DEBUG] ===== 帧 %d 结束 =====\n", frame_count - 1);

        /* 实时FPS计算和性能分析 (通过宏控制报告频率) */
        if (frame_count % FPS_REPORT_INTERVAL == 0) {
            gettimeofday(&current_time, NULL);
            total_time = get_time_diff(stream_start, current_time);
            double current_fps = frame_count / total_time;

            printf("[视频流%d] [%d帧] FPS: %.1f | 车道线检测中\n",
                   current_video_stream, frame_count, current_fps);
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
    save_frame_to_ppm(image_buf_888, "last_frame_with_lane_detection.ppm");
    
    /////////////////////////////////////////////////////////////////////////////
    // 系统清理区域
    /////////////////////////////////////////////////////////////////////////////
    printf("\n=== 系统清理 ===\n");

#if ENABLE_RENDERING
    render_engine_cleanup(&render_engine);
#endif
    pcie_manager_cleanup_streaming(&pcie_manager);
    shared_memory_cleanup(&pcie_shm_manager);
    curve_detection_shm_cleanup();  // 清理弯道检测共享内存
    yolo_engine_cleanup(&yolo_engine);
    pcie_manager_cleanup(&pcie_manager);
    
    /* 释放全局缓冲区 */
    if (image_buf_temp) {
        free(image_buf_temp);
        image_buf_temp = NULL;
    }
    if (image_buf_888) {
        free(image_buf_888);
        image_buf_888 = NULL;
    }
    if (image_buf_565_out) {
        free(image_buf_565_out);
        image_buf_565_out = NULL;
    }
    
    /* 清理FSPI资源 */
    fspi_cleanup();
    
    printf("\n=== PCIE+YOLOPv2车道线检测系统执行完成 ===\n");
    return 0;
}