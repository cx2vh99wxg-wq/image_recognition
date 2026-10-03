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

//=========================================================================
// 共享内存管理实现
//=========================================================================

int shared_memory_init(SharedMemoryManager* manager, SharedMemoryType type, int is_writer) {
    if (!manager) {
        fprintf(stderr, "*** 致命错误: manager指针为空 ***\n");
        return -1;
    }
    
    memset(manager, 0, sizeof(SharedMemoryManager));
    manager->is_writer = is_writer;
    manager->type = type;
    
    // 根据类型确定共享内存大小和key
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
        // 写入者创建共享内存
        manager->shm_id = shmget(shm_key, shm_size, 
                                IPC_CREAT | IPC_EXCL | 0666);
        if (manager->shm_id == -1) {
            if (errno == EEXIST) {
                // 共享内存已存在，删除后重新创建
                int old_shm_id = shmget(shm_key, 0, 0);
                if (old_shm_id != -1) {
                    shmctl(old_shm_id, IPC_RMID, NULL);
                    printf("已删除现有的%s共享内存\n", type_name);
                }
                manager->shm_id = shmget(shm_key, shm_size, 
                                        IPC_CREAT | 0666);
            }
        }
    } else {
        // 读取者连接到现有共享内存
        manager->shm_id = shmget(shm_key, shm_size, 0666);
    }
    
    if (manager->shm_id == -1) {
        fprintf(stderr, "*** 致命错误: %s共享内存创建/连接失败 ***\n", type_name);
        fprintf(stderr, "*** 错误原因: %s ***\n", strerror(errno));
        fprintf(stderr, "*** 错误码: %d ***\n", errno);
        if (errno == ENOSPC) {
            fprintf(stderr, "*** 系统共享内存资源不足 ***\n");
            fprintf(stderr, "*** 建议: 增加系统共享内存限制或清理现有共享内存 ***\n");
        } else if (errno == ENOENT) {
            fprintf(stderr, "*** 共享内存不存在 (读取者需要写入者先创建) ***\n");
        } else if (errno == EACCES) {
            fprintf(stderr, "*** 权限不足，无法访问共享内存 ***\n");
        }
        return -1;
    }
    
    // 连接到共享内存
    manager->shm_ptr.generic_ptr = shmat(manager->shm_id, NULL, 0);
    if (manager->shm_ptr.generic_ptr == (void*)-1) {
        fprintf(stderr, "*** 致命错误: %s共享内存连接失败 ***\n", type_name);
        fprintf(stderr, "*** 错误原因: %s ***\n", strerror(errno));
        return -1;
    }
    
    // 如果是写入者，初始化控制结构
    if (is_writer) {
        if (type == SHM_TYPE_PCIE) {
            memset(manager->shm_ptr.pcie_ptr, 0, sizeof(PCIeSharedMemory));
            manager->shm_ptr.pcie_ptr->control.writer_active = 1;
            manager->shm_ptr.pcie_ptr->control.frame_width = PCIE_IMAGE_WIDTH;
            manager->shm_ptr.pcie_ptr->control.frame_height = PCIE_IMAGE_HEIGHT;
        } else if (type == SHM_TYPE_UDP) {
            memset(manager->shm_ptr.udp_ptr, 0, sizeof(UDPSharedMemory));
            manager->shm_ptr.udp_ptr->control.writer_active = 1;
            manager->shm_ptr.udp_ptr->control.frame_width = UDP_IMAGE_WIDTH;
            manager->shm_ptr.udp_ptr->control.frame_height = UDP_IMAGE_HEIGHT;
        }
    } else {
        // 读取者增加计数
        if (type == SHM_TYPE_PCIE) {
            __sync_fetch_and_add(&manager->shm_ptr.pcie_ptr->control.reader_count, 1);
        } else if (type == SHM_TYPE_UDP) {
            __sync_fetch_and_add(&manager->shm_ptr.udp_ptr->control.reader_count, 1);
        }
    }
    
    // 初始化信号量
    char sem_name_write[64], sem_name_read[64];
    snprintf(sem_name_write, sizeof(sem_name_write), "/%s_write_%08X", type_name, shm_key);
    snprintf(sem_name_read, sizeof(sem_name_read), "/%s_read_%08X", type_name, shm_key);
    
    if (is_writer) {
        // 写入者创建信号量
        sem_unlink(sem_name_write);
        sem_unlink(sem_name_read);
        
        manager->write_sem = sem_open(sem_name_write, O_CREAT | O_EXCL, 0666, 1);
        manager->read_sem = sem_open(sem_name_read, O_CREAT | O_EXCL, 0666, 0);
    } else {
        // 读取者打开现有信号量
        manager->write_sem = sem_open(sem_name_write, 0);
        manager->read_sem = sem_open(sem_name_read, 0);
    }
    
    if (manager->write_sem == SEM_FAILED || manager->read_sem == SEM_FAILED) {
        fprintf(stderr, "Warning: sem_open failed: %s\n", strerror(errno));
        fprintf(stderr, "Continuing without semaphores (no synchronization protection)\n");
        manager->write_sem = SEM_FAILED;
        manager->read_sem = SEM_FAILED;
        // 不返回错误，继续执行但没有信号量保护
    }
    
    printf("Shared memory initialized successfully (is_writer: %d)\n", is_writer);
    return 0;
}

