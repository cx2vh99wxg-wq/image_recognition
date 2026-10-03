#include "shared_memory.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XShm.h>  /* MIT-SHM共享内存扩展 */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/* 为了使用aligned_alloc，需要定义_ISOC11_SOURCE */
#ifndef _ISOC11_SOURCE
#define _ISOC11_SOURCE
#endif

//=========================================================================
// *作   者：辉哥大盗（C语言重构版：X11双通道渲染程序）
// *完成时间：2025年10月14日
// *实现功能：读取PCIe和UDP共享内存，合成1280×480图像显示
// *性能优化：专注于渲染显示，支持双缓冲和高性能显示
//=========================================================================

//=========================================================================
// 显示配置
//=========================================================================
#define PCIE_IMAGE_WIDTH 640
#define PCIE_IMAGE_HEIGHT 480
#define UDP_IMAGE_WIDTH 640
#define UDP_IMAGE_HEIGHT 480
#define COMBINED_IMAGE_WIDTH  (PCIE_IMAGE_WIDTH + UDP_IMAGE_WIDTH)  // 1280
#define COMBINED_IMAGE_HEIGHT PCIE_IMAGE_HEIGHT  // 480
#define FRAME_SIZE (COMBINED_IMAGE_WIDTH * COMBINED_IMAGE_HEIGHT * 2) // RGB565格式

// 单个摄像头区块尺寸
#define CAMERA_BLOCK_WIDTH 320
#define CAMERA_BLOCK_HEIGHT 240

// 使用头文件中定义的DisplayMode枚举

//=========================================================================
// 切换动画结构体
//=========================================================================
typedef struct {
    int is_active;              /* 是否正在播放动画 */
    DisplayMode from_mode;      /* 源显示模式 */
    DisplayMode to_mode;        /* 目标显示模式 */
    float progress;             /* 动画进度 (0.0 - 1.0) */
    int animation_frames;       /* 动画总帧数 */
    int current_frame;          /* 当前动画帧 */
    struct timeval start_time;  /* 动画开始时间 */
    float scale_factor;         /* 当前缩放因子 */
    int show_text;              /* 是否显示文字 */
    int text_frames;            /* 文字显示帧数 */
    uint64_t text_start_time;   /* 文字显示开始时间 */
    uint64_t text_duration_ms;  /* 文字显示持续时间(毫秒) */
} TransitionAnimation;

//=========================================================================
// 渲染引擎结构体
//=========================================================================
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
    TransitionAnimation transition; /* 切换动画状态 */
} RenderEngine;

//=========================================================================
// 全局变量
//=========================================================================
static volatile int keep_running = 1;
static SharedMemoryManager pcie_shm_manager;
static SharedMemoryManager udp_shm_manager;
static DisplayControlSharedMemory* display_control_shm = NULL;  // 显示控制共享内存
static EmergencyBrakeSharedMemory* emergency_brake_shm = NULL;  // 紧急制动共享内存

// PNG图像数据
static uint8_t* car_png_data = NULL;
static int car_png_width = 0;
static int car_png_height = 0;
static int car_png_channels = 0;

// 红光闪烁状态
static uint64_t last_blink_time = 0;
static int blink_state = 1;

// 弯道检测状态
static CurveType current_curve_type = CURVE_NONE;
static uint32_t current_curve_confidence = 0;

//=========================================================================
// 函数声明
//=========================================================================
// 渲染引擎函数
int render_engine_init(RenderEngine* engine);
int render_engine_render_combined_frame(RenderEngine* engine, 
                                       const uint8_t* pcie_data, 
                                       const uint8_t* udp_data);
int render_engine_render_camera_view(RenderEngine* engine, 
                                    const uint8_t* pcie_data, 
                                    const uint8_t* udp_data,
                                    DisplayMode mode);
int render_engine_check_exit_event(RenderEngine* engine);
void render_engine_cleanup(RenderEngine* engine);

// 切换动画函数
void transition_animation_init(TransitionAnimation* anim);
void transition_animation_start(TransitionAnimation* anim, DisplayMode from_mode, DisplayMode to_mode);
int transition_animation_update(TransitionAnimation* anim);
int render_engine_render_with_transition(RenderEngine* engine, 
                                        const uint8_t* pcie_data, 
                                        const uint8_t* udp_data,
                                        DisplayMode current_mode);

// 信号处理函数
void signal_handler(int sig);

// 工具函数
double get_time_diff(struct timeval start, struct timeval end);
uint64_t get_current_time_ms(void);
void rgb565_to_rgb888(const uint16_t* rgb565_data, uint8_t* rgb888_data, size_t pixel_count);
void save_combined_frame_to_ppm(const uint8_t* pcie_data, const uint8_t* udp_data, const char* filename);
void render_camera_block(uint32_t* display_pixels, const uint16_t* source_pixels,
                        int src_x, int src_y, int dst_x, int dst_y);
int load_car_png(const char* filename);
void render_car_png(uint32_t* display_pixels, int dst_x, int dst_y, int dst_width, int dst_height);
void draw_lane_lines(uint32_t* display_pixels, int dst_x, int dst_y, int dst_width, int dst_height,
                     CurveType curve_type, uint32_t confidence);

void render_scaled_camera_block(uint32_t* display_pixels, const uint16_t* source_pixels,
                              int src_x, int src_y, float scale_factor, int center_x, int center_y);
void get_camera_position(DisplayMode mode, int* src_x, int* src_y);
const char* get_camera_mode_name(DisplayMode mode);
void draw_text_overlay(RenderEngine* engine, const char* text);

//=========================================================================
// 信号处理函数
//=========================================================================
void signal_handler(int sig) {
    printf("\n接收到信号 %d，正在关闭渲染程序...\n", sig);
    keep_running = 0;
}

//=========================================================================
// 工具函数实现
//=========================================================================
double get_time_diff(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
}

uint64_t get_current_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
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

void save_combined_frame_to_ppm(const uint8_t* pcie_data, const uint8_t* udp_data, const char* filename) {
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        perror("无法创建PPM文件");
        return;
    }
    
    // 分配合成图像缓冲区
    uint8_t* combined_rgb565 = (uint8_t*)malloc(FRAME_SIZE);
    uint8_t* combined_rgb888 = (uint8_t*)malloc(COMBINED_IMAGE_WIDTH * COMBINED_IMAGE_HEIGHT * 3);
    if (!combined_rgb565 || !combined_rgb888) {
        fclose(fp);
        free(combined_rgb565);
        free(combined_rgb888);
        return;
    }
    
    // 合成图像：PCIe在左侧，UDP在右侧
    for (int y = 0; y < COMBINED_IMAGE_HEIGHT; y++) {
        // 复制PCIe数据到左侧
        memcpy(combined_rgb565 + y * COMBINED_IMAGE_WIDTH * 2,
               pcie_data + y * PCIE_IMAGE_WIDTH * 2,
               PCIE_IMAGE_WIDTH * 2);
        
        // 复制UDP数据到右侧
        memcpy(combined_rgb565 + y * COMBINED_IMAGE_WIDTH * 2 + PCIE_IMAGE_WIDTH * 2,
               udp_data + y * UDP_IMAGE_WIDTH * 2,
               UDP_IMAGE_WIDTH * 2);
    }
    
    // 转换为RGB888格式
    rgb565_to_rgb888((const uint16_t*)combined_rgb565, combined_rgb888, 
                     COMBINED_IMAGE_WIDTH * COMBINED_IMAGE_HEIGHT);
    
    // 写入PPM头
    fprintf(fp, "P6\n%d %d\n255\n", COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT);
    
    // 写入像素数据
    fwrite(combined_rgb888, 3, COMBINED_IMAGE_WIDTH * COMBINED_IMAGE_HEIGHT, fp);
    
    fclose(fp);
    free(combined_rgb565);
    free(combined_rgb888);
    printf("合成图像已保存为: %s\n", filename);
}

void render_camera_block(uint32_t* display_pixels, const uint16_t* source_pixels, 
                        int src_x, int src_y, int dst_x, int dst_y) {
    // 缩放参数：将320x240放大到640x480 (2倍缩放)
    const int scale_factor = 2;
    const int scaled_width = CAMERA_BLOCK_WIDTH * scale_factor;   // 640
    const int scaled_height = CAMERA_BLOCK_HEIGHT * scale_factor; // 480
    
    // 计算居中位置 - 将放大后的640x480图像居中显示在1280x480的屏幕上
    int center_x = (COMBINED_IMAGE_WIDTH - scaled_width) / 2;
    int center_y = (COMBINED_IMAGE_HEIGHT - scaled_height) / 2;
    
    // 使用最近邻插值进行2倍放大
    for (int y = 0; y < scaled_height; y++) {
        for (int x = 0; x < scaled_width; x++) {
            // 计算原始图像中对应的像素位置
            int orig_x = x / scale_factor;
            int orig_y = y / scale_factor;
            
            // 从源图像读取像素
            int source_idx = (src_y + orig_y) * (src_x >= 640 ? UDP_IMAGE_WIDTH : PCIE_IMAGE_WIDTH) + (src_x + orig_x);
            uint16_t pixel = source_pixels[source_idx];
            
            // BGR565转RGB888 (修正颜色通道顺序)
            uint8_t b = ((pixel >> 11) & 0x1F) << 3;
            uint8_t g = ((pixel >> 5) & 0x3F) << 2;
            uint8_t r = (pixel & 0x1F) << 3;
            
            // 组合为32位ARGB格式
            uint32_t argb = 0xFF000000 | (r << 16) | (g << 8) | b;
            
            // 写入显示缓冲区（居中位置，放大后的坐标）
            int display_x = center_x + x;
            int display_y = center_y + y;
            if (display_x >= 0 && display_x < COMBINED_IMAGE_WIDTH && 
                display_y >= 0 && display_y < COMBINED_IMAGE_HEIGHT) {
                display_pixels[display_y * COMBINED_IMAGE_WIDTH + display_x] = argb;
            }
        }
    }
}

