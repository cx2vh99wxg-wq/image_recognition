#include "fspi_driver.h"
#include "fpga_regmap.h"
#define LOG_TAG "CONTROL"
#include "log.h"
#include <string.h>
#if defined(__linux__) && !defined(USE_STUB)
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
struct fspi_driver {int fd; uint32_t speed;};
int fspi_driver_init(fspi_driver_t **ctx)
{
    if(!ctx)return -1;
    *ctx=NULL;
    fspi_driver_t *d=calloc(1,sizeof(*d));if(!d)return -1;
    const char *device=getenv("FSPI_DEVICE"),*speed=getenv("FSPI_SPEED_HZ");
    d->speed=FSPI_SPEED_HZ;
    if(speed){char *end;unsigned long n=strtoul(speed,&end,10);if(*end||n<100000||n>1000000){free(d);return -1;}d->speed=(uint32_t)n;}
    d->fd=open(device?device:FSPI_DEVICE,O_RDWR|O_CLOEXEC);
    uint32_t mode=SPI_MODE_3|SPI_TX_QUAD|SPI_RX_QUAD;uint8_t bits=8;
    if(d->fd<0||ioctl(d->fd,SPI_IOC_WR_MODE32,&mode)<0||ioctl(d->fd,SPI_IOC_WR_BITS_PER_WORD,&bits)<0||ioctl(d->fd,SPI_IOC_WR_MAX_SPEED_HZ,&d->speed)<0){if(d->fd>=0)close(d->fd);free(d);return -1;}
    *ctx=d;return 0;
}
void fspi_driver_deinit(fspi_driver_t *d){if(d){close(d->fd);free(d);}}
int fspi_driver_write_reg(fspi_driver_t *d,uint8_t addr,uint8_t value)
{
    if(!d)return -1;
    uint8_t tx[3]={FSPI_CMD_WRITE,addr,value};
    struct spi_ioc_transfer t;memset(&t,0,sizeof(t));
    t.tx_buf=(uintptr_t)tx;t.len=3;t.speed_hz=d->speed;t.bits_per_word=8;t.tx_nbits=4;
    return ioctl(d->fd,SPI_IOC_MESSAGE(1),&t)==3?0:-1;
}
int fspi_driver_read_reg(fspi_driver_t *d,uint8_t addr,uint8_t *out)
{
    if(!d||!out)return -1;
    uint8_t tx[3]={FSPI_CMD_READ,addr,0},rx=0;
    struct spi_ioc_transfer t[2];memset(t,0,sizeof(t));
    t[0].tx_buf=(uintptr_t)tx;t[0].len=3;t[0].tx_nbits=4;
    t[1].rx_buf=(uintptr_t)&rx;t[1].len=1;t[1].rx_nbits=4;
    for(int i=0;i<2;i++){t[i].speed_hz=d->speed;t[i].bits_per_word=8;}
    /* cs_change=0 keeps direction changes within a single chip select. */
    if(ioctl(d->fd,SPI_IOC_MESSAGE(2),t)!=4)return -1;
    *out=rx;return 0;
}
int fspi_driver_move(fspi_driver_t *d,fspi_move_t m,uint8_t e){return m<0||m>3||e>1?-1:fspi_driver_write_reg(d,(uint8_t)m,fspi_encode_enable(e));}
int fspi_driver_emergency_stop(fspi_driver_t *d){return fspi_driver_write_reg(d,FSPI_REG_STOP,0);}
int fspi_driver_emergency_release(fspi_driver_t *d){return fspi_driver_write_reg(d,FSPI_REG_STOP,1);}
#else /* !__linux__ 或 USE_STUB：桩实现，仅供语法/链接自检与无硬件联调 */

struct fspi_driver { int fd; };

int fspi_driver_init(fspi_driver_t **ctx)
{
    (void)ctx;
    LOGW("非 Linux 或 USE_STUB，FSPI 驱动为桩实现\n");
    return -1;
}

void fspi_driver_deinit(fspi_driver_t *ctx) { (void)ctx; }

int fspi_driver_write_reg(fspi_driver_t *ctx, uint8_t addr, uint8_t data)
{
    (void)ctx; (void)addr; (void)data;
    return -1;
}

int fspi_driver_read_reg(fspi_driver_t *ctx, uint8_t addr, uint8_t *data)
{
    (void)ctx; (void)addr; (void)data;
    return -1;
}

int fspi_driver_move(fspi_driver_t *ctx, fspi_move_t move, uint8_t enable)
{
    (void)ctx; (void)move; (void)enable;
    return -1;
}

int fspi_driver_emergency_stop(fspi_driver_t *ctx)
{
    (void)ctx;
    return -1;
}

int fspi_driver_emergency_release(fspi_driver_t *ctx)
{
    (void)ctx;
    return -1;
}

#endif /* __linux__ && !USE_STUB */
