#define ioctl speed_test_ioctl
#include "csrc/fspi_module.c"
#undef ioctl
#include <assert.h>
#include <stdarg.h>
static uint8_t fixture[4];static int result=4,io_error=0;
int speed_test_ioctl(int fd,unsigned long request,...) {
 (void)fd;
 if(request==SPI_IOC_WR_MODE32||request==SPI_IOC_WR_BITS_PER_WORD||request==SPI_IOC_WR_MAX_SPEED_HZ)return 0;
 assert(request==SPI_IOC_MESSAGE(1));va_list a;va_start(a,request);struct spi_ioc_transfer* t=va_arg(a,void*);va_end(a);
 assert(t->tx_buf==0&&t->rx_buf&&t->rx_nbits==4&&t->speed_hz==24000000);
 assert(t->len==4);memcpy((void*)(uintptr_t)t->rx_buf,fixture,4);
 if(io_error){errno=io_error;return -1;}return result;
}
int main(void) {
 g_spi_fd=7;g_config.spi_mode=SPI_CPOL|SPI_CPHA;g_config.speed_hz=24000000;g_config.bits_per_word=8;g_config.transfer_size=3;
 for(unsigned gear=1;gear<=3;++gear)for(unsigned bits=0;bits<4;++bits){
  fixture[0]=0xdd;fixture[1]=0xa7;fixture[2]=(uint8_t)(0xc0|gear|(bits<<4));fixture[3]=speed_crc8(fixture[2]);
  SpeedSnapshot s={0};assert(fspi_read_speed_snapshot(&s,0)==0&&s.gear==gear&&s.known);
  assert(s.pressed==((bits>>1)&1)&&s.driving==(bits&1)&&g_config.transfer_size==3);
 }
 SpeedSnapshot s={99,0,0,0};fixture[3]^=1;errno=0;assert(fspi_read_speed_snapshot(&s,0)==-1&&errno==EPROTO&&s.gear==99);
 memset(fixture,0xcc,4);assert(fspi_read_speed_snapshot(&s,0)==-1&&errno==EPROTO);
 result=3;assert(fspi_read_speed_snapshot(&s,0)==-1&&errno==EIO);result=4;
 io_error=ETIMEDOUT;assert(fspi_read_speed_snapshot(&s,0)==-1&&errno==ETIMEDOUT);io_error=0;
 fixture[0]=0xaa;fixture[1]=0xa7;fixture[2]=0x81;fixture[3]=speed_crc8(fixture[2]);
 assert(!fspi_read_speed_snapshot(&s,0)&&!s.known); // released-baseline qualification pending
 fixture[2]=0xc0;fixture[3]=speed_crc8(fixture[2]);assert(fspi_read_speed_snapshot(&s,0)==-1);
 fixture[2]=0xc4;fixture[3]=speed_crc8(fixture[2]);assert(fspi_read_speed_snapshot(&s,0)==-1);
 fixture[0]=0xab;fixture[2]=0xc2;fixture[3]=speed_crc8(fixture[2]);assert(fspi_read_speed_snapshot(&s,0)==-1);
 SpeedWarningSegment segment={0};SpeedWarningIPC p={0,&segment};SpeedWarningData d={0},o={0};
 d.gear=2;d.known=1;d.link_ok=1;speed_set_deadline(&d,3000000000ULL);speed_ipc_write(&p,&d);
 assert(!speed_ipc_read(&p,&o)&&o.gear==2&&speed_fresh(&o,1000000000ULL));
 assert(!speed_fresh(&o,3000000000ULL));o.link_ok=0;assert(!speed_fresh(&o,1000000000ULL));
 segment.sequence|=1;assert(speed_ipc_read(&p,&o));
 puts("SPEED transport: native RX-only, all gear/flags, CRC, old FPGA, short/I/O errors, no writes, IPC expiry PASS");return 0;
}
