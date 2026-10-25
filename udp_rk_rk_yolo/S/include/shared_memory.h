#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdint.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 每类 System V 共享段使用固定且不同的键；图像帧均为 640×480 RGB565。 */
#define SHM_KEY_BASE 0x12345678
#define SHM_PCIE_IMAGE_KEY (SHM_KEY_BASE + 1)
#define SHM_UDP_IMAGE_KEY (SHM_KEY_BASE + 2)
#define SHM_CONTROL_KEY (SHM_KEY_BASE + 3)
#define SHM_DISPLAY_CONTROL_KEY (SHM_KEY_BASE + 4)
#define SHM_EMERGENCY_BRAKE_KEY (SHM_KEY_BASE + 5)
#define SHM_CURVE_DETECTION_KEY (SHM_KEY_BASE + 6)

#define PCIE_IMAGE_WIDTH 640
#define PCIE_IMAGE_HEIGHT 480
#define PCIE_FRAME_SIZE (PCIE_IMAGE_WIDTH * PCIE_IMAGE_HEIGHT * 2)

#define UDP_IMAGE_WIDTH 640
#define UDP_IMAGE_HEIGHT 480
#define UDP_FRAME_SIZE (UDP_IMAGE_WIDTH * UDP_IMAGE_HEIGHT * 2)

typedef struct {
    volatile uint32_t frame_id;
    /* 控制头用帧号和就绪位通知读端；volatile 并不代替信号量同步。 */
    volatile uint32_t frame_ready;
    volatile uint32_t writer_active;
    volatile uint32_t reader_count;
    volatile uint64_t timestamp;
    volatile uint32_t frame_width;
    volatile uint32_t frame_height;
    volatile uint32_t checksum;
    uint8_t reserved[64];
} SharedMemoryControl;

typedef struct {
    SharedMemoryControl control;
    /* 原始像素按 RGB565 顺序连续存放，容量由上方宽、高常量确定。 */
    uint8_t image_data[PCIE_FRAME_SIZE];
} PCIeSharedMemory;

typedef struct {
    SharedMemoryControl control;
    uint8_t image_data[UDP_FRAME_SIZE];
} UDPSharedMemory;

typedef enum {
    /* PCIe 与 UDP 各有四个 320×240 子画面，0x99 表示同时显示八路。 */
    DISPLAY_MODE_PCIE_TOP_LEFT = 0x11,
    DISPLAY_MODE_PCIE_TOP_RIGHT = 0x22,
    DISPLAY_MODE_PCIE_BOTTOM_LEFT = 0x33,
    DISPLAY_MODE_PCIE_BOTTOM_RIGHT = 0x44,
    DISPLAY_MODE_UDP_TOP_LEFT = 0x55,
    DISPLAY_MODE_UDP_TOP_RIGHT = 0x66,
    DISPLAY_MODE_UDP_BOTTOM_LEFT = 0x77,
    DISPLAY_MODE_UDP_BOTTOM_RIGHT = 0x88,
    DISPLAY_MODE_ALL_CAMERAS = 0x99
} DisplayMode;

typedef struct {
    /* 显示控制段保存当前模式、最近更新时间、按键次数及有效位。 */
    volatile uint32_t mode;
    volatile uint32_t last_update;
    volatile uint32_t button_count;
    volatile uint32_t valid;
    uint8_t reserved[48];
} DisplayControlSharedMemory;

typedef struct {
    /* 超过行人阈值时 person_detected=1。brake_enable=0 表示软件制动请求有效，
     * 包括检出人员或本次模型推理失败；连续两秒有效无人后才为 1。
     * 请求清除不会解除 FPGA 的 STOP 锁存；人工 REARM 可强制解除当前报警。 */
    volatile uint32_t person_detected;
    volatile uint32_t brake_enable;
    volatile uint32_t last_update;
    volatile uint32_t frame_count;
    volatile uint32_t detection_count;
    volatile uint32_t brake_active_count;
    volatile uint32_t valid;
    volatile int32_t box_left;
    volatile int32_t box_top;
    volatile int32_t box_right;
    volatile int32_t box_bottom;
    uint8_t reserved[16];
} EmergencyBrakeSharedMemory;

typedef enum {
    /* 弯道类别会直接写入共享段，数值顺序是两端通信协议的一部分。 */
    CURVE_NONE = 0,
    CURVE_LEFT = 1,
    CURVE_RIGHT = 2,
    CURVE_BOTH = 3
} CurveType;