int shared_memory_write_image(SharedMemoryManager* manager, 
                             const uint8_t* image_data, 
                             uint32_t width, uint32_t height) {
    if (!manager || !manager->shm_ptr.generic_ptr || !image_data || !manager->is_writer) {
        return -1;
    }
    
    SharedMemoryControl* control;
    uint8_t* image_buffer;
    size_t max_image_size;
    
    if (manager->type == SHM_TYPE_PCIE) {
        control = &manager->shm_ptr.pcie_ptr->control;
        image_buffer = manager->shm_ptr.pcie_ptr->image_data;
        max_image_size = PCIE_FRAME_SIZE;
    } else if (manager->type == SHM_TYPE_UDP) {
        control = &manager->shm_ptr.udp_ptr->control;
        image_buffer = manager->shm_ptr.udp_ptr->image_data;
        max_image_size = UDP_FRAME_SIZE;
    } else {
        return -1;
    }
    
    size_t image_size = width * height * 2; // RGB565
    if (image_size > max_image_size) {
        fprintf(stderr, "Error: Image size too large: %zu > %zu\n", 
                image_size, max_image_size);
        return -1;
    }
    
    // 等待写锁（如果信号量可用）
    if (manager->write_sem != SEM_FAILED) {
        if (sem_wait(manager->write_sem) != 0) {
            return -1;
        }
    }
    
    // 更新控制信息
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint64_t timestamp = ts.tv_sec * 1000000000ULL + ts.tv_nsec;
    
    control->timestamp = timestamp;
    control->frame_width = width;
    control->frame_height = height;
    
    // 复制图像数据
    memcpy(image_buffer, image_data, image_size);
    
    // 计算校验和
    control->checksum = calculate_data_checksum(image_data, image_size);
    
    // 原子更新帧信息
    __sync_fetch_and_add(&control->frame_id, 1);
    __sync_bool_compare_and_swap(&control->frame_ready, 0, 1);
    
    // 释放写锁并通知读取者（如果信号量可用）
    if (manager->write_sem != SEM_FAILED) {
        sem_post(manager->write_sem);
    }
    if (manager->read_sem != SEM_FAILED) {
        sem_post(manager->read_sem);
    }
    
    return 0;
}

