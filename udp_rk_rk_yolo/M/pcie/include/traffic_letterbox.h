#ifndef TRAFFIC_LETTERBOX_H
#define TRAFFIC_LETTERBOX_H
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <vector>
namespace traffic {
struct Letterbox {
    float scale;
    int width, height, x, y;
};
// Same center-sampled bilinear convention as the existing detector. At exact
// unit scale the weights are integers, so row copy is byte-for-byte identical.
inline Letterbox prepare_letterbox(const uint8_t* rgb,int w,int h,int stride,
                                  int dw,int dh,float view,std::vector<uint8_t>& out,
                                  bool unit_copy=true) {
    Letterbox b;
    b.scale=std::min(float(dw)/w,float(dh)/h)*view;
    b.width=std::max(1,(int)std::lround(w*b.scale));
    b.height=std::max(1,(int)std::lround(h*b.scale));
    b.x=(dw-b.width)/2;b.y=(dh-b.height)/2;
    out.resize((size_t)dw*dh*3);std::fill(out.begin(),out.end(),114);
    if(unit_copy && b.scale==1.f && b.width==w && b.height==h) {
        for(int y=0;y<h;++y)std::memcpy(out.data()+((y+b.y)*dw+b.x)*3,rgb+y*stride,(size_t)w*3);
        return b;
    }
    // Exact 2x center-sampled bilinear: weights are 0, 1/4 or 3/4,
    // hence integer /16 + half-up rounding equals the float reference.
    if(unit_copy && b.scale==2.f && b.width==2*w && b.height==2*h) {
        for(int y=0;y<b.height;++y) {
            int y0=y?((y-1)/2):0,wy=y?(y%2?1:3):0;
            const uint8_t *a=rgb+y0*stride,*z=rgb+std::min(h-1,y0+1)*stride;
            uint8_t *dest=out.data()+((y+b.y)*dw+b.x)*3;
            for(int x=0;x<b.width;++x) {
                int x0=x?((x-1)/2):0,wx=x?(x%2?1:3):0,x1=std::min(w-1,x0+1);
                for(int c=0;c<3;++c) {
                    int top=a[x0*3+c]*(4-wx)+a[x1*3+c]*wx;
                    int bot=z[x0*3+c]*(4-wx)+z[x1*3+c]*wx;
                    dest[x*3+c]=(uint8_t)((top*(4-wy)+bot*wy+8)/16);
                }
            }
        }
        return b;
    }
    // The tight signal view is 128 -> 320. Cache each horizontally filtered
    // source row once, retaining the reference float operations and rounding.
    // Adjacent output rows reuse the same two source rows; no new pixel values
    // or quantized interpolation weights are introduced.
    if(unit_copy && b.scale==2.5f) {
        struct Column{int x0,x1;float ax;};std::vector<Column> cols(b.width);
        for(int x=0;x<b.width;++x){float fx=std::max(0.f,(x+.5f)/b.scale-.5f);int x0=std::min(w-1,(int)fx);cols[x]={x0,std::min(w-1,x0+1),fx-x0};}
        std::vector<float> rows((size_t)b.width*3*2);int ids[2]={-1,-1};
        const auto row=[&](int y){unsigned slot=y%2;float*dst=rows.data()+slot*b.width*3;
            if(ids[slot]!=y){const uint8_t*src=rgb+y*stride;for(int x=0;x<b.width;++x){const auto&s=cols[x];for(int c=0;c<3;++c)dst[x*3+c]=src[s.x0*3+c]*(1-s.ax)+src[s.x1*3+c]*s.ax;}ids[slot]=y;}
            return dst;};
        for(int y=0;y<b.height;++y){float fy=std::max(0.f,(y+.5f)/b.scale-.5f);int y0=std::min(h-1,(int)fy),y1=std::min(h-1,y0+1);float ay=fy-y0;
            const float*a=row(y0),*z=row(y1);uint8_t*dst=out.data()+((y+b.y)*dw+b.x)*3;
            for(int i=0;i<b.width*3;++i)dst[i]=(uint8_t)std::lround(a[i]*(1-ay)+z[i]*ay);
        }
        return b;
    }
    for(int y=0;y<b.height;++y) {
        float fy=std::max(0.f,(y+.5f)/b.scale-.5f);
        int y0=std::min(h-1,(int)fy),y1=std::min(h-1,y0+1);float ay=fy-y0;
        for(int x=0;x<b.width;++x) {
            float fx=std::max(0.f,(x+.5f)/b.scale-.5f);
            int x0=std::min(w-1,(int)fx),x1=std::min(w-1,x0+1);float ax=fx-x0;
            for(int c=0;c<3;++c) {
                float top=rgb[y0*stride+x0*3+c]*(1-ax)+rgb[y0*stride+x1*3+c]*ax;
                float bot=rgb[y1*stride+x0*3+c]*(1-ax)+rgb[y1*stride+x1*3+c]*ax;
                out[((y+b.y)*dw+x+b.x)*3+c]=(uint8_t)std::lround(top*(1-ay)+bot*ay);
            }
        }
    }
    return b;
}
}
#endif
