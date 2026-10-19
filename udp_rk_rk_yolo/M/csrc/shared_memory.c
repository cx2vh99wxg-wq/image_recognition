#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include "shared_memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

/* RGB565 帧按字节累加为 32 位校验值；溢出行为与原有四处循环相同。 */
static uint32_t sum_rgb565_bytes(const uint8_t *data, size_t byte_count) {
    uint32_t checksum = 0;
    for (size_t index = 0; index < byte_count; ++index) {
        checksum += data[index];
    }
    return checksum;
}

/* 根据 PCIe/UDP 类型选取固定键和容量；写端可创建或复用已有段，读端只连接。成功附加后，写端重置控制区。 */
int shared_memory_init(SharedMemoryManager* manager, SharedMemoryType type, int is_writer) {
    if (!manager) {
        fprintf(stderr, "*** 致命错误: manager指针为空 ***\n");
        return -1;
    }

    memset(manager, 0, sizeof(SharedMemoryManager));
    manager->is_writer = is_writer;
    manager->type = type;

    size_t shm_size;
    key_t shm_key;
    const char* type_name;

    if (type == SHM_TYPE_PCIE) {
        shm_size = sizeof(PCIeSharedMemory);
        shm_key = SHM_PCIE_IMAGE_KEY;
        type_name = "PCIe";
    } else if (type == SHM_TYPE_UDP) {
        shm_size = sizeof(UDPSharedMemory);
        shm_key = SHM_UDP_IMAGE_KEY;
        type_name = "UDP";
    } else {
        fprintf(stderr, "*** 致命错误: 不支持的共享内存类型: %d ***\n", type);
        return -1;
    }

    printf("正在初始化%s共享内存 (key=0x%x, size=%zu bytes)\n",
           type_name, shm_key, shm_size);

    if (is_writer) {

        manager->shm_id = shmget(shm_key, shm_size,
                                IPC_CREAT | IPC_EXCL | 0666);
        if (manager->shm_id == -1) {
            if (errno == EEXIST) {

                printf("Shared memory already exists, trying to connect...\n");
                manager->shm_id = shmget(shm_key, shm_size, 0666);
                if (manager->shm_id == -1) {
                    perror("Failed to connect to existing shared memory");
                    return -1;
                }
            } else {
                perror("Failed to create shared memory");
                return -1;
            }
        }
    } else {

        manager->shm_id = shmget(shm_key, shm_size, 0666);
        if (manager->shm_id == -1) {
            perror("Failed to connect to shared memory");
            return -1;
        }
    }

    manager->shm_ptr = shmat(manager->shm_id, NULL, 0);
    if (manager->shm_ptr == (void*)-1) {
        perror("Failed to attach to shared memory");
        return -1;
    }

    manager->shm_size = shm_size;
    printf("%s shared memory initialized successfully (id=%d, ptr=%p)\n",
           type_name, manager->shm_id, manager->shm_ptr);

    if (is_writer) {
        SharedMemoryControl* control = NULL;
        if (type == SHM_TYPE_PCIE) {
            control = &((PCIeSharedMemory*)manager->shm_ptr)->control;
        } else if (type == SHM_TYPE_UDP) {
            control = &((UDPSharedMemory*)manager->shm_ptr)->control;
        }

        if (control) {
            memset(control, 0, sizeof(SharedMemoryControl));
            control->writer_active = 1;
            control->frame_width = (type == SHM_TYPE_PCIE) ? PCIE_IMAGE_WIDTH : UDP_IMAGE_WIDTH;
            control->frame_height = (type == SHM_TYPE_PCIE) ? PCIE_IMAGE_HEIGHT : UDP_IMAGE_HEIGHT;
            printf("Initialized control structure for %s\n", type_name);
        }
    }

    return 0;
}

