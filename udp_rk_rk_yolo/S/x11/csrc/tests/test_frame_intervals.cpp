#include "../../../../common/performance/frame_intervals.h"
#include <cassert>
#include <cstdio>
int main(){
    performance::FrameIntervals stats;uint64_t now=1;stats.observe(now);
    for(uint64_t i=1;i<=100;++i){now+=i*1000000ULL;stats.observe(now);}
    auto s=stats.take();assert(s.p50==50&&s.p95==95&&s.p99==99&&s.worst==100);
    assert(stats.take().worst==0);
    for(uint64_t i=1;i<=300;++i){now+=i*1000000ULL;stats.observe(now);}
    s=stats.take();assert(s.p50>=170&&s.p95>=285&&s.p99>=295&&s.worst==300);
    now+=1000000ULL;stats.observe(now);s=stats.take();assert(s.p50==1&&s.worst==1);
    puts("FRAME INTERVAL PASS: percentiles, bounded wrap, window reset, continuous timestamps");
}