typedef struct {
    volatile uint32_t frame_id;
    /* 此段把弯道类别、置信度、角度与对应的图像帧号放在一起。 */
    volatile uint32_t curve_type;
    volatile uint32_t confidence;
    volatile uint32_t curve_angle;
    volatile uint64_t timestamp;
    volatile uint32_t writer_active;
    uint8_t reserved[40];
} CurveDetectionSharedMemory;

typedef enum {
    SHM_TYPE_PCIE = 1,
    SHM_TYPE_UDP = 2
} SharedMemoryType;

typedef struct {
    int shm_id;
    SharedMemoryType type;
    /* 按共享类型选择 PCIe 或 UDP 图像映射；两把信号量协调读写。 */
    union {
        PCIeSharedMemory* pcie_ptr;
        UDPSharedMemory* udp_ptr;
        void* generic_ptr;
    } shm_ptr;
    sem_t* write_sem;
    sem_t* read_sem;
    int is_writer;
} SharedMemoryManager;

/* 创建或连接指定图像段及其信号量。写端初始化控制头，读端记录自身连接。 */
int shared_memory_init(SharedMemoryManager* manager, SharedMemoryType type, int is_writer);

/* 以 PCIe 图像类型调用通用初始化入口，成功返回 0，失败返回 -1。 */
int shared_memory_init_pcie(SharedMemoryManager* manager, int is_writer);

/* 以 UDP 图像类型调用通用初始化入口，成功返回 0，失败返回 -1。 */
int shared_memory_init_udp(SharedMemoryManager* manager, int is_writer);

/* 写入指定尺寸的 RGB565 帧并发布校验值；成功返回 0，失败返回 -1。 */
int shared_memory_write_image(SharedMemoryManager* manager,
                             const uint8_t* image_data,
                             uint32_t width, uint32_t height);

/* 将新帧复制到容量为 max_size 的输出缓冲区；成功返回字节数，无新帧返回 0，失败返回 -1。 */
int shared_memory_read_image(SharedMemoryManager* manager,
                            uint8_t* image_data,
                            size_t max_size,
                            uint32_t* width,
                            uint32_t* height,
                            uint32_t* frame_id);

/* 以最近读到的帧号判断新帧：1 为有新帧，0 为无新帧，-1 为错误。 */
int shared_memory_has_new_frame(SharedMemoryManager* manager, uint32_t last_frame_id);

/* 按读写角色分离映射并关闭信号量；写端另删除共享段和信号量名称。 */
void shared_memory_cleanup(SharedMemoryManager* manager);

/* 对 size 字节执行循环左移后异或的 32 位校验；空数据返回零。 */
uint32_t calculate_data_checksum(const uint8_t* data, size_t size);

/* 连接显示控制段；写端默认选择全部八路摄像头并设置有效位。 */
int display_control_shm_init(DisplayControlSharedMemory** shm_ptr, int is_writer);

/* 写端清除有效位并删除段；读端只分离映射。 */
void display_control_shm_cleanup(DisplayControlSharedMemory* shm_ptr, int is_writer);

/* 连接紧急制动段；写端在首帧有效检测前发布制动请求。 */
int emergency_brake_shm_init(EmergencyBrakeSharedMemory** shm_ptr, int is_writer);

/* 发布人员状态及边界框；检测失败时 detection_valid=0，制动请求保持有效。 */
int emergency_brake_update_status(EmergencyBrakeSharedMemory* shm_ptr, int person_detected,
                                 int detection_valid, int box_left, int box_top,
                                 int box_right, int box_bottom);

/* 从有效的紧急制动段读取人员状态与制动使能值；两个输出指针均可为空。 */
int emergency_brake_read_status(EmergencyBrakeSharedMemory* shm_ptr, int* person_detected, int* brake_enable);

/* 写端标记无效、输出统计并删除段；读端只分离映射。 */
void emergency_brake_shm_cleanup(EmergencyBrakeSharedMemory* shm_ptr, int is_writer);

/* 创建或连接弯道结果段；写端初始化为 CURVE_NONE。 */
int curve_detection_shm_init(int is_writer);

/* 分离弯道结果映射，只有写端清除活跃位并删除共享段。 */
void curve_detection_shm_cleanup(void);

/* 写端按帧号发布类别、置信度和角度；未初始化或非写端返回 -1。 */
int curve_detection_update(uint32_t frame_id, CurveType curve_type,
                          uint32_t confidence, uint32_t curve_angle);

/* 读取最近一次弯道结果到四个非空输出指针；成功返回 0。 */
int curve_detection_read(uint32_t* frame_id, CurveType* curve_type,
                        uint32_t* confidence, uint32_t* curve_angle);

#ifdef __cplusplus
}
#endif

#endif
