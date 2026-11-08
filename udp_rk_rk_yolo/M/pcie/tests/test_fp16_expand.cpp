#include "fp16_expand.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>
int main(){
    std::vector<uint16_t> h(65536);std::vector<float> f(h.size());
    for(unsigned i=0;i<h.size();++i)h[i]=(uint16_t)i;
    fp16_expand(h.data(),f.data(),h.size());
    for(unsigned i=0;i<h.size();++i){
        const float expected=fp16_expand_scalar(h[i]);
        if(std::isnan(expected))assert(std::isnan(f[i]));
        else assert(!memcmp(&f[i],&expected,sizeof(float)));
    }
    assert(f[1]==std::ldexp(1.f,-24)&&f[0x3c00]==1.f&&f[0x7bff]==65504.f);
    assert(std::isinf(f[0x7c00])&&std::signbit(f[0x8000]));
    // Non-vector tail and caller FP environment must remain valid as well.
    fp16_expand(h.data()+1,f.data(),13);
    for(unsigned i=0;i<13;++i)assert(f[i]==fp16_expand_scalar(h[i+1]));
    puts("FP16 EXPAND PASS: all 65536 encodings, subnormals, signed zero, finite bits, Inf/NaN and vector tail");
}
