#include "fpga_imu_snapshot.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void store32(uint8_t *p,uint32_t n){p[0]=(uint8_t)(n>>24);p[1]=(uint8_t)(n>>16);p[2]=(uint8_t)(n>>8);p[3]=(uint8_t)n;}
static void sign_crc(uint8_t *p){store32(p+60,imu_crc32(p,60));}
int main(void){
    assert(imu_crc32((const uint8_t*)"123456789",9)==0xcbf43926u);
    uint8_t p[64]={0};memcpy(p,"IMU1",4);p[4]=1;p[5]=64;p[6]=7;p[8]=0x71;p[9]=0x68;p[10]=100;
    store32(p+12,0xffffffffu);store32(p+20,1000);store32(p+28,1200);
    p[32]=0x80;p[33]=0;p[34]=0x7f;p[35]=0xff;p[38]=0xff;p[39]=0xff;
    p[44]=0xff;p[45]=0x7d;sign_crc(p);
    fpga_imu_snapshot_t a={0},saved;
    assert(fpga_imu_decode(&a,p,64)==STEER_OK);
    assert(a.accel[0]==-32768&&a.accel[1]==32767&&a.temperature==-1&&a.gyro[2]==-131);
    assert(a.sequence==UINT32_MAX&&a.sample_us==1000&&a.now_us==1200);
    saved=a;p[35]^=1;
    assert(fpga_imu_decode(&a,p,64)==STEER_ERR_IO&&!memcmp(&a,&saved,sizeof(a)));p[35]^=1;
    p[6]=8;sign_crc(p);assert(fpga_imu_decode(&a,p,64)==STEER_ERR_DEVICE);p[6]=7;
    store32(p+20,1300);sign_crc(p);assert(fpga_imu_decode(&a,p,64)==STEER_ERR_STALE);
    memset(p,0xff,64);assert(fpga_imu_decode(&a,p,64)==STEER_ERR_NOT_READY);
    assert(fpga_imu_decode(&a,p,63)==STEER_ERR_NOT_READY);
    puts("PASS IMU1: CRC vector, signed axes/temperature, endianness, torn packet, flags, clock reversal and old FPGA rejection");
    return 0;
}