/* 写端先清除活跃标志，再分离并删除段；读端只分离。保留原有空指针处理方式。 */
void shared_memory_cleanup(SharedMemoryManager* manager) {
    if (!manager) return;

    if (manager->is_writer && manager->shm_ptr != (void*)-1) {
        SharedMemoryControl* control = NULL;
        if (manager->type == SHM_TYPE_PCIE) {
            control = &((PCIeSharedMemory*)manager->shm_ptr)->control;
        } else if (manager->type == SHM_TYPE_UDP) {
            control = &((UDPSharedMemory*)manager->shm_ptr)->control;
        }

        if (control) {
            control->writer_active = 0;
        }
    }

    if (manager->shm_ptr != (void*)-1) {
        if (shmdt(manager->shm_ptr) == -1) {
            perror("Failed to detach from shared memory");
        }
        manager->shm_ptr = (void*)-1;
    }

    if (manager->is_writer && manager->shm_id != -1) {
        if (shmctl(manager->shm_id, IPC_RMID, NULL) == -1) {
            perror("Failed to remove shared memory");
        }
    }

    manager->shm_id = -1;
    printf("Shared memory cleaned up\n");
}

/* 将 RGB565 PCIe 帧复制到共享区，累加校验值，更新元数据，最后置位就绪标志。 */
int pcie_shared_memory_write_frame(SharedMemoryManager* manager,
                                  const uint8_t* image_data,
                                  uint32_t width, uint32_t height) {
    if (!manager || !image_data || manager->type != SHM_TYPE_PCIE) {
        fprintf(stderr, "Error: invalid parameters for PCIe frame write\n");
        return -1;
    }

    if (manager->shm_ptr == (void*)-1) {
        fprintf(stderr, "Error: shared memory not attached\n");
        return -1;
    }

    PCIeSharedMemory* shm = (PCIeSharedMemory*)manager->shm_ptr;

    if (width > PCIE_IMAGE_WIDTH || height > PCIE_IMAGE_HEIGHT) {
        fprintf(stderr, "Error: image size too large (%dx%d > %dx%d)\n",
                width, height, PCIE_IMAGE_WIDTH, PCIE_IMAGE_HEIGHT);
        return -1;
    }

    shm->control.frame_ready = 0;

    size_t data_size = width * height * 2;
    memcpy(shm->image_data, image_data, data_size);

    uint32_t checksum = sum_rgb565_bytes(shm->image_data, data_size);

    shm->control.frame_id++;
    shm->control.frame_width = width;
    shm->control.frame_height = height;
    shm->control.timestamp = (uint64_t)time(NULL) * 1000000;
    shm->control.checksum = checksum;

    shm->control.frame_ready = 1;

    printf("PCIe frame written: id=%u, size=%dx%d, checksum=0x%x\n",
           shm->control.frame_id, width, height, checksum);

    return 0;
}

/* 跳过未就绪或重复的 PCIe 帧；复制后复查帧号和就绪标志，再核对校验值。 */
int pcie_shared_memory_read_frame(SharedMemoryManager* manager,
                                 uint8_t* image_data,
                                 uint32_t* width, uint32_t* height,
                                 uint32_t* frame_id) {
    if (!manager || !image_data || !width || !height || !frame_id ||
        manager->type != SHM_TYPE_PCIE) {
        fprintf(stderr, "Error: invalid parameters for PCIe frame read\n");
        return -1;
    }

    if (manager->shm_ptr == (void*)-1) {
        fprintf(stderr, "Error: shared memory not attached\n");
        return -1;
    }

    PCIeSharedMemory* shm = (PCIeSharedMemory*)manager->shm_ptr;

    static uint32_t last_read_frame_id = 0;

    if (!shm->control.frame_ready) {
        return 1;
    }

    if (!shm->control.writer_active) {
        fprintf(stderr, "Warning: writer is not active\n");
        return -1;
    }

    uint32_t current_frame_id = shm->control.frame_id;

    if (current_frame_id == last_read_frame_id) {
        return 1;
    }

    if (!shm->control.frame_ready) {
        return 1;
    }

    *frame_id = current_frame_id;
    *width = shm->control.frame_width;
    *height = shm->control.frame_height;
    uint32_t expected_checksum = shm->control.checksum;

    size_t data_size = (*width) * (*height) * 2;
    memcpy(image_data, shm->image_data, data_size);

    if (!shm->control.frame_ready || shm->control.frame_id != current_frame_id) {

        return -1;
    }

    uint32_t checksum = sum_rgb565_bytes(image_data, data_size);

    if (checksum != expected_checksum) {
        fprintf(stderr, "Warning: checksum mismatch (expected=0x%x, actual=0x%x)\n",
                expected_checksum, checksum);
        return -1;
    }

    last_read_frame_id = current_frame_id;

    if (current_frame_id % 1000 == 0) {
        printf("PCIe frame read: id=%u, size=%dx%d, checksum=0x%x\n",
               *frame_id, *width, *height, checksum);
    }

    return 0;
}

