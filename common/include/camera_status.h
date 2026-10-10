#ifndef CAMERA_STATUS_H
#define CAMERA_STATUS_H
#include "shm_ipc.h"
/* One atomic 64-bit word: monotonic milliseconds + 3 live camera bits.
 * Both processes are on the SAME board; never compare this clock over UDP. */
#define SHM_KEY_CAMERA_STATUS 0x12345684L
static inline int camera_status_write(uint8_t mask,uint64_t now)
{
    void *p=NULL;
    if(shm_create("camera_health",SHM_KEY_CAMERA_STATUS,8,&p)<0)return -1;
    __atomic_store_n((uint64_t *)p,((now/1000)<<8)|(mask&7),__ATOMIC_RELEASE);
    return 0;
}
static inline int camera_status_read(uint64_t now)
{
    void *p=NULL;
    if(shm_open(SHM_KEY_CAMERA_STATUS,8,&p)<0)return 0;
    uint64_t word=__atomic_load_n((uint64_t *)p,__ATOMIC_ACQUIRE),t=word>>8;
    return t && now/1000>=t && now/1000-t<=300 ? (int)(word&7):0;
}
#endif
