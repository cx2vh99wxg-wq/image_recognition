#define ioctl control_test_ioctl
#include "csrc/fspi_module.c"
#undef ioctl
#include <assert.h>
#include <stdarg.h>
static uint8_t fixture[8];static int result=8,io_error=0;
int control_test_ioctl(int fd,unsigned long request,...){
 (void)fd;
 if(request==SPI_IOC_WR_MODE32||request==SPI_IOC_WR_BITS_PER_WORD||request==SPI_IOC_WR_MAX_SPEED_HZ)return 0;
 assert(request==SPI_IOC_MESSAGE(1));va_list a;va_start(a,request);struct spi_ioc_transfer* t=va_arg(a,void*);va_end(a);
 assert(!t->tx_buf&&t->rx_buf&&t->rx_nbits==4&&t->speed_hz==24000000&&t->len==8);
 memcpy((void*)(uintptr_t)t->rx_buf,fixture,8);
 if(io_error){errno=io_error;return -1;}return result;
}
static void checksum(void){fixture[3]=speed_crc8(fixture[2]);fixture[7]=control_crc_byte(control_crc_byte(control_crc_byte(0,fixture[4]),fixture[5]),fixture[6]);}
int main(void){
 g_spi_fd=7;g_config.spi_mode=SPI_CPOL|SPI_CPHA;g_config.speed_hz=24000000;g_config.bits_per_word=8;g_config.transfer_size=3;
 fixture[0]=0xaa;fixture[1]=0xa7;fixture[2]=0xc2;fixture[4]=0xd7;
 unsigned contacts[]={0xff,0xfe,0xfd,0xfb,0xf7,0xef,0x40,0x80},gates[]={0xc0,0xc2,0xc4,0xc8,0xd0,0xff};
 for(unsigned i=0;i<sizeof(contacts)/sizeof(*contacts);i++)for(unsigned j=0;j<sizeof(gates)/sizeof(*gates);j++){
  fixture[5]=(uint8_t)contacts[i];fixture[6]=(uint8_t)gates[j];checksum();ControlSnapshot s={0};
  assert(!fspi_read_control_snapshot(&s)&&s.hmi==0xaa&&s.speed.gear==2);
  assert(s.contacts==contacts[i]&&s.gates==gates[j]&&g_config.transfer_size==3);
 }
 ControlSnapshot s={.hmi=0x55};fixture[7]^=1;
 assert(fspi_read_control_snapshot(&s)==-1&&errno==EPROTO&&s.hmi==0x55);
 memset(fixture+4,0xff,4);assert(fspi_read_control_snapshot(&s)==-1&&errno==EPROTO&&s.hmi==0x55);
 result=7;assert(fspi_read_control_snapshot(&s)==-1&&errno==EIO);result=8;
 io_error=ETIMEDOUT;assert(fspi_read_control_snapshot(&s)==-1&&errno==ETIMEDOUT);io_error=0;
 assert(fspi_read_control_snapshot(0)==-1&&errno==EINVAL);
 puts("CONTROL diagnostic: native RX-only, contact/gate masks, two CRCs, old FPGA rejection, short/error transfers and no writes PASS");return 0;
}
