#include "../../../common/image/frame_pixels.h"
#include "pcie_dma_read_test.h"
#include "../../../common/pcie/capture_support.h"
#include "fspi_module.h"
#include "yolo_integration.h"
#include "latest_lane_worker.h"
#include "front_radial_preview.h"
#include "line_style_ipc.h"
#include "traffic_service.h"
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
#ifndef ENABLE_RENDERING
#define ENABLE_RENDERING 1 // 1=启用渲染, 0=禁用渲染（仅PCIe采集）
#endif

#if ENABLE_RENDERING
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

/* aligned_alloc 的声明需要先启用 _ISOC11_SOURCE。 */
#ifndef _ISOC11_SOURCE
#define _ISOC11_SOURCE
#endif

// ── 图像处理 ──
// *作   者：科研轮转（PCIE模块）
// 本版更新：2026年10月18日
// *实现功能：PCIe视频流采集与YOLO实时目标识别（可选渲染功能）
// *性能优化：在图像拼接完成后直接进行YOLO识别和标注绘制
// ── 图像处理 ──

// 本节设置处理性能相关选项。
#define HIGH_PERFORMANCE_MODE 1 // 1=高性能模式, 0=调试模式

// 预设性能设置 - 选择其中一个
#define PERFORMANCE_PRESET 2 // 1=最高精度, 2=平衡模式, 3=最高帧率

#if PERFORMANCE_PRESET == 1
// 最高精度模式：每帧识别
#define YOLO_DETECT_INTERVAL 1
#define FPS_REPORT_INTERVAL 100
#elif PERFORMANCE_PRESET == 2
// 平衡模式：每3帧识别
#define YOLO_DETECT_INTERVAL 3
#define FPS_REPORT_INTERVAL 200
#elif PERFORMANCE_PRESET == 3
// 最高帧率模式：每5帧识别
#define YOLO_DETECT_INTERVAL 5
#define FPS_REPORT_INTERVAL 300
#else
// 该分支使用用户设定的模式。
#define YOLO_DETECT_INTERVAL 3  // 可自定义识别间隔
#define FPS_REPORT_INTERVAL 200 // 可自定义FPS报告间隔
#endif

// 识别结果去重设置
#define ENABLE_DETECTION_DEDUP 1 // 1=启用去重, 0=禁用去重
#ifndef DEDUP_IOU_THRESHOLD
#define DEDUP_IOU_THRESHOLD 0.8f // IoU阈值，超过此值的同类别识别框会被合并
#endif

#if HIGH_PERFORMANCE_MODE
#define PERF_PRINTF(...) // 高性能模式下关闭调试输出
#else
#define PERF_PRINTF printf // 调试模式下保留输出
#endif

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define LEADING_PIXELS 120  /* 图像处理：每行前导像素数量 */
#define EXTRA_SKIP_PIXELS 0 /* 图像处理：额外需要跳过的前导像素（微调） */

/* 图像处理：列重排设置：将第n列到第m列移动到行末 */
#define COLUMN_REARRANGE_START 0 /* 图像处理：起始列n (0-based索引，0表示第一列) */
#define COLUMN_REARRANGE_END 0   /* 图像处理：结束列m (0-based索引，包含此列) */
#define COLUMN_SHIFT_UP_ROWS 0   /* 图像处理：移动到末尾的列向上移动k行 (0表示不移动) */

/* 图像处理：控制是否呈现前导像素 */
#define SHOW_LEADING_PIXELS 0 /* 图像处理：0=不呈现前导像素(默认), 1=呈现前导像素 */

#if SHOW_LEADING_PIXELS
#define DISPLAY_WIDTH IMAGE_WIDTH /* 图像处理：呈现前导像素时，保持呈现宽度为640 */
#define ACTUAL_WIDTH IMAGE_WIDTH  /* 图像处理：实际复制的像素宽度 */
#define SKIP_PIXELS 0             /* 图像处理：从前导像素开始 */
#else
#define DISPLAY_WIDTH IMAGE_WIDTH                        /* 图像处理：不呈现前导像素，呈现宽度为640 */
#define ACTUAL_WIDTH IMAGE_WIDTH                         /* 图像处理：实际复制的像素宽度 */
#define SKIP_PIXELS (LEADING_PIXELS + EXTRA_SKIP_PIXELS) /* 图像处理：跳过前导像素+额外偏移 */
#endif

#define DISPLAY_HEIGHT IMAGE_HEIGHT /* 图像处理：直接使用原始图像尺寸 */

// ── 图像处理 ──

// ── 图像处理 ──
typedef struct
{
    int pci_driver_fd;
    COMMAND_OPERATION command_operation;
    DMA_OPERATION dma_operation;
    int dma_rows; // 该值固定为 1：此时 FPGA 每次 DMA 请求只推进一行。
} PCIEManager;

#if ENABLE_RENDERING
typedef struct
{
    Display *display;
    Window window;
    GC gc;
    XImage *ximage;
    char *display_buffer;
    Pixmap pixmap;           /* 图像处理：双缓冲pixmap */
    int use_fast_mode;       /* 图像处理：是否使用快速模式 */
    int use_shm;             /* 图像处理：是否使用共享内存 */
    XShmSegmentInfo shminfo; /* 图像处理：共享内存信息 */
    XFontStruct *font;       /* 本对象描述字形及度量数据。 */
    int is_init;
} RenderEngine;
#endif

// ── 图像处理 ──
// 供后续实现调用的接口声明。
// ── 图像处理 ──
// 本节统一处理运行未成功。
void emergency_stop(const char *error_msg, const char *component);
void cleanup_all_resources(PCIEManager *pcie_mgr, SharedMemoryManager *shm_mgr,
                           YOLOEngine *yolo_eng);


int pcie_manager_init(PCIEManager *manager);
void pcie_manager_get_device_info(PCIEManager *manager);
int pcie_manager_capture_frame(PCIEManager *manager, uint8_t *image_buffer, size_t buffer_size);
int pcie_manager_init_streaming(PCIEManager *manager);
void pcie_manager_cleanup_streaming(PCIEManager *manager);
void pcie_manager_cleanup(PCIEManager *manager);

#if ENABLE_RENDERING
// 本节实现画面呈现接口。
int render_engine_init(RenderEngine *engine);
int render_engine_render_frame_with_detections(RenderEngine *engine, const uint8_t *rgb888_data);
int render_engine_check_exit_event(RenderEngine *engine);
void render_engine_cleanup(RenderEngine *engine);
#endif

// 本模块使用的辅助操作。
int nano_delay(long delay);
void rgb565_to_rgb888(const uint16_t *image565, uint8_t *image888, size_t num_pixels);
void rgb888_to_rgb565(const uint8_t *image888, uint16_t *image565, size_t num_pixels);
int save_frame_to_ppm(const uint8_t *rgb888_data, const char *filename);
double get_time_diff(struct timeval start, struct timeval end);
void rearrange_columns(uint16_t *row_buffer, int width, int start_col, int end_col);
void shift_columns_up(uint8_t *image_buffer, int width, int height, int start_col, int end_col,
                      int shift_rows);

