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
// 共享内存管理器
//=========================================================================
typedef enum {
    SHM_TYPE_PCIE = 1,
    SHM_TYPE_UDP = 2
} SharedMemoryType;

typedef struct {
    int shm_id;                          /* 共享内存ID */
    void* shm_ptr;                       /* 共享内存指针 */
    size_t shm_size;                     /* 共享内存大小 */
    int is_writer;                       /* 是否为写入者 */
    SharedMemoryType type;               /* 共享内存类型 */
} SharedMemoryManager;

//=========================================================================
// 共享内存管理函数
//=========================================================================

/**
 * 初始化共享内存管理器
 * @param manager 管理器实例
 * @param type 共享内存类型
 * @param is_writer 是否为写入者(1=写入者, 0=读取者)
 * @return 0=成功, -1=失败
 */
int shared_memory_init(SharedMemoryManager* manager, SharedMemoryType type, int is_writer);

/**
 * 清理共享内存管理器
 * @param manager 管理器实例
 */
void shared_memory_cleanup(SharedMemoryManager* manager);

//=========================================================================
// PCIe图像共享内存操作函数
//=========================================================================

/**
 * 写入PCIe图像帧到共享内存
 * @param manager 管理器实例
 * @param image_data 图像数据(RGB565格式)
 * @param width 图像宽度
 * @param height 图像高度
 * @return 0=成功, -1=失败
 */
int pcie_shared_memory_write_frame(SharedMemoryManager* manager, 
                                  const uint8_t* image_data, 
                                  uint32_t width, uint32_t height);

/**
 * 从共享内存读取PCIe图像帧
 * @param manager 管理器实例
 * @param image_data 输出图像数据缓冲区
 * @param width 输出图像宽度
 * @param height 输出图像高度
 * @param frame_id 输出帧ID
 * @return 0=成功, 1=没有新帧, -1=失败
 */
int pcie_shared_memory_read_frame(SharedMemoryManager* manager, 
                                 uint8_t* image_data, 
                                 uint32_t* width, uint32_t* height,
                                 uint32_t* frame_id);

//=========================================================================
// UDP图像共享内存操作函数
//=========================================================================

/**
 * 写入UDP图像帧到共享内存
 * @param manager 管理器实例
 * @param image_data 图像数据(RGB565格式)
 * @param width 图像宽度
 * @param height 图像高度
 * @return 0=成功, -1=失败
 */
int udp_shared_memory_write_frame(SharedMemoryManager* manager, 
                                 const uint8_t* image_data, 
                                 uint32_t width, uint32_t height);

/**
 * 从共享内存读取UDP图像帧
 * @param manager 管理器实例
 * @param image_data 输出图像数据缓冲区
 * @param width 输出图像宽度
 * @param height 输出图像高度
 * @param frame_id 输出帧ID
 * @return 0=成功, 1=没有新帧, -1=失败
 */
int udp_shared_memory_read_frame(SharedMemoryManager* manager, 
                                uint8_t* image_data, 
                                uint32_t* width, uint32_t* height,
                                uint32_t* frame_id);

//=========================================================================
// 实用工具函数
//=========================================================================

/**
 * 打印共享内存统计信息
 * @param manager 管理器实例
 */
void shared_memory_print_stats(SharedMemoryManager* manager);

/**
 * 等待新帧到达
 * @param manager 管理器实例
 * @param timeout_ms 超时时间(毫秒)
 * @return 0=有新帧, 1=超时, -1=错误
 */
int shared_memory_wait_for_frame(SharedMemoryManager* manager, int timeout_ms);

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

#endif /* SHARED_MEMORY_H */