#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdint.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

//=========================================================================
// 共享内存配置
//=========================================================================
#define SHM_KEY_BASE 0x12345678
#define SHM_PCIE_IMAGE_KEY (SHM_KEY_BASE + 1)
#define SHM_UDP_IMAGE_KEY (SHM_KEY_BASE + 2)
#define SHM_CONTROL_KEY (SHM_KEY_BASE + 3)
#define SHM_DISPLAY_CONTROL_KEY (SHM_KEY_BASE + 4)  // 显示控制共享内存
#define SHM_EMERGENCY_BRAKE_KEY (SHM_KEY_BASE + 5)  // 紧急制动控制共享内存
#define SHM_CURVE_DETECTION_KEY (SHM_KEY_BASE + 6)  // 0x1234567E for curve detection

#define PCIE_IMAGE_WIDTH 640
#define PCIE_IMAGE_HEIGHT 480
#define PCIE_FRAME_SIZE (PCIE_IMAGE_WIDTH * PCIE_IMAGE_HEIGHT * 2) // RGB565格式

#define UDP_IMAGE_WIDTH 640
#define UDP_IMAGE_HEIGHT 480
#define UDP_FRAME_SIZE (UDP_IMAGE_WIDTH * UDP_IMAGE_HEIGHT * 2) // RGB565格式

//=========================================================================
// 共享内存控制结构
//=========================================================================
typedef struct {
    volatile uint32_t frame_id;          /* 帧ID，用于检测新帧 */
    volatile uint32_t frame_ready;       /* 帧就绪标志，1表示有新帧 */
    volatile uint32_t writer_active;     /* 写入者活跃标志 */
    volatile uint32_t reader_count;      /* 读取者计数 */
    volatile uint64_t timestamp;         /* 时间戳 */
    volatile uint32_t frame_width;       /* 图像宽度 */
    volatile uint32_t frame_height;      /* 图像高度 */
    volatile uint32_t checksum;          /* 数据校验和 */
    uint8_t reserved[64];                /* 预留空间 */
} SharedMemoryControl;

//=========================================================================
// PCIe图像共享内存结构
//=========================================================================
typedef struct {
    SharedMemoryControl control;         /* 控制信息 */
    uint8_t image_data[PCIE_FRAME_SIZE]; /* 图像数据(RGB565) */
} PCIeSharedMemory;

//=========================================================================
// UDP图像共享内存结构
//=========================================================================
typedef struct {
    SharedMemoryControl control;         /* 控制信息 */
    uint8_t image_data[UDP_FRAME_SIZE];  /* 图像数据(RGB565) */
} UDPSharedMemory;

//=========================================================================
// 显示模式枚举
//=========================================================================
typedef enum {
    DISPLAY_MODE_PCIE_TOP_LEFT = 0x11,     // PCIe左上角摄像头 (320x240)
    DISPLAY_MODE_PCIE_TOP_RIGHT = 0x22,    // PCIe右上角摄像头 (320x240)
    DISPLAY_MODE_PCIE_BOTTOM_LEFT = 0x33,  // PCIe左下角摄像头 (320x240)
    DISPLAY_MODE_PCIE_BOTTOM_RIGHT = 0x44, // PCIe右下角摄像头 (320x240)
    DISPLAY_MODE_UDP_TOP_LEFT = 0x55,      // UDP左上角摄像头 (320x240)
    DISPLAY_MODE_UDP_TOP_RIGHT = 0x66,     // UDP右上角摄像头 (320x240)
    DISPLAY_MODE_UDP_BOTTOM_LEFT = 0x77,   // UDP左下角摄像头 (320x240)
    DISPLAY_MODE_UDP_BOTTOM_RIGHT = 0x88,  // UDP右下角摄像头 (320x240)
    DISPLAY_MODE_ALL_CAMERAS = 0x99        // 显示所有8个摄像头
} DisplayMode;

//=========================================================================
// 显示控制共享内存结构
//=========================================================================
typedef struct {
    volatile uint32_t mode;           // 显示模式 (DisplayMode枚举值)
    volatile uint32_t last_update;    // 最后更新时间戳
    volatile uint32_t button_count;   // 按钮按下次数
    volatile uint32_t valid;          // 数据有效标志 (1=有效, 0=无效)
    uint8_t reserved[48];             // 预留空间，对齐到64字节
} DisplayControlSharedMemory;

