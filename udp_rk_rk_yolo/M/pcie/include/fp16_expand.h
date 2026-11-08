#ifndef M_FP16_EXPAND_H
#define M_FP16_EXPAND_H
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#if defined(__aarch64__) && defined(__linux__)
#include <arm_neon.h>
#include <sys/auxv.h>
#include <asm/hwcap.h>
#endif
static inline float fp16_expand_scalar(uint16_t h)
{
    uint32_t bits=(uint32_t)(h&0x8000u)<<16;
    unsigned exp=(h>>10)&31u,mant=h&1023u;
    if(exp==31)bits|=0x7f800000u|(mant<<13);
    else if(exp)bits|=((exp+112u)<<23)|(mant<<13);
    else if(mant){int power=-14;while(!(mant&1024u)){mant<<=1;--power;}
        bits|=((uint32_t)(power+127)<<23)|((mant&1023u)<<13);}
    float f;memcpy(&f,&bits,sizeof(f));return f;
}
#if defined(__aarch64__) && defined(__linux__)
__attribute__((target("arch=armv8.2-a+fp16")))
static void fp16_expand_neon(const uint16_t *src,float *dst,size_t n)
{
    uint64_t fpcr;__asm__ volatile("mrs %0, fpcr":"=r"(fpcr));
    // A library may have enabled FZ16; preserve subnormal output values and
    // restore the caller's per-thread FP environment after conversion.
    const uint64_t exact=fpcr&~(1ULL<<19);
    __asm__ volatile("msr fpcr, %0"::"r"(exact):"memory");
    size_t i=0;
    for(;i+8<=n;i+=8){uint16x8_t bits=vld1q_u16(src+i);
        vst1q_f32(dst+i,vcvt_f32_f16(vreinterpret_f16_u16(vget_low_u16(bits))));
        vst1q_f32(dst+i+4,vcvt_f32_f16(vreinterpret_f16_u16(vget_high_u16(bits))));}
    for(;i<n;++i)dst[i]=fp16_expand_scalar(src[i]);
    __asm__ volatile("msr fpcr, %0"::"r"(fpcr):"memory");
}
#endif
static inline void fp16_expand(const uint16_t *src,float *dst,size_t n)
{
#if defined(__aarch64__) && defined(__linux__)
    const unsigned long caps=getauxval(AT_HWCAP);
    if((caps&HWCAP_ASIMDHP)&&(caps&HWCAP_FPHP)){fp16_expand_neon(src,dst,n);return;}
#endif
    for(size_t i=0;i<n;++i)dst[i]=fp16_expand_scalar(src[i]);
}
#endif
