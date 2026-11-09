#ifndef FRAME_INTERVALS_H
#define FRAME_INTERVALS_H
#include <algorithm>
#include <cstdint>
#include <cstddef>
namespace performance {
// Bounded per-window timing. No allocation in the frame loop.
struct FrameIntervals {
    uint64_t last=0, samples[256]={}, maximum=0;
    size_t count=0;
    void observe(uint64_t now){
        if(last && now>last){uint64_t gap=now-last;samples[count++%256]=gap;maximum=std::max(maximum,gap);}
        last=now;
    }
    struct Summary {double p50=0,p95=0,p99=0,worst=0;};
    Summary take(){
        Summary s;const size_t n=std::min(count,size_t(256));
        if(n){std::sort(samples,samples+n);s.p50=samples[(n-1)*50/100]/1e6;s.p95=samples[(n-1)*95/100]/1e6;s.p99=samples[(n-1)*99/100]/1e6;s.worst=maximum/1e6;}
        count=0;maximum=0;return s;
    }
};
}
#endif
