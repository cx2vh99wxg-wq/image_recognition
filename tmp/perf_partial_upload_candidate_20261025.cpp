#include "shared_memory.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <limits.h>
#include <poll.h>
#include <errno.h>
#include <new>
#include <vector>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XShm.h>  /* MIT-SHM共享内存扩展 */
#ifdef PERCEPTION_HAVE_XRENDER
#include <X11/extensions/Xrender.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "dashboard_ui.h"

/* 为了使用aligned_alloc，需要定义_ISOC11_SOURCE */
#ifndef _ISOC11_SOURCE
#define _ISOC11_SOURCE
#endif

//=========================================================================
// *作   者：科研轮转（C语言重构版：X11双通道渲染程序）
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

// 只在显示端上下翻转 S 端 PCIe 摄像头区块，保持各区块原来的位置。
// 采集端的识别图像与车控坐标不变。
static inline int pcie_display_source_row(int row) {
    return CAMERA_BLOCK_HEIGHT - 1 - row;
}

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

// Pixels in the fixed vehicle layer depend only on this immutable snapshot.
// The same snapshot supplies both the cache key and the renderer.
struct VehicleLayerState {
    const uint8_t* png;
    uint64_t png_revision;
    int png_width, png_height;
    int person, box_left, box_top, box_right, box_bottom, blink;
    CurveType curve;
    uint32_t confidence;
    bool operator==(const VehicleLayerState& other) const {
        return png == other.png && png_revision == other.png_revision &&
            png_width == other.png_width && png_height == other.png_height &&
            person == other.person && box_left == other.box_left && box_top == other.box_top &&
            box_right == other.box_right && box_bottom == other.box_bottom && blink == other.blink &&
            curve == other.curve && confidence == other.confidence;
    }
};

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
    // 1280x480 is the unchanged video canvas; the dashboard is presented separately.
    perception_ui::Dashboard* dashboard;
    XImage* presentation_image;
    uint32_t* vehicle_layer;
    int window_width, window_height;
    int fullscreen, needs_redraw, headless;
    Atom wm_delete;
    DisplayMode presented_mode;
    uint64_t pcie_last_seen, udp_last_seen, last_presented;
    float display_fps;
    int preview_recorded;
    int use_xrender;
    VehicleLayerState vehicle_state;
    int vehicle_cached;
    int use_shm_completion, shm_completion_disabled, shm_completion_type, shm_pending;
    unsigned long shm_pending_serial;
    Drawable shm_pending_drawable;
    uint64_t profile_reuse_wait_ns, profile_upload_completions;
    uint64_t profile_upload_bytes, profile_upload_rects;
    int upload_initialized, uploaded_this_frame;
#ifdef PERCEPTION_HAVE_XRENDER
    Picture source_picture, window_picture;
#endif
    uint32_t pcie_frame_id, udp_frame_id, shown_pcie_id, shown_udp_id;
    uint32_t profile_pcie_start, profile_udp_start;
    uint64_t profile_start, profile_frames, profile_s_shown, profile_m_shown;
    uint64_t profile_compose_ns, profile_scale_ns, profile_present_ns;
    uint64_t last_layout_ns, profile_layout_ns, profile_car_ns, profile_dashboard_ns, profile_car_redraws;
    float pcie_shown_fps, udp_shown_fps;
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
static uint64_t car_png_revision = 0;

// 红光闪烁状态
static uint64_t last_blink_time = 0;
static int blink_state = 1;

// 弯道检测状态
static CurveType current_curve_type = CURVE_NONE;
static uint32_t current_curve_confidence = 0;
static int start_fullscreen = 1;
static int frame_limit = 0;
static const char* snapshot_path = NULL;

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
int present_dashboard(RenderEngine* engine);
int resize_presentation(RenderEngine* engine, int width, int height);
void set_fullscreen(RenderEngine* engine, int enabled);
static perception_ui::Dashboard* create_dashboard_theme();
static uint64_t display_clock_ns();

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
            int source_y = src_y + (src_x < PCIE_IMAGE_WIDTH ? pcie_display_source_row(orig_y) : orig_y);
            int source_idx = source_y * (src_x >= 640 ? UDP_IMAGE_WIDTH : PCIE_IMAGE_WIDTH)
                             + ((src_x >= 640 ? src_x - 640 : src_x) + orig_x);
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
                pixel_y = src_y + pcie_display_source_row((int)orig_y);
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