int shared_memory_read_image(SharedMemoryManager* manager, 
                            uint8_t* image_data, 
                            size_t max_size,
                            uint32_t* width, 
                            uint32_t* height,
                            uint32_t* frame_id) {
    if (!manager || !manager->shm_ptr.generic_ptr || !image_data || manager->is_writer) {
        return -1;
    }
    
    SharedMemoryControl* control;
    uint8_t* image_buffer;
    
    if (manager->type == SHM_TYPE_PCIE) {
        control = &manager->shm_ptr.pcie_ptr->control;
        image_buffer = manager->shm_ptr.pcie_ptr->image_data;
    } else if (manager->type == SHM_TYPE_UDP) {
        control = &manager->shm_ptr.udp_ptr->control;
        image_buffer = manager->shm_ptr.udp_ptr->image_data;
    } else {
        return -1;
    }
    
    // 检查写入者是否活跃
    if (!control->writer_active) {
        return -1;
    }
    
    // 非阻塞检查是否有新帧
    if (!control->frame_ready) {
        return 0;  // 无新数据
    }
    
    // 尝试获取读锁（非阻塞，如果信号量可用）
    if (manager->read_sem != SEM_FAILED) {
        struct timespec timeout;
        clock_gettime(CLOCK_REALTIME, &timeout);
        timeout.tv_nsec += 10000000;  // 10ms超时
        if (timeout.tv_nsec >= 1000000000) {
            timeout.tv_sec += 1;
            timeout.tv_nsec -= 1000000000;
        }
        
        if (sem_timedwait(manager->read_sem, &timeout) != 0) {
            return 0;  // 超时，无新数据
        }
    }
    
    // 读取控制信息
    uint32_t img_width = control->frame_width;
    uint32_t img_height = control->frame_height;
    uint32_t current_frame_id = control->frame_id;
    uint32_t expected_checksum = control->checksum;
    
    size_t image_size = img_width * img_height * 2; // RGB565
    if (image_size > max_size) {
        fprintf(stderr, "Error: Buffer too small: %zu > %zu\n", image_size, max_size);
        return -1;
    }
    
    // 复制图像数据
    memcpy(image_data, image_buffer, image_size);
    
    // 验证校验和
    uint32_t actual_checksum = calculate_data_checksum(image_data, image_size);
    if (actual_checksum != expected_checksum) {
        fprintf(stderr, "Warning: Checksum mismatch (expected: 0x%08X, actual: 0x%08X)\n",
                expected_checksum, actual_checksum);
    }
    
    // 标记帧已被读取
    __sync_bool_compare_and_swap(&control->frame_ready, 1, 0);
    
    // 返回信息
    if (width) *width = img_width;
    if (height) *height = img_height;
    if (frame_id) *frame_id = current_frame_id;
    
    return image_size;
}

int shared_memory_has_new_frame(SharedMemoryManager* manager, uint32_t last_frame_id) {
    if (!manager || !manager->shm_ptr.generic_ptr) {
        return -1;
    }
    
    SharedMemoryControl* control;
    if (manager->type == SHM_TYPE_PCIE) {
        control = &manager->shm_ptr.pcie_ptr->control;
    } else if (manager->type == SHM_TYPE_UDP) {
        control = &manager->shm_ptr.udp_ptr->control;
    } else {
        return -1;
    }
    
    if (!control->writer_active) {
        return -1;  // 写入者不活跃
    }
    
    uint32_t current_frame_id = control->frame_id;
    return (current_frame_id > last_frame_id && control->frame_ready) ? 1 : 0;
}

void shared_memory_cleanup(SharedMemoryManager* manager) {
    if (!manager) {
        return;
    }
    
    if (manager->shm_ptr.generic_ptr && manager->shm_ptr.generic_ptr != (void*)-1) {
        SharedMemoryControl* control;
        if (manager->type == SHM_TYPE_PCIE) {
            control = &manager->shm_ptr.pcie_ptr->control;
        } else if (manager->type == SHM_TYPE_UDP) {
            control = &manager->shm_ptr.udp_ptr->control;
        } else {
            control = NULL;
        }
        
        if (control) {
            if (!manager->is_writer) {
                // 读取者减少计数
                __sync_fetch_and_sub(&control->reader_count, 1);
            } else {
                // 写入者标记非活跃
                control->writer_active = 0;
            }
        }
        
        shmdt(manager->shm_ptr.generic_ptr);
        manager->shm_ptr.generic_ptr = NULL;
    }
    
    if (manager->is_writer && manager->shm_id != -1) {
        // 写入者删除共享内存
        shmctl(manager->shm_id, IPC_RMID, NULL);
    }
    
    if (manager->write_sem && manager->write_sem != SEM_FAILED) {
        sem_close(manager->write_sem);
        if (manager->is_writer) {
            char sem_name[64];
            key_t shm_key = (manager->type == SHM_TYPE_PCIE) ? SHM_PCIE_IMAGE_KEY : SHM_UDP_IMAGE_KEY;
            const char* type_name = (manager->type == SHM_TYPE_PCIE) ? "PCIe" : "UDP";
            snprintf(sem_name, sizeof(sem_name), "/%s_write_%08X", type_name, shm_key);
            sem_unlink(sem_name);
        }
    }
    
    if (manager->read_sem && manager->read_sem != SEM_FAILED) {
        sem_close(manager->read_sem);
        if (manager->is_writer) {
            char sem_name[64];
            key_t shm_key = (manager->type == SHM_TYPE_PCIE) ? SHM_PCIE_IMAGE_KEY : SHM_UDP_IMAGE_KEY;
            const char* type_name = (manager->type == SHM_TYPE_PCIE) ? "PCIe" : "UDP";
            snprintf(sem_name, sizeof(sem_name), "/%s_read_%08X", type_name, shm_key);
            sem_unlink(sem_name);
        }
    }
    
    memset(manager, 0, sizeof(SharedMemoryManager));
    printf("Shared memory cleaned up\n");
}