// ── 图像处理 ──
// 为文件级共用状态分配存储。
// ── 图像处理 ──
uint8_t *image_buf_temp;
uint8_t *image_buf_888;
uint8_t *image_buf_565_out; // RGB565输出缓冲区（用来共享内存）
YOLOEngine yolo_engine;

// 此时视频流跟踪变量
int current_video_stream = 5; // 选择并保持视频流编号 5。

// 视频流切换相关变量
int available_streams[] = {2, 5, 6};
int stream_count = 3;
int current_stream_index = 0;
struct timeval last_switch_time;

// 全局变量用来信号处理
static volatile int g_running = 1;
static PCIEManager *g_pcie_manager = NULL;
static SharedMemoryManager *g_shm_manager = NULL;
static YOLOEngine *g_yolo_engine = NULL;
#if ENABLE_RENDERING
static RenderEngine *g_render_engine = NULL;
#endif

// ── 图像处理 ──
// 处理进程信号的回调入口。
// ── 图像处理 ──
void signal_handler(int signum)
{
    (void)signum;
    // RKNN、Xlib、stdio 和 DMA 移除都不满足异步信号安全，
    // 因此由主循环完成此时调用后，各项资源只释放一次。
    g_running = 0;
}

void setup_signal_handlers()
{
    signal(SIGINT, signal_handler);  // 响应 Ctrl+C 中断请求。
    signal(SIGTERM, signal_handler); // 响应进程终止请求。
    signal(SIGQUIT, signal_handler); // 响应 Ctrl+反斜杠退出请求。
}

// ── 图像处理 ──
// PCIE管理函数实现 - PCIE设备管理
// ── 图像处理 ──

int pcie_manager_init(PCIEManager *manager)
{
    printf("开始初始化PCIE设备...\n");

    if (!manager)
    {
        printf("*** 致命错误：PCIE管理器指针无效 ***\n");
        printf("*** 程序立即停止 ***\n");
        return -1;
    }

    // 先建立结构体的初始值。
    manager->pci_driver_fd = -1;
    manager->dma_rows = 1;
    memset(&manager->command_operation, 0, sizeof(COMMAND_OPERATION));
    memset(&manager->dma_operation, 0, sizeof(DMA_OPERATION));

    // 取得 PCIe 设备文件句柄。
    manager->pci_driver_fd = open(PCIE_DRIVER_FILE_PATH, O_RDWR);
    if (manager->pci_driver_fd < 0)
    {
        printf("*** 致命错误：PCIE设备打开失败 ***\n");
        printf("*** 错误原因：%s ***\n", strerror(errno));
        printf("*** 检查项目：***\n");
        printf("*** 1. PCIE驱动是否正确加载 ***\n");
        printf("*** 2. 设备文件路径是否正确：%s ***\n", PCIE_DRIVER_FILE_PATH);
        printf("*** 3. 是否有足够的权限访问设备 ***\n");
        printf("*** 程序立即停止 ***\n");
        return -1;
    }

    // 查询此时设备的描述信息。
    pcie_manager_get_device_info(manager);

    printf("PCIE设备初始化完成\n");
    return 0;
}

void pcie_manager_get_device_info(PCIEManager *manager)
{
    if (!manager || manager->pci_driver_fd < 0)
        return;

    manager->command_operation.w_r = read_num;
    if (ioctl(manager->pci_driver_fd, PCI_READ_DATA_CMD, &manager->command_operation) == 0)
    {
        printf("PCIE设备信息获取已完成\n");
        printf("设备ID: 0x%x, 厂商ID: 0x%x\n",
               manager->command_operation.get_pci_dev_info.device_id,
               manager->command_operation.get_pci_dev_info.vendor_id);
    }
    else
    {
        printf("PCIE设备信息获取未成功\n");
    }
}

int pcie_manager_capture_frame(PCIEManager *manager, uint8_t *image_buffer, size_t buffer_size)
{
    if (!manager || manager->pci_driver_fd < 0 || !image_buffer ||
        buffer_size < (LEADING_PIXELS + IMAGE_WIDTH) * IMAGE_HEIGHT * 2)
        return -1;

    const unsigned int row_bytes = (LEADING_PIXELS + IMAGE_WIDTH) * 2;
    const unsigned int rows = 1; // FPGA 使用流式读取协议，不能视为线性 DDR 批量读取。
    for (unsigned int i = 0; i < IMAGE_HEIGHT; i += rows)
    {
        if (!g_running)
        {
            errno = ECANCELED;
            return -1;
        }
        const unsigned int count = (i + rows <= IMAGE_HEIGHT) ? rows : IMAGE_HEIGHT - i;
        manager->dma_operation.offset_addr = i * row_bytes;
        manager->dma_operation.current_len = count * row_bytes / 4;
        if (ioctl(manager->pci_driver_fd, PCI_DMA_WRITE_CMD, &manager->dma_operation) < 0 ||
            ioctl(manager->pci_driver_fd, PCI_READ_FROM_KERNEL_CMD, &manager->dma_operation) < 0)
            return -1;
        for (unsigned int row = 0; row < count; ++row)
        {
            const unsigned int target_row = i + row;
#if SHOW_LEADING_PIXELS
            const unsigned int skip = 0;
#else
            const unsigned int skip = SKIP_PIXELS * 2;
#endif
            memcpy(image_buffer + target_row * ACTUAL_WIDTH * 2,
                   manager->dma_operation.data.read_buf + row * row_bytes + skip, ACTUAL_WIDTH * 2);
            if (COLUMN_REARRANGE_START >= 0 && COLUMN_REARRANGE_END < ACTUAL_WIDTH &&
                COLUMN_REARRANGE_START <= COLUMN_REARRANGE_END &&
                !(COLUMN_REARRANGE_START == 0 && COLUMN_REARRANGE_END == 0))
            {
                rearrange_columns((uint16_t *)(image_buffer + target_row * ACTUAL_WIDTH * 2),
                                  ACTUAL_WIDTH, COLUMN_REARRANGE_START, COLUMN_REARRANGE_END);
            }
        }
    }

    /* 在帧采集完成后，若需要对移动到末尾的列进行垂直上移 */
    if (COLUMN_REARRANGE_START >= 0 && COLUMN_REARRANGE_END < ACTUAL_WIDTH &&
        COLUMN_REARRANGE_START <= COLUMN_REARRANGE_END &&
        !(COLUMN_REARRANGE_START == 0 && COLUMN_REARRANGE_END == 0) && COLUMN_SHIFT_UP_ROWS > 0)
    {
        /* 求出移动到末尾的列的起始位置。 */
        int moved_col_start = ACTUAL_WIDTH - (COLUMN_REARRANGE_END - COLUMN_REARRANGE_START + 1);
        int moved_col_end = ACTUAL_WIDTH - 1;

        /* 图像处理：对移动到末尾的列进行垂直上移 */
        shift_columns_up(image_buffer, ACTUAL_WIDTH, IMAGE_HEIGHT, moved_col_start, moved_col_end,
                         COLUMN_SHIFT_UP_ROWS);
    }

    /* 在帧结束后，更新下一帧的起始位置 - 重置为0，保证每次从帧头开始 */
    // 上述各次请求均给出相对于帧起点的绝对偏移。

    return 0;
}