void get_camera_position(DisplayMode mode, int* src_x, int* src_y) {
    switch (mode) {
        case DISPLAY_MODE_PCIE_TOP_LEFT:
            *src_x = 0; *src_y = 0; break;
        case DISPLAY_MODE_PCIE_TOP_RIGHT:
            *src_x = 320; *src_y = 0; break;
        case DISPLAY_MODE_PCIE_BOTTOM_LEFT:
            *src_x = 0; *src_y = 240; break;
        case DISPLAY_MODE_PCIE_BOTTOM_RIGHT:
            *src_x = 320; *src_y = 240; break;
        case DISPLAY_MODE_UDP_TOP_LEFT:
            *src_x = 640; *src_y = 0; break;
        case DISPLAY_MODE_UDP_TOP_RIGHT:
            *src_x = 960; *src_y = 0; break;
        case DISPLAY_MODE_UDP_BOTTOM_LEFT:
            *src_x = 640; *src_y = 240; break;
        case DISPLAY_MODE_UDP_BOTTOM_RIGHT:
            *src_x = 960; *src_y = 240; break;
        default:
            *src_x = 0; *src_y = 0; break;
    }
}



void render_scaled_camera_block(uint32_t* display_pixels, const uint16_t* source_pixels,
                              int src_x, int src_y, float scale_factor, int center_x, int center_y) {
    // 计算缩放后的尺寸
    int scaled_width = (int)(CAMERA_BLOCK_WIDTH * scale_factor);
    int scaled_height = (int)(CAMERA_BLOCK_HEIGHT * scale_factor);
    
    // 计算起始位置（居中）
    int start_x = center_x - scaled_width / 2;
    int start_y = center_y - scaled_height / 2;
    
    for (int y = 0; y < scaled_height; y++) {
        for (int x = 0; x < scaled_width; x++) {
            // 计算原始图像中对应的像素位置（双线性插值的简化版本）
            float orig_x = (float)x / scale_factor;
            float orig_y = (float)y / scale_factor;
            
            // 计算在源图像中的位置
            int pixel_x, pixel_y;
            if (src_x >= 640) {
                // UDP摄像头：src_x是绝对位置，需要转换为相对于UDP图像的位置
                pixel_x = (src_x - 640) + (int)orig_x;
                pixel_y = src_y + (int)orig_y;
            } else {
                // PCIe摄像头：src_x已经是相对位置
                pixel_x = src_x + (int)orig_x;
                pixel_y = src_y + (int)orig_y;
            }
            
            // 边界检查
            if (pixel_x >= 0 && pixel_x < (src_x >= 640 ? UDP_IMAGE_WIDTH : PCIE_IMAGE_WIDTH) && 
                pixel_y >= 0 && pixel_y < (src_x >= 640 ? UDP_IMAGE_HEIGHT : PCIE_IMAGE_HEIGHT)) {
                
                int source_idx = pixel_y * (src_x >= 640 ? UDP_IMAGE_WIDTH : PCIE_IMAGE_WIDTH) + pixel_x;
                uint16_t pixel = source_pixels[source_idx];
                
                // BGR565转RGB888
                uint8_t b = ((pixel >> 11) & 0x1F) << 3;
                uint8_t g = ((pixel >> 5) & 0x3F) << 2;
                uint8_t r = (pixel & 0x1F) << 3;
                
                // 组合为32位ARGB格式
                uint32_t argb = 0xFF000000 | (r << 16) | (g << 8) | b;
                
                // 写入显示缓冲区
                int display_x = start_x + x;
                int display_y = start_y + y;
                if (display_x >= 0 && display_x < COMBINED_IMAGE_WIDTH && 
                    display_y >= 0 && display_y < COMBINED_IMAGE_HEIGHT) {
                    display_pixels[display_y * COMBINED_IMAGE_WIDTH + display_x] = argb;
                }
            }
        }
    }
}

const char* get_camera_mode_name(DisplayMode mode) {
    switch (mode) {
        case DISPLAY_MODE_PCIE_TOP_LEFT: return "Rear Center (0x11)";
        case DISPLAY_MODE_PCIE_TOP_RIGHT: return "Rear Right (0x22)";
        case DISPLAY_MODE_PCIE_BOTTOM_LEFT: return "Rear Left (0x33)";
        case DISPLAY_MODE_PCIE_BOTTOM_RIGHT: return "Rear Center (0x44)";
        case DISPLAY_MODE_UDP_TOP_LEFT: return "Front Center (0x55)";
        case DISPLAY_MODE_UDP_TOP_RIGHT: return "Front Right (0x66)";
        case DISPLAY_MODE_UDP_BOTTOM_LEFT: return "Front Left (0x77)";
        case DISPLAY_MODE_UDP_BOTTOM_RIGHT: return "Front Center (0x88)";
        case DISPLAY_MODE_ALL_CAMERAS: return "Surround View (0x99)";
        default: return "Unknown Mode";
    }
}

void draw_screen_flash(RenderEngine* engine, float alpha) {
    if (!engine || !engine->is_init || alpha <= 0.0f) {
        return;
    }
    
    // 限制alpha值在0-1之间
    if (alpha > 1.0f) alpha = 1.0f;
    
    uint32_t* display_pixels = (uint32_t*)engine->display_buffer;
    
    // 创建白色闪烁效果，alpha控制透明度
    uint8_t flash_intensity = (uint8_t)(255 * alpha);
    uint32_t flash_color = 0xFF000000 | (flash_intensity << 16) | (flash_intensity << 8) | flash_intensity;
    
    // 在整个屏幕上叠加白色闪烁效果
    for (int i = 0; i < COMBINED_IMAGE_WIDTH * COMBINED_IMAGE_HEIGHT; i++) {
        uint32_t original = display_pixels[i];
        
        // 提取原始RGB分量
        uint8_t orig_r = (original >> 16) & 0xFF;
        uint8_t orig_g = (original >> 8) & 0xFF;
        uint8_t orig_b = original & 0xFF;
        
        // 与白色闪烁混合
        uint8_t new_r = orig_r + (uint8_t)((255 - orig_r) * alpha);
        uint8_t new_g = orig_g + (uint8_t)((255 - orig_g) * alpha);
        uint8_t new_b = orig_b + (uint8_t)((255 - orig_b) * alpha);
        
        display_pixels[i] = 0xFF000000 | (new_r << 16) | (new_g << 8) | new_b;
    }
}

void draw_text_overlay(RenderEngine* engine, const char* text) {
    if (!engine || !engine->is_init || !text || !engine->font) {
        return;
    }
    
    // 设置文字绘制属性（确保颜色稳定，避免闪烁）
    XSetForeground(engine->display, engine->gc, WhitePixel(engine->display, DefaultScreen(engine->display)));  // 白色文字
    XSetBackground(engine->display, engine->gc, BlackPixel(engine->display, DefaultScreen(engine->display)));  // 黑色背景
    XSetFont(engine->display, engine->gc, engine->font->fid);
    
    // 计算文字尺寸
    int text_width = XTextWidth(engine->font, text, strlen(text));
    int text_height = engine->font->ascent + engine->font->descent;
    
    // 左上角位置（带一些边距）
    int text_x = 10;  // 左边距10像素
    int text_y = 10 + engine->font->ascent;  // 上边距10像素 + 字体上升高度
    
    // 绘制半透明黑色背景矩形（左上角位置）
    int bg_padding = 5;
    int bg_x = text_x - bg_padding;
    int bg_y = text_y - engine->font->ascent - bg_padding/2;
    int bg_width = text_width + bg_padding * 2;
    int bg_height = text_height + bg_padding;
    
    // 直接在缓冲区中绘制纯黑色背景矩形（避免半透明计算闪烁）
    uint32_t* display_pixels = (uint32_t*)engine->display_buffer;
    uint32_t bg_color = 0xFF000000;  // 纯黑色背景，避免闪烁
    
    for (int y = bg_y; y < bg_y + bg_height && y < COMBINED_IMAGE_HEIGHT; y++) {
        if (y >= 0) {
            for (int x = bg_x; x < bg_x + bg_width && x < COMBINED_IMAGE_WIDTH; x++) {
                if (x >= 0) {
                    display_pixels[y * COMBINED_IMAGE_WIDTH + x] = bg_color;
                }
            }
        }
    }
    
    // 先将缓冲区渲染到pixmap
    if (engine->pixmap) {
        XPutImage(engine->display, engine->pixmap, engine->gc, engine->ximage,
                 0, 0, 0, 0, COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT);
        
        // 在pixmap上绘制文字
        XDrawString(engine->display, engine->pixmap, engine->gc, text_x, text_y, text, strlen(text));
        
        // 复制到窗口
        XCopyArea(engine->display, engine->pixmap, engine->window, engine->gc,
                 0, 0, COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT, 0, 0);
    } else {
        // 直接在窗口上绘制
        XPutImage(engine->display, engine->window, engine->gc, engine->ximage,
                 0, 0, 0, 0, COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT);
        XDrawString(engine->display, engine->window, engine->gc, text_x, text_y, text, strlen(text));
    }
}