/* 将 RGB565 UDP 帧写入对应共享区；元数据与校验值写完后才置位就绪标志。 */
int udp_shared_memory_write_frame(SharedMemoryManager* manager,
                                 const uint8_t* image_data,
                                 uint32_t width, uint32_t height) {
    if (!manager || !image_data || manager->type != SHM_TYPE_UDP) {
        fprintf(stderr, "Error: invalid parameters for UDP frame write\n");
        return -1;
    }

    if (manager->shm_ptr == (void*)-1) {
        fprintf(stderr, "Error: shared memory not attached\n");
        return -1;
    }

    UDPSharedMemory* shm = (UDPSharedMemory*)manager->shm_ptr;

    if (width > UDP_IMAGE_WIDTH || height > UDP_IMAGE_HEIGHT) {
        fprintf(stderr, "Error: image size too large (%dx%d > %dx%d)\n",
                width, height, UDP_IMAGE_WIDTH, UDP_IMAGE_HEIGHT);
        return -1;
    }

    shm->control.frame_ready = 0;

    size_t data_size = width * height * 2;
    memcpy(shm->image_data, image_data, data_size);

    uint32_t checksum = sum_rgb565_bytes(shm->image_data, data_size);

    shm->control.frame_id++;
    shm->control.frame_width = width;
    shm->control.frame_height = height;
    shm->control.timestamp = (uint64_t)time(NULL) * 1000000;
    shm->control.checksum = checksum;

    shm->control.frame_ready = 1;

    printf("UDP frame written: id=%u, size=%dx%d, checksum=0x%x\n",
           shm->control.frame_id, width, height, checksum);

    return 0;
}

/* 读取新的 UDP 帧并校验完整性；每次成功读取均保留原有日志输出。 */
int udp_shared_memory_read_frame(SharedMemoryManager* manager,
                                uint8_t* image_data,
                                uint32_t* width, uint32_t* height,
                                uint32_t* frame_id) {
    if (!manager || !image_data || !width || !height || !frame_id ||
        manager->type != SHM_TYPE_UDP) {
        fprintf(stderr, "Error: invalid parameters for UDP frame read\n");
        return -1;
    }

    if (manager->shm_ptr == (void*)-1) {
        fprintf(stderr, "Error: shared memory not attached\n");
        return -1;
    }

    UDPSharedMemory* shm = (UDPSharedMemory*)manager->shm_ptr;

    static uint32_t last_read_frame_id = 0;

    if (!shm->control.frame_ready) {
        return 1;
    }

    if (!shm->control.writer_active) {
        fprintf(stderr, "Warning: writer is not active\n");
        return -1;
    }

    uint32_t current_frame_id = shm->control.frame_id;

    if (current_frame_id == last_read_frame_id) {
        return 1;
    }

    if (!shm->control.frame_ready) {
        return 1;
    }

    *frame_id = current_frame_id;
    *width = shm->control.frame_width;
    *height = shm->control.frame_height;
    uint32_t expected_checksum = shm->control.checksum;

    size_t data_size = (*width) * (*height) * 2;
    memcpy(image_data, shm->image_data, data_size);

    if (!shm->control.frame_ready || shm->control.frame_id != current_frame_id) {

        return -1;
    }

    uint32_t checksum = sum_rgb565_bytes(image_data, data_size);

    if (checksum != expected_checksum) {
        fprintf(stderr, "Warning: checksum mismatch (expected=0x%x, actual=0x%x)\n",
                expected_checksum, checksum);
        return -1;
    }

    last_read_frame_id = current_frame_id;

    printf("UDP frame read: id=%u, size=%dx%d, checksum=0x%x\n",
           *frame_id, *width, *height, checksum);

    return 0;
}

