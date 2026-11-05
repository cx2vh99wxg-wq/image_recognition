#include "../../../common/image/frame_pixels.h"
#include "pcie_dma_read_test.h"
#include "../../../common/pcie/capture_support.h"
#include "yolo_integration.h"
#include "latest_detection.h"
#include "person_brake_trigger.h"
#include "camera_orientation.h"
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
#include <memory>

/* aligned_alloc 的声明需要先启用 _ISOC11_SOURCE。 */
#ifndef _ISOC11_SOURCE
#define _ISOC11_SOURCE
#endif

// ── 图像处理 ──
// *作   者：科研轮转（PCIE+YOLO整合版）
// 本版更新：2026年9月26日
// *实现功能：图像采集与YOLO实时目标识别（整合PCIE驱动和YOLO识别）
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
#define CAMERA_TILE_WIDTH 320
#define CAMERA_TILE_HEIGHT 240
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
    int dma_rows;
} PCIEManager;

// 使用共享内存管理器替换渲染引擎
// typedef struct RenderEngine 已经被 SharedMemoryManager 替换

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

// 处理共享内存管理的接口。
int shm_manager_init(SharedMemoryManager *manager);
int shm_manager_send_frame_with_detections(SharedMemoryManager *manager,
                                           const uint8_t *rgb888_data);
void shm_manager_cleanup(SharedMemoryManager *manager);

// 本模块使用的辅助操作。
int nano_delay(long delay);
void rgb565_to_rgb888(const uint16_t *image565, uint8_t *image888, size_t num_pixels);
void rgb888_to_rgb565(const uint8_t *rgb888_data, uint16_t *rgb565_data, size_t num_pixels);
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
YOLOEngine yolo_engine;

// 全局变量用来信号处理
static volatile sig_atomic_t g_running = 1;
static PCIEManager *g_pcie_manager = NULL;
static SharedMemoryManager *g_shm_manager = NULL;
static YOLOEngine *g_yolo_engine = NULL;
static EmergencyBrakeSharedMemory *g_brake_shm = NULL;

static uint64_t pipeline_clock_ns(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) != 0)
        return 0;
    return (uint64_t)now.tv_sec * 1000000000ULL + (uint64_t)now.tv_nsec;
}

typedef struct
{
    uint64_t start_ns, frames, infer_jobs, raw_changed;
    uint64_t capture_ns, orient_ns, color_ns, infer_ns, draw_ns, publish_ns;
    uint64_t last_raw_sample;
    int have_raw_sample;
} PipelineProfile;