void draw_text_overlay_center(RenderEngine* engine, const char* text) {
    if (!engine || !engine->is_init || !text || !engine->font) {
        return;
    }

    // 设置文字绘制属性
    XSetForeground(engine->display, engine->gc, 0xFFFFFF);  // 白色文字
    XSetBackground(engine->display, engine->gc, 0x000000);  // 黑色背景
    XSetFont(engine->display, engine->gc, engine->font->fid);

    // 计算文字尺寸
    int text_width = XTextWidth(engine->font, text, strlen(text));
    int text_height = engine->font->ascent + engine->font->descent;

    // 计算居中位置
    int text_x = (COMBINED_IMAGE_WIDTH - text_width) / 2;
    int text_y = COMBINED_IMAGE_HEIGHT / 2 + engine->font->ascent / 2;

    // 绘制黑色背景矩形
    int bg_padding = 20;
    int bg_x = text_x - bg_padding;
    int bg_y = text_y - engine->font->ascent - bg_padding/2;
    int bg_width = text_width + bg_padding * 2;
    int bg_height = text_height + bg_padding;

    // 直接在缓冲区中绘制背景矩形
    uint32_t* display_pixels = (uint32_t*)engine->display_buffer;

    for (int y = bg_y; y < bg_y + bg_height && y < COMBINED_IMAGE_HEIGHT; y++) {
        if (y >= 0) {
            for (int x = bg_x; x < bg_x + bg_width && x < COMBINED_IMAGE_WIDTH; x++) {
                if (x >= 0) {
                    // 半透明混合
                    uint32_t original = display_pixels[y * COMBINED_IMAGE_WIDTH + x];
                    uint8_t orig_r = (original >> 16) & 0xFF;
                    uint8_t orig_g = (original >> 8) & 0xFF;
                    uint8_t orig_b = original & 0xFF;

                    // 与黑色背景混合（70%透明度）
                    uint8_t new_r = orig_r * 0.3f;
                    uint8_t new_g = orig_g * 0.3f;
                    uint8_t new_b = orig_b * 0.3f;

                    display_pixels[y * COMBINED_IMAGE_WIDTH + x] = 0xFF000000 | (new_r << 16) | (new_g << 8) | new_b;
                }
            }
        }
    }

    // 先将缓冲区渲染到pixmap
    if (engine->pixmap) {
        XPutImage(engine->display, engine->pixmap, engine->gc, engine->ximage,
                 0, 0, 0, 0, COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT);

        // 在pixmap上绘制文字
        XDrawString(engine->display, engine->pixmap, engine->gc, text_x, text_y, text, strlen(text));

        // 复制到窗口
        XCopyArea(engine->display, engine->pixmap, engine->window, engine->gc,
                 0, 0, COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT, 0, 0);
    } else {
        // 直接在窗口上绘制
        XPutImage(engine->display, engine->window, engine->gc, engine->ximage,
                 0, 0, 0, 0, COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT);
        XDrawString(engine->display, engine->window, engine->gc, text_x, text_y, text, strlen(text));
    }
}

// 加载car.png图像
int load_car_png(const char* filename) {
    car_png_data = stbi_load(filename, &car_png_width, &car_png_height, &car_png_channels, 4);
    if (!car_png_data) {
        printf("无法加载PNG图像: %s\n", filename);
        return -1;
    }
    printf("成功加载PNG图像: %s (%dx%d, %d通道)\n", filename, car_png_width, car_png_height, car_png_channels);
    return 0;
}

// 计算检测框在car.png上的映射位置（车尾对应的边缘）
void calculate_car_edge_position(int box_left, int box_top, int box_right, int box_bottom,
                                 int* edge_type, float* edge_position) {
    // 640x480的PCIe图像分为4个320x240区域
    // 检测框中心点
    float center_x = (box_left + box_right) / 2.0f;
    float center_y = (box_top + box_bottom) / 2.0f;

    // 判断在哪个区域：0=左上, 1=右上, 2=左下, 3=右下
    int region = (center_x >= 320 ? 1 : 0) + (center_y >= 240 ? 2 : 0);

    // car.png显示的是车辆俯视图，车头朝上
    // PCIe后置摄像头：左上=后中, 右上=后左, 左下=后右, 右下=后中复制
    // 映射关系需要镜像翻转

    if (region == 0) {  // 左上角 - 后中摄像头
        *edge_position = 1.0f - (center_y / 240.0f);  // y轴反转
        *edge_type = 3;  // 车尾边缘（下边缘）
    } else if (region == 1) {  // 右上角 - 后左摄像头（实际车辆左侧）
        *edge_position = 1.0f - (center_y / 240.0f);
        *edge_type = 2;  // 右边缘（镜像后对应车辆左侧）
    } else if (region == 2) {  // 左下角 - 后右摄像头（实际车辆右侧）
        *edge_position = 1.0f - ((center_y - 240) / 240.0f);
        *edge_type = 1;  // 左边缘（镜像后对应车辆右侧）
    } else {  // 右下角 - 后中复制
        *edge_position = 1.0f - ((center_y - 240) / 240.0f);
        *edge_type = 3;  // 车尾边缘
    }
}

// 绘制车道线
void draw_lane_lines(uint32_t* display_pixels, int dst_x, int dst_y, int dst_width, int dst_height,
                     CurveType curve_type, uint32_t confidence) {
    if (confidence < 60) return;  // 置信度低于60%不绘制

    const int lane_width = dst_width / 3;  // 车道宽度为PNG宽度的1/3
    const int center_x = dst_x + dst_width / 2;  // PNG中心X坐标
    const int line_length = dst_height / 2;  // 线条长度为PNG高度的一半
    const int line_thickness = 3;  // 线条粗细
    const uint32_t line_color = 0xFF00FF00;  // 绿色车道线
    const int start_y = dst_y + dst_height / 2;  // 从PNG中间开始

    // 根据弯道类型绘制不同的车道线
    for (int i = 0; i < line_length; i++) {
        // t从0(底部中心)到1(顶部)
        float t = (float)i / line_length;
        int offset_left = 0, offset_right = 0;

        if (curve_type == CURVE_LEFT) {
            // 左弯：从底部中心向上向左弯曲
            offset_left = -(int)(lane_width * 0.5f * t * t);
            offset_right = -(int)(lane_width * 0.2f * t * t);
        } else if (curve_type == CURVE_RIGHT) {
            // 右弯：从底部中心向上向右弯曲
            offset_left = (int)(lane_width * 0.2f * t * t);
            offset_right = (int)(lane_width * 0.5f * t * t);
        }

        int py = start_y - i;  // 从中间往上画
        if (py < 0 || py >= COMBINED_IMAGE_HEIGHT) continue;

        int left_x = center_x - lane_width/2 + offset_left;
        int right_x = center_x + lane_width/2 + offset_right;

        // 绘制渐变可行驶区域（从中心向外变淡）
        for (int px = left_x; px <= right_x; px++) {
            if (px >= 0 && px < COMBINED_IMAGE_WIDTH) {
                // 计算距离中心的归一化距离 (0=中心, 1=边缘)
                float dist_from_center = fabsf((float)(px - center_x - (offset_left + offset_right)/2) / (lane_width/2.0f));
                if (dist_from_center > 1.0f) dist_from_center = 1.0f;

                // 渐变透明度：中心最亮(0.3)，边缘最淡(0.05)
                float alpha = 0.3f * (1.0f - dist_from_center * 0.85f);

                // 与原始像素混合
                uint32_t original = display_pixels[py * COMBINED_IMAGE_WIDTH + px];
                uint8_t orig_r = (original >> 16) & 0xFF;
                uint8_t orig_g = (original >> 8) & 0xFF;
                uint8_t orig_b = original & 0xFF;

                uint8_t new_r = orig_r * (1.0f - alpha);
                uint8_t new_g = orig_g * (1.0f - alpha) + 255 * alpha;
                uint8_t new_b = orig_b * (1.0f - alpha);

                display_pixels[py * COMBINED_IMAGE_WIDTH + px] = 0xFF000000 | (new_r << 16) | (new_g << 8) | new_b;
            }
        }

        // 绘制左车道线
        for (int thick = -line_thickness/2; thick <= line_thickness/2; thick++) {
            int px = left_x + thick;
            if (px >= 0 && px < COMBINED_IMAGE_WIDTH) {
                display_pixels[py * COMBINED_IMAGE_WIDTH + px] = line_color;
            }
        }

        // 绘制右车道线
        for (int thick = -line_thickness/2; thick <= line_thickness/2; thick++) {
            int px = right_x + thick;
            if (px >= 0 && px < COMBINED_IMAGE_WIDTH) {
                display_pixels[py * COMBINED_IMAGE_WIDTH + px] = line_color;
            }
        }
    }
}

