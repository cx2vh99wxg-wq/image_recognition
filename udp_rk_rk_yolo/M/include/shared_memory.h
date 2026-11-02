#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdint.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* System V 各类段使用互不重叠的固定键；图像槽采用每像素两字节的 RGB565。 */
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

    typedef struct
    {
        /* 每个图像段映射其控制头，读端根据帧号和就绪位识别更新。
     * volatile 维持原接口，但普通字段读写不构成跨进程原子同步。 */
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

    typedef struct
    {
        /* PCIe 与 UDP 段的布局、容量一致，仅通过各自的共享键区分来源。 */
        SharedMemoryControl control;
        uint8_t image_data[PCIE_FRAME_SIZE];
    } PCIeSharedMemory;

    typedef struct
    {
        SharedMemoryControl control;
        uint8_t image_data[UDP_FRAME_SIZE];
    } UDPSharedMemory;

    typedef enum
    {
        /* 弯道枚举的数值直接进入共享段，接收方必须遵循同一编号约定。 */
        CURVE_NONE = 0,
        CURVE_LEFT = 1,
        CURVE_RIGHT = 2,
        CURVE_BOTH = 3
    } CurveType;

    typedef struct
    {
        volatile uint32_t frame_id;
        /* 弯道结果绑定图像帧号，置信度及角度仍按既有整数协议编码。 */
        volatile uint32_t curve_type;
        volatile uint32_t confidence;
        volatile uint32_t curve_angle;
        volatile uint64_t timestamp;
        volatile uint32_t writer_active;
        uint8_t reserved[40];
    } CurveDetectionSharedMemory;

    typedef enum
    {
        SHM_TYPE_PCIE = 1,
        SHM_TYPE_UDP = 2
    } SharedMemoryType;

    typedef struct
    {
        /* 本地管理对象保存段号、映射指针、大小以及读写角色。 */
        int shm_id;
        void *shm_ptr;
        size_t shm_size;
        int is_writer;
        SharedMemoryType type;
        sem_t *image_sem;       /* 这是进程本地句柄，不属于共享内存 ABI。 */
        uint32_t last_frame_id; /* 各读者独立记录并跟随最新帧。 */
    } SharedMemoryManager;

    /* 依类型附加共享段；写端可创建或复用并初始化控制头。附加成功返回 0，否则返回 -1。 */
    int shared_memory_init(SharedMemoryManager *manager, SharedMemoryType type, int is_writer);

    /* 生产代次放在 reserved 内，控制头 ABI 仍保持 104 字节。 */
    uint64_t shared_memory_image_generation(const SharedMemoryManager *manager);

    /* 写端撤销活跃位并删除段，读端只分离映射；空指针无需处理。 */
    void shared_memory_cleanup(SharedMemoryManager *manager);

    /* 发布指定尺寸的 PCIe RGB565 图像，先填帧数据、校验和时间，最后置就绪。 */
    int pcie_shared_memory_write_frame(SharedMemoryManager *manager, const uint8_t *image_data,
                                       uint32_t width, uint32_t height);

    /* 读取并校验新的 PCIe 帧；0 表示读到，1 表示没有新帧，-1 表示失败。 */
    int pcie_shared_memory_read_frame(SharedMemoryManager *manager, uint8_t *image_data,
                                      uint32_t *width, uint32_t *height, uint32_t *frame_id);

    /* UDP 写接口沿用 PCIe 的像素格式和返回约定，但使用独立的 UDP 段。 */
    int udp_shared_memory_write_frame(SharedMemoryManager *manager, const uint8_t *image_data,
                                      uint32_t width, uint32_t height);

    /* 从 UDP 段获取并校验新帧，同时回传图像尺寸及帧号。 */
    int udp_shared_memory_read_frame(SharedMemoryManager *manager, uint8_t *image_data,
                                     uint32_t *width, uint32_t *height, uint32_t *frame_id);

    /* 打印当前共享段的尺寸、帧状态、校验值和地址映射。 */
    void shared_memory_print_stats(SharedMemoryManager *manager);

    /* 每 10 ms 检查帧号更新且已就绪；新帧、超时、参数错误分别返回 0、1、-1。 */
    int shared_memory_wait_for_frame(SharedMemoryManager *manager, int timeout_ms);

    /* 连接弯道结果段；写端清空后建立默认直道状态，读端仅附加。 */
    int curve_detection_shm_init(int is_writer);

    /* 分离弯道结果映射，写角色还负责撤销活跃状态并删除段。 */
    void curve_detection_shm_cleanup(void);

    /* 仅已初始化的写角色可发布弯道帧号、类别、置信度及角度，否则返回 -1。 */
    int curve_detection_update(uint32_t frame_id, CurveType curve_type, uint32_t confidence,
                               uint32_t curve_angle);

    /* 把最新弯道字段写入四个有效输出指针；尚未初始化时返回 -1。 */
    int curve_detection_read(uint32_t *frame_id, CurveType *curve_type, uint32_t *confidence,
                             uint32_t *curve_angle);

    /* 一致的 NN 结果同时携带输入采集 CLOCK_MONOTONIC
 * 时间（ns）。有限次读取遇到竞争或写端不活跃返回 -1，零时间戳表示旧协议。 */
    int curve_detection_update_source(uint32_t frame_id, CurveType curve_type, uint32_t confidence,
                                      uint32_t curve_angle, uint64_t capture_monotonic_ns);
    int curve_detection_read_source(uint32_t *frame_id, CurveType *curve_type, uint32_t *confidence,
                                    uint32_t *curve_angle, uint64_t *capture_monotonic_ns);

#ifdef __cplusplus
}
#endif

#endif