uint32_t calculate_data_checksum(const uint8_t* data, size_t size) {
    if (!data || size == 0) {
        return 0;
    }
    
    uint32_t checksum = 0;
    for (size_t i = 0; i < size; i++) {
        checksum = ((checksum << 1) | (checksum >> 31)) ^ data[i];
    }
    return checksum;
}

//=========================================================================
// 兼容性函数
//=========================================================================

int shared_memory_init_pcie(SharedMemoryManager* manager, int is_writer) {
    return shared_memory_init(manager, SHM_TYPE_PCIE, is_writer);
}

int shared_memory_init_udp(SharedMemoryManager* manager, int is_writer) {
    return shared_memory_init(manager, SHM_TYPE_UDP, is_writer);
}

//=========================================================================
// 显示控制共享内存管理实现
//=========================================================================

int display_control_shm_init(DisplayControlSharedMemory** shm_ptr, int is_writer) {
    if (!shm_ptr) {
        fprintf(stderr, "*** 致命错误: shm_ptr指针为空 ***\n");
        return -1;
    }
    
    int shm_id;
    size_t shm_size = sizeof(DisplayControlSharedMemory);
    
    printf("正在初始化显示控制共享内存 (key=0x%x, size=%zu bytes, is_writer=%d)\n", 
           SHM_DISPLAY_CONTROL_KEY, shm_size, is_writer);
    
    if (is_writer) {
        // 写入者创建共享内存
        shm_id = shmget(SHM_DISPLAY_CONTROL_KEY, shm_size, IPC_CREAT | IPC_EXCL | 0666);
        if (shm_id == -1) {
            if (errno == EEXIST) {
                // 共享内存已存在，删除后重新创建
                int old_shm_id = shmget(SHM_DISPLAY_CONTROL_KEY, 0, 0);
                if (old_shm_id != -1) {
                    shmctl(old_shm_id, IPC_RMID, NULL);
                    printf("已删除现有的显示控制共享内存\n");
                }
                shm_id = shmget(SHM_DISPLAY_CONTROL_KEY, shm_size, IPC_CREAT | 0666);
            }
        }
    } else {
        // 读取者连接到现有共享内存
        shm_id = shmget(SHM_DISPLAY_CONTROL_KEY, shm_size, 0666);
    }
    
    if (shm_id == -1) {
        fprintf(stderr, "*** 致命错误: 显示控制共享内存创建/连接失败 ***\n");
        fprintf(stderr, "*** 错误原因: %s ***\n", strerror(errno));
        if (errno == ENOENT && !is_writer) {
            fprintf(stderr, "*** 共享内存不存在，请先启动FSPI控制程序 ***\n");
        }
        return -1;
    }
    
    // 连接到共享内存
    *shm_ptr = (DisplayControlSharedMemory*)shmat(shm_id, NULL, 0);
    if (*shm_ptr == (void*)-1) {
        fprintf(stderr, "*** 致命错误: 显示控制共享内存连接失败 ***\n");
        fprintf(stderr, "*** 错误原因: %s ***\n", strerror(errno));
        return -1;
    }
    
    // 如果是写入者，初始化控制结构
    if (is_writer) {
        memset(*shm_ptr, 0, sizeof(DisplayControlSharedMemory));
        (*shm_ptr)->mode = DISPLAY_MODE_ALL_CAMERAS;  // 默认显示所有摄像头
        (*shm_ptr)->valid = 1;                        // 标记有效
        
        // 设置初始时间戳
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        (*shm_ptr)->last_update = (uint32_t)ts.tv_sec;
        
        printf("显示控制共享内存初始化完成，默认模式: 所有摄像头 (0x99)\n");
    } else {
        printf("显示控制共享内存连接成功 (读取者模式)\n");
    }
    
    return 0;
}

