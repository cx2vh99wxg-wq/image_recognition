/* 验证模块独占关系，不打开设备或执行 ioctl。 */
#define _DEFAULT_SOURCE
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
static unsigned device_opens;
static int owner_test_open(const char *path,int flags,...){
    if(!strcmp(path,"/dev/spidev4.0")){++device_opens;errno=ENODEV;return -1;}
    va_list a;va_start(a,flags);mode_t mode=(flags&O_CREAT)?(mode_t)va_arg(a,int):0;va_end(a);
    return open(path,flags,mode);
}
#define open owner_test_open
#include "csrc/fspi_module.c"
#undef open
#include <assert.h>
int main(void){
    int owner=open("/tmp/steering-actuator.lock",O_CREAT|O_RDWR,0600);assert(owner>=0);
    assert(flock(owner,LOCK_EX|LOCK_NB)==0);
    assert(fspi_init()==-1&&(errno==EWOULDBLOCK||errno==EAGAIN)&&device_opens==0);
    close(owner);
    assert(fspi_init()==-1&&device_opens==1&&g_spi_owner_lock==-1);
    owner=open("/tmp/steering-actuator.lock",O_RDWR);assert(owner>=0&&flock(owner,LOCK_EX|LOCK_NB)==0);close(owner);
    puts("PASS SPI ownership: competing owner refused before device open, failed initialization releases owner lock; no device access");return 0;
}