/* 按共享区类型选取控制头并输出帧号、状态、尺寸、校验值及映射信息。 */
void shared_memory_print_stats(SharedMemoryManager* manager) {
    if (!manager || manager->shm_ptr == (void*)-1) {
        printf("Shared memory not initialized\n");
        return;
    }

    SharedMemoryControl* control = NULL;
    const char* type_name = "";

    if (manager->type == SHM_TYPE_PCIE) {
        control = &((PCIeSharedMemory*)manager->shm_ptr)->control;
        type_name = "PCIe";
    } else if (manager->type == SHM_TYPE_UDP) {
        control = &((UDPSharedMemory*)manager->shm_ptr)->control;
        type_name = "UDP";
    } else {
        printf("Unknown shared memory type\n");
        return;
    }

    printf("\n=== %s Shared Memory Stats ===\n", type_name);
    printf("Frame ID: %u\n", control->frame_id);
    printf("Frame Ready: %s\n", control->frame_ready ? "Yes" : "No");
    printf("Writer Active: %s\n", control->writer_active ? "Yes" : "No");
    printf("Reader Count: %u\n", control->reader_count);
    printf("Frame Size: %ux%u\n", control->frame_width, control->frame_height);
    printf("Timestamp: %lu\n", control->timestamp);
    printf("Checksum: 0x%x\n", control->checksum);
    printf("Memory ID: %d\n", manager->shm_id);
    printf("Memory Size: %zu bytes\n", manager->shm_size);
    printf("Memory Address: %p\n", manager->shm_ptr);
    printf("===========================\n\n");
}

/* 记录当前帧号，以 10 ms 为周期等待新的就绪帧，达到给定时间后返回超时。 */
int shared_memory_wait_for_frame(SharedMemoryManager* manager, int timeout_ms) {
    if (!manager || manager->shm_ptr == (void*)-1) {
        return -1;
    }

    SharedMemoryControl* control = NULL;
    if (manager->type == SHM_TYPE_PCIE) {
        control = &((PCIeSharedMemory*)manager->shm_ptr)->control;
    } else if (manager->type == SHM_TYPE_UDP) {
        control = &((UDPSharedMemory*)manager->shm_ptr)->control;
    } else {
        return -1;
    }

    uint32_t last_frame_id = control->frame_id;
    int elapsed_ms = 0;
    const int sleep_ms = 10;

    while (elapsed_ms < timeout_ms) {
        if (control->frame_ready && control->frame_id != last_frame_id) {
            return 0;
        }

        usleep(sleep_ms * 1000);
        elapsed_ms += sleep_ms;
    }

    return 1;
}

/* 以下三个全局量保存弯道结果共享段的标识、映射地址和写端角色。 */
static int g_curve_shm_id = -1;
static CurveDetectionSharedMemory* g_curve_shm_ptr = NULL;
static int g_curve_is_writer = 0;

/* 连接弯道检测专用共享段；写端可创建并初始化控制结构，读端只附加。 */
int curve_detection_shm_init(int is_writer) {
    size_t shm_size = sizeof(CurveDetectionSharedMemory);
    key_t shm_key = SHM_CURVE_DETECTION_KEY;

    printf("正在初始化弯道检测共享内存 (key=0x%x, size=%zu bytes)\n",
           shm_key, shm_size);

    g_curve_is_writer = is_writer;

    if (is_writer) {

        g_curve_shm_id = shmget(shm_key, shm_size,
                                IPC_CREAT | IPC_EXCL | 0666);
        if (g_curve_shm_id == -1) {
            if (errno == EEXIST) {

                printf("弯道检测共享内存已存在，尝试连接...\n");
                g_curve_shm_id = shmget(shm_key, shm_size, 0666);
                if (g_curve_shm_id == -1) {
                    perror("连接弯道检测共享内存失败");
                    return -1;
                }
            } else {
                perror("创建弯道检测共享内存失败");
                return -1;
            }
        }
    } else {

        g_curve_shm_id = shmget(shm_key, shm_size, 0666);
        if (g_curve_shm_id == -1) {
            perror("连接弯道检测共享内存失败");
            return -1;
        }
    }

    g_curve_shm_ptr = (CurveDetectionSharedMemory*)shmat(g_curve_shm_id, NULL, 0);
    if (g_curve_shm_ptr == (void*)-1) {
        perror("附加弯道检测共享内存失败");
        g_curve_shm_ptr = NULL;
        return -1;
    }

    printf("弯道检测共享内存初始化成功 (id=%d, ptr=%p)\n",
           g_curve_shm_id, (void*)g_curve_shm_ptr);

    if (is_writer) {
        memset(g_curve_shm_ptr, 0, sizeof(CurveDetectionSharedMemory));
        g_curve_shm_ptr->writer_active = 1;
        g_curve_shm_ptr->curve_type = CURVE_NONE;
        printf("初始化弯道检测控制结构\n");
    }

    return 0;
}