int pcie_manager_init_streaming(PCIEManager *manager)
{
    if (!manager || manager->pci_driver_fd < 0)
    {
        printf("*** 致命错误：PCIE设备未初始化，无法启动数据流传输 ***\n");
        printf("*** 程序立即停止 ***\n");
        return -1;
    }

    printf("初始化视频流采集...\n");

    /* 设置DMA操作参数（只需要设置一次）。 */
    /* 单行读取量为 LEADING_PIXELS 前导加 IMAGE_WIDTH 有效像素；按每像素 2 字节换算，即 (LEADING_PIXELS+IMAGE_WIDTH)*2/4 个 dword。 */
    const char *row_option = getenv("VISION_DMA_ROWS");
    if (row_option && strcmp(row_option, "1") != 0)
    {
        fprintf(stderr,
                "VISION_DMA_ROWS must be 1: current FPGA counts one line per DMA request\n");
        return -1;
    }
    manager->dma_rows = 1;
    printf("DMA rows/request=1 (fixed by the current FPGA streaming protocol)\n");
    manager->dma_operation.current_len = (LEADING_PIXELS + IMAGE_WIDTH) * 2 / 4;
    manager->dma_operation.offset_addr = 0;
    memset(manager->dma_operation.data.write_buf, 0, DMA_MAX_PACKET_SIZE);

    if (capture_start_dma(manager->pci_driver_fd, &manager->dma_operation) != 0)
        return -1;

    printf("视频流采集初始化完成\n");
    return 0;
}

void pcie_manager_cleanup_streaming(PCIEManager *manager)
{
    if (manager && manager->pci_driver_fd >= 0)
    {
        /* 解除此前建立的地址映射。 */
        ioctl(manager->pci_driver_fd, PCI_UMAP_ADDR_CMD, &manager->dma_operation);
        printf("视频流采集已结束\n");
    }
}

void pcie_manager_cleanup(PCIEManager *manager)
{
    if (manager && manager->pci_driver_fd >= 0)
    {
        close(manager->pci_driver_fd);
        manager->pci_driver_fd = -1;
        printf("PCIE设备已关闭\n");
    }
}

#if ENABLE_RENDERING
// ── 图像处理 ──
// 渲染引擎函数实现 - X11图形渲染
// ── 图像处理 ──

static int m_shm_attach_error = 0;
static int m_shm_error_handler(Display *, XErrorEvent *)
{
    m_shm_attach_error = 1;
    return 0;
}

static int try_shared_ximage(RenderEngine *engine, int screen)
{
    if (!XShmQueryExtension(engine->display))
        return 0;
    XShmSegmentInfo memory = {};
    memory.shmid = -1;
    XImage *image = XShmCreateImage(engine->display, DefaultVisual(engine->display, screen),
                                    DefaultDepth(engine->display, screen), ZPixmap, NULL, &memory,
                                    DISPLAY_WIDTH, DISPLAY_HEIGHT);
    if (!image)
        return 0;
    if (image->bits_per_pixel != 32 || image->bytes_per_line < DISPLAY_WIDTH * 4)
    {
        XDestroyImage(image);
        return 0;
    }
    memory.shmid =
        shmget(IPC_PRIVATE, (size_t)image->bytes_per_line * image->height, IPC_CREAT | 0600);
    if (memory.shmid < 0)
    {
        XDestroyImage(image);
        return 0;
    }
    memory.shmaddr = (char *)shmat(memory.shmid, NULL, 0);
    if (memory.shmaddr == (char *)-1)
    {
        shmctl(memory.shmid, IPC_RMID, NULL);
        XDestroyImage(image);
        return 0;
    }
    image->data = memory.shmaddr;
    memory.readOnly = False;
    XSync(engine->display, False);
    m_shm_attach_error = 0;
    int (*previous)(Display *, XErrorEvent *) = XSetErrorHandler(m_shm_error_handler);
    Bool attached = XShmAttach(engine->display, &memory);
    XSync(engine->display, False);
    XSetErrorHandler(previous);
    if (!attached || m_shm_attach_error)
    {
        image->data = NULL;
        XDestroyImage(image);
        shmdt(memory.shmaddr);
        shmctl(memory.shmid, IPC_RMID, NULL);
        return 0;
    }
    // 双方已附加共享段，立即标记删除以防进程被强制结束时泄漏。
    shmctl(memory.shmid, IPC_RMID, NULL);
    engine->shminfo = memory;
    // XShmCreateImage 会为 XShmPutImage 保存此指针；它必须比
    // 辅助函数栈副本存活更久，并一直有效到 XImage 销毁。
    image->obdata = (XPointer)&engine->shminfo;
    engine->ximage = image;
    engine->display_buffer = memory.shmaddr;
    engine->use_shm = 1;
    return 1;
}

