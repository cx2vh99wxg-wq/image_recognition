#ifndef FRAME_CHECKSUM_H
#define FRAME_CHECKSUM_H
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#if defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h>
#endif

// Wire-compatible rotate-left-one/XOR checksum. After 32 input bytes the
// previous checksum has rotated a full circle, so the 32-byte contributions
// can be XOR-folded before doing the 32 scalar rotations just once.
static inline uint32_t frame_rotating_checksum(const uint8_t *data, size_t size)
{
    if (!data || !size)return 0;
    uint8_t folded[32];
#if defined(__ARM_NEON) || defined(__aarch64__)
    uint8x16_t first=vdupq_n_u8(0),second=vdupq_n_u8(0);
    while(size>=32)
    {
        first=veorq_u8(first,vld1q_u8(data));second=veorq_u8(second,vld1q_u8(data+16));
        data+=32;size-=32;
    }
    vst1q_u8(folded,first);vst1q_u8(folded+16,second);
#else
    uint64_t lanes[4]={0,0,0,0};
    while(size>=32)
    {
        uint64_t chunk[4];memcpy(chunk,data,32);
        for(int i=0;i<4;++i)lanes[i]^=chunk[i];
        data+=32;size-=32;
    }
    memcpy(folded,lanes,32);
#endif
    uint32_t checksum=0;
    for(int i=0;i<32;++i)checksum=((checksum<<1)|(checksum>>31))^folded[i];
    while(size--)checksum=((checksum<<1)|(checksum>>31))^*data++;
    return checksum;
}
#endif