void display_control_shm_cleanup(DisplayControlSharedMemory* shm_ptr, int is_writer) {
    if (!shm_ptr || shm_ptr == (void*)-1) {
        return;
    }
    
    if (is_writer) {
        // 写入者标记无效
        shm_ptr->valid = 0;
        printf("显示控制共享内存已标记为无效\n");
    }
    
    // 断开连接
    shmdt(shm_ptr);
    
    if (is_writer) {
        // 写入者删除共享内存
        int shm_id = shmget(SHM_DISPLAY_CONTROL_KEY, 0, 0);
        if (shm_id != -1) {
            shmctl(shm_id, IPC_RMID, NULL);
            printf("显示控制共享内存已删除\n");
        }
    }
    
    printf("显示控制共享内存清理完成\n");
}

//=========================================================================
// 紧急制动共享内存管理实现
//=========================================================================

int emergency_brake_shm_init(EmergencyBrakeSharedMemory** shm_ptr, int is_writer) {
    if (!shm_ptr) {
        fprintf(stderr, "*** 致命错误: shm_ptr指针为空 ***\n");
        return -1;
    }
    
    int shm_id;
    size_t shm_size = sizeof(EmergencyBrakeSharedMemory);
    
    printf("正在初始化紧急制动共享内存 (key=0x%x, size=%zu bytes, is_writer=%d)\n", 
           SHM_EMERGENCY_BRAKE_KEY, shm_size, is_writer);
    printf("角色: %s\n", is_writer ? "写入者(PCIe-YOLO检测)" : "读取者(FSPI-制动控制)");
    
    if (is_writer) {
        // 写入者(PCIe程序)创建共享内存
        shm_id = shmget(SHM_EMERGENCY_BRAKE_KEY, shm_size, IPC_CREAT | IPC_EXCL | 0666);
        if (shm_id == -1) {
            if (errno == EEXIST) {
                // 共享内存已存在，删除后重新创建
                int old_shm_id = shmget(SHM_EMERGENCY_BRAKE_KEY, 0, 0);
                if (old_shm_id != -1) {
                    shmctl(old_shm_id, IPC_RMID, NULL);
                    printf("已删除现有的紧急制动共享内存\n");
                }
                shm_id = shmget(SHM_EMERGENCY_BRAKE_KEY, shm_size, IPC_CREAT | 0666);
            }
        }
    } else {
        // 读取者(FSPI程序)连接到现有共享内存
        shm_id = shmget(SHM_EMERGENCY_BRAKE_KEY, shm_size, 0666);
    }
    
    if (shm_id == -1) {
        fprintf(stderr, "*** 致命错误: 紧急制动共享内存创建/连接失败 ***\n");
        fprintf(stderr, "*** 错误原因: %s ***\n", strerror(errno));
        if (errno == ENOENT && !is_writer) {
            fprintf(stderr, "*** 共享内存不存在，请先启动PCIe检测程序 ***\n");
        }
        return -1;
    }
    
    // 连接到共享内存
    *shm_ptr = (EmergencyBrakeSharedMemory*)shmat(shm_id, NULL, 0);
    if (*shm_ptr == (void*)-1) {
        fprintf(stderr, "*** 致命错误: 紧急制动共享内存连接失败 ***\n");
        fprintf(stderr, "*** 错误原因: %s ***\n", strerror(errno));
        return -1;
    }
    
    // 如果是写入者，初始化控制结构
    if (is_writer) {
        memset(*shm_ptr, 0, sizeof(EmergencyBrakeSharedMemory));
        (*shm_ptr)->person_detected = 0;        // 初始未检测到人
        (*shm_ptr)->brake_enable = 1;           // 初始制动信号为1(正常运行)
        (*shm_ptr)->valid = 1;                  // 标记有效
        
        // 设置初始时间戳
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        (*shm_ptr)->last_update = (uint32_t)ts.tv_sec;
        
        printf("紧急制动共享内存初始化完成，默认状态: 正常运行 (brake_enable=1)\n");
    } else {
        printf("紧急制动共享内存连接成功 (读取者模式)\n");
    }
    
    return 0;
}