// 对少量原始像素采样，用于区分完全重复采集与单纯重绘。
// 场景噪声、源帧撕裂以及采样区外运动，决定它不能作为
// 严格的传感器帧计数；统计始终排除绘制后的叠加像素。
static uint64_t sample_raw_fingerprint(const uint16_t *pixels)
{
    uint64_t hash = 14695981039346656037ULL;
    for (int row = 0; row < 64; ++row)
    {
        const int y = row * (IMAGE_HEIGHT - 1) / 63;
        for (int column = 0; column < 32; ++column)
        {
            const int x = column * (IMAGE_WIDTH - 1) / 31;
            hash ^= pixels[y * IMAGE_WIDTH + x];
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}

static void report_pipeline_profile(PipelineProfile *profile, int dma_rows)
{
    uint64_t now = pipeline_clock_ns();
    if (!profile->start_ns)
        profile->start_ns = now;
    if (!now || now - profile->start_ns < 5000000000ULL || !profile->frames)
        return;
    double seconds = (now - profile->start_ns) / 1e9;
    double frames = (double)profile->frames;
    double jobs = profile->infer_jobs ? (double)profile->infer_jobs : 1.0;
    printf(
        "[PERF S-PCIE] capture_loop_fps=%.2f raw_changed_fps=%.2f infer_fps=%.2f dma_ms=%.2f orient_ms=%.2f "
        "color_ms=%.2f infer_job_ms=%.2f draw_ms=%.2f publish_ms=%.2f rows=%d ioctls_per_frame=%d\n",
        frames / seconds, profile->raw_changed / seconds, profile->infer_jobs / seconds,
        profile->capture_ns / frames / 1e6, profile->orient_ns / frames / 1e6,
        profile->color_ns / frames / 1e6, profile->infer_ns / jobs / 1e6,
        profile->draw_ns / frames / 1e6, profile->publish_ns / frames / 1e6, dma_rows,
        2 * ((IMAGE_HEIGHT + dma_rows - 1) / dma_rows));
    fflush(stdout);
    uint64_t last_sample = profile->last_raw_sample;
    int have_sample = profile->have_raw_sample;
    memset(profile, 0, sizeof(*profile));
    profile->last_raw_sample = last_sample;
    profile->have_raw_sample = have_sample;
    profile->start_ns = now;
}

// ── 图像处理 ──
// 处理进程信号的回调入口。
// ── 图像处理 ──
void signal_handler(int signum)
{
    (void)signum;
    // 信号回调只提出退出请求；RKNN、IPC、stdio 及 free
    // 都不满足异步信号安全，统一交由主循环移除一次。
    g_running = 0;
}

void setup_signal_handlers()
{
    signal(SIGINT, signal_handler);  // 响应 Ctrl+C 中断请求。
    signal(SIGTERM, signal_handler); // 响应进程终止请求。
    signal(SIGQUIT, signal_handler); // Ctrl+Backslash
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
    // FPGA 插入 120 像素前导，并在每个
    // 95-beat DMA 边界推进图像行；增大主机缓冲区不意味着可安全一次读两行。
    manager->dma_rows = 1;
    const char *rows = getenv("PCIE_DMA_ROWS");
    if (!rows)
        rows = getenv("VISION_DMA_ROWS");
    if (rows && strcmp(rows, "1") != 0)
    {
        fprintf(stderr,
                "Multi-row DMA ignored: current FPGA requires one 1520-byte line per request\n");
    }
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
    const unsigned int row_bytes = (LEADING_PIXELS + IMAGE_WIDTH) * 2;
    if (!manager || manager->pci_driver_fd < 0 || !image_buffer ||
        buffer_size < (size_t)row_bytes * IMAGE_HEIGHT)
    {
        fprintf(stderr, "Invalid PCIe frame capture arguments\n");
        return -1;
    }
    const int rows_per_transfer = 1; // 行边界由硬件协议规定，不能作为吞吐调节参数。
    for (int row = 0; row < IMAGE_HEIGHT; row += rows_per_transfer)
    {
        if (!g_running)
        {
            errno = EINTR;
            return -1;
        }
        const int count =
            IMAGE_HEIGHT - row < rows_per_transfer ? IMAGE_HEIGHT - row : rows_per_transfer;
        manager->dma_operation.current_len = row_bytes * count / 4;
        manager->dma_operation.offset_addr = row * row_bytes;
        if (ioctl(manager->pci_driver_fd, PCI_DMA_WRITE_CMD, &manager->dma_operation) < 0 ||
            ioctl(manager->pci_driver_fd, PCI_READ_FROM_KERNEL_CMD, &manager->dma_operation) < 0)
        {
            fprintf(stderr, "PCIe DMA failed at row %d/%d: %s\n", row, IMAGE_HEIGHT,
                    strerror(errno));
            return -1;
        }
        for (int local = 0; local < count; ++local)
        {
            uint8_t *destination = image_buffer + (row + local) * ACTUAL_WIDTH * 2;
            const uint8_t *source = manager->dma_operation.data.read_buf + local * row_bytes;
            memcpy(destination, source + SKIP_PIXELS * 2, ACTUAL_WIDTH * 2);
            if (COLUMN_REARRANGE_START >= 0 && COLUMN_REARRANGE_END < ACTUAL_WIDTH &&
                COLUMN_REARRANGE_START <= COLUMN_REARRANGE_END &&
                !(COLUMN_REARRANGE_START == 0 && COLUMN_REARRANGE_END == 0))
            {
                rearrange_columns((uint16_t *)destination, ACTUAL_WIDTH, COLUMN_REARRANGE_START,
                                  COLUMN_REARRANGE_END);
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

    printf("初始化视频采集...\n");

    /* 设置DMA操作参数（只需要设置一次）。 */
    /* 单行读取量为 LEADING_PIXELS 前导加 IMAGE_WIDTH 有效像素；按每像素 2 字节换算，即 (LEADING_PIXELS+IMAGE_WIDTH)*2/4 个 dword。 */
    manager->dma_operation.current_len = (LEADING_PIXELS + IMAGE_WIDTH) * 2 / 4;
    manager->dma_operation.offset_addr = 0;
    memset(manager->dma_operation.data.write_buf, 0, DMA_MAX_PACKET_SIZE);

    if (capture_start_dma(manager->pci_driver_fd, &manager->dma_operation) != 0)
        return -1;

    printf("视频采集初始化完成\n");
    return 0;
}

void pcie_manager_cleanup_streaming(PCIEManager *manager)
{
    if (manager && manager->pci_driver_fd >= 0)
    {
        /* 解除此前建立的地址映射。 */
        ioctl(manager->pci_driver_fd, PCI_UMAP_ADDR_CMD, &manager->dma_operation);
        printf("视频采集已结束\n");
    }
}

void pcie_manager_cleanup(PCIEManager *manager)
{
    if (!manager)
        return;

    if (manager->pci_driver_fd >= 0)
    {
        close(manager->pci_driver_fd);
        manager->pci_driver_fd = -1;
    }

    printf("PCIE设备移除完成\n");
}

// ── 图像处理 ──
// 共享内存管理函数实现 - 替换X11呈现管理
// ── 图像处理 ──

int shm_manager_init(SharedMemoryManager *manager)
{
    if (!manager)
    {
        printf("*** 致命错误：共享内存管理器指针无效 ***\n");
        printf("*** 程序立即停止 ***\n");
        return -1;
    }

    printf("初始化共享内存管理器...\n");

    // 为PCIe共享内存管理器作为写入者建立初始状态。
    if (shared_memory_init_pcie(manager, 1) != 0)
    {
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

    printf("PCIe共享内存管理器初始化完成 - 类型: %d, 作为写入者\n", manager->type);
    return 0;
}

int shm_manager_send_frame_with_detections(SharedMemoryManager *manager, const uint8_t *rgb888_data)
{
    if (!manager || !rgb888_data)
        return -1;

    // 把RGB888转为RGB565格式用于共享内存传输。
    static uint16_t *rgb565_buffer = NULL;
    static int first_call = 1;

    if (first_call)
    {
        rgb565_buffer = (uint16_t *)malloc(PCIE_IMAGE_WIDTH * PCIE_IMAGE_HEIGHT * sizeof(uint16_t));
        if (!rgb565_buffer)
        {
            printf("分配RGB565缓冲区未成功\n");
            return -1;
        }
        first_call = 0;
    }


    rgb888_to_rgb565(rgb888_data, rgb565_buffer, PCIE_IMAGE_WIDTH * PCIE_IMAGE_HEIGHT);

    // 将最新结果发布到共享区。
    if (shared_memory_write_image(manager, (uint8_t *)rgb565_buffer, PCIE_IMAGE_WIDTH,
                                  PCIE_IMAGE_HEIGHT) != 0)
    {
        printf("PCIe共享内存写入未成功\n");
        return -1;
    }

    // 添加调试信息：确认写入的是PCIe共享内存
    static int debug_count = 0;
    if (++debug_count % 100 == 0)
    {
        printf("PCIe程序写入帧%d到共享内存类型:%d (应该是1)\n", debug_count, manager->type);
    }

    return 0;
}

void shm_manager_cleanup(SharedMemoryManager *manager)
{
    if (!manager)
        return;

    shared_memory_cleanup(manager);
    printf("共享内存管理器移除完成\n");
}

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

    // 移除紧急制动共享内存
    if (g_brake_shm)
    {
        printf("移除紧急制动共享内存...\n");
        emergency_brake_shm_cleanup(g_brake_shm, 1);
        g_brake_shm = NULL;
    }

    // 移除共享内存管理器
    if (shm_mgr)
    {
        printf("移除共享内存管理器...\n");
        shm_manager_cleanup(shm_mgr);
    }

    // 移除PCIE流传输
    if (pcie_mgr)
    {
        printf("移除PCIE流传输...\n");
        pcie_manager_cleanup_streaming(pcie_mgr);
    }

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

void rgb888_to_rgb565(const uint8_t *rgb888_data, uint16_t *rgb565_data, size_t num_pixels)
{
    frame_pack_rgb565(rgb888_data, rgb565_data, num_pixels);
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
    frame_rotate_column_rows(image_buffer, width, height, start_col, end_col, shift_rows, 2);
}

// ── 图像处理 ──
// 下面实现网络传输接口。
// ── 图像处理 ──

// ── 图像处理 ──
// MAIN函数 - 整合PCIE视频流和YOLO识别
// ── 图像处理 ──

int main()
{
    setvbuf(stdout, NULL, _IOLBF, 0);
    PipelineProfile profile = {};
    std::unique_ptr<s_vision::LatestDetection> latest_detection;
    uint64_t last_async_capture = 0;
    const char *async = getenv("S_VISION_ASYNC");
    auto stop_async = [&] {
        if (!latest_detection)
            return;
        emergency_brake_update_status(g_brake_shm, 0, 0, 0, 0, 0, 0);
        latest_detection.reset(); // join before the RKNN context is destroyed
    };
    printf("=== PCIE图像采集 + YOLO目标检测 + 紧急制动整合系统 ===\n");

    PCIEManager pcie_manager;
    SharedMemoryManager shm_manager; // 使用共享内存管理器替换渲染引擎
    detect_result_group_t detection_results;

    struct timeval stream_start, current_time;
    int frame_count = 0;
    double total_time;

    // 设置全局指针用于信号处理。
    g_pcie_manager = &pcie_manager;
    g_shm_manager = &shm_manager;
    g_yolo_engine = &yolo_engine;

    // 设置信号处理程序。
    setup_signal_handlers();

    // ── 图像处理 ──
    // 本阶段准备系统各项运行资源。
    // ── 图像处理 ──

    /* 图像处理：1. 分配图像缓冲区 */
    printf("\n=== 内存分配 ===\n");
    size_t temp_buffer_size = (LEADING_PIXELS + IMAGE_WIDTH) * IMAGE_HEIGHT * 2;
    size_t rgb888_buffer_size = IMAGE_WIDTH * IMAGE_HEIGHT * 3;

    image_buf_temp = (uint8_t *)aligned_alloc(32, temp_buffer_size);
    if (!image_buf_temp)
    {
        printf("临时图像缓冲区分配未成功！\n");
        goto cleanup;
    }

    image_buf_888 = (uint8_t *)aligned_alloc(32, rgb888_buffer_size);
    if (!image_buf_888)
    {
        printf("RGB888图像缓冲区分配未成功！\n");
        goto cleanup;
    }

    printf("缓冲区分配已完成: 临时=%zu字节, RGB888=%zu字节\n", temp_buffer_size, rgb888_buffer_size);

    /* 图像处理：3. 初始化PCIE管理器 */
    printf("\n=== PCIE设备初始化 ===\n");
    if (pcie_manager_init(&pcie_manager) != 0)
    {
        emergency_stop("PCIE设备初始化失败", "PCIE管理器");
        printf("!!! 这是系统运行的第一个关键步骤，必须已完成 !!!\n");
        printf("!!! 请核对PCIE驱动和硬件接入 !!!\n");
        cleanup_all_resources(&pcie_manager, NULL, NULL);
        return -1;
    }
    printf("✓ PCIE设备初始化完成\n");

    /* 图像处理：4. 初始化YOLO引擎 */
    printf("\n=== YOLO引擎初始化 ===\n");
    if (yolo_engine_init(&yolo_engine, YOLO_MODEL_PATH, YOLO_LABELS_PATH) != 0)
    {
        emergency_stop("YOLO引擎初始化失败", "YOLO引擎");
        printf("!!! 检查模型文件和标签文件是否存在 !!!\n");
        printf("!!! 模型路径: %s !!!\n", YOLO_MODEL_PATH);
        printf("!!! 标签路径: %s !!!\n", YOLO_LABELS_PATH);
        cleanup_all_resources(&pcie_manager, NULL, &yolo_engine);
        return -1;
    }
    printf("✓ YOLO引擎初始化完成\n");

    /* 图像处理：5. 初始化PCIE图像传输 */
    printf("\n=== 图像传输初始化 ===\n");
    if (pcie_manager_init_streaming(&pcie_manager) != 0)
    {
        emergency_stop("PCIE图像传输初始化失败", "PCIE流传输");
        printf("!!! DMA操作或地址映射出现问题 !!!\n");
        printf("!!! 请核对PCIE接入和驱动状态 !!!\n");
        cleanup_all_resources(&pcie_manager, NULL, &yolo_engine);
        return -1;
    }
    printf("✓ 图像传输初始化完成\n");

    /* 图像处理：6. 初始化共享内存管理器 */
    printf("\n=== 共享内存管理器初始化 ===\n");
    if (shm_manager_init(&shm_manager) != 0)
    {
        emergency_stop("共享内存管理器初始化失败", "共享内存管理器");
        printf("!!! 这通常是由于前面的PCIE初始化问题导致的连锁反应 !!!\n");
        printf("!!! 或者系统共享内存资源不足 !!!\n");
        cleanup_all_resources(&pcie_manager, &shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ 共享内存管理器初始化完成\n");

    /* 图像处理：7. 初始化紧急制动共享内存 */
    printf("\n=== 紧急制动共享内存初始化 ===\n");
    if (emergency_brake_shm_init(&g_brake_shm, 1) != 0)
    {
        emergency_stop("紧急制动共享内存初始化失败", "紧急制动系统");
        printf("!!! 无法建立人员识别与FSPI制动通信通道 !!!\n");
        cleanup_all_resources(&pcie_manager, &shm_manager, &yolo_engine);
        return -1;
    }
    printf("✓ 紧急制动共享内存初始化完成\n");

    if (!async || strcmp(async, "0") != 0)
    {
        try
        {
            latest_detection.reset(new s_vision::LatestDetection(
                rgb888_buffer_size, [](const uint8_t *pixels, detect_result_group_t *result) {
                    return yolo_engine_detect(&yolo_engine, pixels, IMAGE_WIDTH, IMAGE_HEIGHT, result);
                }));
        }
        catch (const std::exception &e)
        {
            fprintf(stderr, "[S-VISION] async unavailable: %s; using synchronous inference\n", e.what());
        }
    }
    printf("[S-VISION] %s\n", latest_detection ? "latest-frame worker" : "synchronous");

    printf("\n=== 系统初始化完成 ===\n");
    printf("图像尺寸: %dx%d\n", IMAGE_WIDTH, IMAGE_HEIGHT);
    printf("共享内存尺寸: %dx%d\n", PCIE_IMAGE_WIDTH, PCIE_IMAGE_HEIGHT);
    printf("YOLO模型: %s\n", YOLO_MODEL_PATH);
    printf("紧急制动系统: 已启用，识别到人员时将通知FSPI程序紧急制动\n");
    printf("使用Ctrl+C退出程序...\n\n");

    gettimeofday(&stream_start, NULL);

    // ── 图像处理 ──
    // 主处理循环 - PCIE视频流 + YOLO识别
    // ── 图像处理 ──

    printf("=== 开始图像处理和目标检测 ===\n");

    // 打印或保存此时性能选项。
    printf("=== 性能配置 ===\n");
#if PERFORMANCE_PRESET == 1
    printf("性能模式: 最高精度 (每帧识别)\n");
#elif PERFORMANCE_PRESET == 2
    printf("性能模式: 平衡模式 (每%d帧识别)\n", YOLO_DETECT_INTERVAL);
#elif PERFORMANCE_PRESET == 3
    printf("性能模式: 最高帧率 (每%d帧识别)\n", YOLO_DETECT_INTERVAL);
#else
    printf("性能模式: 自定义 (每%d帧识别)\n", YOLO_DETECT_INTERVAL);
#endif
    printf("高性能模式: %s\n", HIGH_PERFORMANCE_MODE ? "启用" : "禁用");
    printf("FPS报告间隔: %d帧\n", FPS_REPORT_INTERVAL);
    printf("识别去重: %s (IoU阈值: %.2f)\n", ENABLE_DETECTION_DEDUP ? "启用" : "禁用",
           DEDUP_IOU_THRESHOLD);

    // 验证文字渲染支持
    printf("字体支持: 数字0-9, 字母A-Z, 符号. %% space\n");
    printf("常见COCO类别: bottle, person, car, chair, cup, book等\n");

    profile.start_ns = pipeline_clock_ns();
    while (g_running)
    {
        uint64_t phase_started = pipeline_clock_ns();
        /* 图像处理：步骤1: 从PCIE采集一帧数据 */
        if (pcie_manager_capture_frame(&pcie_manager, image_buf_temp, temp_buffer_size) != 0)
        {
            if (!g_running)
                break; // 这是主动退出请求，不是 DMA 硬件故障。
            emergency_stop("PCIE帧采集失败", "PCIE数据采集");
            printf("!!! 可能的原因: !!!\n");
            printf("!!! 1. PCIE接入中断 !!!\n");
            printf("!!! 2. DMA传输错误 !!!\n");
            printf("!!! 3. 硬件设备异常 !!!\n");
            stop_async();
            cleanup_all_resources(&pcie_manager, &shm_manager, &yolo_engine);
            return -1;
        }

        profile.capture_ns += pipeline_clock_ns() - phase_started;
        phase_started = pipeline_clock_ns();
        const uint64_t captured = phase_started;

        /* 图像处理：S端画面按摄像头区块旋转180度，保持视角位置和识别坐标一致。 */
        rotate_rgb565_tiles_180((uint16_t *)image_buf_temp, IMAGE_WIDTH, IMAGE_HEIGHT,
                                CAMERA_TILE_WIDTH, CAMERA_TILE_HEIGHT);

        uint64_t raw_sample = sample_raw_fingerprint((const uint16_t *)image_buf_temp);
        if (!profile.have_raw_sample || raw_sample != profile.last_raw_sample)
            profile.raw_changed++;
        profile.have_raw_sample = 1;
        profile.last_raw_sample = raw_sample;
        profile.orient_ns += pipeline_clock_ns() - phase_started;
        phase_started = pipeline_clock_ns();

        /* 图像处理：步骤2: 将RGB565转换为RGB888格式 */
        rgb565_to_rgb888((uint16_t *)image_buf_temp, image_buf_888, IMAGE_WIDTH * IMAGE_HEIGHT);

        profile.color_ns += pipeline_clock_ns() - phase_started;

        /* 图像处理：步骤3: 使用YOLO进行目标识别 (通过宏控制识别间隔) */
        static detect_result_group_t cached_results = {0};
        static int detection_valid = 0;

        if (latest_detection)
        {
            if (frame_count % YOLO_DETECT_INTERVAL == 0)
                latest_detection->submit(image_buf_888, captured);
            const auto result = latest_detection->latest();
            if (result.captured > last_async_capture)
            {
                last_async_capture = result.captured;
                ++profile.infer_jobs;
                profile.infer_ns += result.elapsed;
            }
            detection_valid = result.fresh(pipeline_clock_ns());
            detection_results = detection_valid ? result.results : detect_result_group_t{};
        }
        else if (frame_count % YOLO_DETECT_INTERVAL == 0)
        {
            phase_started = pipeline_clock_ns();
            profile.infer_jobs++;
            // 按宏定义的间隔执行识别
            memset(&detection_results, 0, sizeof(detection_results));
            if (yolo_engine_detect(&yolo_engine, image_buf_888, IMAGE_WIDTH, IMAGE_HEIGHT,
                                   &detection_results) == 0)
            {
                detection_valid = 1;
                // 保存本次识别供后续读取。
                cached_results = detection_results;

#if !HIGH_PERFORMANCE_MODE
                if (detection_results.count > 0)
                {
                    printf("[帧%d] 检测到 %d 个目标 (间隔%d帧)\n", frame_count,
                           detection_results.count, YOLO_DETECT_INTERVAL);
                }
#endif
            }
            else
            {
                detection_valid = 0;
                printf("注意: YOLO识别未成功，车辆保持制动；画面沿用上一次识别结果\n");
                detection_results = cached_results;
            }
            profile.infer_ns += pipeline_clock_ns() - phase_started;
        }
        else
        {
            // 使用缓存的识别结果
            detection_results = cached_results;
        }

        // 制动使用所有候选框的独立统计，不受 64 个呈现框或绘图去重限制。
        // 必须是精确的 person 类别，且未四舍五入的模型置信度不低于 15%。
        int person_count = detection_valid ? detection_results.person_brake_count : 0;
        int person_detected = person_count > 0;
        int person_box_left = 0, person_box_top = 0, person_box_right = 0, person_box_bottom = 0;
        static int last_person_detected = -1; // 记录上次识别状态

        if (person_detected)
        {
            const BOX_RECT *box = &detection_results.person_brake_result.box;
            person_box_left = box->left;
            person_box_top = box->top;
            person_box_right = box->right;
            person_box_bottom = box->bottom;
        }

        // 先发布制动请求，避免绘图和控制台输出延迟 FSPI 响应。
        if (g_brake_shm)
        {
            emergency_brake_update_status(g_brake_shm, person_detected, detection_valid,
                                          person_box_left, person_box_top, person_box_right,
                                          person_box_bottom);
        }

        phase_started = pipeline_clock_ns();
        if (detection_results.count > 0)
        {
            // 把识别信息叠加到输出图像。
            draw_detection_results(image_buf_888, IMAGE_WIDTH, IMAGE_HEIGHT, &detection_results);
        }

        profile.draw_ns += pipeline_clock_ns() - phase_started;
        if (person_detected != last_person_detected)
        {
            printf("[S-PERSON] frame=%d person=%d candidates=%d detection_valid=%d\n", frame_count,
                   person_detected, person_count, detection_valid);
        }
        last_person_detected = person_detected;
        phase_started = pipeline_clock_ns();

        /* 图像处理：步骤4: 通过共享内存发出带有识别结果的图像 */
        if (shm_manager_send_frame_with_detections(&shm_manager, image_buf_888) != 0)
        {
            emergency_stop("共享内存发送失败", "共享内存传输");
            printf("!!! 可能的原因: !!!\n");
            printf("!!! 1. 共享内存被其他进程破坏 !!!\n");
            printf("!!! 2. 系统内存不足 !!!\n");
            printf("!!! 3. 权限被更改 !!!\n");
            stop_async();
            cleanup_all_resources(&pcie_manager, &shm_manager, &yolo_engine);
            return -1;
        }

        /* 图像处理：步骤6: 简单的程序运行控制（可以添加信号处理或其他退出条件） */
        // 由于不使用X11，本处可以添加其他退出条件，例如运行指定帧数后退出
        // 目前保持无限运行，可以通过Ctrl+C终止

        profile.publish_ns += pipeline_clock_ns() - phase_started;
        profile.frames++;
        report_pipeline_profile(&profile, pcie_manager.dma_rows);
        frame_count++;

        /* 图像处理：实时FPS计算和性能分析 (通过宏控制报告频率) */
        if (frame_count % FPS_REPORT_INTERVAL == 0)
        {
            gettimeofday(&current_time, NULL);
            total_time = get_time_diff(stream_start, current_time);
            double current_fps = frame_count / total_time;

            printf("[%d帧] FPS: %.1f | 检测数: %d | 人员检测: %s\n", frame_count, current_fps,
                   detection_results.count, person_detected ? "是" : "否");
        }
    }

    // ── 图像处理 ──
    // 性能统计和结果保存区域
    // ── 图像处理 ──
    gettimeofday(&current_time, NULL);
    total_time = get_time_diff(stream_start, current_time);
    printf("\n=== 性能统计结果 ===\n");
    printf("总帧数: %d\n", frame_count);
    printf("总时间: %.2f秒\n", total_time);
    printf("平均FPS: %.1f\n", frame_count / total_time);

    /* 保留最后一帧为PPM文件以供后续使用。 */
    printf("\n=== 保存最后一帧 ===\n");
    if (frame_count > 0)
        save_frame_to_ppm(image_buf_888, "last_frame_with_detections.ppm");

    // ── 图像处理 ──
    // 本阶段集中释放系统资源。
    // ── 图像处理 ──
    printf("\n=== 系统移除 ===\n");
    stop_async();

    // 移除紧急制动共享内存
    if (g_brake_shm)
    {
        emergency_brake_shm_cleanup(g_brake_shm, 1);
        g_brake_shm = NULL;
    }

    shm_manager_cleanup(&shm_manager);
    pcie_manager_cleanup_streaming(&pcie_manager);
    yolo_engine_cleanup(&yolo_engine);
    pcie_manager_cleanup(&pcie_manager);

cleanup:
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

    printf("\n=== PCIE+YOLO+紧急制动整合系统执行完成 ===\n");
    return 0;
}