int render_engine_init(RenderEngine *engine)
{
    if (!engine)
        return -1;

    printf("开始初始化渲染引擎...\n");

    // 先建立结构体的初始值。
    memset(engine, 0, sizeof(RenderEngine));
    engine->shminfo.shmid = -1;

    // 读取并检查 DISPLAY 设置。
    char *display_env = getenv("DISPLAY");
    printf("DISPLAY环境变量: %s\n", display_env ? display_env : "未设置");

    if (!display_env)
    {
        printf("注意: DISPLAY环境变量未设置，尝试使用默认值 :0\n");
        setenv("DISPLAY", ":0", 0);
    }

    // 建立到 X 服务器的接入。
    printf("尝试接入X服务器...\n");
    engine->display = XOpenDisplay(NULL);
    if (!engine->display)
    {
        printf("✗ 异常: 无法接入到X服务器\n");
        printf("可能的原因:\n");
        printf("1. X11服务未运行 (检查: ps aux | grep Xorg)\n");
        printf("2. DISPLAY变量错误 (此时: %s)\n", getenv("DISPLAY"));
        printf("3. SSH接入缺少X11转发 (使用: ssh -X)\n");
        printf("4. 权限问题 (尝试: xhost +local:)\n");
        printf("建议修复:\n");
        printf("- 本地运行: export DISPLAY=:0\n");
        printf("- SSH接入: ssh -X user@host\n");
        printf("- 虚拟呈现: Xvfb :1 -screen 0 1024x768x24 &; export DISPLAY=:1\n");
        return -1;
    }
    printf("✓ X服务器接入已完成\n");

    // 选择 X 接入的默认屏幕。
    int screen = DefaultScreen(engine->display);
    int screen_width = DisplayWidth(engine->display, screen);
    int screen_height = DisplayHeight(engine->display, screen);
    int depth = DefaultDepth(engine->display, screen);

    printf("屏幕信息: %dx%d, 深度: %d位, 屏幕: %d\n", screen_width, screen_height, depth, screen);

    // 为呈现建立 X 窗口。
    printf("建立窗口: %dx%d...\n", DISPLAY_WIDTH, DISPLAY_HEIGHT);
    engine->window =
        XCreateSimpleWindow(engine->display, RootWindow(engine->display, screen), 100, 100,
                            DISPLAY_WIDTH, DISPLAY_HEIGHT, 2, BlackPixel(engine->display, screen),
                            WhitePixel(engine->display, screen));

    if (!engine->window)
    {
        printf("✗ 异常: 窗口建立未成功\n");
        XCloseDisplay(engine->display);
        return -1;
    }
    printf("✓ 窗口建立已完成\n");

    // 设置新建窗口的属性。
    printf("设置窗口属性...\n");
    XSelectInput(engine->display, engine->window,
                 ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask);

    printf("映射窗口到屏幕...\n");
    XMapWindow(engine->display, engine->window);
    XStoreName(engine->display, engine->window, "PCIE+YOLO 目标检测系统");
    XFlush(engine->display);

    // 等候 X 窗口映射完成。
    printf("等待窗口呈现...\n");
    XEvent event;
    while (1)
    {
        XNextEvent(engine->display, &event);
        if (event.type == MapNotify)
        {
            printf("✓ 窗口已已完成呈现\n");
            break;
        }
        if (event.type == Expose)
        {
            printf("✓ 窗口准备就绪\n");
            break;
        }
    }

    // 取得窗口绘制所需的 GC。
    engine->gc = XCreateGC(engine->display, engine->window, 0, NULL);

    // 为双缓冲呈现申请 Pixmap。
    engine->pixmap = XCreatePixmap(engine->display, engine->window, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                                   DefaultDepth(engine->display, screen));

    if (try_shared_ximage(engine, screen))
    {
        printf("M X11 transfer=MIT-SHM (server-read fence enabled)\n");
    }
    else
    {
        engine->display_buffer = (char *)malloc(DISPLAY_WIDTH * DISPLAY_HEIGHT * 4);
        if (!engine->display_buffer)
        {
            render_engine_cleanup(engine);
            return -1;
        }
        engine->ximage =
            XCreateImage(engine->display, DefaultVisual(engine->display, screen),
                         DefaultDepth(engine->display, screen), ZPixmap, 0, engine->display_buffer,
                         DISPLAY_WIDTH, DISPLAY_HEIGHT, 32, DISPLAY_WIDTH * 4);
        if (!engine->ximage)
        {
            render_engine_cleanup(engine);
            return -1;
        }
        printf("M X11 transfer=XPutImage fallback\n");
    }

    // 读取呈现所需的字体资源。
    engine->font = XLoadQueryFont(engine->display, "fixed");
    if (engine->font)
    {
        XSetFont(engine->display, engine->gc, engine->font->fid);
    }

    engine->is_init = 1;
    printf("渲染引擎初始化完成\n");
    return 0;
}

int render_engine_render_frame_with_detections(RenderEngine *engine, const uint8_t *rgb888_data)
{
    if (!engine || !engine->is_init || !rgb888_data)
        return -1;
    for (int y = 0; y < DISPLAY_HEIGHT; ++y)
    {
        unsigned char *dst =
            (unsigned char *)engine->display_buffer + y * engine->ximage->bytes_per_line;
        const uint8_t *src = rgb888_data + y * DISPLAY_WIDTH * 3;
        for (int x = 0; x < DISPLAY_WIDTH; ++x)
        {
            dst[x * 4] = src[x * 3];
            dst[x * 4 + 1] = src[x * 3 + 1];
            dst[x * 4 + 2] = src[x * 3 + 2];
            dst[x * 4 + 3] = 255;
        }
    }
    if (engine->use_shm)
        XShmPutImage(engine->display, engine->pixmap, engine->gc, engine->ximage, 0, 0, 0, 0,
                     DISPLAY_WIDTH, DISPLAY_HEIGHT, False);
    else
        XPutImage(engine->display, engine->pixmap, engine->gc, engine->ximage, 0, 0, 0, 0,
                  DISPLAY_WIDTH, DISPLAY_HEIGHT);
    XCopyArea(engine->display, engine->pixmap, engine->window, engine->gc, 0, 0, DISPLAY_WIDTH,
              DISPLAY_HEIGHT, 0, 0);
    if (engine->use_shm)
    {
        // 在 Xorg 完成读取前，不得覆盖该共享图像的像素。
        XSync(engine->display, False);
    }
    else
        XFlush(engine->display);
    return 0;
}

int render_engine_check_exit_event(RenderEngine *engine)
{
    if (!engine || !engine->is_init)
        return 1;

    XEvent event;
    while (XPending(engine->display))
    {
        XNextEvent(engine->display, &event);

        switch (event.type)
        {
        case KeyPress:
            return 1; // 收取到任意按键后退出。
        case ButtonPress:
            return 1; // 任意鼠标按键退出
        case ClientMessage:
            return 1; // 处理窗口管理器发出的关闭请求。
        }
    }

    return 0; // 维持此时处理循环。
}

void render_engine_cleanup(RenderEngine *engine)
{
    if (!engine)
        return;
    if (engine->use_shm && engine->display)
    {
        XShmDetach(engine->display, &engine->shminfo);
        XSync(engine->display, False);
    }
    if (engine->ximage)
    {
        engine->ximage->data = NULL;
        XDestroyImage(engine->ximage);
        engine->ximage = NULL;
    }
    if (engine->use_shm)
    {
        if (engine->shminfo.shmaddr && engine->shminfo.shmaddr != (char *)-1)
            shmdt(engine->shminfo.shmaddr);
        if (engine->shminfo.shmid >= 0)
            shmctl(engine->shminfo.shmid, IPC_RMID, NULL);
    }
    else
        free(engine->display_buffer);
    engine->display_buffer = NULL;
    engine->use_shm = 0;
    engine->shminfo.shmaddr = NULL;
    engine->shminfo.shmid = -1;
    if (engine->font && engine->display)
        XFreeFont(engine->display, engine->font);
    engine->font = NULL;
    if (engine->pixmap && engine->display)
        XFreePixmap(engine->display, engine->pixmap);
    engine->pixmap = 0;
    if (engine->gc && engine->display)
        XFreeGC(engine->display, engine->gc);
    engine->gc = NULL;
    if (engine->window && engine->display)
        XDestroyWindow(engine->display, engine->window);
    engine->window = 0;
    if (engine->display)
        XCloseDisplay(engine->display);
    engine->display = NULL;
    engine->is_init = 0;
    printf("渲染引擎移除完成\n");
}
#endif

// ── 图像处理 ──
// 下面实现错误处理接口。
// ── 图像处理 ──

void emergency_stop(const char *error_msg, const char *component)
{
    capture_report_stop(error_msg, component);
}

