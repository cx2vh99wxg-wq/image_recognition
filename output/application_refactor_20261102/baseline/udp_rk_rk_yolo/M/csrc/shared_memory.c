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

typedef char image_header_abi_must_stay_104[(sizeof(SharedMemoryControl) == 104) ? 1 : -1];
typedef char curve_header_abi_must_stay_72[(sizeof(CurveDetectionSharedMemory) == 72) ? 1 : -1];
#define IMAGE_GENERATION_MAGIC 0x494D4733u
#define CURVE_SOURCE_MAGIC 0x43525633u
static uint32_t* reserved_word(uint8_t* reserved, unsigned index) {
    return (uint32_t*)(void*)(reserved + index * sizeof(uint32_t));
}
uint64_t shared_memory_image_generation(const SharedMemoryManager* manager) {
    if (!manager || !manager->shm_ptr || manager->shm_ptr == (void*)-1) return 0;
    SharedMemoryControl* control = (SharedMemoryControl*)manager->shm_ptr;
    if (!__atomic_load_n(&control->writer_active, __ATOMIC_ACQUIRE) ||
        __atomic_load_n(reserved_word(control->reserved, 0), __ATOMIC_ACQUIRE) != IMAGE_GENERATION_MAGIC) return 0;
    uint32_t lo = __atomic_load_n(reserved_word(control->reserved, 1), __ATOMIC_RELAXED);
    uint32_t hi = __atomic_load_n(reserved_word(control->reserved, 2), __ATOMIC_RELAXED);
    return ((uint64_t)hi << 32) | lo;
}

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
    manager->image_sem = SEM_FAILED;
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

    char image_sem_name[64];
    snprintf(image_sem_name, sizeof(image_sem_name), "/M_%s_image_%08X", type_name, shm_key);
    if (is_writer) {
        sem_unlink(image_sem_name);
        mode_t previous_mask = umask(0);
        manager->image_sem = sem_open(image_sem_name, O_CREAT | O_EXCL, 0666, 1);
        umask(previous_mask);
    } else {
        manager->image_sem = sem_open(image_sem_name, 0);
    }
    if (manager->image_sem == SEM_FAILED) {
        perror("Image semaphore open failed (restart producer and consumer together)");
        shmdt(manager->shm_ptr);
        manager->shm_ptr = (void*)-1;
        if (is_writer) shmctl(manager->shm_id, IPC_RMID, NULL);
        return -1;
    }

    if (is_writer) {
        SharedMemoryControl* control = NULL;
        if (type == SHM_TYPE_PCIE) {
            control = &((PCIeSharedMemory*)manager->shm_ptr)->control;
        } else if (type == SHM_TYPE_UDP) {
            control = &((UDPSharedMemory*)manager->shm_ptr)->control;
        }

        if (control) {
            memset(control, 0, sizeof(SharedMemoryControl));
            control->frame_width = (type == SHM_TYPE_PCIE) ? PCIE_IMAGE_WIDTH : UDP_IMAGE_WIDTH;
            control->frame_height = (type == SHM_TYPE_PCIE) ? PCIE_IMAGE_HEIGHT : UDP_IMAGE_HEIGHT;
            struct timespec generation;
            clock_gettime(CLOCK_MONOTONIC, &generation);
            uint64_t epoch = (uint64_t)generation.tv_sec * 1000000000ULL + generation.tv_nsec;
            __atomic_store_n(reserved_word(control->reserved, 1), (uint32_t)epoch, __ATOMIC_RELAXED);
            __atomic_store_n(reserved_word(control->reserved, 2), (uint32_t)(epoch >> 32), __ATOMIC_RELAXED);
            __atomic_store_n(reserved_word(control->reserved, 0), IMAGE_GENERATION_MAGIC, __ATOMIC_RELEASE);
            __atomic_store_n(&control->writer_active, 1, __ATOMIC_RELEASE);
            printf("Initialized control structure for %s\n", type_name);
        }
    }

    return 0;
}