//=========================================================================
// 紧急制动控制共享内存结构
//=========================================================================
typedef struct {
    volatile uint32_t person_detected;     // 人员检测状态 (1=检测到人, 0=未检测到人)
    volatile uint32_t brake_enable;        // 紧急制动使能信号 (1=正常运行, 0=紧急制动)
    volatile uint32_t last_update;         // 最后更新时间戳
    volatile uint32_t frame_count;         // 检测帧计数
    volatile uint32_t detection_count;     // 累计人员检测次数
    volatile uint32_t brake_active_count;  // 紧急制动激活次数
    volatile uint32_t valid;               // 数据有效标志 (1=有效, 0=无效)
    volatile int32_t box_left;             // 检测框左边界
    volatile int32_t box_top;              // 检测框上边界
    volatile int32_t box_right;            // 检测框右边界
    volatile int32_t box_bottom;           // 检测框下边界
    uint8_t reserved[16];                  // 预留空间，对齐到64字节
} EmergencyBrakeSharedMemory;

//=========================================================================
// 弯道检测结果共享内存结构
//=========================================================================
typedef enum {
    CURVE_NONE = 0,       /* 无弯道 */
    CURVE_LEFT = 1,       /* 左弯 */
    CURVE_RIGHT = 2,      /* 右弯 */
    CURVE_BOTH = 3        /* 左右都有弯道 */
} CurveType;

typedef struct {
    volatile uint32_t frame_id;           /* 帧ID,与图像帧同步 */
    volatile uint32_t curve_type;         /* 弯道类型: 0=无, 1=左弯, 2=右弯, 3=左右弯 */
    volatile uint32_t confidence;         /* 检测置信度 (0-100) */
    volatile uint32_t curve_angle;        /* 弯道角度 (度数) */
    volatile uint64_t timestamp;          /* 时间戳 (微秒) */
    volatile uint32_t writer_active;      /* 写入者活跃标志 */
    uint8_t reserved[40];                 /* 预留空间 */
} CurveDetectionSharedMemory;

//=========================================================================
// 共享内存类型枚举
//=========================================================================
typedef enum {
    SHM_TYPE_PCIE = 1,
    SHM_TYPE_UDP = 2
} SharedMemoryType;

//=========================================================================
// 共享内存管理器
//=========================================================================
typedef struct {
    int shm_id;                          /* 共享内存ID */
    SharedMemoryType type;               /* 共享内存类型 */
    union {
        PCIeSharedMemory* pcie_ptr;      /* PCIe共享内存指针 */
        UDPSharedMemory* udp_ptr;        /* UDP共享内存指针 */
        void* generic_ptr;               /* 通用指针 */
    } shm_ptr;
    sem_t* write_sem;                    /* 写信号量 */
    sem_t* read_sem;                     /* 读信号量 */
    int is_writer;                       /* 是否为写入者 */
} SharedMemoryManager;

//=========================================================================
// 函数声明
//=========================================================================

/**
 * 初始化共享内存管理器
 * @param manager 共享内存管理器指针
 * @param type 共享内存类型(SHM_TYPE_PCIE或SHM_TYPE_UDP)
 * @param is_writer 是否为写入者(1=写入者，0=读取者)
 * @return 成功返回0，失败返回-1
 */
int shared_memory_init(SharedMemoryManager* manager, SharedMemoryType type, int is_writer);

/**
 * 初始化PCIe共享内存管理器（兼容性函数）
 * @param manager 共享内存管理器指针
 * @param is_writer 是否为写入者(1=写入者，0=读取者)
 * @return 成功返回0，失败返回-1
 */
int shared_memory_init_pcie(SharedMemoryManager* manager, int is_writer);

/**
 * 初始化UDP共享内存管理器
 * @param manager 共享内存管理器指针
 * @param is_writer 是否为写入者(1=写入者，0=读取者)
 * @return 成功返回0，失败返回-1
 */
int shared_memory_init_udp(SharedMemoryManager* manager, int is_writer);

/**
 * 写入图像数据到共享内存
 * @param manager 共享内存管理器指针
 * @param image_data 图像数据指针
 * @param width 图像宽度
 * @param height 图像高度
 * @return 成功返回0，失败返回-1
 */
int shared_memory_write_image(SharedMemoryManager* manager, 
                             const uint8_t* image_data, 
                             uint32_t width, uint32_t height);

/**
 * 从共享内存读取图像数据
 * @param manager 共享内存管理器指针
 * @param image_data 输出图像数据缓冲区
 * @param max_size 缓冲区最大大小
 * @param width 输出图像宽度指针
 * @param height 输出图像高度指针
 * @param frame_id 输出帧ID指针
 * @return 成功返回读取的数据大小，失败返回-1，无新数据返回0
 */
