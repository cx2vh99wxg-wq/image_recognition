#include "../frame_checksum.h"
#include <cassert>
#include <cstdio>
#include <chrono>
#include <vector>
static __attribute__((noinline)) uint32_t reference(const uint8_t *p,size_t n)
{
    uint32_t c=0;
    if(!p)return 0;
    while(n>=8){c=((c<<8)|(c>>24))^(uint32_t(p[0])<<7)^(uint32_t(p[1])<<6)^
        (uint32_t(p[2])<<5)^(uint32_t(p[3])<<4)^(uint32_t(p[4])<<3)^
        (uint32_t(p[5])<<2)^(uint32_t(p[6])<<1)^p[7];p+=8;n-=8;}
    while(n--)c=((c<<1)|(c>>31))^*p++;
    return c;
}
template<class F> static double bench(F f){auto t=std::chrono::steady_clock::now();
    for(int i=0;i<60;++i)f();
    return std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-t).count()/60;
}
int main(){
    std::vector<uint8_t> data(614400+64);uint32_t seed=12345;
    for(auto &b:data){seed=seed*1664525+1013904223;b=seed>>24;}
    unsigned tests=0;
    for(size_t offset=0;offset<32;++offset)for(size_t n=0;n<=128;++n){
        assert(frame_rotating_checksum(data.data()+offset,n)==reference(data.data()+offset,n));++tests;}
    for(size_t offset=0;offset<4;++offset)for(size_t n:{size_t(255),size_t(4095),size_t(4096),size_t(614399),size_t(614400)}){
        assert(frame_rotating_checksum(data.data()+offset,n)==reference(data.data()+offset,n));++tests;}
    for(size_t bit=0;bit<256;++bit){data[bit/8]^=uint8_t(1u<<(bit%8));
        assert(frame_rotating_checksum(data.data(),614400)==reference(data.data(),614400));++tests;}
    assert(frame_rotating_checksum(nullptr,55)==0);
    volatile uint32_t sink=0;
    double old_ms=bench([&]{data[0]++;sink=reference(data.data(),614400);});
    double new_ms=bench([&]{data[0]++;sink=frame_rotating_checksum(data.data(),614400);});
    printf("CHECKSUM PASS cases=%u reference_ms=%.3f folded_ms=%.3f sink=%u\n",tests,old_ms,new_ms,unsigned(sink));
}
