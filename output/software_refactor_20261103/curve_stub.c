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