int emergency_brake_update_status(EmergencyBrakeSharedMemory* shm_ptr, int person_detected,
                                 int box_left, int box_top, int box_right, int box_bottom) {
    if (!shm_ptr || shm_ptr == (void*)-1) {
        fprintf(stderr, "错误: 紧急制动共享内存指针无效\n");
        return -1;
    }

    if (!shm_ptr->valid) {
        fprintf(stderr, "警告: 紧急制动共享内存已标记为无效\n");
        return -1;
    }

    // 更新检测状态和制动信号
    int old_person_detected = shm_ptr->person_detected;
    int old_brake_enable = shm_ptr->brake_enable;

    // 原子更新人员检测状态
    __sync_lock_test_and_set(&shm_ptr->person_detected, person_detected ? 1 : 0);

    // 更新检测框位置
    if (person_detected) {
        __sync_lock_test_and_set(&shm_ptr->box_left, box_left);
        __sync_lock_test_and_set(&shm_ptr->box_top, box_top);
        __sync_lock_test_and_set(&shm_ptr->box_right, box_right);
        __sync_lock_test_and_set(&shm_ptr->box_bottom, box_bottom);
    }

    // 根据人员检测状态设置制动信号
    // person_detected = 1 -> brake_enable = 0 (紧急制动)
    // person_detected = 0 -> brake_enable = 1 (正常运行)
    int new_brake_enable = person_detected ? 0 : 1;
    __sync_lock_test_and_set(&shm_ptr->brake_enable, new_brake_enable);

    // 更新时间戳和计数器
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    __sync_lock_test_and_set(&shm_ptr->last_update, (uint32_t)ts.tv_sec);
    __sync_fetch_and_add(&shm_ptr->frame_count, 1);

    // 如果检测状态发生变化，更新相应计数器
    if (person_detected && !old_person_detected) {
        __sync_fetch_and_add(&shm_ptr->detection_count, 1);
        printf("[紧急制动] 检测到人员！触发紧急制动 (brake_enable: %d->0)\n", old_brake_enable);
    } else if (!person_detected && old_person_detected) {
        printf("[紧急制动] 人员消失，恢复正常运行 (brake_enable: %d->1)\n", old_brake_enable);
    }

    if (new_brake_enable != old_brake_enable) {
        if (new_brake_enable == 0) {
            __sync_fetch_and_add(&shm_ptr->brake_active_count, 1);
        }
    }

    return 0;
}

int emergency_brake_read_status(EmergencyBrakeSharedMemory* shm_ptr, int* person_detected, int* brake_enable) {
    if (!shm_ptr || shm_ptr == (void*)-1) {
        fprintf(stderr, "错误: 紧急制动共享内存指针无效\n");
        return -1;
    }
    
    if (!shm_ptr->valid) {
        fprintf(stderr, "警告: 紧急制动共享内存已标记为无效\n");
        return -1;
    }
    
    // 原子读取状态
    if (person_detected) {
        *person_detected = shm_ptr->person_detected;
    }
    
    if (brake_enable) {
        *brake_enable = shm_ptr->brake_enable;
    }
    
    return 0;
}

void emergency_brake_shm_cleanup(EmergencyBrakeSharedMemory* shm_ptr, int is_writer) {
    if (!shm_ptr || shm_ptr == (void*)-1) {
        return;
    }

    if (is_writer) {
        // 写入者标记无效
        shm_ptr->valid = 0;
        printf("紧急制动共享内存已标记为无效\n");

        // 输出统计信息
        printf("=== 紧急制动统计 ===\n");
        printf("总检测帧数: %u\n", shm_ptr->frame_count);
        printf("人员检测次数: %u\n", shm_ptr->detection_count);
        printf("紧急制动激活次数: %u\n", shm_ptr->brake_active_count);
    }

    // 断开连接
    shmdt(shm_ptr);

    if (is_writer) {
        // 写入者删除共享内存
        int shm_id = shmget(SHM_EMERGENCY_BRAKE_KEY, 0, 0);
        if (shm_id != -1) {
            shmctl(shm_id, IPC_RMID, NULL);
            printf("紧急制动共享内存已删除\n");
        }
    }

    printf("紧急制动共享内存清理完成\n");
}