void cleanup_all_resources(PCIEManager *pcie_mgr, SharedMemoryManager *shm_mgr,
                           YOLOEngine *yolo_eng)
{
    printf("\n=== 紧急资源移除 ===\n");

#if ENABLE_RENDERING
    // 释放呈现引擎持有的对象。
    if (g_render_engine)
    {
        printf("移除渲染引擎...\n");
        render_engine_cleanup(g_render_engine);
    }
#endif

    // 移除PCIE流传输
    if (pcie_mgr)
    {
        printf("移除PCIE流传输...\n");
        pcie_manager_cleanup_streaming(pcie_mgr);
    }

    // 移除共享内存管理器
    if (shm_mgr)
    {
        printf("移除共享内存管理器...\n");
        shared_memory_cleanup(shm_mgr);
    }

    // 移除弯道识别共享内存
    printf("移除弯道识别共享内存...\n");
    curve_detection_shm_cleanup();

    // 移除YOLO引擎
    if (yolo_eng)
    {
        printf("移除YOLO引擎...\n");
        yolo_engine_cleanup(yolo_eng);
    }

    // 移除PCIE管理器
    if (pcie_mgr)
    {
        printf("移除PCIE管理器...\n");
        pcie_manager_cleanup(pcie_mgr);
    }

    // 回收文件级共用的图像内存。
    if (image_buf_temp)
    {
        printf("释放临时图像缓冲区...\n");
        free(image_buf_temp);
        image_buf_temp = NULL;
    }
    if (image_buf_888)
    {
        printf("释放RGB888图像缓冲区...\n");
        free(image_buf_888);
        image_buf_888 = NULL;
    }
    if (image_buf_565_out)
    {
        printf("释放RGB565输出缓冲区...\n");
        free(image_buf_565_out);
        image_buf_565_out = NULL;
    }

    printf("紧急资源移除完成\n");
}

// ── 图像处理 ──
// 下面实现本模块辅助函数。
// ── 图像处理 ──

int nano_delay(long delay)
{
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = delay;
    return nanosleep(&ts, NULL);
}

void rgb565_to_rgb888(const uint16_t *image565, uint8_t *image888, size_t num_pixels)
{
    frame_unpack_rgb565(image565, image888, num_pixels);
}

void rgb888_to_rgb565(const uint8_t *image888, uint16_t *image565, size_t num_pixels)
{
    frame_pack_rgb565(image888, image565, num_pixels);
}

int save_frame_to_ppm(const uint8_t *rgb888_data, const char *filename)
{
    return capture_save_ppm(rgb888_data, filename, DISPLAY_WIDTH, DISPLAY_HEIGHT);
}

double get_time_diff(struct timeval start, struct timeval end)
{
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
}

void rearrange_columns(uint16_t *row_buffer, int width, int start_col, int end_col)
{
    frame_move_rgb565_columns(row_buffer, width, start_col, end_col);
}

void shift_columns_up(uint8_t *image_buffer, int width, int height, int start_col, int end_col,
                      int shift_rows)
{
    frame_rotate_column_rows(image_buffer, width, height, start_col, end_col, shift_rows, 3);
}

static uint64_t monotonic_ns(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
        return 0;
    return (uint64_t)ts.tv_sec * 1000000000ull + ts.tv_nsec;
}

struct CapturePerf
{
    uint64_t start, frames, raw_changed, inferred, inference_calls, published, renders, overlays;
    uint64_t dma, colors, inference, overlay, shared, render;
};

static void report_capture_perf(CapturePerf *stats, int dma_rows, m_lane::LatestWorker *worker)
{
    uint64_t now = monotonic_ns();
    if (now <= stats->start || now - stats->start < 5000000000ull)
        return;
    double seconds = (now - stats->start) / 1000000000.0;
    double frame_ms = stats->frames ? 1.0 / (stats->frames * 1000000.0) : 0;
    static m_lane::Totals previous_worker = {};
    m_lane::Totals totals = worker ? worker->totals() : m_lane::Totals{};
    uint64_t inferred = worker ? totals.successful - previous_worker.successful : stats->inferred;
    uint64_t calls = worker ? totals.calls - previous_worker.calls : stats->inference_calls;
    uint64_t inference =
        worker ? totals.inference_ns - previous_worker.inference_ns : stats->inference;
    uint64_t fresh =
        worker ? totals.fresh_results - previous_worker.fresh_results : stats->inferred;
    uint64_t stale = worker ? totals.stale_results - previous_worker.stale_results : 0;
    uint64_t replaced = worker ? totals.replaced - previous_worker.replaced : 0;
    uint64_t analysis = worker ? totals.analysis_ns - previous_worker.analysis_ns : 0;
    printf("[PERF M] capture_loop_fps=%.2f raw_changed_fps=%.2f inference_fps=%.2f "
           "fresh_result_fps=%.2f publish_fps=%.2f dma_ms=%.2f color_ms=%.2f infer_ms=%.2f "
           "overlay_ms=%.2f analysis_ms=%.2f shm_ms=%.2f render_ms=%.2f "
           "pending_replaced=%llu stale_results=%llu worker_failed=%d dma_rows=%d\n",
           stats->frames / seconds, stats->raw_changed / seconds, inferred / seconds,
           fresh / seconds, stats->published / seconds, stats->dma * frame_ms,
           stats->colors * frame_ms, calls ? inference / (calls * 1000000.0) : 0,
           stats->overlays ? stats->overlay / (stats->overlays * 1000000.0) : 0,
           inferred ? analysis / (inferred * 1000000.0) : 0, stats->shared * frame_ms,
           stats->renders ? stats->render / (stats->renders * 1000000.0) : 0,
           (unsigned long long)replaced, (unsigned long long)stale, worker && worker->failed(),
           dma_rows);
    previous_worker = totals;
    fflush(stdout);
    memset(stats, 0, sizeof(*stats));
    stats->start = now;
}

/* 原始图像的抽样指纹只用于遥测，不能据此跳过
 * NN。它可区分重复读取与抽样区域变化，
 * 但不等同传感器 VSYNC 计数。 */
static uint64_t raw_fingerprint(const uint16_t *pixels)
{
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned y = 0; y < 64; ++y)
        for (unsigned x = 0; x < 64; ++x)
        {
            unsigned sy = y * (IMAGE_HEIGHT - 1) / 63;
            unsigned sx = x * (IMAGE_WIDTH - 1) / 63;
            hash ^= pixels[sy * IMAGE_WIDTH + sx];
            hash *= 1099511628211ULL;
        }
    return hash;
}

static unsigned capture_fps_setting()
{
    const char *value = getenv("VISION_CAPTURE_FPS");
    if (!value)
        return 30;
    int fps = atoi(value);
    if (fps < 0)
        return 30;
    return fps > 60 ? 60 : (unsigned)fps;
}

static void wait_for_capture_deadline(uint64_t *deadline, unsigned fps)
{
    if (!fps)
        return;
    uint64_t now = monotonic_ns();
    uint64_t period = 1000000000ULL / fps;
    if (*deadline > now)
    {
        struct timespec time = {(time_t)(*deadline / 1000000000ULL),
                                (long)(*deadline % 1000000000ULL)};
        while (g_running && clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &time, NULL) == EINTR)
        {
        }
    }
    now = monotonic_ns();
    *deadline += period;
    if (*deadline < now)
        *deadline = now + period;
}

// ── 图像处理 ──
// 主函数 - PCIe采集与可选渲染
// ── 图像处理 ──