// 加载car.png图像
int load_car_png(const char* filename) {
    car_png_data = stbi_load(filename, &car_png_width, &car_png_height, &car_png_channels, 4);
    if (!car_png_data) {
        printf("无法加载PNG图像: %s\n", filename);
        return -1;
    }
    ++car_png_revision;
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
static VehicleLayerState snapshot_vehicle_state() {
    uint64_t current_time = get_current_time_ms();
    if (car_png_data && current_time - last_blink_time >= 300) {
        blink_state = !blink_state;
        last_blink_time = current_time;
    }
    VehicleLayerState state = {};
    state.png = car_png_data;
    state.png_revision = car_png_revision;
    state.png_width = car_png_width;
    state.png_height = car_png_height;
    if (emergency_brake_shm && emergency_brake_shm->valid && emergency_brake_shm->person_detected) {
        state.person = 1;
        state.box_left = emergency_brake_shm->box_left;
        state.box_top = emergency_brake_shm->box_top;
        state.box_right = emergency_brake_shm->box_right;
        state.box_bottom = emergency_brake_shm->box_bottom;
    }
    // Blink and coordinates do not affect pixels while person is absent.
    state.blink = state.person ? blink_state : 0;
    state.curve = current_curve_type;
    state.confidence = current_curve_confidence;
    return state;
}

static void render_car_png_with_state(uint32_t* display_pixels, int dst_x, int dst_y,
                                     int dst_width, int dst_height, const VehicleLayerState& state) {
    if (!state.png) return;

    // 计算边缘位置
    int edge_type = 0;
    float edge_position = 0.5f;
    if (state.person) {
        calculate_car_edge_position(state.box_left, state.box_top, state.box_right, state.box_bottom, &edge_type, &edge_position);
    }

    // 计算缩放比例
    float scale_x = (float)state.png_width / dst_width;
    float scale_y = (float)state.png_height / dst_height;

    const int edge_width = 80;  // 红光渐变宽度
    float blink_intensity = state.blink ? 0.8f : 0.3f;

    for (int y = 0; y < dst_height; y++) {
        for (int x = 0; x < dst_width; x++) {
            int src_x = (int)(x * scale_x);
            int src_y = (int)(y * scale_y);

            if (src_x >= 0 && src_x < state.png_width && src_y >= 0 && src_y < state.png_height) {
                int src_idx = (src_y * state.png_width + src_x) * 4;
                uint8_t r = state.png[src_idx];
                uint8_t g = state.png[src_idx + 1];
                uint8_t b = state.png[src_idx + 2];
                uint8_t a = state.png[src_idx + 3];

                int dst_px = dst_x + x;
                int dst_py = dst_y + y;

                if (dst_px >= 0 && dst_px < COMBINED_IMAGE_WIDTH && dst_py >= 0 && dst_py < COMBINED_IMAGE_HEIGHT) {
                    // 计算红光效果
                    float red_intensity = 0.0f;
                    if (state.person) {
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

void render_car_png(uint32_t* display_pixels, int dst_x, int dst_y, int dst_width, int dst_height) {
    const VehicleLayerState state = snapshot_vehicle_state();
    render_car_png_with_state(display_pixels, dst_x, dst_y, dst_width, dst_height, state);
}

static bool update_vehicle_layer(RenderEngine* engine) {
    const VehicleLayerState state = snapshot_vehicle_state();
    if (engine->vehicle_cached && engine->vehicle_state == state) return false;
    // Its 320x480 target, 1280-pixel stride and background are fixed by the
    // logical dashboard, independently of physical window resizing.
    for (int y = 0; y < 480; ++y)
        std::fill_n(engine->vehicle_layer + y * COMBINED_IMAGE_WIDTH, 320, 0xff080b20u);
    render_car_png_with_state(engine->vehicle_layer, 0, 0, 320, 480, state);
    draw_lane_lines(engine->vehicle_layer, 0, 0, 320, 480, state.curve, state.confidence);
    engine->vehicle_state = state;
    engine->vehicle_cached = 1;
    return true;
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
    
    engine->presented_mode = current_mode;
    uint32_t* display_pixels = (uint32_t*)engine->display_buffer;
    const uint16_t* pcie_pixels = (const uint16_t*)pcie_data;
    const uint16_t* udp_pixels = (const uint16_t*)udp_data;
    
    // 如果没有动画或动画已结束，使用普通渲染
    if (!engine->transition.is_active) {
        return render_engine_render_camera_view(engine, pcie_data, udp_data, current_mode);
    }
    
    uint64_t layout_started = display_clock_ns();
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
        int source_y = pcie_display_source_row(y);
        // 后右 0x33
        for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
            uint16_t pixel = pcie_pixels[(source_y + 240) * PCIE_IMAGE_WIDTH + x];
            uint8_t b = ((pixel >> 11) & 0x1F) << 3;
            uint8_t g = ((pixel >> 5) & 0x3F) << 2;
            uint8_t r = (pixel & 0x1F) << 3;
            bool in_target = (target_src_x == 0 && target_src_y == 240);
            if (!in_target) { r *= dimming; g *= dimming; b *= dimming; }
            display_pixels[display_y * COMBINED_IMAGE_WIDTH + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
        // 后中复制 0x44
        for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
            uint16_t pixel = pcie_pixels[(source_y + 240) * PCIE_IMAGE_WIDTH + 320 + x];
            uint8_t b = ((pixel >> 11) & 0x1F) << 3;
            uint8_t g = ((pixel >> 5) & 0x3F) << 2;
            uint8_t r = (pixel & 0x1F) << 3;
            bool in_target = (target_src_x == 320 && target_src_y == 240);
            if (!in_target) { r *= dimming; g *= dimming; b *= dimming; }
            display_pixels[display_y * COMBINED_IMAGE_WIDTH + 320 + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
        // 后左 0x22
        for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
            uint16_t pixel = pcie_pixels[source_y * PCIE_IMAGE_WIDTH + 320 + x];
            uint8_t b = ((pixel >> 11) & 0x1F) << 3;
            uint8_t g = ((pixel >> 5) & 0x3F) << 2;
            uint8_t r = (pixel & 0x1F) << 3;
            bool in_target = (target_src_x == 320 && target_src_y == 0);
            if (!in_target) { r *= dimming; g *= dimming; b *= dimming; }
            display_pixels[display_y * COMBINED_IMAGE_WIDTH + 640 + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
        // 后中 0x11
        for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
            uint16_t pixel = pcie_pixels[source_y * PCIE_IMAGE_WIDTH + x];
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
    engine->last_layout_ns = display_clock_ns() - layout_started;
    return present_dashboard(engine);
}

//=========================================================================
// 渲染引擎函数实现
//=========================================================================

void set_fullscreen(RenderEngine* engine, int enabled) {
    if (!engine || !engine->display) return;
    XEvent event = {};
    event.type = ClientMessage;
    event.xclient.window = engine->window;
    event.xclient.message_type = XInternAtom(engine->display, "_NET_WM_STATE", False);
    event.xclient.format = 32;
    event.xclient.data.l[0] = enabled ? 1 : 0;
    event.xclient.data.l[1] = XInternAtom(engine->display, "_NET_WM_STATE_FULLSCREEN", False);
    event.xclient.data.l[3] = 1;
    XSendEvent(engine->display, DefaultRootWindow(engine->display), False,
               SubstructureRedirectMask | SubstructureNotifyMask, &event);
    engine->fullscreen = enabled;
    engine->needs_redraw = 1;
    XFlush(engine->display);
}

static bool display_verbose() {
    static const char* value = getenv("VISION_VERBOSE");
    static const bool enabled = value && strcmp(value, "0") != 0;
    return enabled;
}

static uint64_t display_clock_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

static int shm_attach_error = 0;
static int catch_shm_error(Display*, XErrorEvent* event) {
    shm_attach_error = event->error_code;
    return 0;
}

// Only this client's one outstanding upload may release its shared image.
static Bool pending_shm_completion(Display*, XEvent* event, XPointer context) {
    RenderEngine* engine = reinterpret_cast<RenderEngine*>(context);
    if (!engine->shm_pending || event->type != engine->shm_completion_type) return False;
    const XShmCompletionEvent* completion = reinterpret_cast<XShmCompletionEvent*>(event);
    return completion->shmseg == engine->shminfo.shmseg &&
           completion->drawable == engine->shm_pending_drawable &&
           completion->serial == engine->shm_pending_serial && completion->offset == 0;
}

static bool consume_shm_completion(RenderEngine* engine, XEvent* event) {
    if (!pending_shm_completion(engine->display, event, reinterpret_cast<XPointer>(engine))) return false;
    engine->shm_pending = 0;
    ++engine->profile_upload_completions;
    return true;
}

static void wait_for_shared_image(RenderEngine* engine) {
    if (!engine->shm_pending) return;
    const uint64_t started = display_clock_ns();
    const uint64_t deadline = started + 250000000ULL;
    XEvent event;
    while (engine->shm_pending) {
        if (XCheckIfEvent(engine->display, &event, pending_shm_completion,
                         reinterpret_cast<XPointer>(engine))) {
            consume_shm_completion(engine, &event);
            break;
        }
        if (!keep_running || engine->shm_completion_disabled || display_clock_ns() >= deadline) {
            // No completion means this image must not be reused yet. The
            // synchronous fallback also covers servers that lose these events.
            XSync(engine->display, False);
            if (XCheckIfEvent(engine->display, &event, pending_shm_completion,
                             reinterpret_cast<XPointer>(engine)))
                consume_shm_completion(engine, &event);
            else { engine->shm_pending = 0; ++engine->profile_upload_completions; }
            engine->use_shm_completion = 0;
            engine->shm_completion_disabled = 1;
            fprintf(stderr, "[RENDER] SHM completion wait timed out; using synchronized fallback\n");
            break;
        }
        pollfd descriptor = {ConnectionNumber(engine->display), POLLIN, 0};
        int result = poll(&descriptor, 1, 10);
        if (result < 0 && errno != EINTR) {
            // The next iteration safely finishes with XSync rather than
            // writing a segment whose owner is still the X server.
            engine->shm_completion_disabled = 1;
        }
        if (result > 0) XEventsQueued(engine->display, QueuedAfterReading);
    }
    engine->profile_reuse_wait_ns += display_clock_ns() - started;
}

struct UploadRegion { int x, y, width, height; };

static bool partial_upload_requested() {
    static const char* value = getenv("VISION_PARTIAL_UPLOAD");
    static const bool enabled = value && !strcmp(value, "1");
    return enabled;
}

// Regions cover every dynamic Dashboard pixel and do not overlap. Static
// layout/theme changes must rebuild this image or update these bounds.
static void prepare_xrender_upload(RenderEngine* engine, const uint32_t* pixels,
                                   bool partial, std::vector<UploadRegion>& regions) {
    XImage* image = engine->presentation_image;
    regions.clear();
    if (!partial || !engine->upload_initialized) {
        for (int y = 0; y < perception_ui::HEIGHT; ++y)
            memcpy(image->data + y * image->bytes_per_line,
                   pixels + y * perception_ui::WIDTH, perception_ui::WIDTH * 4);
        regions.push_back({0, 0, perception_ui::WIDTH, perception_ui::HEIGHT});
        return;
    }
    static const UploadRegion areas[] = {
        {0, 180, 1600, 44}, {72, 224, 1092, 504},
        {0, 728, 1600, 152}, {1188, 230, 412, 498}
    };
    bool changed[4] = {};
    for (int area_index = 0; area_index < 4; ++area_index) {
        const UploadRegion& area = areas[area_index];
        int first_row = -1;
        for (int row = 0; row < area.height; ++row) {
            char* destination = image->data + (area.y + row) * image->bytes_per_line + area.x * 4;
            const uint32_t* source = pixels + (area.y + row) * perception_ui::WIDTH + area.x;
            bool differs = memcmp(destination, source, area.width * 4) != 0;
            if (differs) {
                memcpy(destination, source, area.width * 4);
                changed[area_index] = true;
                if (first_row < 0) first_row = row;
            } else if (first_row >= 0) {
                regions.push_back({area.x, area.y + first_row, area.width, row - first_row});
                first_row = -1;
            }
        }
        if (first_row >= 0)
            regions.push_back({area.x, area.y + first_row, area.width, area.height - first_row});
    }
    // Avoid hundreds of tiny protocol requests on alternating changed rows.
    if (regions.size() > 64) {
        regions.clear();
        for (int i = 0; i < 4; ++i) if (changed[i]) regions.push_back(areas[i]);
    }
}

static int put_presentation_region(RenderEngine* engine, Drawable target,
                                   const UploadRegion& area, bool last_region) {
    const bool completion = engine->use_shm_completion && last_region;
    if (engine->use_shm) {
        if (completion) {
            // XShmPutImage may first flush a dirty GC. Tag the upload itself.
            XFlushGC(engine->display, engine->gc);
            engine->shm_pending_serial = NextRequest(engine->display);
            engine->shm_pending_drawable = target;
            engine->shm_pending = 1;
        }
        if (!XShmPutImage(engine->display, target, engine->gc, engine->presentation_image,
                          area.x, area.y, area.x, area.y, area.width, area.height,
                          completion ? True : False)) {
            engine->shm_pending = 0;
            return -1;
        }
    } else XPutImage(engine->display, target, engine->gc, engine->presentation_image,
                     area.x, area.y, area.x, area.y, area.width, area.height);
    engine->uploaded_this_frame = 1;
    engine->profile_upload_bytes += uint64_t(area.width) * area.height *
                                    ((engine->presentation_image->bits_per_pixel + 7) / 8);
    ++engine->profile_upload_rects;
    return 0;
}

static int put_presentation_image(RenderEngine* engine, Drawable target, int width, int height) {
    return put_presentation_region(engine, target, {0, 0, width, height}, true);
}

static void flush_presentation(RenderEngine* engine, bool warmup = false) {
    if (warmup) {
        // First full image/scale may initialize GPU resources. Complete it
        // before pipelining; no shader warmup can expire a reuse deadline.
        XSync(engine->display, False);
        if (engine->shm_pending) wait_for_shared_image(engine);
        else if (engine->uploaded_this_frame) ++engine->profile_upload_completions;
    } else if (engine->use_shm_completion) XFlush(engine->display);
    else {
        XSync(engine->display, False);
        if (engine->uploaded_this_frame) ++engine->profile_upload_completions;
    }
}

static void release_presentation(RenderEngine* engine) {
    if (engine->display && engine->shm_pending) {
        XSync(engine->display, False);
        XEvent event;
        if (XCheckIfEvent(engine->display, &event, pending_shm_completion, reinterpret_cast<XPointer>(engine)))
            consume_shm_completion(engine, &event);
        engine->shm_pending = 0;
    }
#ifdef PERCEPTION_HAVE_XRENDER
    if (engine->display && engine->source_picture)
        XRenderFreePicture(engine->display, engine->source_picture);
    if (engine->display && engine->window_picture)
        XRenderFreePicture(engine->display, engine->window_picture);
    engine->source_picture = engine->window_picture = 0;
#endif
    if (engine->presentation_image) {
        if (engine->use_shm) {
            XShmDetach(engine->display, &engine->shminfo);
            XSync(engine->display, False);
            engine->presentation_image->data = NULL;
            XDestroyImage(engine->presentation_image);
            shmdt(engine->shminfo.shmaddr);
            // The segment was already marked for removal immediately after attach.
            engine->shminfo.shmaddr = NULL;
            engine->shminfo.shmid = -1;
        } else XDestroyImage(engine->presentation_image);
        engine->presentation_image = NULL;
    }
    if (engine->pixmap && engine->display) XFreePixmap(engine->display, engine->pixmap);
    engine->pixmap = 0;
    engine->use_shm = 0;
    engine->use_shm_completion = 0;
    engine->upload_initialized = 0;
    engine->uploaded_this_frame = 0;
}

int resize_presentation(RenderEngine* engine, int width, int height) {
    if (!engine || !engine->display || width < 1 || height < 1 ||
        width > 8192 || height > 8192) return -1;
    if (engine->presentation_image &&
        (engine->use_xrender ||
         (width == engine->window_width && height == engine->window_height))) {
        engine->window_width = width; engine->window_height = height;
        engine->needs_redraw = 1;
        return 0;
    }
    release_presentation(engine);
    int screen = DefaultScreen(engine->display);
    int image_w = engine->use_xrender ? perception_ui::WIDTH : width;
    int image_h = engine->use_xrender ? perception_ui::HEIGHT : height;
    XImage* image = NULL;
    const char* backend = getenv("VISION_RENDER_BACKEND");
    bool allow_shm = !backend || strcmp(backend, "putimage") != 0;
    // MIT-SHM is local-only. Reuse is gated by completion or XSync.
    // https://www.x.org/releases/X11R7.5/doc/Xext/mit-shm.pdf
    if (allow_shm && XShmQueryExtension(engine->display)) {
        memset(&engine->shminfo, 0, sizeof(engine->shminfo));
        engine->shminfo.shmid = -1;
        image = XShmCreateImage(engine->display, DefaultVisual(engine->display, screen),
            DefaultDepth(engine->display, screen), ZPixmap, NULL, &engine->shminfo,
            image_w, image_h);
        if (image) {
            size_t bytes = (size_t)image->bytes_per_line * image_h;
            engine->shminfo.shmid = shmget(IPC_PRIVATE, bytes, IPC_CREAT | 0600);
            if (engine->shminfo.shmid >= 0) {
                engine->shminfo.shmaddr = (char*)shmat(engine->shminfo.shmid, NULL, 0);
                if (engine->shminfo.shmaddr != (char*)-1) {
                    image->data = engine->shminfo.shmaddr;
                    engine->shminfo.readOnly = True;
                    XSync(engine->display, False);
                    shm_attach_error = 0;
                    XErrorHandler previous = XSetErrorHandler(catch_shm_error);
                    Bool attached = XShmAttach(engine->display, &engine->shminfo);
                    XSync(engine->display, False);
                    XSetErrorHandler(previous);
                    if (attached && !shm_attach_error) engine->use_shm = 1;
                }
            }
            if (engine->shminfo.shmid >= 0)
                shmctl(engine->shminfo.shmid, IPC_RMID, NULL);
            if (!engine->use_shm) {
                image->data = NULL;
                XDestroyImage(image); image = NULL;
                if (engine->shminfo.shmaddr && engine->shminfo.shmaddr != (char*)-1)
                    shmdt(engine->shminfo.shmaddr);
                engine->shminfo.shmaddr = NULL;
                engine->shminfo.shmid = -1;
            }
        }
    }
    if (!image) {
        image = XCreateImage(engine->display, DefaultVisual(engine->display, screen),
            DefaultDepth(engine->display, screen), ZPixmap, 0, NULL, image_w, image_h, 32, 0);
        if (!image) return -1;
        image->data = (char*)calloc((size_t)image->bytes_per_line, image_h);
        if (!image->data) { XDestroyImage(image); return -1; }
    }
    engine->presentation_image = image;
    if (engine->use_xrender && (image->bits_per_pixel != 32 || image->byte_order != LSBFirst ||
        image->red_mask != 0xff0000 || image->green_mask != 0xff00 || image->blue_mask != 0xff)) {
        release_presentation(engine); engine->use_xrender = 0;
        return resize_presentation(engine, width, height);
    }
    engine->pixmap = XCreatePixmap(engine->display, engine->window, image_w, image_h,
                                  DefaultDepth(engine->display, screen));
    engine->window_width = width; engine->window_height = height;
#ifdef PERCEPTION_HAVE_XRENDER
    if (engine->use_xrender) {
        XRenderPictFormat* format = XRenderFindVisualFormat(engine->display, DefaultVisual(engine->display, screen));
        if (!format) {
            release_presentation(engine); engine->use_xrender = 0;
            return resize_presentation(engine, width, height);
        }
        XSync(engine->display, False);
        shm_attach_error = 0;
        XErrorHandler previous = XSetErrorHandler(catch_shm_error);
        engine->source_picture = XRenderCreatePicture(engine->display, engine->pixmap, format, 0, NULL);
        engine->window_picture = XRenderCreatePicture(engine->display, engine->window, format, 0, NULL);
        XRenderSetPictureFilter(engine->display, engine->source_picture, FilterBilinear, NULL, 0);
        XSync(engine->display, False);
        if (shm_attach_error) {
            if (engine->source_picture) XRenderFreePicture(engine->display, engine->source_picture);
            if (engine->window_picture) XRenderFreePicture(engine->display, engine->window_picture);
            XSync(engine->display, False);
            engine->source_picture = engine->window_picture = 0;
            XSetErrorHandler(previous);
            release_presentation(engine); engine->use_xrender = 0;
            return resize_presentation(engine, width, height);
        }
        XSetErrorHandler(previous);
    }
#endif
    const char* sync_mode = getenv("VISION_SHM_SYNC");
    engine->use_shm_completion = engine->use_shm && !engine->shm_completion_disabled &&
                                 sync_mode && !strcmp(sync_mode, "completion");
    if (engine->use_shm_completion)
        engine->shm_completion_type = XShmGetEventBase(engine->display) + ShmCompletion;
    engine->needs_redraw = 1;
    printf("[RENDER] scale=%s upload=%s image=%dx%d window=%dx%d\n",
           engine->use_xrender ? "XRender" : "CPU",
           engine->use_shm ? "MIT-SHM" : "XPutImage", image_w, image_h, width, height);
    return 0;
}

static void record_display_perf(RenderEngine* engine, uint64_t start, uint64_t car_done,
                                uint64_t compose_done, uint64_t scale_done, uint64_t end, bool car_redrawn) {
    if (!engine->profile_start) {
        engine->profile_start = start;
        engine->profile_pcie_start = engine->pcie_frame_id;
        engine->profile_udp_start = engine->udp_frame_id;
    }
    engine->profile_frames++;
    engine->profile_compose_ns += compose_done - start;
    engine->profile_layout_ns += engine->last_layout_ns;
    engine->last_layout_ns = 0;
    engine->profile_car_ns += car_done - start;
    engine->profile_dashboard_ns += compose_done - car_done;
    engine->profile_car_redraws += car_redrawn;
    engine->profile_scale_ns += scale_done - compose_done;
    engine->profile_present_ns += end - scale_done;
    bool overview = engine->presented_mode == DISPLAY_MODE_ALL_CAMERAS;
    if (engine->pcie_frame_id != engine->shown_pcie_id) {
        if (overview || engine->presented_mode <= DISPLAY_MODE_PCIE_BOTTOM_RIGHT)
            engine->profile_s_shown++;
        engine->shown_pcie_id = engine->pcie_frame_id;
    }
    if (engine->udp_frame_id != engine->shown_udp_id) {
        if (overview || (engine->presented_mode >= DISPLAY_MODE_UDP_TOP_LEFT && !overview))
            engine->profile_m_shown++;
        engine->shown_udp_id = engine->udp_frame_id;
    }
    uint64_t span = end - engine->profile_start;
    if (span >= 5000000000ULL) {
        double seconds = span / 1e9;
        engine->display_fps = engine->profile_frames / seconds;
        engine->pcie_shown_fps = engine->profile_s_shown / seconds;
        engine->udp_shown_fps = engine->profile_m_shown / seconds;
        double divisor = engine->profile_frames * 1e6;
        printf("[PERF DISPLAY] render_fps=%.2f S_input_fps=%.2f M_input_fps=%.2f S_shown_fps=%.2f M_shown_fps=%.2f compose_ms=%.2f legacy_ms=%.2f car_ms=%.2f dashboard_ms=%.2f car_redraw_pct=%.1f scale_ms=%.2f present_ms=%.2f reuse_wait_ms=%.2f upload_done_fps=%.2f upload_MB=%.2f upload_rects=%.1f partial=%d backend=%s/%s sync=%s\n",
               engine->display_fps,
               (uint32_t)(engine->pcie_frame_id - engine->profile_pcie_start) / seconds,
               (uint32_t)(engine->udp_frame_id - engine->profile_udp_start) / seconds,
               engine->pcie_shown_fps, engine->udp_shown_fps,
               engine->profile_compose_ns / divisor, engine->profile_layout_ns / divisor,
               engine->profile_car_ns / divisor, engine->profile_dashboard_ns / divisor,
               100.0 * engine->profile_car_redraws / engine->profile_frames, engine->profile_scale_ns / divisor,
               engine->profile_present_ns / divisor, engine->profile_reuse_wait_ns / divisor,
               engine->profile_upload_completions / seconds,
               engine->profile_upload_bytes / double(engine->profile_frames) / 1e6,
               engine->profile_upload_rects / double(engine->profile_frames),
               engine->use_xrender && engine->use_shm && partial_upload_requested(),
               engine->use_xrender ? "xrender" : "cpu", engine->use_shm ? "shm" : "putimage",
               engine->use_shm_completion ? "completion" : "sync");
        engine->profile_start = end;
        engine->profile_pcie_start = engine->pcie_frame_id;
        engine->profile_udp_start = engine->udp_frame_id;
        engine->profile_frames = engine->profile_s_shown = engine->profile_m_shown = 0;
        engine->profile_compose_ns = engine->profile_scale_ns = engine->profile_present_ns = 0;
        engine->profile_layout_ns = engine->profile_car_ns = engine->profile_dashboard_ns = engine->profile_car_redraws = 0;
        engine->profile_reuse_wait_ns = engine->profile_upload_completions = 0;
        engine->profile_upload_bytes = engine->profile_upload_rects = 0;
    }
}

int present_dashboard(RenderEngine* engine) {
    if (!engine || !engine->dashboard || !engine->display_buffer) return -1;
    uint64_t render_started = display_clock_ns();
    uint64_t now = get_current_time_ms();
    bool car_redrawn = update_vehicle_layer(engine);
    uint64_t car_done = display_clock_ns();
    perception_ui::ViewStatus status;
    status.recorded = engine->preview_recorded != 0;
    status.pcie_live = engine->pcie_last_seen && now >= engine->pcie_last_seen &&
                       now - engine->pcie_last_seen < 2000;
    status.udp_live = engine->udp_last_seen && now >= engine->udp_last_seen &&
                      now - engine->udp_last_seen < 2000;
    status.person = emergency_brake_shm && emergency_brake_shm->valid &&
                    emergency_brake_shm->person_detected;
    status.fps = status.recorded ? 0.0f : engine->display_fps;
    status.pcie_fps = engine->pcie_shown_fps;
    status.udp_fps = engine->udp_shown_fps;
    float progress = engine->transition.is_active ? engine->transition.progress :
        (engine->presented_mode == DISPLAY_MODE_ALL_CAMERAS ? -1.0f : 1.0f);
    char mode_label[64];
    snprintf(mode_label, sizeof(mode_label), "%s", get_camera_mode_name(engine->presented_mode));
    if (char* suffix = strchr(mode_label, '(')) {
        while (suffix > mode_label && suffix[-1] == ' ') --suffix;
        *suffix = 0;
    }
    const uint32_t* pixels = engine->dashboard->compose(
        (const uint32_t*)engine->display_buffer, COMBINED_IMAGE_WIDTH,
        engine->vehicle_layer, COMBINED_IMAGE_WIDTH,
        engine->presented_mode == DISPLAY_MODE_ALL_CAMERAS, progress,
        mode_label, status);
    uint64_t compose_done = display_clock_ns();
    engine->last_presented = now;
    engine->needs_redraw = 0;
    if (engine->headless) return 0;
    XImage* image = engine->presentation_image;
    if (!image) return -1;
    wait_for_shared_image(engine);
    engine->uploaded_this_frame = 0;
    int draw_w = engine->window_width;
    int draw_h = draw_w * perception_ui::HEIGHT / perception_ui::WIDTH;
    if (draw_h > engine->window_height) {
        draw_h = engine->window_height;
        draw_w = draw_h * perception_ui::WIDTH / perception_ui::HEIGHT;
    }
    draw_w = std::max(1, draw_w);
    draw_h = std::max(1, draw_h);
    int offset_x = (engine->window_width - draw_w) / 2;
    int offset_y = (engine->window_height - draw_h) / 2;
#ifdef PERCEPTION_HAVE_XRENDER
    if (engine->use_xrender) {
        if (image->bits_per_pixel != 32 || image->byte_order != LSBFirst ||
            image->red_mask != 0xff0000 || image->green_mask != 0xff00 || image->blue_mask != 0xff)
            return -1;
        const bool partial = engine->use_shm && partial_upload_requested();
        const bool warmup = partial && !engine->upload_initialized;
        static thread_local std::vector<UploadRegion> regions;
        prepare_xrender_upload(engine, pixels, partial, regions);
        uint64_t scale_done = display_clock_ns();
        for (size_t i = 0; i < regions.size(); ++i)
            if (put_presentation_region(engine, engine->pixmap, regions[i], i + 1 == regions.size()) != 0) return -1;
        engine->upload_initialized = 1;
        XTransform transform = {};
        transform.matrix[0][0] = XDoubleToFixed((double)perception_ui::WIDTH / draw_w);
        transform.matrix[1][1] = XDoubleToFixed((double)perception_ui::HEIGHT / draw_h);
        transform.matrix[2][2] = XDoubleToFixed(1.0);
        XRenderSetPictureTransform(engine->display, engine->source_picture, &transform);
        XSetForeground(engine->display, engine->gc, 0x050715);
        XFillRectangle(engine->display, engine->window, engine->gc, 0, 0,
                       engine->window_width, engine->window_height);
        XRenderComposite(engine->display, PictOpSrc, engine->source_picture, None,
                         engine->window_picture, 0, 0, 0, 0, offset_x, offset_y, draw_w, draw_h);
        flush_presentation(engine, warmup);
        record_display_perf(engine, render_started, car_done, compose_done, scale_done, display_clock_ns(), car_redrawn);
        return 0;
    }
#endif
    bool native32 = image->bits_per_pixel == 32 && image->red_mask == 0xff0000 &&
                    image->green_mask == 0xff00 && image->blue_mask == 0xff &&
                    image->byte_order == LSBFirst;
    static std::vector<int> x_map;
    if ((int)x_map.size() != draw_w) {
        x_map.resize(draw_w);
        for (int x = 0; x < draw_w; ++x) x_map[x] = x * perception_ui::WIDTH / draw_w;
    }
    for (int y = 0; y < engine->window_height; ++y) {
        bool inside = y >= offset_y && y < offset_y + draw_h;
        const uint32_t* source_row = inside ?
            pixels + ((y - offset_y) * perception_ui::HEIGHT / draw_h) * perception_ui::WIDTH : NULL;
        if (native32) {
            uint32_t* row = (uint32_t*)(image->data + y * image->bytes_per_line);
            std::fill_n(row, engine->window_width, 0xff050715u);
            if (inside) {
                if (draw_w == perception_ui::WIDTH)
                    memcpy(row + offset_x, source_row, draw_w * sizeof(uint32_t));
                else for (int x = 0; x < draw_w; ++x) row[offset_x + x] = source_row[x_map[x]];
            }
        } else {
            for (int x = 0; x < engine->window_width; ++x) {
                uint32_t color = inside && x >= offset_x && x < offset_x + draw_w ?
                    source_row[x_map[x - offset_x]] : 0xff050715u;
                unsigned long pixel =
                    ((((color >> 16) & 255) * image->red_mask / 255) & image->red_mask) |
                    ((((color >> 8) & 255) * image->green_mask / 255) & image->green_mask) |
                    (((color & 255) * image->blue_mask / 255) & image->blue_mask);
                XPutPixel(image, x, y, pixel);
            }
        }
    }
    uint64_t scale_done = display_clock_ns();
    Drawable target = engine->pixmap ? engine->pixmap : engine->window;
    if (put_presentation_image(engine, target, engine->window_width, engine->window_height) != 0) return -1;
    if (engine->pixmap) XCopyArea(engine->display, engine->pixmap, engine->window,
        engine->gc, 0, 0, engine->window_width, engine->window_height, 0, 0);
    flush_presentation(engine);
    record_display_perf(engine, render_started, car_done, compose_done, scale_done, display_clock_ns(), car_redrawn);
    return 0;
}

int render_engine_init(RenderEngine* engine) {
    if (!engine) return -1;
    memset(engine, 0, sizeof(*engine));
    engine->display = XOpenDisplay(NULL);
    if (!engine->display) {
        fprintf(stderr, "Cannot connect to X11 display\n"); return -1;
    }
    int screen = DefaultScreen(engine->display);
    int screen_w = DisplayWidth(engine->display, screen);
    int screen_h = DisplayHeight(engine->display, screen);
    int width = start_fullscreen ? screen_w : std::min(1440, std::max(320, screen_w - 96));
    int height = start_fullscreen ? screen_h : std::min(width * 9 / 16, std::max(180, screen_h - 96));
    engine->window = XCreateSimpleWindow(engine->display, RootWindow(engine->display, screen),
        0, 0, width, height, 0, BlackPixel(engine->display, screen), BlackPixel(engine->display, screen));
    const char* title = "Perception Engine | Surround Vision";
    XStoreName(engine->display, engine->window, title);
    XChangeProperty(engine->display, engine->window,
        XInternAtom(engine->display, "_NET_WM_NAME", False),
        XInternAtom(engine->display, "UTF8_STRING", False), 8, PropModeReplace,
        (const unsigned char*)title, strlen(title));
    engine->wm_delete = XInternAtom(engine->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(engine->display, engine->window, &engine->wm_delete, 1);
    XSelectInput(engine->display, engine->window, ExposureMask | KeyPressMask | StructureNotifyMask);
    XSizeHints hints = {};
    hints.flags = PMinSize; hints.min_width = 320; hints.min_height = 180;
    XSetWMNormalHints(engine->display, engine->window, &hints);
#ifdef PERCEPTION_HAVE_XRENDER
    const char* backend = getenv("VISION_RENDER_BACKEND");
    int major = 0, minor = 0;
    engine->use_xrender = (!backend || !strcmp(backend, "auto") || !strcmp(backend, "xrender")) &&
                         XRenderQueryVersion(engine->display, &major, &minor);
#endif
    engine->gc = XCreateGC(engine->display, engine->window, 0, NULL);
    engine->display_buffer = (char*)calloc(COMBINED_IMAGE_WIDTH * COMBINED_IMAGE_HEIGHT, 4);
    engine->vehicle_layer = (uint32_t*)calloc(COMBINED_IMAGE_WIDTH * COMBINED_IMAGE_HEIGHT, 4);
    try { engine->dashboard = create_dashboard_theme(); }
    catch (const std::bad_alloc&) { render_engine_cleanup(engine); return -1; }
    if (!engine->gc || !engine->display_buffer || !engine->vehicle_layer ||
        resize_presentation(engine, width, height) != 0) {
        render_engine_cleanup(engine); return -1;
    }
    engine->presented_mode = DISPLAY_MODE_ALL_CAMERAS;
    transition_animation_init(&engine->transition);
    engine->is_init = 1;
    XMapWindow(engine->display, engine->window);
    XEvent event;
    do { XNextEvent(engine->display, &event); } while (event.type != MapNotify);
    if (start_fullscreen) set_fullscreen(engine, 1);
    printf("Perception Engine: %dx%d, F11 fullscreen, Q exit\n", width, height);
    printf("PERCEPTION_WINDOW_ID=0x%lx\n", engine->window);
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
    
    uint64_t layout_started = display_clock_ns();
    engine->presented_mode = mode;
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
                int source_y = pcie_display_source_row(y);

                // 后右 0x33 (PCIe bottom-left) -> display (0, 240)
                for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
                    uint16_t pixel = pcie_pixels[(source_y + 240) * PCIE_IMAGE_WIDTH + x];
                    uint8_t b = ((pixel >> 11) & 0x1F) << 3;
                    uint8_t g = ((pixel >> 5) & 0x3F) << 2;
                    uint8_t r = (pixel & 0x1F) << 3;
                    display_pixels[display_y * COMBINED_IMAGE_WIDTH + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
                }

                // 后中复制 0x44 (PCIe bottom-right) -> display (320, 240)
                for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
                    uint16_t pixel = pcie_pixels[(source_y + 240) * PCIE_IMAGE_WIDTH + 320 + x];
                    uint8_t b = ((pixel >> 11) & 0x1F) << 3;
                    uint8_t g = ((pixel >> 5) & 0x3F) << 2;
                    uint8_t r = (pixel & 0x1F) << 3;
                    display_pixels[display_y * COMBINED_IMAGE_WIDTH + 320 + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
                }

                // 后左 0x22 (PCIe top-right) -> display (640, 240)
                for (int x = 0; x < CAMERA_BLOCK_WIDTH; x++) {
                    uint16_t pixel = pcie_pixels[source_y * PCIE_IMAGE_WIDTH + 320 + x];
                    uint8_t b = ((pixel >> 11) & 0x1F) << 3;
                    uint8_t g = ((pixel >> 5) & 0x3F) << 2;
                    uint8_t r = (pixel & 0x1F) << 3;
                    display_pixels[display_y * COMBINED_IMAGE_WIDTH + 640 + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
                }

                // 后中 0x11 (PCIe top-left) -> display (960, 240) - 不渲染,留给car.png
                // 这部分被car.png替换
            }

            // 渲染car.png到最右边320x480区域
            // Vehicle artwork and its alert layer are presented in their own dashboard panel.

            // 在PNG上方绘制车道线

            break;
        }
    }
    
    engine->last_layout_ns = display_clock_ns() - layout_started;
    return present_dashboard(engine);
}

int render_engine_check_exit_event(RenderEngine* engine) {
    if (!engine || !engine->is_init) return 1;
    while (XPending(engine->display)) {
        XEvent event;
        XNextEvent(engine->display, &event);
        if (consume_shm_completion(engine, &event)) continue;
        switch (event.type) {
            case KeyPress: {
                KeySym key = XLookupKeysym(&event.xkey, 0);
                if (key == XK_q || key == XK_Q) return 1;
                if (key == XK_F11 || key == XK_f || key == XK_F)
                    set_fullscreen(engine, !engine->fullscreen);
                else if (key == XK_Escape) {
                    if (engine->fullscreen) set_fullscreen(engine, 0);
                    else return 1;
                }
                break;
            }
            case ConfigureNotify:
                if (resize_presentation(engine, event.xconfigure.width, event.xconfigure.height) != 0)
                    return 1;
                break;
            case Expose: engine->needs_redraw = 1; break;
            case ClientMessage:
                if ((Atom)event.xclient.data.l[0] == engine->wm_delete) return 1;
                break;
            case DestroyNotify: return 1;
        }
    }
    return 0;
}

void render_engine_cleanup(RenderEngine* engine) {
    if (!engine) {
        return;
    }
    
    release_presentation(engine);
    delete engine->dashboard;
    engine->dashboard = NULL;
    free(engine->vehicle_layer);
    engine->vehicle_layer = NULL;

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

static int find_display_asset(const char* name, char* output, size_t capacity) {
    const char* prefixes[] = {"../", "", "udp_rk_rk_yolo/S/x11/"};
    for (const char* prefix : prefixes) {
        int count = snprintf(output, capacity, "%s%s", prefix, name);
        if (count > 0 && count < (int)capacity && access(output, R_OK) == 0) return 0;
    }
    char executable[PATH_MAX];
    ssize_t n = readlink("/proc/self/exe", executable, sizeof(executable) - 1);
    if (n > 0) {
        executable[n] = 0;
        if (char* slash = strrchr(executable, '/')) {
            *slash = 0;
            int count = snprintf(output, capacity, "%s/../../%s", executable, name);
            if (count > 0 && count < (int)capacity && access(output, R_OK) == 0) return 0;
        }
    }
    return -1;
}

static int load_vehicle_artwork() {
    char path[PATH_MAX];
    return find_display_asset("car.png", path, sizeof(path)) == 0 ? load_car_png(path) : -1;
}

static perception_ui::Dashboard* create_dashboard_theme() {
    char path[PATH_MAX];
    int width = 0, height = 0, channels;
    unsigned char* atlas = find_display_asset("dashboard_font.png", path, sizeof(path)) == 0 ?
        stbi_load(path, &width, &height, &channels, 1) : NULL;
    perception_ui::Dashboard* result;
    try { result = new perception_ui::Dashboard(atlas, width, height); }
    catch (...) { stbi_image_free(atlas); throw; }
    stbi_image_free(atlas);
    return result;
}

static int save_dashboard(RenderEngine* engine, const char* path) {
    const uint32_t* pixels = engine->dashboard->pixels();
    FILE* file = fopen(path, "wb");
    if (!file) { perror(path); return -1; }
    fprintf(file, "P6\n%d %d\n255\n", perception_ui::WIDTH, perception_ui::HEIGHT);
    for (int i = 0; i < perception_ui::WIDTH * perception_ui::HEIGHT; ++i) {
        unsigned char rgb[3] = {(unsigned char)(pixels[i] >> 16),
                               (unsigned char)(pixels[i] >> 8), (unsigned char)pixels[i]};
        if (fwrite(rgb, 1, 3, file) != 3) { fclose(file); return -1; }
    }
    return fclose(file) == 0 ? 0 : -1;
}

static int preview_dashboard(const char* output, const char* input, DisplayMode mode, int alarm) {
    int width, height, channels;
    unsigned char* rgb = stbi_load(input, &width, &height, &channels, 3);
    if (!rgb || width != 1280 || height != 480) {
        fprintf(stderr, "Preview requires an existing 1280x480 combined_last_frame.ppm: %s\n", input);
        stbi_image_free(rgb); return 1;
    }
    std::vector<uint16_t> pcie(640 * 480), udp(640 * 480);
    for (int y = 0; y < 480; ++y) for (int x = 0; x < 640; ++x) {
        for (int stream = 0; stream < 2; ++stream) {
            const unsigned char* c = rgb + (y * 1280 + x + stream * 640) * 3;
            uint16_t pixel = ((c[0] >> 3) << 11) | ((c[1] >> 2) << 5) | (c[2] >> 3);
            (stream ? udp : pcie)[y * 640 + x] = pixel;
        }
    }
    stbi_image_free(rgb);
    RenderEngine engine = {};
    engine.headless = engine.is_init = engine.preview_recorded = 1;
    engine.pcie_last_seen = engine.udp_last_seen = get_current_time_ms();
    engine.display_buffer = (char*)calloc(1280 * 480, 4);
    engine.vehicle_layer = (uint32_t*)calloc(1280 * 480, 4);
    engine.dashboard = create_dashboard_theme();
    transition_animation_init(&engine.transition);
    if (!engine.display_buffer || !engine.vehicle_layer || load_vehicle_artwork() != 0) {
        render_engine_cleanup(&engine); return 1;
    }
    EmergencyBrakeSharedMemory simulated_alert = {};
    if (alarm) {
        simulated_alert.valid = simulated_alert.person_detected = 1;
        simulated_alert.box_left = 20; simulated_alert.box_top = 70;
        simulated_alert.box_right = 140; simulated_alert.box_bottom = 300;
        emergency_brake_shm = &simulated_alert;
        blink_state = 1; last_blink_time = get_current_time_ms();
    }
    int result = render_engine_render_camera_view(&engine, (const uint8_t*)pcie.data(),
                                                  (const uint8_t*)udp.data(), mode);
    if (!result) result = save_dashboard(&engine, output);
    emergency_brake_shm = NULL;
    render_engine_cleanup(&engine);
    stbi_image_free(car_png_data); car_png_data = NULL;
    printf("Dashboard preview: %s\n", output);
    return result != 0;
}

int main(int argc, char** argv) {
    setvbuf(stdout, NULL, _IOLBF, 0);
    const char* preview_output = NULL;
    const char* preview_input = "combined_last_frame.ppm";
    DisplayMode preview_mode = DISPLAY_MODE_ALL_CAMERAS;
    int preview_alarm = 0;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--windowed")) start_fullscreen = 0;
        else if (!strcmp(argv[i], "--fullscreen")) start_fullscreen = 1;
        else if (!strcmp(argv[i], "--preview") && i + 1 < argc) preview_output = argv[++i];
        else if (!strcmp(argv[i], "--input") && i + 1 < argc) preview_input = argv[++i];
        else if (!strcmp(argv[i], "--preview-mode") && i + 1 < argc)
            preview_mode = (DisplayMode)strtol(argv[++i], NULL, 0);
        else if (!strcmp(argv[i], "--preview-alarm")) preview_alarm = 1;
        else if (!strcmp(argv[i], "--snapshot") && i + 1 < argc) snapshot_path = argv[++i];
        else if (!strcmp(argv[i], "--frames") && i + 1 < argc) frame_limit = atoi(argv[++i]);
        else if (!strcmp(argv[i], "--help")) {
            printf("Perception Engine: [--windowed|--fullscreen] [--snapshot output.ppm] [--frames N]\n");
            printf("Offline preview: --preview output.ppm [--input combined_last_frame.ppm] [--preview-mode 0x66] [--preview-alarm]\n");
            return 0;
        } else {
            fprintf(stderr, "Unknown or incomplete option: %s\n", argv[i]); return 1;
        }
    }
    if (preview_output) return preview_dashboard(preview_output, preview_input, preview_mode, preview_alarm);

    RenderEngine render_engine;
    struct timeval stream_start, current_time;
    uint8_t* pcie_buffer = NULL;
    uint8_t* udp_buffer = NULL;
    int pcie_available = 0;
    int udp_available = 0;
    time_t last_pcie_retry = 0;
    time_t last_udp_retry = 0;
    int frame_count = 0;
    double total_time = 0.0;
    uint32_t last_pcie_frame_id = 0;
    uint32_t last_udp_frame_id = 0;
    DisplayMode current_display_mode = DISPLAY_MODE_ALL_CAMERAS;  // 默认全部显示
    uint32_t last_control_update = 0;
    
    // 设置信号处理
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    printf("=== Perception Engine / S-End Surround Vision ===\n");
    printf("Six-view dashboard, eight selectable cameras, F11 fullscreen, Q exit\n");
    printf("Source streams remain 640x480; dashboard canvas is 1600x900\n");

    // 分配图像缓冲区
    pcie_buffer = (uint8_t*)aligned_alloc(64, PCIE_IMAGE_WIDTH * PCIE_IMAGE_HEIGHT * 2);
    udp_buffer = (uint8_t*)aligned_alloc(64, UDP_IMAGE_WIDTH * UDP_IMAGE_HEIGHT * 2);
    if (!pcie_buffer || !udp_buffer) {
        printf("图像缓冲区分配失败！\n");
        free(pcie_buffer);
        free(udp_buffer);
        return -1;
    }
    
    // 本地PCIe尚未运行时也允许远端UDP画面进入窗口。
    memset(pcie_buffer, 0, PCIE_IMAGE_WIDTH * PCIE_IMAGE_HEIGHT * 2);
    memset(udp_buffer, 0, UDP_IMAGE_WIDTH * UDP_IMAGE_HEIGHT * 2);
    printf("\n=== 初始化PCIe共享内存连接 ===\n");
    if (shared_memory_init_pcie(&pcie_shm_manager, 0) == 0) {
        pcie_available = 1;
        printf("PCIe共享内存连接成功\n");
    } else {
        printf("PCIe共享内存暂不可用，继续显示UDP图像\n");
    }

    // 接收进程可以稍后启动；窗口持续等待UDP共享内存出现。
    printf("\n=== 初始化UDP共享内存连接 ===\n");
    if (shared_memory_init_udp(&udp_shm_manager, 0) == 0) {
        udp_available = 1;
        printf("UDP共享内存连接成功\n");
    } else {
        printf("UDP共享内存暂不可用，等待网络接收进程\n");
    }

    // 加载car.png图像
    printf("\n=== 加载car.png图像 ===\n");
    if (load_vehicle_artwork() != 0) {
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
        if (pcie_available) shared_memory_cleanup(&pcie_shm_manager);
        if (udp_available) shared_memory_cleanup(&udp_shm_manager);
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
            if (display_verbose() && ++curve_read_count % 30 == 0) {
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
        
        // 每秒重试连接本地采集进程；远端UDP渲染不依赖其启动顺序。
        if (!pcie_available && time(NULL) != last_pcie_retry) {
            last_pcie_retry = time(NULL);
            if (shared_memory_init_pcie(&pcie_shm_manager, 0) == 0) {
                pcie_available = 1;
                printf("PCIe共享内存已连接\n");
            }
        }
        int pcie_result = pcie_available
            ? shared_memory_read_image(&pcie_shm_manager, pcie_buffer,
                                       PCIE_IMAGE_WIDTH * PCIE_IMAGE_HEIGHT * 2,
                                       &pcie_width, &pcie_height, &pcie_frame_id)
            : 0;

        if (pcie_result > 0 && pcie_frame_id != last_pcie_frame_id) {
            last_pcie_frame_id = pcie_frame_id;
            pcie_updated = 1;
            render_engine.pcie_last_seen = get_current_time_ms();
            render_engine.pcie_frame_id = pcie_frame_id;
        }
        
        if (!udp_available && time(NULL) != last_udp_retry) {
            last_udp_retry = time(NULL);
            if (shared_memory_init_udp(&udp_shm_manager, 0) == 0) {
                udp_available = 1;
                printf("UDP共享内存已连接\n");
            }
        }
        int udp_result = udp_available
            ? shared_memory_read_image(&udp_shm_manager, udp_buffer,
                                       UDP_IMAGE_WIDTH * UDP_IMAGE_HEIGHT * 2,
                                       &udp_width, &udp_height, &udp_frame_id)
            : 0;

        if (udp_result > 0 && udp_frame_id != last_udp_frame_id) {
            last_udp_frame_id = udp_frame_id;
            udp_updated = 1;
            render_engine.udp_last_seen = get_current_time_ms();
            render_engine.udp_frame_id = udp_frame_id;
        }
        
        // 如果有任意一个通道更新，或者正在播放动画，或者正在显示文字，就重新渲染
        if (pcie_updated || udp_updated) render_engine.needs_redraw = 1;
        uint64_t render_now = get_current_time_ms();
        unsigned int minimum_interval = render_engine.transition.is_active ? 16 : 33;
        if ((render_engine.needs_redraw || render_engine.transition.is_active ||
             render_now - render_engine.last_presented >= 100) &&
            render_now - render_engine.last_presented >= minimum_interval) {
            if (!render_engine.transition.is_active)
                transition_animation_update(&render_engine.transition);
            if (render_engine_render_with_transition(&render_engine, pcie_buffer, udp_buffer, current_display_mode) != 0) {
                printf("渲染失败！\n");
                break;
            }
            
            frame_count++;
            if (frame_limit > 0 && frame_count >= frame_limit) keep_running = 0;
            
            if (display_verbose() && frame_count % 30 == 0) {  // 减少输出频率
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
            if (display_verbose() && ++no_update_count % 100 == 0) {
                printf("等待数据更新... PCIe result:%d, UDP result:%d\n", pcie_result, udp_result);
            }
        }
        
        // 检查退出事件
        if (render_engine_check_exit_event(&render_engine)) {
            printf("检测到退出事件\n");
            break;
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
    if (last_pcie_frame_id > 0 || last_udp_frame_id > 0) {
        printf("\n=== 保存最后一帧合成图像 ===\n");
        save_combined_frame_to_ppm(pcie_buffer, udp_buffer, "combined_last_frame.ppm");
    }
    
    // 系统清理
    printf("\n=== 系统清理 ===\n");
    if (snapshot_path) save_dashboard(&render_engine, snapshot_path);
    render_engine_cleanup(&render_engine);

    if (display_control_shm) {
        display_control_shm_cleanup(display_control_shm, 0);
    }

    if (emergency_brake_shm) {
        emergency_brake_shm_cleanup(emergency_brake_shm, 0);
    }

    curve_detection_shm_cleanup();

    if (pcie_available) shared_memory_cleanup(&pcie_shm_manager);
    if (udp_available) shared_memory_cleanup(&udp_shm_manager);
    
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