/* 写端清除活跃标志、分离映射并删除共享段；读端只分离映射。 */
void curve_detection_shm_cleanup(void) {

    if (g_curve_is_writer && g_curve_shm_ptr != NULL) {
        g_curve_shm_ptr->writer_active = 0;
    }

    if (g_curve_shm_ptr != NULL) {
        if (shmdt(g_curve_shm_ptr) == -1) {
            perror("分离弯道检测共享内存失败");
        }
        g_curve_shm_ptr = NULL;
    }

    if (g_curve_is_writer && g_curve_shm_id != -1) {
        if (shmctl(g_curve_shm_id, IPC_RMID, NULL) == -1) {
            perror("删除弯道检测共享内存失败");
        }
    }

    g_curve_shm_id = -1;
    printf("弯道检测共享内存已清理\n");
}

/* 仅写端可以发布弯道结果；保留帧号、分类、置信度、角度和原有时间戳更新顺序。 */
int curve_detection_update(uint32_t frame_id, CurveType curve_type,
                          uint32_t confidence, uint32_t curve_angle) {
    if (g_curve_shm_ptr == NULL) {
        fprintf(stderr, "错误: 弯道检测共享内存未初始化\n");
        return -1;
    }

    if (!g_curve_is_writer) {
        fprintf(stderr, "错误: 只有写入者可以更新弯道检测结果\n");
        return -1;
    }

    g_curve_shm_ptr->frame_id = frame_id;
    g_curve_shm_ptr->curve_type = curve_type;
    g_curve_shm_ptr->confidence = confidence;
    g_curve_shm_ptr->curve_angle = curve_angle;
    g_curve_shm_ptr->timestamp = (uint64_t)time(NULL) * 1000000;

    if (frame_id % 100 == 0) {
        const char* curve_name = "未知";
        switch(curve_type) {
            case CURVE_NONE: curve_name = "直道"; break;
            case CURVE_LEFT: curve_name = "左弯"; break;
            case CURVE_RIGHT: curve_name = "右弯"; break;
            case CURVE_BOTH: curve_name = "左右弯"; break;
        }
        printf("弯道检测更新: frame=%u, type=%s, confidence=%u%%, angle=%u度\n",
               frame_id, curve_name, confidence, curve_angle);
    }

    return 0;
}

/* 从已附加的弯道共享段读出四个字段；初始化和输出参数均须有效。 */
int curve_detection_read(uint32_t* frame_id, CurveType* curve_type,
                        uint32_t* confidence, uint32_t* curve_angle) {
    if (g_curve_shm_ptr == NULL) {
        fprintf(stderr, "错误: 弯道检测共享内存未初始化\n");
        return -1;
    }

    if (!frame_id || !curve_type || !confidence || !curve_angle) {
        fprintf(stderr, "错误: 弯道检测读取参数为空\n");
        return -1;
    }

    *frame_id = g_curve_shm_ptr->frame_id;
    *curve_type = (CurveType)g_curve_shm_ptr->curve_type;
    *confidence = g_curve_shm_ptr->confidence;
    *curve_angle = g_curve_shm_ptr->curve_angle;

    return 0;
}