int main()
{
    setvbuf(stdout, NULL, _IOLBF, 0);
    PCIEManager pcie_manager = {};
    pcie_manager.pci_driver_fd = -1;
#if ENABLE_RENDERING
    RenderEngine render_engine = {};
#endif
    yolopv2_result_t yolopv2_results = {};
    SharedMemoryManager pcie_shm_manager = {};

    // 对象声明顺序还保证 NPU 所有者先于其可选任务结束。
    std::unique_ptr<traffic::Service> traffic_service;
    std::unique_ptr<m_lane::LatestWorker> lane_worker;
    int frame_count = 0;
    struct timeval stream_start, current_time;
    double total_time;
    size_t temp_buffer_size = (LEADING_PIXELS + IMAGE_WIDTH) * IMAGE_HEIGHT * 2;

    // 声明路径变量，避免goto跨越初始化
    const char *model_path = "model/yolopv2_Nx3x480x640_rk3568.rknn";
    const char *labels_path = NULL;

    // 设置全局指针用于信号处理。
    g_pcie_manager = &pcie_manager;
    g_shm_manager = &pcie_shm_manager;
    g_yolo_engine = &yolo_engine;
#if ENABLE_RENDERING
    g_render_engine = &render_engine;
#endif

    // 设置信号处理程序。
    setup_signal_handlers();

    printf("\n==============================\n");
    printf("PCIE+YOLOPv2车道线识别系统运行\n");
    printf("==============================\n");
    printf("构建时间: %s %s\n", __DATE__, __TIME__);
    printf("渲染功能: %s\n", ENABLE_RENDERING ? "启用" : "禁用");
    printf("==============================\n\n");

    // ── 图像处理 ──
    // 本阶段准备系统各项运行资源。
    // ── 图像处理 ──

    /* 图像处理：1. 分配图像缓冲区 */
    printf("=== 缓冲区分配 ===\n");
    image_buf_temp = (uint8_t *)aligned_alloc(32, temp_buffer_size);
    image_buf_888 = (uint8_t *)aligned_alloc(32, IMAGE_WIDTH * IMAGE_HEIGHT * 3);
    image_buf_565_out = (uint8_t *)aligned_alloc(32, IMAGE_WIDTH * IMAGE_HEIGHT * 2);

    if (!image_buf_temp || !image_buf_888 || !image_buf_565_out)
    {
        emergency_stop("图像缓冲区分配失败", "内存分配");
        printf("!!! 系统内存不足，无法分配图像处理缓冲区 !!!\n");
        cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
        return -1;
    }

    printf("RGB565原始缓冲区: %zu bytes\n", temp_buffer_size);
    printf("RGB888处理缓冲区: %d bytes\n", IMAGE_WIDTH * IMAGE_HEIGHT * 3);
    printf("RGB565输出缓冲区: %d bytes\n", IMAGE_WIDTH * IMAGE_HEIGHT * 2);

    /* FPGA 通过 PCIe 提供固定布局的四宫格。 */
    printf("\n=== PCIE设备初始化 ===\n");
    if (pcie_manager_init(&pcie_manager) != 0)
    {
        emergency_stop("PCIE设备初始化失败", "PCIE管理器");
        printf("!!! 这是系统运行的关键步骤，必须已完成 !!!\n");
        printf("!!! 请核对PCIE驱动和硬件接入 !!!\n");
        cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ PCIE设备初始化完成\n");

    /* 图像处理：4. 初始化YOLOPv2引擎 */
    printf("\n=== YOLOPv2引擎初始化 ===\n");

    if (yolo_engine_init(&yolo_engine, model_path, labels_path) != 0)
    {
        emergency_stop("YOLOPv2引擎初始化失败", "YOLOPv2引擎");
        printf("!!! 检查模型文件是否存在 !!!\n");
        printf("!!! 模型路径: %s !!!\n", model_path);
        cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ YOLOPv2引擎初始化完成\n");

    /* 图像处理：5. 初始化PCIe共享内存 */
    printf("\n=== PCIe共享内存初始化 ===\n");
    if (shared_memory_init(&pcie_shm_manager, SHM_TYPE_PCIE, 1) != 0)
    {
        emergency_stop("PCIe共享内存初始化失败", "共享内存管理器");
        printf("!!! 系统共享内存资源不足或权限问题 !!!\n");
        printf("!!! 建议操作：ipcs -m 查看共享内存使用情况 !!!\n");
        cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ PCIe共享内存初始化完成\n");

    /* 图像处理：5.5. 初始化弯道识别共享内存 */
    printf("\n=== 弯道识别共享内存初始化 ===\n");
    if (curve_detection_shm_init(1) != 0)
    { // 参数 1 表示以生产者角色建立或附加。
        emergency_stop("弯道检测共享内存初始化失败", "弯道检测共享内存");
        printf("!!! 系统共享内存资源不足或权限问题 !!!\n");
        cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ 弯道识别共享内存初始化完成\n");

    /* 图像处理：6. 初始化视频流采集 */
    printf("\n=== 视频流采集初始化 ===\n");
    if (pcie_manager_init_streaming(&pcie_manager) != 0)
    {
        emergency_stop("视频流采集初始化失败", "PCIE流传输");
        printf("!!! DMA操作或地址映射出现问题 !!!\n");
        printf("!!! 请核对PCIE接入和驱动状态 !!!\n");
        cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ 视频流采集初始化完成\n");

#if ENABLE_RENDERING
    /* 图像处理：7. 初始化渲染引擎 */
    printf("\n=== 渲染引擎初始化 ===\n");
    if (render_engine_init(&render_engine) != 0)
    {
        emergency_stop("渲染引擎初始化失败", "X11渲染引擎");
        printf("!!! X11呈现系统无法正常工作 !!!\n");
        printf("!!! 程序将继续运行但无法呈现图像 !!!\n");
        // 需注意：渲染未成功不应导致整个程序退出，因为核心功能仍可工作
        cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ 渲染引擎初始化完成\n");
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

    // ── 图像处理 ──
    // 主处理循环 - PCIE视频流 + YOLOPv2车道线和可行驶区域识别
    // ── 图像处理 ──

    printf("=== 开始视频流处理和车道线检测 ===\n");

    printf(
        "NN submission interval: %d captured frames; completed inference FPS is reported separately\n",
        YOLO_DETECT_INTERVAL);
    printf("Performance timing report interval: 5 seconds\n");

    CapturePerf perf = {};
    perf.start = monotonic_ns();
    const char *async_option = getenv("VISION_ASYNC");
    bool asynchronous = !async_option || atoi(async_option) != 0;
    unsigned capture_fps = capture_fps_setting();
    uint64_t capture_deadline = monotonic_ns() + (capture_fps ? 1000000000ULL / capture_fps : 0);
    uint64_t last_raw_hash = 0, last_curve_source = 0, last_curve_capture_ns = 0;
    bool have_raw_hash = false, curve_fresh = false;
    if (asynchronous)
    {
        try
        {
            const char *traffic_model = getenv("VISION_TRAFFIC_MODEL");
            // M 端物理中间镜头映射到 UDP 右下格，对应 S 的 0x88。
            // 仅在交通灯任务自有副本中归一化 BGR，车道与视频缓冲区不改。
            traffic_service.reset(new traffic::Service(
                traffic_model && *traffic_model ? traffic_model
                                                : "model/traffic_yolov5n_rk3568.rknn",
                traffic::InputView(320, 240, traffic::PixelOrder::BGR)));
            lane_worker.reset(new m_lane::LatestWorker(&yolo_engine, IMAGE_WIDTH, IMAGE_HEIGHT,
                                                       traffic_service.get()));
        }
        catch (const std::exception &error)
        {
            fprintf(stderr, "Cannot start M inference worker: %s\n", error.what());
            cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
            return -1;
        }
        curve_detection_update_source(0, CURVE_NONE, 0, 0, 0);
    }
    printf("M inference=%s, capture target=%u fps (0=unlimited), NN result max age=500ms\n",
           asynchronous ? "latest-frame worker" : "synchronous", capture_fps);
    LineStyleIPC line_ipc = {-1, nullptr};
    line_ipc_open(&line_ipc, LINE_STYLE_KEY, 1);
    LineStyleData line_initial = {};
    line_ipc_write(&line_ipc, &line_initial);
    m_lane::LineStyleAnalyzer synchronous_line_analyzer;
#if ENABLE_RENDERING
    std::unique_ptr<m_lane::FrontRadialPreview> front_preview;
    const char *preview_mode = getenv("LANE_RADIAL_PREVIEW");
    if (!preview_mode || strcmp(preview_mode, "0") != 0)
    {
        const auto camera = m_lane::FrontRadial::for_camera(getenv("LANE_RADIAL_LAMBDA"),
                                                           "front_radial.conf");
        if (camera.valid() && camera.enabled())
        {
            try
            {
                front_preview.reset(new m_lane::FrontRadialPreview(IMAGE_WIDTH, IMAGE_HEIGHT, camera));
                if (front_preview->enabled())
                    printf("[FRONT_PREVIEW] radial correction enabled lambda=%.6f scale=%.3f\n",
                           camera.coefficient(), front_preview->display_scale());
            }
            catch (const std::exception &error)
            {
                fprintf(stderr, "[FRONT_PREVIEW] unavailable: %s\n", error.what());
            }
        }
    }
#endif
    const uint32_t line_session = (uint32_t)(monotonic_ns() ^ ((uint64_t)getpid() << 16)) | 1u;
    while (g_running)
    {
        uint64_t mark = monotonic_ns();
        // 停用示例，仅供对照：printf("[DEBUG] ===== 帧 %d 开始 =====\n", frame_count);
        /* 图像处理：步骤1: 从PCIE采集一帧数据 */
        if (pcie_manager_capture_frame(&pcie_manager, image_buf_temp, temp_buffer_size) != 0)
        {
            if (!g_running)
                break;
            lane_worker.reset(); // 先等待工作线程退出，再释放 RKNN 及输入缓冲区。
            traffic_service.reset();
            emergency_stop("PCIE帧采集失败", "PCIE数据采集");
            printf("!!! 可能的原因: !!!\n");
            printf("!!! 1. PCIE接入中断 !!!\n");
            printf("!!! 2. DMA传输错误 !!!\n");
            printf("!!! 3. 硬件设备异常 !!!\n");
            cleanup_all_resources(&pcie_manager, &pcie_shm_manager, &yolo_engine);
            return -1;
        }

        uint64_t captured_ns = monotonic_ns();
        perf.dma += captured_ns - mark;
        uint64_t raw_hash = raw_fingerprint((const uint16_t *)image_buf_temp);
        if (!have_raw_hash || raw_hash != last_raw_hash)
            ++perf.raw_changed;
        have_raw_hash = true;
        last_raw_hash = raw_hash;
        mark = monotonic_ns();

        /* 图像处理：步骤2: 将RGB565转换为RGB888格式 */
        // 停用示例，仅供对照：printf("[DEBUG] RGB565转RGB888开始\n");
        rgb565_to_rgb888((uint16_t *)image_buf_temp, image_buf_888, IMAGE_WIDTH * IMAGE_HEIGHT);
        // 停用示例，仅供对照：printf("[DEBUG] RGB565转RGB888完成\n");

        perf.colors += monotonic_ns() - mark;

        if (lane_worker)
        {
            uint64_t source_id = (uint64_t)frame_count + 1;
            if (traffic_service)
                traffic_service->submit_raw(image_buf_888, IMAGE_WIDTH, IMAGE_HEIGHT, source_id,
                                            captured_ns);
            // 提交未经标注的原图，不把上一次叠加掩码的图当作新输入。
            if (frame_count % YOLO_DETECT_INTERVAL == 0)
                lane_worker->submit(image_buf_888, source_id, captured_ns);
            std::shared_ptr<const m_lane::Snapshot> result = lane_worker->latest();
            bool fresh = result && result->fresh(monotonic_ns()) && !lane_worker->failed();
            if (fresh)
            {
                mark = monotonic_ns();
                yolopv2_result_t masks = result->masks();
                yolo_overlay_lane_and_drivable(image_buf_888, IMAGE_WIDTH, IMAGE_HEIGHT, &masks);
                perf.overlay += monotonic_ns() - mark;
                ++perf.overlays;
                // 把缓存掩码贴到另一视频帧上，并不产生新的 NN 观测。
                if (!curve_fresh || result->source_id != last_curve_source)
                {
                    curve_detection_update_source(
                        (uint32_t)result->source_id, (CurveType)result->curve.curve_type,
                        result->curve.confidence, result->curve.angle, result->captured_ns);
                    last_curve_source = result->source_id;
                    last_curve_capture_ns = result->captured_ns;
                    LineStyleData line =
                        line_result_data(&result->line_style, line_session,
                                         (uint32_t)result->source_id, result->captured_ns);
                    line_ipc_write(&line_ipc, &line);
                }
                curve_fresh = true;
            }
            else if (curve_fresh)
            {
                curve_detection_update_source((uint32_t)last_curve_source, CURVE_NONE, 0, 0,
                                              last_curve_capture_ns);
                curve_fresh = false;
            }
        }
        else
        {
            /* 图像处理：步骤3: 使用YOLOPv2进行车道线和可行驶区域识别 (通过宏控制识别间隔) */
            static int last_detect_frame = -YOLO_DETECT_INTERVAL; // 记录上一份真实识别输入的帧编号。

            if (frame_count - last_detect_frame >= YOLO_DETECT_INTERVAL)
            {
                // 停用示例，仅供对照：printf("[DEBUG] 进入识别逻辑: frame_count=%d\n", frame_count);
                // 按宏定义的间隔执行识别
                memset(&yolopv2_results, 0, sizeof(yolopv2_results));
                // 停用示例，仅供对照：printf("[DEBUG] 调用yolo_engine_detect...\n");
                mark = monotonic_ns();
                int detection_result = yolo_engine_detect(&yolo_engine, image_buf_888, IMAGE_WIDTH,
                                                          IMAGE_HEIGHT, &yolopv2_results);
                perf.inference += monotonic_ns() - mark;
                ++perf.inference_calls;
                if (detection_result == 0)
                {
                    ++perf.inferred;
                    mark = monotonic_ns();

#if !HIGH_PERFORMANCE_MODE
                    printf("[帧%d] YOLOPv2检测完成 (间隔%d帧)\n", frame_count,
                           YOLO_DETECT_INTERVAL);
#endif

                    // 立即绘制车道线和可行驶区域到RGB888图像上
                    // 停用示例，仅供对照：printf("[DEBUG] 检查结果指针: lane=%p, drivable=%p\n",
                    // yolopv2_results.lane_line_data, yolopv2_results.drivable_area_data);
                    if (yolopv2_results.lane_line_data && yolopv2_results.drivable_area_data)
                    {
                        auto raw_line = synchronous_line_analyzer.analyze_front_camera(
                            yolopv2_results.lane_line_data, yolopv2_results.width,
                            yolopv2_results.height, image_buf_888, IMAGE_WIDTH, IMAGE_HEIGHT,
                            nullptr, yolopv2_results.drivable_area_data);
                        auto stable_line = synchronous_line_analyzer.confirm(
                            raw_line, (uint64_t)frame_count + 1, captured_ns);
                        LineStyleData line = line_result_data(
                            &stable_line, line_session, (uint32_t)frame_count + 1, captured_ns);
                        if (line_fresh(&line, monotonic_ns()))
                            line_ipc_write(&line_ipc, &line);
                        // 停用示例，仅供对照：printf("[DEBUG] 调用draw_lane_and_drivable...\n");
                        draw_lane_and_drivable(image_buf_888, IMAGE_WIDTH, IMAGE_HEIGHT,
                                               &yolopv2_results);
                        // 停用示例，仅供对照：printf("[DEBUG] draw_lane_and_drivable完成\n");
                    }

                    perf.overlay += monotonic_ns() - mark;
                    ++perf.overlays;
                    last_detect_frame = frame_count;
                }
                else
                {
                    printf("注意: YOLOPv2识别未成功\n");
                }
                // 停用示例，仅供对照：printf("[DEBUG] 识别逻辑完成\n");
            }
            // 需注意：若不在识别间隔，直接使用原始图像，不绘制车道线
        }

        /* 步骤 3.5：把车道与可行驶区已叠加的 RGB888 转回 RGB565，再发布到供 UDP 读取的 PCIe 共享区。 */
        // 转换RGB888回RGB565格式到专用的输出缓冲区（避免与原始数据缓冲区冲突）
        // 停用示例，仅供对照：printf("[DEBUG] RGB888转RGB565开始\n");
        mark = monotonic_ns();
        rgb888_to_rgb565(image_buf_888, (uint16_t *)image_buf_565_out, IMAGE_WIDTH * IMAGE_HEIGHT);
        perf.colors += monotonic_ns() - mark;
        // 停用示例，仅供对照：printf("[DEBUG] RGB888转RGB565完成\n");

        // 写入RGB565格式的带识别结果的图像数据到共享内存
        // 停用示例，仅供对照：printf("[DEBUG] 写入共享内存开始\n");
        mark = monotonic_ns();
        if (pcie_shared_memory_write_frame(&pcie_shm_manager, image_buf_565_out, IMAGE_WIDTH,
                                           IMAGE_HEIGHT) == 0)
        {
            ++perf.published;
        }
        else
        {
            static uint64_t last_write_warning = 0;
            uint64_t now = monotonic_ns();
            if (now - last_write_warning >= 5000000000ull)
            {
                fprintf(stderr, "M image shared-memory publication failed; retrying next frame\n");
                last_write_warning = now;
            }
        }
        perf.shared += monotonic_ns() - mark;
        // 停用示例，仅供对照：printf("[DEBUG] 写入共享内存完成\n");

#if ENABLE_RENDERING
        /* 图像处理：步骤4: 渲染带有识别结果的图像 */
        // 停用示例，仅供对照：printf("[DEBUG] 渲染开始\n");
        mark = monotonic_ns();
        // M 的前视校正仅用于本机显示。共享内存已发布原坐标画面供 S 端使用。
        if (front_preview)
            front_preview->apply(image_buf_888);
        if (render_engine_render_frame_with_detections(&render_engine, image_buf_888) != 0)
        {
            printf("注意: 渲染未成功，但继续处理下一帧\n");
        }
        perf.render += monotonic_ns() - mark;
        ++perf.renders;
        // 停用示例，仅供对照：printf("[DEBUG] 渲染完成\n");

        /* 图像处理：步骤5: 检查退出事件 */
        // 停用示例，仅供对照：printf("[DEBUG] 检查退出事件\n");
        if (render_engine_check_exit_event(&render_engine))
        {
            printf("识别到退出事件\n");
            break;
        }
        // 停用示例，仅供对照：printf("[DEBUG] 退出事件检查完成\n");
#endif

        frame_count++;
        ++perf.frames;
        report_capture_perf(&perf, pcie_manager.dma_rows, lane_worker.get());
        wait_for_capture_deadline(&capture_deadline, capture_fps);
        // 停用示例，仅供对照：printf("[DEBUG] ===== 帧 %d 结束 =====\n", frame_count - 1);
    }

    lane_worker.reset(); // 从这里开始，所有尚在执行的 RKNN 调用均已结束。
    traffic_service.reset();

    // ── 图像处理 ──
    // 性能统计和结果保存区域
    // ── 图像处理 ──
    gettimeofday(&current_time, NULL);
    total_time = get_time_diff(stream_start, current_time);
    printf("\n=== 性能统计结果 ===\n");
    printf("总帧数: %d\n", frame_count);
    printf("总时间: %.2f秒\n", total_time);
    printf("平均FPS: %.1f\n", frame_count / total_time);

    if (getenv("VISION_SAVE_LAST_FRAME") && atoi(getenv("VISION_SAVE_LAST_FRAME")) != 0)
    {
        save_frame_to_ppm(image_buf_888, "last_frame_with_lane_detection.ppm");
    }

    // ── 图像处理 ──
    // 本阶段集中释放系统资源。
    // ── 图像处理 ──
    printf("\n=== 系统移除 ===\n");

#if ENABLE_RENDERING
    render_engine_cleanup(&render_engine);
#endif
    pcie_manager_cleanup_streaming(&pcie_manager);
    shared_memory_cleanup(&pcie_shm_manager);
    line_ipc_close(&line_ipc);
    curve_detection_shm_cleanup(); // 移除弯道识别共享内存
    yolo_engine_cleanup(&yolo_engine);
    pcie_manager_cleanup(&pcie_manager);

    /* 退出阶段释放共用图像缓冲区。 */
    if (image_buf_temp)
    {
        free(image_buf_temp);
        image_buf_temp = NULL;
    }
    if (image_buf_888)
    {
        free(image_buf_888);
        image_buf_888 = NULL;
    }
    if (image_buf_565_out)
    {
        free(image_buf_565_out);
        image_buf_565_out = NULL;
    }

    printf("\n=== PCIE+YOLOPv2车道线识别系统执行完成 ===\n");
    return 0;
}
