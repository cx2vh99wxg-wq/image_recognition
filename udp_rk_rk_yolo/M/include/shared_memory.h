#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdint.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 各类 System V 共享段使用互不重叠的固定键。帧缓存均存放 RGB565，每像素两字节。 */
#define SHM_KEY_BASE 0x12345678
#define SHM_PCIE_IMAGE_KEY (SHM_KEY_BASE + 1)
#define SHM_UDP_IMAGE_KEY (SHM_KEY_BASE + 2)
#define SHM_CONTROL_KEY (SHM_KEY_BASE + 3)
#define SHM_CURVE_DETECTION_KEY (SHM_KEY_BASE + 6)

#define PCIE_IMAGE_WIDTH 640
#define PCIE_IMAGE_HEIGHT 480
#define PCIE_FRAME_SIZE (PCIE_IMAGE_WIDTH * PCIE_IMAGE_HEIGHT * 2)

#define UDP_IMAGE_WIDTH 640
#define UDP_IMAGE_HEIGHT 480
#define UDP_FRAME_SIZE (UDP_IMAGE_WIDTH * UDP_IMAGE_HEIGHT * 2)

typedef struct {
    /* 控制头随每块图像段一起映射；帧号和就绪位供读端识别新帧。
     * volatile 保留原始接口约定，但这里的普通存取并非跨进程原子同步。 */
    volatile uint32_t frame_id;
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
    /* PCIe 与 UDP 缓冲区保持相同布局和容量，只由共享键区分来源。 */
    SharedMemoryControl control;
    uint8_t image_data[PCIE_FRAME_SIZE];
} PCIeSharedMemory;

typedef struct {
    SharedMemoryControl control;
    uint8_t image_data[UDP_FRAME_SIZE];
} UDPSharedMemory;

typedef enum {
    /* 弯道分类的数值会写入共享段，枚举值须与读端约定一致。 */
    CURVE_NONE = 0,
    CURVE_LEFT = 1,
    CURVE_RIGHT = 2,
    CURVE_BOTH = 3
} CurveType;

typedef struct {
    volatile uint32_t frame_id;
    /* 弯道结果与图像帧号关联；置信度和角度按原始整数协议传递。 */
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
    /* 管理器同时保存 System V 段编号、映射地址、长度及读写角色。 */
    int shm_id;
    void* shm_ptr;
    size_t shm_size;
    int is_writer;
    SharedMemoryType type;
} SharedMemoryManager;

/* 按类型连接共享段。写端可创建或复用并初始化控制头；成功返回 0，失败返回 -1。 */
int shared_memory_init(SharedMemoryManager* manager, SharedMemoryType type, int is_writer);

/* 写端清除活跃标志并删除段，读端只分离映射；传入空指针时直接返回。 */
void shared_memory_cleanup(SharedMemoryManager* manager);

/* 复制指定尺寸的 PCIe RGB565 帧，写入校验值和时间戳，最后发布就绪位。 */
int pcie_shared_memory_write_frame(SharedMemoryManager* manager,
                                  const uint8_t* image_data,
                                  uint32_t width, uint32_t height);

/* 复制新的 PCIe 帧并验证校验值；返回 0 表示成功、1 表示无新帧、-1 表示失败。 */
int pcie_shared_memory_read_frame(SharedMemoryManager* manager,
                                 uint8_t* image_data,
                                 uint32_t* width, uint32_t* height,
                                 uint32_t* frame_id);

/* 与 PCIe 写接口同样的帧格式和返回约定，但写入 UDP 专用共享段。 */
int udp_shared_memory_write_frame(SharedMemoryManager* manager,
                                 const uint8_t* image_data,
                                 uint32_t width, uint32_t height);

/* 从 UDP 专用段读出新帧并校验；输出参数包含尺寸与帧号。 */
int udp_shared_memory_read_frame(SharedMemoryManager* manager,
                                uint8_t* image_data,
                                uint32_t* width, uint32_t* height,
                                uint32_t* frame_id);

/* 以文本形式输出当前段的帧状态、尺寸、校验值及映射信息。 */
void shared_memory_print_stats(SharedMemoryManager* manager);

/* 以 10 ms 步长等待帧号变化且帧就绪；0 为新帧，1 为超时，-1 为参数错误。 */
int shared_memory_wait_for_frame(SharedMemoryManager* manager, int timeout_ms);

/* 初始化弯道结果专用段；写端清空并设置默认直道状态，读端只连接。 */
int curve_detection_shm_init(int is_writer);

/* 分离弯道结果共享段；写端还清除活跃位并删除该段。 */
void curve_detection_shm_cleanup(void);

/* 写端发布帧号、弯道类别、置信度和角度；未初始化或非写端时返回 -1。 */
int curve_detection_update(uint32_t frame_id, CurveType curve_type,
                          uint32_t confidence, uint32_t curve_angle);

/* 读出最新弯道结果到四个非空输出指针；未初始化时返回 -1。 */
int curve_detection_read(uint32_t* frame_id, CurveType* curve_type,
                        uint32_t* confidence, uint32_t* curve_angle);

#ifdef __cplusplus
}
#endif

#endif