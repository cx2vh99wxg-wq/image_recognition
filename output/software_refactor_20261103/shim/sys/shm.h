#ifndef TEST_SYSV_SHM_H
#define TEST_SYSV_SHM_H
#include <stddef.h>
typedef int key_t;
#define IPC_CREAT 01000
#define IPC_EXCL 02000
#define IPC_RMID 0
int shmget(key_t, size_t, int);
void *shmat(int, const void *, int);
int shmdt(const void *);
int shmctl(int, int, void *);
#endif
