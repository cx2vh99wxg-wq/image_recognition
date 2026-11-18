#ifndef M_HDMI_SIGNAL_DETAIL_H
#define M_HDMI_SIGNAL_DETAIL_H
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <vector>

namespace hdmi_front {
struct YuvMatrix {
    bool full;
    int rv, gu, gv, bu;
    YuvMatrix(bool full_range, bool bt709) : full(full_range),
        rv(bt709 ? (full ? 403 : 459) : (full ? 359 : 409)),
        gu(bt709 ? (full ? 48 : 55) : (full ? 88 : 100)),
        gv(bt709 ? (full ? 120 : 136) : (full ? 183 : 208)),
        bu(bt709 ? (full ? 475 : 541) : (full ? 454 : 516)) {}
    static uint8_t channel(int value) {
        return uint8_t(std::max(0,std::min(255,(value+128)>>8)));
    }
    void decode(const uint8_t *pair,int odd,uint8_t &r,uint8_t &g,uint8_t &b) const {
        const int y=full ? pair[2*odd]*256 : (int(pair[2*odd])-16)*298;
        const int u=int(pair[1])-128, v=int(pair[3])-128;
        r=channel(y+rv*v); g=channel(y-gu*u-gv*v); b=channel(y+bu*u);
    }
};

// Native signal detail is independent of the 320x240 lane/object image.
// Only upper rows are copied before the V4L2 buffer is queued again.
struct SignalDetail {
    static constexpr int WIDTH=640, HEIGHT=480, ROWS=HEIGHT*63/100;
    std::vector<uint8_t> yuyv;
    bool full=false, bt709=false;
    void capture(const uint8_t *src,unsigned w,unsigned h,unsigned stride,bool range,bool matrix) {
        if(w!=WIDTH || h!=HEIGHT){ yuyv.clear(); return; }
        full=range;bt709=matrix;
        yuyv.resize(WIDTH*ROWS*2);
        if(stride==WIDTH*2) std::memcpy(yuyv.data(),src,yuyv.size());
        else for(int y=0;y<ROWS;++y) std::memcpy(yuyv.data()+y*WIDTH*2,src+y*stride,WIDTH*2);
    }
    void swap(SignalDetail &other) {
        yuyv.swap(other.yuyv); std::swap(full,other.full); std::swap(bt709,other.bt709);
    }
    bool rgb(std::vector<uint8_t> &out) const {
        if(yuyv.size()!=WIDTH*ROWS*2) return false;
        const YuvMatrix matrix(full,bt709);
        out.resize(WIDTH*ROWS*3);
        for(int i=0;i<WIDTH*ROWS;i+=2) {
            const auto *p=yuyv.data()+i*2;
            auto *v=out.data()+i*3;
            matrix.decode(p,0,v[0],v[1],v[2]); matrix.decode(p,1,v[3],v[4],v[5]);
        }
        return true;
    }
};
}
#endif
