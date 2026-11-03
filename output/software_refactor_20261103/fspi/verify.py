from pathlib import Path
import re,subprocess,hashlib
w=Path(__file__).resolve().parent
root=w.parents[2]/'udp_rk_rk_yolo'
common=root/'common/transport/include'
prefix=r'''
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <linux/spi/spidev.h>
static int mock_ioctl(int fd,unsigned long request,void *argument);
static int mock_usleep(unsigned int delay);
#define ioctl mock_ioctl
#define usleep mock_usleep
'''
fixture=r'''
static int call, fail_call, short_io, sample;
static int mock_usleep(unsigned int delay) {
    printf("delay %u\n",delay);
    return 0;
}
static int mock_ioctl(int fd,unsigned long request,void *argument) {
    assert(fd==44);
    ++call;
    printf("ioctl %lu",request);
    if(request==SPI_IOC_WR_MODE32 || request==SPI_IOC_WR_MAX_SPEED_HZ)
        printf(" %u",*(uint32_t*)argument);
    if(request==SPI_IOC_WR_BITS_PER_WORD)
        printf(" %u",*(uint8_t*)argument);
    int result=0;
    if(request==SPI_IOC_MESSAGE(1)) {
        struct spi_ioc_transfer *tr=argument;
        printf(" len=%u speed=%u delay=%u bits=%u width=%u/%u",
               tr->len,tr->speed_hz,tr->delay_usecs,tr->bits_per_word,tr->tx_nbits,tr->rx_nbits);
        if(tr->tx_buf) {
            const uint8_t *tx=(const uint8_t*)(uintptr_t)tr->tx_buf;
            for(unsigned i=0;i<tr->len;++i) printf(" %02x",tx[i]);
        }
        if(tr->rx_buf && call!=fail_call) {
            uint8_t *rx=(uint8_t*)(uintptr_t)tr->rx_buf;
            *rx=sample++%2 ? 0xA0 : 0x10;
        }
        result=tr->len-(short_io ? 1 : 0);
    }
    putchar('\n');
    if(call==fail_call) { errno=EIO; return -1; }
    return result;
}
int main(void) {
    for(int mode=0;mode<4;++mode)
      for(int failure=0;failure<=9;++failure)
        for(short_io=0;short_io<=1;++short_io)
          for(int op=0;op<4;++op) {
            g_config.speed_hz=SPI_SPEED;
            g_config.bits_per_word=8;
            g_config.transfer_size=SPI_TRANSFER_SIZE;
            g_config.spi_mode=SPI_CPHA|SPI_CPOL;
            g_config.protocol_mode=(SpiTransferMode)mode;
            call=sample=0;
            fail_call=failure;
            uint8_t data=0xcc;
            errno=0;
            int rc;
            if(op==0) rc=spi_write_reliable(44,0x0203,0x31);
            if(op==1) rc=spi_read_reliable(44,0x0203,&data);
            if(op==2) rc=spi_read_reliable(44,0,&data);
            if(op==3) {
#ifdef DEFAULT_S
                rc=spi_read_direct_reliable(44,&data);
#else
                rc=spi_read_addr_data(44,0x0203,&data);
#endif
            }
            printf("result %d %u %u %u %d\n",rc,data,g_config.spi_mode,g_config.transfer_size,errno);
          }
}
'''
files={'M':'M/pcie/csrc/fspi_module.c','legacy':'S/pcie/csrc/fspi_module.c','S':'S/fspi/csrc/fspi_module.c'}
for role,rel in files.items():
    results=[]
    for old in (True,False):
        source=(w/(role+'.c')) if old else root/rel
        src=source.read_text(encoding='utf-8')
        defs='\n'.join(re.findall(r'^#define (?:SPI_SPEED|SPI_TRANSFER_SIZE|MAX_DEVICE_PATH|WRITE_RETRY_COUNT|READ_RETRY_COUNT) .*$',src,re.M))
        a=src.index('typedef enum' if old else '#include "../../../common')
        b=src.index('typedef struct\n{\n    double tx_rate',a)
        structs=src[a:b]
        if not old: structs='#include "fspi_register_io.h"'
        first=src.index('static int configure_spi_interface' if old else 'static int perform_transfer')
        marker='static void run_rw_test_loop(int fd)\n' if role=='M' else 'int fspi_init(void)\n'
        last=src.index(marker,first)
        body=src[first:last]
        # Linux unsigned long can store a pointer. Use uintptr_t for that same
        # width in the Windows fixture, where unsigned long is only 32 bits.
        body=body.replace('(unsigned long)tx_buf','(uintptr_t)tx_buf').replace('(unsigned long)rx_buf','(uintptr_t)rx_buf')
        code=prefix+'\n'+defs+'\n'+structs+'\nstatic SpiTestConfig g_config;\n'+body+fixture
        test=w/f'{role}_{"before" if old else "after"}.fixture.c'
        test.write_text(code,encoding='utf-8')
        exe=test.with_suffix('.exe')
        flags=['gcc','-std=gnu11','-O2','-I'+str(w/'shim'),'-I'+str(common)]
        if role=='S':flags+=['-DDEFAULT_S']
        subprocess.run(flags+[str(test),'-o',str(exe)],check=True,cwd=w)
        run=subprocess.run([str(exe)],check=True,cwd=w,capture_output=True)
        results.append((run.stdout,run.stderr))
    assert results[0]==results[1],role
    print(role,'320 equivalent FSPI scenarios',hashlib.sha256(b''.join(results[1])).hexdigest())
