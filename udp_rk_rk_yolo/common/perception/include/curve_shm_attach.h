#ifndef CURVE_SHM_ATTACH_H
#define CURVE_SHM_ATTACH_H

#include <errno.h>
#include <stdio.h>
#include <sys/shm.h>

// 两端共用段连接策略。写端创建失败时，只有已存在的段允许重新连接。
static inline int curve_shm_open(key_t key, size_t size, int is_writer)
{
    if (is_writer)
    {
        int id = shmget(key, size, IPC_CREAT | IPC_EXCL | 0666);
        if (id >= 0)
            return id;
        if (errno != EEXIST)
        {
            perror("创建弯道检测共享内存失败");
            return -1;
        }
        printf("弯道检测共享内存已存在，尝试连接...\n");
    }

    int id = shmget(key, size, 0666);
    if (id == -1)
        perror("连接弯道检测共享内存失败");
    return id;
}

#endif