/* 写端先清除活跃标志，再分离并删除段；读端只分离。保留原有空指针处理方式。 */
void shared_memory_cleanup(SharedMemoryManager* manager) {
    if (!manager) return;

    if (manager->is_writer && manager->shm_ptr && manager->shm_ptr != (void*)-1) {
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

    if (manager->shm_ptr && manager->shm_ptr != (void*)-1) {
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

    if (manager->image_sem && manager->image_sem != SEM_FAILED) {
        sem_close(manager->image_sem);
        if (manager->is_writer) {
            char name[64];
            const char* kind = manager->type == SHM_TYPE_PCIE ? "PCIe" : "UDP";
            key_t key = manager->type == SHM_TYPE_PCIE ? SHM_PCIE_IMAGE_KEY : SHM_UDP_IMAGE_KEY;
            snprintf(name, sizeof(name), "/M_%s_image_%08X", kind, key);
            sem_unlink(name);
        }
        manager->image_sem = SEM_FAILED;
    }

    manager->shm_id = -1;
    printf("Shared memory cleaned up\n");
}

/* One protected image slot: publishing replaces the previous image, never queues it.
 * The checksum is computed before taking the lock; readers copy a coherent snapshot. */
static int write_image_frame(SharedMemoryManager* manager, SharedMemoryType type,
                             const uint8_t* data, uint32_t width, uint32_t height) {
    if (!manager || manager->type != type || !data || !manager->is_writer ||
        !manager->shm_ptr || manager->shm_ptr == (void*)-1 ||
        !manager->image_sem || manager->image_sem == SEM_FAILED ||
        !width || !height || width > 640 || height > 480) return -1;
    SharedMemoryControl* control = type == SHM_TYPE_PCIE ?
        &((PCIeSharedMemory*)manager->shm_ptr)->control :
        &((UDPSharedMemory*)manager->shm_ptr)->control;
    uint8_t* target = type == SHM_TYPE_PCIE ?
        ((PCIeSharedMemory*)manager->shm_ptr)->image_data :
        ((UDPSharedMemory*)manager->shm_ptr)->image_data;
    size_t size = (size_t)width * height * 2;
    uint32_t checksum = sum_rgb565_bytes(data, size);
    int result;
    do { result = sem_wait(manager->image_sem); } while (result && errno == EINTR);
    if (result) return -1;
    memcpy(target, data, size);
    control->frame_width = width;
    control->frame_height = height;
    control->timestamp = (uint64_t)time(NULL) * 1000000;
    control->checksum = checksum;
    control->frame_id++;
    control->frame_ready = 1;
    sem_post(manager->image_sem);
    return 0;
}

static int read_image_frame(SharedMemoryManager* manager, SharedMemoryType type,
                            uint8_t* data, uint32_t* width, uint32_t* height,
                            uint32_t* frame_id) {
    if (!manager || manager->type != type || !data || !width || !height || !frame_id ||
        manager->is_writer || !manager->shm_ptr || manager->shm_ptr == (void*)-1 ||
        !manager->image_sem || manager->image_sem == SEM_FAILED) return -1;
    SharedMemoryControl* control = type == SHM_TYPE_PCIE ?
        &((PCIeSharedMemory*)manager->shm_ptr)->control :
        &((UDPSharedMemory*)manager->shm_ptr)->control;
    const uint8_t* source = type == SHM_TYPE_PCIE ?
        ((PCIeSharedMemory*)manager->shm_ptr)->image_data :
        ((UDPSharedMemory*)manager->shm_ptr)->image_data;
    if (sem_trywait(manager->image_sem)) return errno == EAGAIN || errno == EINTR ? 1 : -1;
    if (!control->writer_active) { sem_post(manager->image_sem); return -1; }
    if (!control->frame_ready || control->frame_id == manager->last_frame_id) {
        sem_post(manager->image_sem); return 1;
    }
    uint32_t w = control->frame_width, h = control->frame_height;
    uint32_t id = control->frame_id, expected = control->checksum;
    if (!w || !h || w > 640 || h > 480) { sem_post(manager->image_sem); return -1; }
    size_t size = (size_t)w * h * 2;
    memcpy(data, source, size);
    sem_post(manager->image_sem);
    if (sum_rgb565_bytes(data, size) != expected) return -1;
    manager->last_frame_id = id;
    *width = w; *height = h; *frame_id = id;
    return 0;
}

int pcie_shared_memory_write_frame(SharedMemoryManager* manager, const uint8_t* data,
                                  uint32_t width, uint32_t height) {
    return write_image_frame(manager, SHM_TYPE_PCIE, data, width, height);
}
int pcie_shared_memory_read_frame(SharedMemoryManager* manager, uint8_t* data,
                                 uint32_t* width, uint32_t* height, uint32_t* id) {
    return read_image_frame(manager, SHM_TYPE_PCIE, data, width, height, id);
}
int udp_shared_memory_write_frame(SharedMemoryManager* manager, const uint8_t* data,
                                 uint32_t width, uint32_t height) {
    return write_image_frame(manager, SHM_TYPE_UDP, data, width, height);
}
int udp_shared_memory_read_frame(SharedMemoryManager* manager, uint8_t* data,
                                uint32_t* width, uint32_t* height, uint32_t* id) {
    return read_image_frame(manager, SHM_TYPE_UDP, data, width, height, id);
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
        __atomic_store_n(reserved_word(g_curve_shm_ptr->reserved, 0), CURVE_SOURCE_MAGIC, __ATOMIC_RELEASE);
        __atomic_store_n(&g_curve_shm_ptr->writer_active, 1, __ATOMIC_RELEASE);
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

/* Reserved words are naturally 32-bit aligned even though reserved starts at byte 28.
 * Do not cast capture timestamp to atomic uint64_t on AArch64's unaligned offset. */
int curve_detection_update_source(uint32_t frame_id, CurveType curve_type,
                                  uint32_t confidence, uint32_t curve_angle,
                                  uint64_t capture_monotonic_ns) {
    if (!g_curve_shm_ptr || !g_curve_is_writer) return -1;
    uint32_t* seq = reserved_word(g_curve_shm_ptr->reserved, 1);
    __atomic_fetch_add(seq, 1u, __ATOMIC_ACQ_REL);
    __atomic_store_n(&g_curve_shm_ptr->frame_id, frame_id, __ATOMIC_RELAXED);
    __atomic_store_n(&g_curve_shm_ptr->curve_type, (uint32_t)curve_type, __ATOMIC_RELAXED);
    __atomic_store_n(&g_curve_shm_ptr->confidence, confidence, __ATOMIC_RELAXED);
    __atomic_store_n(&g_curve_shm_ptr->curve_angle, curve_angle, __ATOMIC_RELAXED);
    __atomic_store_n(reserved_word(g_curve_shm_ptr->reserved, 2), (uint32_t)capture_monotonic_ns, __ATOMIC_RELAXED);
    __atomic_store_n(reserved_word(g_curve_shm_ptr->reserved, 3), (uint32_t)(capture_monotonic_ns >> 32), __ATOMIC_RELAXED);
    g_curve_shm_ptr->timestamp = (uint64_t)time(NULL) * 1000000;
    __atomic_fetch_add(seq, 1u, __ATOMIC_RELEASE);
    return 0;
}
int curve_detection_update(uint32_t frame_id, CurveType curve_type,
                          uint32_t confidence, uint32_t curve_angle) {
    return curve_detection_update_source(frame_id, curve_type, confidence, curve_angle, 0);
}
int curve_detection_read_source(uint32_t* frame_id, CurveType* curve_type,
                                uint32_t* confidence, uint32_t* curve_angle,
                                uint64_t* capture_monotonic_ns) {
    if (!g_curve_shm_ptr || !frame_id || !curve_type || !confidence || !curve_angle || !capture_monotonic_ns ||
        !__atomic_load_n(&g_curve_shm_ptr->writer_active, __ATOMIC_ACQUIRE)) return -1;
    uint32_t* seq = reserved_word(g_curve_shm_ptr->reserved, 1);
    int modern = __atomic_load_n(reserved_word(g_curve_shm_ptr->reserved, 0), __ATOMIC_ACQUIRE) == CURVE_SOURCE_MAGIC;
    for (unsigned attempt = 0; attempt < 4; ++attempt) {
        uint32_t before = modern ? __atomic_load_n(seq, __ATOMIC_ACQUIRE) : 0;
        if (before & 1u) continue;
        uint32_t id = __atomic_load_n(&g_curve_shm_ptr->frame_id, __ATOMIC_RELAXED);
        uint32_t type = __atomic_load_n(&g_curve_shm_ptr->curve_type, __ATOMIC_RELAXED);
        uint32_t conf = __atomic_load_n(&g_curve_shm_ptr->confidence, __ATOMIC_RELAXED);
        uint32_t angle = __atomic_load_n(&g_curve_shm_ptr->curve_angle, __ATOMIC_RELAXED);
        uint32_t lo = modern ? __atomic_load_n(reserved_word(g_curve_shm_ptr->reserved, 2), __ATOMIC_RELAXED) : 0;
        uint32_t hi = modern ? __atomic_load_n(reserved_word(g_curve_shm_ptr->reserved, 3), __ATOMIC_RELAXED) : 0;
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        if (modern && before != __atomic_load_n(seq, __ATOMIC_RELAXED)) continue;
        if (!__atomic_load_n(&g_curve_shm_ptr->writer_active, __ATOMIC_ACQUIRE)) return -1;
        *frame_id = id; *curve_type = (CurveType)type; *confidence = conf; *curve_angle = angle;
        *capture_monotonic_ns = ((uint64_t)hi << 32) | lo;
        return 0;
    }
    return -1;
}
int curve_detection_read(uint32_t* frame_id, CurveType* curve_type,
                        uint32_t* confidence, uint32_t* curve_angle) {
    uint64_t capture_ns;
    return curve_detection_read_source(frame_id, curve_type, confidence, curve_angle, &capture_ns);
}