//=========================================================================
// 弯道检测结果共享内存实现
//=========================================================================

// 全局变量存储弯道检测共享内存信息
static int g_curve_shm_id = -1;
static CurveDetectionSharedMemory* g_curve_shm_ptr = NULL;
static int g_curve_is_writer = 0;

int curve_detection_shm_init(int is_writer) {
    size_t shm_size = sizeof(CurveDetectionSharedMemory);
    key_t shm_key = SHM_CURVE_DETECTION_KEY;

    printf("正在初始化弯道检测共享内存 (key=0x%x, size=%zu bytes)\n",
           shm_key, shm_size);

    g_curve_is_writer = is_writer;

    if (is_writer) {
        // 写入者创建共享内存
        g_curve_shm_id = shmget(shm_key, shm_size,
                                IPC_CREAT | IPC_EXCL | 0666);
        if (g_curve_shm_id == -1) {
            if (errno == EEXIST) {
                // 共享内存已存在，尝试连接
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
        // 读取者连接现有共享内存
        g_curve_shm_id = shmget(shm_key, shm_size, 0666);
        if (g_curve_shm_id == -1) {
            perror("连接弯道检测共享内存失败");
            return -1;
        }
    }

    // 附加到共享内存
    g_curve_shm_ptr = (CurveDetectionSharedMemory*)shmat(g_curve_shm_id, NULL, 0);
    if (g_curve_shm_ptr == (void*)-1) {
        perror("附加弯道检测共享内存失败");
        g_curve_shm_ptr = NULL;
        return -1;
    }

    printf("弯道检测共享内存初始化成功 (id=%d, ptr=%p)\n",
           g_curve_shm_id, (void*)g_curve_shm_ptr);

    // 如果是写入者，初始化结构
    if (is_writer) {
        memset(g_curve_shm_ptr, 0, sizeof(CurveDetectionSharedMemory));
        g_curve_shm_ptr->writer_active = 1;
        g_curve_shm_ptr->curve_type = CURVE_NONE;
        printf("初始化弯道检测控制结构\n");
    }

    return 0;
}

void curve_detection_shm_cleanup(void) {
    // 如果是写入者，标记为不活跃
    if (g_curve_is_writer && g_curve_shm_ptr != NULL) {
        g_curve_shm_ptr->writer_active = 0;
    }

    // 分离共享内存
    if (g_curve_shm_ptr != NULL) {
        if (shmdt(g_curve_shm_ptr) == -1) {
            perror("分离弯道检测共享内存失败");
        }
        g_curve_shm_ptr = NULL;
    }

    // 如果是写入者，删除共享内存
    if (g_curve_is_writer && g_curve_shm_id != -1) {
        if (shmctl(g_curve_shm_id, IPC_RMID, NULL) == -1) {
            perror("删除弯道检测共享内存失败");
        }
    }

    g_curve_shm_id = -1;
    printf("弯道检测共享内存已清理\n");
}

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

    // 更新弯道检测结果
    g_curve_shm_ptr->frame_id = frame_id;
    g_curve_shm_ptr->curve_type = curve_type;
    g_curve_shm_ptr->confidence = confidence;
    g_curve_shm_ptr->curve_angle = curve_angle;
    g_curve_shm_ptr->timestamp = (uint64_t)time(NULL) * 1000000; // 微秒时间戳

    // 每100帧打印一次，避免刷屏
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

    // 读取弯道检测结果
    *frame_id = g_curve_shm_ptr->frame_id;
    *curve_type = (CurveType)g_curve_shm_ptr->curve_type;
    *confidence = g_curve_shm_ptr->confidence;
    *curve_angle = g_curve_shm_ptr->curve_angle;

    return 0;
}