int shared_memory_read_image(SharedMemoryManager* manager, 
                            uint8_t* image_data, 
                            size_t max_size,
                            uint32_t* width, 
                            uint32_t* height,
                            uint32_t* frame_id);

/**
 * 检查是否有新帧可用
 * @param manager 共享内存管理器指针
 * @param last_frame_id 上次读取的帧ID
 * @return 1=有新帧，0=无新帧，-1=错误
 */
int shared_memory_has_new_frame(SharedMemoryManager* manager, uint32_t last_frame_id);

/**
 * 清理共享内存管理器
 * @param manager 共享内存管理器指针
 */
void shared_memory_cleanup(SharedMemoryManager* manager);

/**
 * 计算数据校验和
 * @param data 数据指针
 * @param size 数据大小
 * @return 校验和
 */
uint32_t calculate_data_checksum(const uint8_t* data, size_t size);

//=========================================================================
// 显示控制共享内存管理函数
//=========================================================================

/**
 * 初始化显示控制共享内存
 * @param shm_ptr 输出的共享内存指针
 * @param is_writer 是否为写入者 (1=写入者, 0=读取者)
 * @return 成功返回0，失败返回-1
 */
int display_control_shm_init(DisplayControlSharedMemory** shm_ptr, int is_writer);

/**
 * 清理显示控制共享内存
 * @param shm_ptr 共享内存指针
 * @param is_writer 是否为写入者
 */
void display_control_shm_cleanup(DisplayControlSharedMemory* shm_ptr, int is_writer);

//=========================================================================
// 紧急制动共享内存管理函数
//=========================================================================

/**
 * 初始化紧急制动共享内存
 * @param shm_ptr 输出的共享内存指针
 * @param is_writer 是否为写入者 (1=写入者(PCIe), 0=读取者(FSPI))
 * @return 成功返回0，失败返回-1
 */
int emergency_brake_shm_init(EmergencyBrakeSharedMemory** shm_ptr, int is_writer);

/**
 * 更新紧急制动状态 (PCIe程序调用)
 * @param shm_ptr 共享内存指针
 * @param person_detected 是否检测到人员 (1=检测到, 0=未检测到)
 * @return 成功返回0，失败返回-1
 */
int emergency_brake_update_status(EmergencyBrakeSharedMemory* shm_ptr, int person_detected,
                                 int box_left, int box_top, int box_right, int box_bottom);

/**
 * 读取紧急制动状态 (FSPI程序调用)
 * @param shm_ptr 共享内存指针
 * @param person_detected 输出检测状态
 * @param brake_enable 输出制动使能信号
 * @return 成功返回0，失败返回-1
 */
int emergency_brake_read_status(EmergencyBrakeSharedMemory* shm_ptr, int* person_detected, int* brake_enable);

/**
 * 清理紧急制动共享内存
 * @param shm_ptr 共享内存指针
 * @param is_writer 是否为写入者
 */
void emergency_brake_shm_cleanup(EmergencyBrakeSharedMemory* shm_ptr, int is_writer);

//=========================================================================
// 弯道检测结果共享内存操作函数
//=========================================================================

/**
 * 初始化弯道检测共享内存
 * @param is_writer 是否为写入者(1=写入者, 0=读取者)
 * @return 0=成功, -1=失败
 */
int curve_detection_shm_init(int is_writer);

/**
 * 清理弯道检测共享内存
 */
void curve_detection_shm_cleanup(void);

/**
 * 更新弯道检测结果到共享内存
 * @param frame_id 帧ID
 * @param curve_type 弯道类型(CURVE_NONE/CURVE_LEFT/CURVE_RIGHT/CURVE_BOTH)
 * @param confidence 检测置信度(0-100)
 * @param curve_angle 弯道角度
 * @return 0=成功, -1=失败
 */
int curve_detection_update(uint32_t frame_id, CurveType curve_type,
                          uint32_t confidence, uint32_t curve_angle);

/**
 * 从共享内存读取弯道检测结果
 * @param frame_id 输出帧ID
 * @param curve_type 输出弯道类型
 * @param confidence 输出检测置信度
 * @param curve_angle 输出弯道角度
 * @return 0=成功, -1=失败
 */
int curve_detection_read(uint32_t* frame_id, CurveType* curve_type,
                        uint32_t* confidence, uint32_t* curve_angle);

#ifdef __cplusplus
}
#endif

#endif // SHARED_MEMORY_H