// 渲染car.png到指定位置，带红光闪烁效果
void render_car_png(uint32_t* display_pixels, int dst_x, int dst_y, int dst_width, int dst_height) {
    if (!car_png_data) return;

    // 更新闪烁状态
    uint64_t current_time = get_current_time_ms();
    if (current_time - last_blink_time >= 300) {  // 300ms闪烁间隔
        blink_state = !blink_state;
        last_blink_time = current_time;
    }

    // 检查是否有人员检测
    int person_detected = 0;
    int box_left = 0, box_top = 0, box_right = 0, box_bottom = 0;
    if (emergency_brake_shm && emergency_brake_shm->valid && emergency_brake_shm->person_detected) {
        person_detected = 1;
        box_left = emergency_brake_shm->box_left;
        box_top = emergency_brake_shm->box_top;
        box_right = emergency_brake_shm->box_right;
        box_bottom = emergency_brake_shm->box_bottom;
    }

    // 计算边缘位置
    int edge_type = 0;
    float edge_position = 0.5f;
    if (person_detected) {
        calculate_car_edge_position(box_left, box_top, box_right, box_bottom, &edge_type, &edge_position);
    }

    // 计算缩放比例
    float scale_x = (float)car_png_width / dst_width;
    float scale_y = (float)car_png_height / dst_height;

    const int edge_width = 80;  // 红光渐变宽度
    float blink_intensity = blink_state ? 0.8f : 0.3f;

    for (int y = 0; y < dst_height; y++) {
        for (int x = 0; x < dst_width; x++) {
            int src_x = (int)(x * scale_x);
            int src_y = (int)(y * scale_y);

            if (src_x >= 0 && src_x < car_png_width && src_y >= 0 && src_y < car_png_height) {
                int src_idx = (src_y * car_png_width + src_x) * 4;
                uint8_t r = car_png_data[src_idx];
                uint8_t g = car_png_data[src_idx + 1];
                uint8_t b = car_png_data[src_idx + 2];
                uint8_t a = car_png_data[src_idx + 3];

                int dst_px = dst_x + x;
                int dst_py = dst_y + y;

                if (dst_px >= 0 && dst_px < COMBINED_IMAGE_WIDTH && dst_py >= 0 && dst_py < COMBINED_IMAGE_HEIGHT) {
                    // 计算红光效果
                    float red_intensity = 0.0f;
                    if (person_detected) {
                        float dist = 1000.0f;
                        if (edge_type == 0) {  // 上边缘（车头）
                            int target_x = (int)(edge_position * dst_width);
                            dist = sqrtf((x - target_x) * (x - target_x) + y * y);
                        } else if (edge_type == 1) {  // 左边缘
                            int target_y = (int)(edge_position * dst_height);
                            dist = sqrtf(x * x + (y - target_y) * (y - target_y));
                        } else if (edge_type == 2) {  // 右边缘
                            int target_y = (int)(edge_position * dst_height);
                            dist = sqrtf((dst_width - x) * (dst_width - x) + (y - target_y) * (y - target_y));
                        } else if (edge_type == 3) {  // 下边缘（车尾）
                            int target_x = (int)(edge_position * dst_width);
                            dist = sqrtf((x - target_x) * (x - target_x) + (dst_height - y) * (dst_height - y));
                        }

                        if (dist < edge_width) {
                            red_intensity = (1.0f - dist / edge_width) * blink_intensity;
                        }
                    }

                    // Alpha混合
                    if (a == 255) {
                        uint8_t final_r = (uint8_t)(r + (255 - r) * red_intensity);
                        uint8_t final_g = (uint8_t)(g * (1.0f - red_intensity));
                        uint8_t final_b = (uint8_t)(b * (1.0f - red_intensity));
                        display_pixels[dst_py * COMBINED_IMAGE_WIDTH + dst_px] = 0xFF000000 | (final_r << 16) | (final_g << 8) | final_b;
                    } else if (a > 0) {
                        uint32_t bg = display_pixels[dst_py * COMBINED_IMAGE_WIDTH + dst_px];
                        uint8_t bg_r = (bg >> 16) & 0xFF;
                        uint8_t bg_g = (bg >> 8) & 0xFF;
                        uint8_t bg_b = bg & 0xFF;

                        float alpha_f = a / 255.0f;
                        uint8_t new_r = (uint8_t)(r * alpha_f + bg_r * (1 - alpha_f));
                        uint8_t new_g = (uint8_t)(g * alpha_f + bg_g * (1 - alpha_f));
                        uint8_t new_b = (uint8_t)(b * alpha_f + bg_b * (1 - alpha_f));

                        uint8_t final_r = (uint8_t)(new_r + (255 - new_r) * red_intensity);
                        uint8_t final_g = (uint8_t)(new_g * (1.0f - red_intensity));
                        uint8_t final_b = (uint8_t)(new_b * (1.0f - red_intensity));

                        display_pixels[dst_py * COMBINED_IMAGE_WIDTH + dst_px] = 0xFF000000 | (final_r << 16) | (final_g << 8) | final_b;
                    }
                }
            }
        }
    }
}

//=========================================================================
// 切换动画函数实现
//=========================================================================
void transition_animation_init(TransitionAnimation* anim) {
    memset(anim, 0, sizeof(TransitionAnimation));
    anim->animation_frames = 20;     // 20帧动画，约0.33秒（60fps）- 加快动画速度
    anim->text_duration_ms = 2000;   // 文字显示2秒
}

void transition_animation_start(TransitionAnimation* anim, DisplayMode from_mode, DisplayMode to_mode) {
    if (from_mode == to_mode) return;  // 相同模式不需要动画
    
    anim->is_active = 1;
    anim->from_mode = from_mode;
    anim->to_mode = to_mode;
    anim->progress = 0.0f;
    anim->current_frame = 0;
    anim->scale_factor = 1.0f;  // 从正常尺寸开始
    anim->show_text = 1;        // 启用左上角文字显示
    anim->text_start_time = get_current_time_ms();  // 记录文字显示开始时间
    gettimeofday(&anim->start_time, NULL);
    
    printf("🎬 开始切换动画: %s -> %s\n", 
           get_camera_mode_name(from_mode), 
           get_camera_mode_name(to_mode));
}

int transition_animation_update(TransitionAnimation* anim) {
    if (!anim->is_active && !anim->show_text) return 0;
    
    uint64_t current_time = get_current_time_ms();
    
    // 更新动画进度
    if (anim->is_active) {
        anim->current_frame++;
        anim->progress = (float)anim->current_frame / anim->animation_frames;
        
        // 使用快速缓出的动画曲线 (更快的感觉)
        float eased_progress = 1.0f - (1.0f - anim->progress) * (1.0f - anim->progress);
        anim->scale_factor = 1.0f + eased_progress * 1.0f;  // 从1倍快速放大到2倍
        
        // 动画结束
        if (anim->current_frame >= anim->animation_frames) {
            anim->is_active = 0;
            printf("✅ 切换动画完成\n");
        }
    }
    
    // 检查文字显示时间是否到期
    if (anim->show_text && (current_time - anim->text_start_time) >= anim->text_duration_ms) {
        anim->show_text = 0;
        printf("📝 左上角文字显示结束 (显示了%lu毫秒)\n", current_time - anim->text_start_time);
    }
    
    return anim->is_active || anim->show_text;  // 动画或文字显示继续
}

