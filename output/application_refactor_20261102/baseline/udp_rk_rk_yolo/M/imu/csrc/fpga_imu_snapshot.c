#include "fpga_imu_snapshot.h"
#include <string.h>
uint32_t imu_crc32(const uint8_t *data, size_t size) {
    uint32_t c=UINT32_MAX;
    for(size_t i=0;i<size;++i) {
        c^=data[i];
        for(unsigned k=0;k<8;++k)c=(c>>1)^((c&1)?UINT32_C(0xedb88320):0);
    }
    return c^UINT32_MAX;
}
static uint32_t u32(const uint8_t *p) {
    return (uint32_t)p[0]<<24 | (uint32_t)p[1]<<16 | (uint32_t)p[2]<<8 | p[3];
}
static uint64_t u64(const uint8_t *p) { return (uint64_t)u32(p)<<32 | u32(p+4); }
static int16_t s16(const uint8_t *p) {
    uint16_t u=(uint16_t)((uint16_t)p[0]<<8 | p[1]);
    return (int16_t)(u<32768? (int32_t)u : (int32_t)u-65536);
}
steer_result_t fpga_imu_decode(fpga_imu_snapshot_t *out,const uint8_t *p,size_t n) {
    if(!out||!p)return STEER_ERR_ARGUMENT;
    if(n!=64||memcmp(p,"IMU1",4)||p[4]!=1||p[5]!=64)return STEER_ERR_NOT_READY;
    if(imu_crc32(p,60)!=u32(p+60))return STEER_ERR_IO;
    if((p[6]&~7u)||p[10]!=100||p[11]||p[46]||p[47]||u32(p+56))return STEER_ERR_DEVICE;
    fpga_imu_snapshot_t s={0};
    s.flags=p[6];s.error=p[7];s.identity=p[8];s.address=p[9];s.sequence=u32(p+12);
    s.sample_us=u64(p+16);s.now_us=u64(p+24);s.errors=u32(p+48);s.generation=u32(p+52);
    for(unsigned k=0;k<3;++k){s.accel[k]=s16(p+32+2*k);s.gyro[k]=s16(p+40+2*k);}
    s.temperature=s16(p+38);
    if(s.sample_us>s.now_us)return STEER_ERR_STALE;
    *out=s;return STEER_OK;
}
