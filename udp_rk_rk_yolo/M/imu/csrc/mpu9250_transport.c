#include "mpu9250_transport.h"
#include "steering_platform.h"
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sys/file.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#endif

steer_result_t mpu9250_capture_timing(uint64_t begin,uint64_t end)
{
    if(!begin||end<begin)return STEER_ERR_STALE;
    return end-begin>20000000?STEER_ERR_NOT_READY:STEER_OK;
}

steer_result_t mpu9250_read_identity(const mpu9250_bus_t *bus, uint8_t *identity)
{
    if (!bus || !bus->read_registers || !identity) return STEER_ERR_ARGUMENT;
    uint8_t value = 0;
    steer_result_t rc = bus->read_registers(bus->context, MPU9250_WHO_AM_I_REGISTER,
                                          &value, 1);
    if (rc != STEER_OK) return rc;
    if (value != MPU9250_EXPECTED_ID) return STEER_ERR_DEVICE;
    *identity = value;
    return STEER_OK;
}
#ifndef _WIN32
typedef struct { int fd,lock_fd; uint32_t speed; mpu9250_link_stats_t stats; } fpga_bus_t;
static steer_result_t transfer(fpga_bus_t *b,const uint8_t *tx,uint8_t *rx,size_t n)
{
    /* Rockchip SMDT inserts the 00/80 opcode. Do not put it in this buffer. */
    uint32_t mode=SPI_MODE_3|(tx?SPI_TX_QUAD:SPI_RX_QUAD);
    uint8_t bits=8;
    if(ioctl(b->fd,SPI_IOC_WR_MODE32,&mode)<0 ||
       ioctl(b->fd,SPI_IOC_WR_BITS_PER_WORD,&bits)<0){++b->stats.io_errors;return STEER_ERR_IO;}
    struct spi_ioc_transfer tr={0};
    tr.tx_buf=(uintptr_t)tx;tr.rx_buf=(uintptr_t)rx;tr.len=(uint32_t)n;
    tr.speed_hz=b->speed;tr.bits_per_word=8;
    tr.tx_nbits=tx?4:0;tr.rx_nbits=rx?4:0;
    ++b->stats.transfers;
    if(ioctl(b->fd,SPI_IOC_MESSAGE(1),&tr)==(int)n)return STEER_OK;
    ++b->stats.io_errors;return STEER_ERR_IO;
}
static steer_result_t command(fpga_bus_t *b,uint8_t reg,uint8_t value)
{
    const uint8_t tx[]={0x4d,reg,value};
    return transfer(b,tx,NULL,sizeof(tx));
}
static steer_result_t snapshot(fpga_bus_t *b,fpga_imu_snapshot_t *out,uint64_t *mid)
{
    uint8_t packet[FPGA_IMU_SPI_PREFIX+FPGA_IMU_PACKET_SIZE];
    uint64_t begin=steering_now_ns();
    if(command(b,1,0xa5)!=STEER_OK)return STEER_ERR_IO;
    steering_sleep_ms(1); /* CRC publication finishes in <10us; allow CDC slack. */
    if(transfer(b,NULL,packet,sizeof(packet))!=STEER_OK)return STEER_ERR_IO;
    uint64_t end=steering_now_ns();
    steer_result_t rc=mpu9250_capture_timing(begin,end);
    if(rc!=STEER_OK){++b->stats.stale_captures;return rc;}
    rc=fpga_imu_decode(out,packet+FPGA_IMU_SPI_PREFIX,FPGA_IMU_PACKET_SIZE);
    if(rc==STEER_ERR_IO)++b->stats.crc_errors;
    if(rc==STEER_OK)b->stats.i2c_errors=out->errors;
    if(rc==STEER_OK&&mid)*mid=begin+(end-begin)/2;
    return rc;
}
static steer_result_t read_cached(void *context,uint8_t reg,uint8_t *out,size_t n)
{
    if(!context||!out||reg!=0x75||n!=1)return STEER_ERR_ARGUMENT;
    fpga_imu_snapshot_t s;
    steer_result_t rc=snapshot(context,&s,NULL);
    if(rc!=STEER_OK)return rc;
    if(!(s.flags&2))return STEER_ERR_NOT_READY;
    *out=s.identity;return STEER_OK;
}
#endif
steer_result_t mpu9250_fpga_open(mpu9250_bus_t *bus)
{
    if (!bus) return STEER_ERR_ARGUMENT;
    memset(bus, 0, sizeof(*bus));
#ifdef _WIN32
    return STEER_ERR_NOT_READY;
#else
    fpga_bus_t *b=calloc(1,sizeof(*b));
    if(!b)return STEER_ERR_IO;
    /* SMDT supports only 24/50/75/100/125/150MHz. Use its lowest
     * supported rate, matching the existing M SDK configuration. */
    b->fd=b->lock_fd=-1;b->speed=24000000;
    b->lock_fd=open("/tmp/mpu9250-m-fspi.lock",O_CREAT|O_RDWR|O_CLOEXEC,0600);
    if(b->lock_fd<0){free(b);return STEER_ERR_IO;}
    if(flock(b->lock_fd,LOCK_EX|LOCK_NB)<0){close(b->lock_fd);free(b);return STEER_ERR_BUSY;}
    b->fd=open("/dev/spidev4.0",O_RDWR|O_CLOEXEC);
    if(b->fd<0){close(b->lock_fd);free(b);return STEER_ERR_NOT_READY;}
    uint8_t packet[72];fpga_imu_snapshot_t s;
    /* Read-only compatibility check before any write to an unknown FPGA. */
    steer_result_t rc=transfer(b,NULL,packet,sizeof(packet));
    if(rc==STEER_OK)rc=fpga_imu_decode(&s,packet+8,64);
    if(rc==STEER_OK)rc=command(b,0,0xa5);
    if(rc!=STEER_OK){close(b->fd);close(b->lock_fd);free(b);return rc;}
    bus->context=b;bus->read_registers=read_cached;
    return STEER_OK;
#endif
}
steer_result_t mpu9250_fpga_snapshot(mpu9250_bus_t *bus,fpga_imu_snapshot_t *out,uint64_t *mid)
{
    if(!bus||!bus->context||!out)return STEER_ERR_ARGUMENT;
#ifdef _WIN32
    (void)mid;return STEER_ERR_NOT_READY;
#else
    return snapshot(bus->context,out,mid);
#endif
}
void mpu9250_fpga_stats(const mpu9250_bus_t *bus,mpu9250_link_stats_t *out)
{
    if(!out)return;
    memset(out,0,sizeof(*out));
#ifndef _WIN32
    if(bus&&bus->context)*out=((const fpga_bus_t*)bus->context)->stats;
#else
    (void)bus;
#endif
}
void mpu9250_fpga_close(mpu9250_bus_t *bus)
{
    if(!bus)return;
#ifndef _WIN32
    fpga_bus_t *b=bus->context;
    if(b){(void)command(b,0,0);close(b->fd);close(b->lock_fd);free(b);}
#endif
    memset(bus,0,sizeof(*bus));
}
