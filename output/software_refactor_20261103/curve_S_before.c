#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/shm.h>
#define SHM_CURVE_DETECTION_KEY 0x1111
#define CURVE_SOURCE_MAGIC 0x43525633u
#define CURVE_NONE 0
typedef struct { uint32_t writer_active,curve_type,reserved[12]; } CurveDetectionSharedMemory;
static uint32_t *reserved_word(uint32_t *reserved, unsigned i) { return reserved+i; }
static CurveDetectionSharedMemory segment;
static int scenario, call;
int shmget(key_t key,size_t size,int flags) {
    printf("get %d %zu %d\n",key,size,flags);
    assert(key==SHM_CURVE_DETECTION_KEY && size==sizeof(segment));
    ++call;
    if (call==1 && (flags&IPC_EXCL) && (scenario==1 || scenario==3)) { errno=EEXIST; return -1; }
    if (scenario==2 || scenario==3) { errno=EACCES; return -1; }
    return 33;
}
void *shmat(int id,const void *ptr,int flags) {
    assert(id==33 && !ptr && !flags);
    printf("attach\n");
    if (scenario==4) { errno=ENOMEM; return (void *)-1; }
    return &segment;
}
int shmdt(const void *ptr) { assert(ptr==&segment); printf("detach %u\n",segment.writer_active); return 0; }
int shmctl(int id,int cmd,void *ptr) { assert(id==33 && cmd==IPC_RMID && !ptr); printf("remove\n"); return 0; }
static int g_curve_shm_id = -1;
static CurveDetectionSharedMemory *g_curve_shm_ptr = NULL;
static int g_curve_is_writer = 0;

/* 共享数据：按固定键创建或连接弯道结果段；写端初始化为直道并标记活跃。 */
int curve_detection_shm_init(int is_writer)
{
    size_t shm_size = sizeof(CurveDetectionSharedMemory);
    key_t shm_key = SHM_CURVE_DETECTION_KEY;

    printf("正在初始化弯道检测共享内存 (key=0x%x, size=%zu bytes)\n", shm_key, shm_size);

    g_curve_is_writer = is_writer;

    if (is_writer)
    {

        g_curve_shm_id = shmget(shm_key, shm_size, IPC_CREAT | IPC_EXCL | 0666);
        if (g_curve_shm_id == -1)
        {
            if (errno == EEXIST)
            {

                printf("弯道检测共享内存已存在，尝试连接...\n");
                g_curve_shm_id = shmget(shm_key, shm_size, 0666);
                if (g_curve_shm_id == -1)
                {
                    perror("连接弯道检测共享内存失败");
                    return -1;
                }
            }
            else
            {
                perror("创建弯道检测共享内存失败");
                return -1;
            }
        }
    }
    else
    {

        g_curve_shm_id = shmget(shm_key, shm_size, 0666);
        if (g_curve_shm_id == -1)
        {
            perror("连接弯道检测共享内存失败");
            return -1;
        }
    }

    g_curve_shm_ptr = (CurveDetectionSharedMemory *)shmat(g_curve_shm_id, NULL, 0);
    if (g_curve_shm_ptr == (void *)-1)
    {
        perror("附加弯道检测共享内存失败");
        g_curve_shm_ptr = NULL;
        return -1;
    }


    if (is_writer)
    {
        memset(g_curve_shm_ptr, 0, sizeof(CurveDetectionSharedMemory));
        g_curve_shm_ptr->writer_active = 1;
        g_curve_shm_ptr->curve_type = CURVE_NONE;
        printf("初始化弯道检测控制结构\n");
    }

    return 0;
}

/* 共享数据：清除写端活跃位，分离映射；只有写端删除段。 */
void curve_detection_shm_cleanup(void)
{

    if (g_curve_is_writer && g_curve_shm_ptr != NULL)
    {
        g_curve_shm_ptr->writer_active = 0;
    }

    if (g_curve_shm_ptr != NULL)
    {
        if (shmdt(g_curve_shm_ptr) == -1)
        {
            perror("分离弯道检测共享内存失败");
        }
        g_curve_shm_ptr = NULL;
    }

    if (g_curve_is_writer && g_curve_shm_id != -1)
    {
        if (shmctl(g_curve_shm_id, IPC_RMID, NULL) == -1)
        {
            perror("删除弯道检测共享内存失败");
        }
    }

    g_curve_shm_id = -1;
    puts("cleanup complete");
}
int main(void) {
    for (int writer=0; writer<=1; ++writer)
        for (scenario=0; scenario<5; ++scenario) {
            memset(&segment,0x55,sizeof segment);
            call=0;
            int rc=curve_detection_shm_init(writer);
            printf("result %d %d %d %u %u %u\n",writer,scenario,rc,
                   segment.writer_active,segment.curve_type,segment.reserved[0]);
            curve_detection_shm_cleanup();
        }
}