int render_engine_render_with_transition(RenderEngine* engine, 
                                        const uint8_t* pcie_data, 
                                        const uint8_t* udp_data,
                                        DisplayMode current_mode) {
    if (!engine || !engine->is_init || !pcie_data || !udp_data) {
        return -1;
    }
    
    uint32_t* display_pixels = (uint32_t*)engine->display_buffer;
    const uint16_t* pcie_pixels = (const uint16_t*)pcie_data;
    const uint16_t* udp_pixels = (const uint16_t*)udp_data;
    
    // 如果没有动画或动画已结束，使用普通渲染
    if (!engine->transition.is_active) {
        return render_engine_render_camera_view(engine, pcie_data, udp_data, current_mode);
    }
    
    // 动画渲染
    TransitionAnimation* anim = &engine->transition;
    
    // 先渲染0x99模式的完整8摄像头画面作为背景（使用新的全景布局）
    // Top row (UDP): 前左(0x77) | 前中复制(0x88) | 前右(0x66) | 前中(0x55)
    // Bottom row (PCIe): 后右(0x33) | 后中复制(0x44) | 后左(0x22) | 后中(0x11)

    float dimming = 1.0f - (anim->progress * 0.7f);
    int target_src_x, target_src_y;
    get_camera_position(anim->to_mode, &target_src_x, &target_src_y);

    // Top row - UDP cameras (y: 0-239)
    for (int y = 0; y < CAMERA_BLOCK_HEIGHT; y++) {
        // 前左 0x77
        for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
            uint16_t pixel = udp_pixels[(y + 240) * UDP_IMAGE_WIDTH + x];
            uint8_t b = ((pixel >> 11) & 0x1F) << 3;
            uint8_t g = ((pixel >> 5) & 0x3F) << 2;
            uint8_t r = (pixel & 0x1F) << 3;
            bool in_target = (target_src_x == 640 && target_src_y == 240 && x < CAMERA_BLOCK_WIDTH && y < CAMERA_BLOCK_HEIGHT);
            if (!in_target) { r *= dimming; g *= dimming; b *= dimming; }
            display_pixels[y * COMBINED_IMAGE_WIDTH + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
        // 前中复制 0x88
        for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
            uint16_t pixel = udp_pixels[(y + 240) * UDP_IMAGE_WIDTH + 320 + x];
            uint8_t b = ((pixel >> 11) & 0x1F) << 3;
            uint8_t g = ((pixel >> 5) & 0x3F) << 2;
            uint8_t r = (pixel & 0x1F) << 3;
            bool in_target = (target_src_x == 960 && target_src_y == 240);
            if (!in_target) { r *= dimming; g *= dimming; b *= dimming; }
            display_pixels[y * COMBINED_IMAGE_WIDTH + 320 + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
        // 前右 0x66
        for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
            uint16_t pixel = udp_pixels[y * UDP_IMAGE_WIDTH + 320 + x];
            uint8_t b = ((pixel >> 11) & 0x1F) << 3;
            uint8_t g = ((pixel >> 5) & 0x3F) << 2;
            uint8_t r = (pixel & 0x1F) << 3;
            bool in_target = (target_src_x == 960 && target_src_y == 0);
            if (!in_target) { r *= dimming; g *= dimming; b *= dimming; }
            display_pixels[y * COMBINED_IMAGE_WIDTH + 640 + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
        // 前中 0x55
        for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
            uint16_t pixel = udp_pixels[y * UDP_IMAGE_WIDTH + x];
            uint8_t b = ((pixel >> 11) & 0x1F) << 3;
            uint8_t g = ((pixel >> 5) & 0x3F) << 2;
            uint8_t r = (pixel & 0x1F) << 3;
            bool in_target = (target_src_x == 640 && target_src_y == 0);
            if (!in_target) { r *= dimming; g *= dimming; b *= dimming; }
            display_pixels[y * COMBINED_IMAGE_WIDTH + 960 + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
    }

    // Bottom row - PCIe cameras (y: 240-479)
    for (int y = 0; y < CAMERA_BLOCK_HEIGHT; y++) {
        int display_y = y + 240;
        // 后右 0x33
        for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
            uint16_t pixel = pcie_pixels[(y + 240) * PCIE_IMAGE_WIDTH + x];
            uint8_t b = ((pixel >> 11) & 0x1F) << 3;
            uint8_t g = ((pixel >> 5) & 0x3F) << 2;
            uint8_t r = (pixel & 0x1F) << 3;
            bool in_target = (target_src_x == 0 && target_src_y == 240);
            if (!in_target) { r *= dimming; g *= dimming; b *= dimming; }
            display_pixels[display_y * COMBINED_IMAGE_WIDTH + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
        // 后中复制 0x44
        for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
            uint16_t pixel = pcie_pixels[(y + 240) * PCIE_IMAGE_WIDTH + 320 + x];
            uint8_t b = ((pixel >> 11) & 0x1F) << 3;
            uint8_t g = ((pixel >> 5) & 0x3F) << 2;
            uint8_t r = (pixel & 0x1F) << 3;
            bool in_target = (target_src_x == 320 && target_src_y == 240);
            if (!in_target) { r *= dimming; g *= dimming; b *= dimming; }
            display_pixels[display_y * COMBINED_IMAGE_WIDTH + 320 + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
        // 后左 0x22
        for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
            uint16_t pixel = pcie_pixels[y * PCIE_IMAGE_WIDTH + 320 + x];
            uint8_t b = ((pixel >> 11) & 0x1F) << 3;
            uint8_t g = ((pixel >> 5) & 0x3F) << 2;
            uint8_t r = (pixel & 0x1F) << 3;
            bool in_target = (target_src_x == 320 && target_src_y == 0);
            if (!in_target) { r *= dimming; g *= dimming; b *= dimming; }
            display_pixels[display_y * COMBINED_IMAGE_WIDTH + 640 + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
        // 后中 0x11
        for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
            uint16_t pixel = pcie_pixels[y * PCIE_IMAGE_WIDTH + x];
            uint8_t b = ((pixel >> 11) & 0x1F) << 3;
            uint8_t g = ((pixel >> 5) & 0x3F) << 2;
            uint8_t r = (pixel & 0x1F) << 3;
            bool in_target = (target_src_x == 0 && target_src_y == 0);
            if (!in_target) { r *= dimming; g *= dimming; b *= dimming; }
            display_pixels[display_y * COMBINED_IMAGE_WIDTH + 960 + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
    }
    
    // 在背景之上渲染放大的目标摄像头区域
    if (anim->progress > 0.1f) {  // 稍微延迟一下放大效果
        int src_x, src_y;
        get_camera_position(anim->to_mode, &src_x, &src_y);
        
        const uint16_t* pixels = (src_x >= 640) ? udp_pixels : pcie_pixels;
        
        // 计算目标摄像头在原始画面中的位置
        int original_x = (src_x >= 640) ? (PCIE_IMAGE_WIDTH + (src_x - 640)) : src_x;
        int original_y = src_y;
        int original_center_x = original_x + CAMERA_BLOCK_WIDTH / 2;
        int original_center_y = original_y + CAMERA_BLOCK_HEIGHT / 2;
        
        // 计算最终的中央位置
        int final_center_x = COMBINED_IMAGE_WIDTH / 2;
        int final_center_y = COMBINED_IMAGE_HEIGHT / 2;
        
        // 插值计算当前的中心位置
        float adjusted_progress = (anim->progress - 0.1f) / 0.9f;  // 调整进度范围
        if (adjusted_progress > 1.0f) adjusted_progress = 1.0f;
        if (adjusted_progress < 0.0f) adjusted_progress = 0.0f;
        
        int current_center_x = original_center_x + (int)((final_center_x - original_center_x) * adjusted_progress);
        int current_center_y = original_center_y + (int)((final_center_y - original_center_y) * adjusted_progress);
        
        // 计算当前缩放比例：从1倍放大到2倍（全屏效果）
        float current_scale = 1.0f + adjusted_progress * 1.0f;
        
        render_scaled_camera_block(display_pixels, pixels, src_x, src_y, 
                                 current_scale, current_center_x, current_center_y);
        
        // 添加高亮边框效果
        if (adjusted_progress < 0.8f) {  // 前80%的动画时间显示边框
            int scaled_width = (int)(CAMERA_BLOCK_WIDTH * current_scale);
            int scaled_height = (int)(CAMERA_BLOCK_HEIGHT * current_scale);
            
            for (int border = 0; border < 3; border++) {
                int border_x1 = current_center_x - scaled_width/2 - border;
                int border_y1 = current_center_y - scaled_height/2 - border;
                int border_x2 = current_center_x + scaled_width/2 + border;
                int border_y2 = current_center_y + scaled_height/2 + border;
                
                // 绘制边框（绿色边框，随着动画进度逐渐变淡）
                uint8_t border_alpha = (uint8_t)(255 * (1.0f - adjusted_progress));
                uint32_t border_color = (0xFF000000) | (border_alpha << 8);  // 绿色边框
                
                for (int x = border_x1; x <= border_x2; x++) {
                    if (x >= 0 && x < COMBINED_IMAGE_WIDTH) {
                        if (border_y1 >= 0 && border_y1 < COMBINED_IMAGE_HEIGHT)
                            display_pixels[border_y1 * COMBINED_IMAGE_WIDTH + x] = border_color;
                        if (border_y2 >= 0 && border_y2 < COMBINED_IMAGE_HEIGHT)
                            display_pixels[border_y2 * COMBINED_IMAGE_WIDTH + x] = border_color;
                    }
                }
                for (int y = border_y1; y <= border_y2; y++) {
                    if (y >= 0 && y < COMBINED_IMAGE_HEIGHT) {
                        if (border_x1 >= 0 && border_x1 < COMBINED_IMAGE_WIDTH)
                            display_pixels[y * COMBINED_IMAGE_WIDTH + border_x1] = border_color;
                        if (border_x2 >= 0 && border_x2 < COMBINED_IMAGE_WIDTH)
                            display_pixels[y * COMBINED_IMAGE_WIDTH + border_x2] = border_color;
                    }
                }
            }
        }
    }
    
    // 更新动画状态
    transition_animation_update(anim);
    
    // 渲染到屏幕
    if (engine->pixmap) {
        XPutImage(engine->display, engine->pixmap, engine->gc, engine->ximage,
                 0, 0, 0, 0, COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT);
        XCopyArea(engine->display, engine->pixmap, engine->window, engine->gc,
                 0, 0, COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT, 0, 0);
    } else {
        XPutImage(engine->display, engine->window, engine->gc, engine->ximage,
                 0, 0, 0, 0, COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT);
    }
    
    // 在左上角显示视角文字（仅在指定时间内显示）
    if (anim->show_text) {
        draw_text_overlay(engine, get_camera_mode_name(anim->to_mode));
    }
    
    XFlush(engine->display);
    
    return 0;
}

//=========================================================================
// 渲染引擎函数实现
//=========================================================================
int render_engine_init(RenderEngine* engine) {
    if (!engine) {
        return -1;
    }
    
    printf("正在初始化X11渲染引擎...\n");
    
    memset(engine, 0, sizeof(RenderEngine));
    
    // 连接到X服务器
    engine->display = XOpenDisplay(NULL);
    if (!engine->display) {
        printf("无法连接到X服务器\n");
        return -1;
    }
    
    int screen = DefaultScreen(engine->display);
    Window root = RootWindow(engine->display, screen);
    
    // 创建窗口
    engine->window = XCreateSimpleWindow(engine->display, root,
                                       100, 100, COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT,
                                       1, BlackPixel(engine->display, screen),
                                       WhitePixel(engine->display, screen));
    
    if (!engine->window) {
        printf("无法创建窗口\n");
        XCloseDisplay(engine->display);
        return -1;
    }
    
    // 设置窗口属性
    XStoreName(engine->display, engine->window, "FPGA双通道视频显示 - PCIe + UDP (1280x480)");
    XSelectInput(engine->display, engine->window, 
                ExposureMask | KeyPressMask | StructureNotifyMask);
    
    // 创建图形上下文
    engine->gc = XCreateGC(engine->display, engine->window, 0, NULL);
    if (!engine->gc) {
        printf("无法创建图形上下文\n");
        XDestroyWindow(engine->display, engine->window);
        XCloseDisplay(engine->display);
        return -1;
    }
    
    // 分配显示缓冲区 (RGB888格式用于X11显示)
    engine->display_buffer = (char*)aligned_alloc(64, COMBINED_IMAGE_WIDTH * COMBINED_IMAGE_HEIGHT * 4);
    if (!engine->display_buffer) {
        printf("显示缓冲区分配失败\n");
        XFreeGC(engine->display, engine->gc);
        XDestroyWindow(engine->display, engine->window);
        XCloseDisplay(engine->display);
        return -1;
    }
    
    // 创建XImage
    engine->ximage = XCreateImage(engine->display, 
                                DefaultVisual(engine->display, screen),
                                DefaultDepth(engine->display, screen),
                                ZPixmap, 0, engine->display_buffer,
                                COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT, 32, 0);
    
    if (!engine->ximage) {
        printf("XImage创建失败\n");
        free(engine->display_buffer);
        XFreeGC(engine->display, engine->gc);
        XDestroyWindow(engine->display, engine->window);
        XCloseDisplay(engine->display);
        return -1;
    }
    
    // 创建双缓冲Pixmap
    engine->pixmap = XCreatePixmap(engine->display, engine->window,
                                 COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT,
                                 DefaultDepth(engine->display, screen));
    
    // 加载字体
    engine->font = XLoadQueryFont(engine->display, "-*-*-bold-*-*-*-24-*-*-*-*-*-*-*");
    if (!engine->font) {
        engine->font = XLoadQueryFont(engine->display, "fixed");
        if (!engine->font) {
            printf("警告: 无法加载字体，文字显示可能不正常\n");
        }
    }
    
    // 显示窗口
    XMapWindow(engine->display, engine->window);
    XFlush(engine->display);
    
    // 等待窗口映射完成
    XEvent event;
    do {
        XNextEvent(engine->display, &event);
    } while (event.type != MapNotify);
    
    // 初始化切换动画
    transition_animation_init(&engine->transition);
    
    engine->is_init = 1;
    
    printf("X11渲染引擎初始化成功\n");
    printf("窗口尺寸: %dx%d\n", COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT);
    printf("显示格式: RGB888 (32位)\n");
    printf("🎬 切换动画特效已启用\n");
    
    return 0;
}

int render_engine_render_combined_frame(RenderEngine* engine, 
                                      const uint8_t* pcie_data, 
                                      const uint8_t* udp_data) {
    // 默认使用全部摄像头显示模式
    return render_engine_render_camera_view(engine, pcie_data, udp_data, DISPLAY_MODE_ALL_CAMERAS);
}

int render_engine_render_camera_view(RenderEngine* engine, 
                                    const uint8_t* pcie_data, 
                                    const uint8_t* udp_data,
                                    DisplayMode mode) {
    if (!engine || !engine->is_init || !pcie_data || !udp_data) {
        return -1;
    }
    
    uint32_t* display_pixels = (uint32_t*)engine->display_buffer;
    const uint16_t* pcie_pixels = (const uint16_t*)pcie_data;
    const uint16_t* udp_pixels = (const uint16_t*)udp_data;
    
    // 清空显示缓冲区（黑色背景）
    for (int i = 0; i < COMBINED_IMAGE_WIDTH * COMBINED_IMAGE_HEIGHT; i++) {
        display_pixels[i] = 0xFF000000;  // 黑色背景
    }
    
    // 根据显示模式选择不同的渲染方式
    switch (mode) {
        case DISPLAY_MODE_PCIE_TOP_LEFT: {
            // Rear Center 0x11 (320x240)
            render_camera_block(display_pixels, pcie_pixels, 0, 0, 0, 0);
            break;
        }
        
        case DISPLAY_MODE_PCIE_TOP_RIGHT: {
            // Rear Left 0x22 (320x240)
            render_camera_block(display_pixels, pcie_pixels, 320, 0, 320, 0);
            break;
        }
        
        case DISPLAY_MODE_PCIE_BOTTOM_LEFT: {
            // Rear Right 0x33 (320x240)
            render_camera_block(display_pixels, pcie_pixels, 0, 240, 0, 240);
            break;
        }
        
        case DISPLAY_MODE_PCIE_BOTTOM_RIGHT: {
            // Rear Center 0x44 (320x240)
            render_camera_block(display_pixels, pcie_pixels, 320, 240, 320, 240);
            break;
        }
        
        case DISPLAY_MODE_UDP_TOP_LEFT: {
            // Front Center 0x55 (320x240)
            render_camera_block(display_pixels, udp_pixels, 640, 0, 0, 0);
            break;
        }
        
        case DISPLAY_MODE_UDP_TOP_RIGHT: {
            // Front Right 0x66 (320x240)  
            render_camera_block(display_pixels, udp_pixels, 960, 0, 320, 0);
            break;
        }
        
        case DISPLAY_MODE_UDP_BOTTOM_LEFT: {
            // Front Left 0x77 (320x240)
            render_camera_block(display_pixels, udp_pixels, 640, 240, 0, 240);
            break;
        }
        
        case DISPLAY_MODE_UDP_BOTTOM_RIGHT: {
            // Rear Left View 0x88 (320x240)
            render_camera_block(display_pixels, udp_pixels, 960, 240, 320, 240);
            break;
        }
        
        case DISPLAY_MODE_ALL_CAMERAS:
        default: {
            // Surround View 0x99 - Panoramic layout
            // Top row (UDP): 前左(0x77) | 前中复制(0x88) | 前右(0x66) | 前中(0x55)
            // Bottom row (PCIe): 后右(0x33) | 后中复制(0x44) | 后左(0x22) | 后中(0x11)

            // Top row - UDP cameras (y: 0-239)
            for (int y = 0; y < CAMERA_BLOCK_HEIGHT; y++) {
                // 前左 0x77 (UDP bottom-left) -> display (0, 0)
                for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
                    uint16_t pixel = udp_pixels[(y + 240) * UDP_IMAGE_WIDTH + x];
                    uint8_t b = ((pixel >> 11) & 0x1F) << 3;
                    uint8_t g = ((pixel >> 5) & 0x3F) << 2;
                    uint8_t r = (pixel & 0x1F) << 3;
                    display_pixels[y * COMBINED_IMAGE_WIDTH + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
                }

                // 前中复制 0x88 (UDP bottom-right) -> display (320, 0)
                for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
                    uint16_t pixel = udp_pixels[(y + 240) * UDP_IMAGE_WIDTH + 320 + x];
                    uint8_t b = ((pixel >> 11) & 0x1F) << 3;
                    uint8_t g = ((pixel >> 5) & 0x3F) << 2;
                    uint8_t r = (pixel & 0x1F) << 3;
                    display_pixels[y * COMBINED_IMAGE_WIDTH + 320 + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
                }

                // 前右 0x66 (UDP top-right) -> display (640, 0)
                for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
                    uint16_t pixel = udp_pixels[y * UDP_IMAGE_WIDTH + 320 + x];
                    uint8_t b = ((pixel >> 11) & 0x1F) << 3;
                    uint8_t g = ((pixel >> 5) & 0x3F) << 2;
                    uint8_t r = (pixel & 0x1F) << 3;
                    display_pixels[y * COMBINED_IMAGE_WIDTH + 640 + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
                }

                // 前中 0x55 (UDP top-left) -> display (960, 0) - 不渲染,留给car.png
                // 这部分被car.png替换
            }

            // Bottom row - PCIe cameras (y: 240-479)
            for (int y = 0; y < CAMERA_BLOCK_HEIGHT; y++) {
                int display_y = y + 240;

                // 后右 0x33 (PCIe bottom-left) -> display (0, 240)
                for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
                    uint16_t pixel = pcie_pixels[(y + 240) * PCIE_IMAGE_WIDTH + x];
                    uint8_t b = ((pixel >> 11) & 0x1F) << 3;
                    uint8_t g = ((pixel >> 5) & 0x3F) << 2;
                    uint8_t r = (pixel & 0x1F) << 3;
                    display_pixels[display_y * COMBINED_IMAGE_WIDTH + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
                }

                // 后中复制 0x44 (PCIe bottom-right) -> display (320, 240)
                for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
                    uint16_t pixel = pcie_pixels[(y + 240) * PCIE_IMAGE_WIDTH + 320 + x];
                    uint8_t b = ((pixel >> 11) & 0x1F) << 3;
                    uint8_t g = ((pixel >> 5) & 0x3F) << 2;
                    uint8_t r = (pixel & 0x1F) << 3;
                    display_pixels[display_y * COMBINED_IMAGE_WIDTH + 320 + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
                }

                // 后左 0x22 (PCIe top-right) -> display (640, 240)
                for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
                    uint16_t pixel = pcie_pixels[y * PCIE_IMAGE_WIDTH + 320 + x];
                    uint8_t b = ((pixel >> 11) & 0x1F) << 3;
                    uint8_t g = ((pixel >> 5) & 0x3F) << 2;
                    uint8_t r = (pixel & 0x1F) << 3;
                    display_pixels[display_y * COMBINED_IMAGE_WIDTH + 640 + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
                }

                // 后中 0x11 (PCIe top-left) -> display (960, 240) - 不渲染,留给car.png
                // 这部分被car.png替换
            }

            // 渲染car.png到最右边320x480区域
            render_car_png(display_pixels, 960, 0, 320, 480);

            // 在PNG上方绘制车道线
            draw_lane_lines(display_pixels, 960, 0, 320, 480, current_curve_type, current_curve_confidence);
            break;
        }
    }
    
    // 使用双缓冲渲染
    if (engine->pixmap) {
        // 先渲染到Pixmap
        XPutImage(engine->display, engine->pixmap, engine->gc, engine->ximage,
                 0, 0, 0, 0, COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT);
        
        // 再从Pixmap复制到窗口
        XCopyArea(engine->display, engine->pixmap, engine->window, engine->gc,
                 0, 0, COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT, 0, 0);
    } else {
        // 直接渲染到窗口
        XPutImage(engine->display, engine->window, engine->gc, engine->ximage,
                 0, 0, 0, 0, COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT);
    }
    
    XFlush(engine->display);
    
    return 0;
}

int render_engine_check_exit_event(RenderEngine* engine) {
    if (!engine || !engine->is_init) {
        return 1;  // 错误状态，退出
    }
    
    XEvent event;
    
    // 非阻塞检查事件
    while (XPending(engine->display)) {
        XNextEvent(engine->display, &event);
        
        switch (event.type) {
            case KeyPress: {
                KeySym keysym = XLookupKeysym(&event.xkey, 0);
                if (keysym == XK_q || keysym == XK_Q || keysym == XK_Escape) {
                    printf("检测到退出键\n");
                    return 1;
                }
                break;
            }
            case ClientMessage:
                printf("检测到窗口关闭事件\n");
                return 1;
            case DestroyNotify:
                printf("窗口被销毁\n");
                return 1;
        }
    }
    
    return 0;  // 继续运行
}

void render_engine_cleanup(RenderEngine* engine) {
    if (!engine) {
        return;
    }
    
    if (engine->ximage) {
        XDestroyImage(engine->ximage);
        engine->ximage = NULL;
        engine->display_buffer = NULL;  // XDestroyImage会释放buffer
    } else if (engine->display_buffer) {
        free(engine->display_buffer);
        engine->display_buffer = NULL;
    }
    
    if (engine->font && engine->display) {
        XFreeFont(engine->display, engine->font);
        engine->font = NULL;
    }
    
    if (engine->pixmap && engine->display) {
        XFreePixmap(engine->display, engine->pixmap);
        engine->pixmap = 0;
    }
    
    if (engine->gc && engine->display) {
        XFreeGC(engine->display, engine->gc);
        engine->gc = NULL;
    }
    
    if (engine->window && engine->display) {
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
// 主函数
//=========================================================================
int main() {
    RenderEngine render_engine;
    struct timeval stream_start, current_time;
    uint8_t* pcie_buffer = NULL;
    uint8_t* udp_buffer = NULL;
    int frame_count = 0;
    double total_time = 0.0;
    uint32_t last_pcie_frame_id = 0;
    uint32_t last_udp_frame_id = 0;
    DisplayMode current_display_mode = DISPLAY_MODE_ALL_CAMERAS;  // 默认全部显示
    uint32_t last_control_update = 0;
    
    // 设置信号处理
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    printf("=== FPGA双通道视频渲染程序 (8摄像头视角切换 + 特效动画) ===\n");
    printf("版本: v4.0 (X11渲染 + 8摄像头视角切换 + 切换特效)\n");
    printf("功能: 读取PCIe和UDP共享内存，支持8个摄像头视角切换，带有炫酷切换动画\n");
    printf("🎬 特效功能:\n");
    printf("   - 切换时先显示完整的8摄像头画面(0x99模式)\n");
    printf("   - 目标摄像头区域高亮并逐步放大移动到屏幕中央\n");
    printf("   - 其他区域逐渐变暗突出目标摄像头\n");
    printf("   - 平滑的缓入缓出动画效果\n");
    printf("Display Modes:\n");
    printf("  - 0x11: Front Left View (320x240)\n");
    printf("  - 0x22: Front View (320x240)\n");
    printf("  - 0x33: Right Side View (320x240)\n");
    printf("  - 0x44: Front Left View (320x240)\n");
    printf("  - 0x55: Rear Left View (320x240)\n");
    printf("  - 0x66: Rear Right View (320x240)\n");
    printf("  - 0x77: Rear View (320x240)\n");
    printf("  - 0x88: Rear Left View (320x240)\n");
    printf("  - 0x99: Surround View (1280x480, PCIe Left + UDP Right)\n");
    printf("显示尺寸: %dx%d (PCIe:%dx%d + UDP:%dx%d)\n", 
           COMBINED_IMAGE_WIDTH, COMBINED_IMAGE_HEIGHT,
           PCIE_IMAGE_WIDTH, PCIE_IMAGE_HEIGHT,
           UDP_IMAGE_WIDTH, UDP_IMAGE_HEIGHT);
    
    // 分配图像缓冲区
    pcie_buffer = (uint8_t*)aligned_alloc(64, PCIE_IMAGE_WIDTH * PCIE_IMAGE_HEIGHT * 2);
    udp_buffer = (uint8_t*)aligned_alloc(64, UDP_IMAGE_WIDTH * UDP_IMAGE_HEIGHT * 2);
    if (!pcie_buffer || !udp_buffer) {
        printf("图像缓冲区分配失败！\n");
        free(pcie_buffer);
        free(udp_buffer);
        return -1;
    }
    
    // 初始化PCIe共享内存（读取者）
    printf("\n=== 初始化PCIe共享内存连接 ===\n");
    if (shared_memory_init_pcie(&pcie_shm_manager, 0) != 0) {  // 0表示读取者
        printf("PCIe共享内存连接失败！\n");
        free(pcie_buffer);
        free(udp_buffer);
        return -1;
    }
    printf("PCIe共享内存连接成功\n");
    
    // 初始化UDP共享内存（读取者）
    printf("\n=== 初始化UDP共享内存连接 ===\n");
    if (shared_memory_init_udp(&udp_shm_manager, 0) != 0) {  // 0表示读取者
        printf("UDP共享内存连接失败！\n");
        shared_memory_cleanup(&pcie_shm_manager);
        free(pcie_buffer);
        free(udp_buffer);
        return -1;
    }
    printf("UDP共享内存连接成功\n");

    // 加载car.png图像
    printf("\n=== 加载car.png图像 ===\n");
    if (load_car_png("../car.png") != 0) {
        printf("警告: 无法加载car.png,将使用原始摄像头画面\n");
    }

    // 初始化显示控制共享内存（读取者）
    printf("\n=== 初始化显示控制共享内存连接 ===\n");
    if (display_control_shm_init(&display_control_shm, 0) != 0) {  // 0表示读取者
        printf("显示控制共享内存连接失败！继续使用默认显示模式\n");
        display_control_shm = NULL;
    } else {
        printf("显示控制共享内存连接成功\n");
    }

    // 初始化紧急制动共享内存（读取者）
    printf("\n=== 初始化紧急制动共享内存连接 ===\n");
    if (emergency_brake_shm_init(&emergency_brake_shm, 0) != 0) {  // 0表示读取者
        printf("紧急制动共享内存连接失败！将无法显示人员检测红光效果\n");
        emergency_brake_shm = NULL;
    } else {
        printf("紧急制动共享内存连接成功\n");
    }

    // 初始化弯道检测共享内存（读取者）
    printf("\n=== 初始化弯道检测共享内存连接 ===\n");
    if (curve_detection_shm_init(0) != 0) {  // 0表示读取者
        printf("弯道检测共享内存连接失败！将无法显示车道线\n");
    } else {
        printf("弯道检测共享内存连接成功\n");
    }
    
    // 初始化渲染引擎
    printf("\n=== 初始化渲染引擎 ===\n");
    if (render_engine_init(&render_engine) != 0) {
        printf("渲染引擎初始化失败！\n");
        if (display_control_shm) {
            display_control_shm_cleanup(display_control_shm, 0);
        }
        if (emergency_brake_shm) {
            emergency_brake_shm_cleanup(emergency_brake_shm, 0);
        }
        shared_memory_cleanup(&pcie_shm_manager);
        shared_memory_cleanup(&udp_shm_manager);
        free(pcie_buffer);
        free(udp_buffer);
        return -1;
    }
    
    printf("\n=== 开始双通道渲染 ===\n");
    printf("按 Q/Esc 键或关闭窗口退出\n");
    
    gettimeofday(&stream_start, NULL);
    
    // 主渲染循环
    while (keep_running) {
        int pcie_updated = 0, udp_updated = 0;
        uint32_t pcie_width, pcie_height, pcie_frame_id;
        uint32_t udp_width, udp_height, udp_frame_id;

        // 读取弯道检测信息
        uint32_t curve_frame_id = 0;
        uint32_t curve_angle = 0;
        static int curve_read_count = 0;
        if (curve_detection_read(&curve_frame_id, &current_curve_type,
                                 &current_curve_confidence, &curve_angle) == 0) {
            // 每30帧打印一次弯道信息
            if (++curve_read_count % 30 == 0) {
                const char* type_name = (current_curve_type == CURVE_LEFT) ? "LEFT" :
                                       (current_curve_type == CURVE_RIGHT) ? "RIGHT" :
                                       (current_curve_type == CURVE_BOTH) ? "BOTH" : "STRAIGHT";
                printf("[Lane] Frame%u: %s, Conf:%u%%, Angle:%u\n",
                       curve_frame_id, type_name, current_curve_confidence, curve_angle);
            }
        }

        // 检查显示控制更新
        if (display_control_shm && display_control_shm->valid) {
            uint32_t control_update = display_control_shm->last_update;
            if (control_update != last_control_update) {
                DisplayMode new_mode = (DisplayMode)display_control_shm->mode;
                if (new_mode != current_display_mode) {
                    const char* mode_name = get_camera_mode_name(new_mode);
                    const char* old_mode_name = get_camera_mode_name(current_display_mode);
                    printf("🎯 显示模式切换: %s -> %s (FSPI信号: 0x%02X, 按键次数: %d)\n", 
                           old_mode_name, mode_name, new_mode,
                           display_control_shm->button_count);
                    
                    // 只有在切换到单个摄像头视图时才启动切换动画
                    // 0x99模式（全部显示）直接切换，不需要动画
                    if (new_mode != DISPLAY_MODE_ALL_CAMERAS) {
                        transition_animation_start(&render_engine.transition, current_display_mode, new_mode);
                    } else {
                        // 0x99模式直接切换，停止任何正在进行的动画
                        render_engine.transition.is_active = 0;
                    }
                    current_display_mode = new_mode;
                }
                last_control_update = control_update;
            }
        }
        
        // 尝试读取PCIe共享内存
        int pcie_result = shared_memory_read_image(&pcie_shm_manager, 
                                                 pcie_buffer, 
                                                 PCIE_IMAGE_WIDTH * PCIE_IMAGE_HEIGHT * 2,
                                                 &pcie_width, &pcie_height, &pcie_frame_id);
        
        if (pcie_result > 0 && pcie_frame_id != last_pcie_frame_id) {
            last_pcie_frame_id = pcie_frame_id;
            pcie_updated = 1;
        }
        
        // 尝试读取UDP共享内存
        int udp_result = shared_memory_read_image(&udp_shm_manager, 
                                                udp_buffer, 
                                                UDP_IMAGE_WIDTH * UDP_IMAGE_HEIGHT * 2,
                                                &udp_width, &udp_height, &udp_frame_id);
        
        if (udp_result > 0 && udp_frame_id != last_udp_frame_id) {
            last_udp_frame_id = udp_frame_id;
            udp_updated = 1;
        }
        
        // 如果有任意一个通道更新，或者正在播放动画，或者正在显示文字，就重新渲染
        if (pcie_updated || udp_updated || render_engine.transition.is_active || render_engine.transition.show_text) {
            if (render_engine_render_with_transition(&render_engine, pcie_buffer, udp_buffer, current_display_mode) != 0) {
                printf("渲染失败！\n");
                break;
            }
            
            frame_count++;
            
            if (frame_count % 30 == 0) {  // 减少输出频率
                const char* mode_name = get_camera_mode_name(current_display_mode);
                const char* anim_status = render_engine.transition.is_active ? " [🎬动画中]" : "";
                printf("渲染帧%d [%s%s] (PCIe:%s ID:%u, UDP:%s ID:%u)\n", 
                       frame_count, mode_name, anim_status,
                       pcie_updated ? "更新" : "无变化", last_pcie_frame_id,
                       udp_updated ? "更新" : "无变化", last_udp_frame_id);
            }
        } else {
            // 调试：如果没有任何更新，也打印状态
            static int no_update_count = 0;
            if (++no_update_count % 100 == 0) {
                printf("等待数据更新... PCIe result:%d, UDP result:%d\n", pcie_result, udp_result);
            }
        }
        
        // 检查退出事件
        if (render_engine_check_exit_event(&render_engine)) {
            printf("检测到退出事件\n");
            break;
        }
        
        // 实时FPS计算
        if (frame_count % 100 == 0 && frame_count > 0) {
            gettimeofday(&current_time, NULL);
            total_time = get_time_diff(stream_start, current_time);
            double current_fps = frame_count / total_time;
            
            const char* mode_name = get_camera_mode_name(current_display_mode);
            printf("[%s] [%d帧] 渲染FPS: %.1f | PCIe帧:%u | UDP帧:%u\n", 
                   mode_name, frame_count, current_fps, 
                   last_pcie_frame_id, last_udp_frame_id);
        }
        
        // 动画期间提高渲染频率，平时降低CPU占用
        if (render_engine.transition.is_active) {
            usleep(16667);  // 约60fps (16.67ms)
        } else {
            usleep(1000);   // 1ms
        }
    }
    
    // 性能统计
    gettimeofday(&current_time, NULL);
    total_time = get_time_diff(stream_start, current_time);
    printf("\n=== 性能统计结果 ===\n");
    printf("总渲染帧数: %d\n", frame_count);
    printf("总时间: %.2f秒\n", total_time);
    printf("平均渲染FPS: %.1f\n", frame_count / total_time);
    printf("最终PCIe帧ID: %u\n", last_pcie_frame_id);
    printf("最终UDP帧ID: %u\n", last_udp_frame_id);
    printf("最终显示模式: %d\n", current_display_mode);
    
    // 保存最后一帧合成图像
    if (frame_count > 0) {
        printf("\n=== 保存最后一帧合成图像 ===\n");
        save_combined_frame_to_ppm(pcie_buffer, udp_buffer, "combined_last_frame.ppm");
    }
    
    // 系统清理
    printf("\n=== 系统清理 ===\n");
    render_engine_cleanup(&render_engine);

    if (display_control_shm) {
        display_control_shm_cleanup(display_control_shm, 0);
    }

    if (emergency_brake_shm) {
        emergency_brake_shm_cleanup(emergency_brake_shm, 0);
    }

    curve_detection_shm_cleanup();

    shared_memory_cleanup(&pcie_shm_manager);
    shared_memory_cleanup(&udp_shm_manager);
    
    if (pcie_buffer) {
        free(pcie_buffer);
    }
    if (udp_buffer) {
        free(udp_buffer);
    }

    // 释放PNG图像内存
    if (car_png_data) {
        stbi_image_free(car_png_data);
    }

    printf("\n=== 双通道渲染程序执行完成 ===\n");
    return 0;
}