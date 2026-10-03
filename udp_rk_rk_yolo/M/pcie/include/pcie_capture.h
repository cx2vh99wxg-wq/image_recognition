#ifndef PCIE_CAPTURE_H
#define PCIE_CAPTURE_H

#include <stdint.h>
#include <sys/time.h>

#ifdef ENABLE_RENDERING
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#endif

//=========================================================================
// PCIe采集模块头文件
//=========================================================================

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
// 结构体定义
//=========================================================================

// PCIe管理器结构体 - 需要包含原有的PCIE驱动结构
typedef struct {
    int pci_driver_fd;
    void* command_operation;  // 对应 COMMAND_OPERATION
    void* dma_operation;      // 对应 DMA_OPERATION
} PCIEManager;

#ifdef ENABLE_RENDERING
// 渲染引擎结构体
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

// PCIE管理函数
/**
 * 初始化PCIE设备
 * @param manager PCIE管理器
 * @return 0=成功, -1=失败
 */
int pcie_manager_init(PCIEManager* manager);

/**
 * 获取PCIE设备信息
 * @param manager PCIE管理器
 */
void pcie_manager_get_device_info(PCIEManager* manager);

/**
 * 采集一帧图像数据
 * @param manager PCIE管理器
 * @param image_buffer 图像缓冲区
 * @param buffer_size 缓冲区大小
 * @return 0=成功, -1=失败
 */
int pcie_manager_capture_frame(PCIEManager* manager, uint8_t* image_buffer, size_t buffer_size);

/**
 * 初始化视频流采集
 * @param manager PCIE管理器
 * @return 0=成功, -1=失败
 */
int pcie_manager_init_streaming(PCIEManager* manager);

/**
 * 清理视频流采集
 * @param manager PCIE管理器
 */
void pcie_manager_cleanup_streaming(PCIEManager* manager);

/**
 * 清理PCIE设备
 * @param manager PCIE管理器
 */
void pcie_manager_cleanup(PCIEManager* manager);

#ifdef ENABLE_RENDERING
// 渲染引擎函数
/**
 * 初始化渲染引擎
 * @param engine 渲染引擎
 * @return 0=成功, -1=失败
 */
int render_engine_init(RenderEngine* engine);

/**
 * 渲染带检测结果的图像
 * @param engine 渲染引擎
 * @param rgb888_data RGB888格式图像数据
 * @return 0=成功, -1=失败
 */
int render_engine_render_frame_with_detections(RenderEngine* engine, const uint8_t* rgb888_data);

/**
 * 检查退出事件
 * @param engine 渲染引擎
 * @return 1=退出, 0=继续
 */
int render_engine_check_exit_event(RenderEngine* engine);

/**
 * 清理渲染引擎
 * @param engine 渲染引擎
 */
void render_engine_cleanup(RenderEngine* engine);
#endif

// 工具函数
/**
 * 纳秒级延时
 * @param delay 延时时间（纳秒）
 * @return 0=成功
 */
int nano_delay(long delay);

/**
 * RGB565转RGB888格式
 * @param image565 RGB565数据
 * @param image888 RGB888数据输出
 * @param num_pixels 像素数量
 */
void rgb565_to_rgb888(const uint16_t* image565, uint8_t* image888, size_t num_pixels);

/**
 * 保存帧到PPM文件
 * @param rgb888_data RGB888数据
 * @param filename 文件名
 * @return 0=成功, -1=失败
 */
int save_frame_to_ppm(const uint8_t* rgb888_data, const char* filename);

/**
 * 计算时间差
 * @param start 开始时间
 * @param end 结束时间
 * @return 时间差（秒）
 */
double get_time_diff(struct timeval start, struct timeval end);

/**
 * 列重排
 * @param row_buffer 行缓冲区
 * @param width 宽度
 * @param start_col 起始列
 * @param end_col 结束列
 */
void rearrange_columns(uint16_t* row_buffer, int width, int start_col, int end_col);

/**
 * 列向上移动
 * @param image_buffer 图像缓冲区
 * @param width 宽度
 * @param height 高度
 * @param start_col 起始列
 * @param end_col 结束列
 * @param shift_rows 移动行数
 */
void shift_columns_up(uint8_t* image_buffer, int width, int height, int start_col, int end_col, int shift_rows);

//=========================================================================
// 全局变量声明
//=========================================================================
extern uint8_t* image_buf_temp;
extern uint8_t* image_buf_888;
extern int current_video_stream;

#endif // PCIE_CAPTURE_H