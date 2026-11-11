#ifndef SIX_CAMERA_RENDER_H
#define SIX_CAMERA_RENDER_H

#include "../../../include/camera_views.h"
#include <algorithm>
#include <stdint.h>
#if defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h>
#endif

namespace six_camera
{
constexpr int width = 1280, height = 480, tile_width = 320, tile_height = 240;

static inline uint32_t argb(uint16_t pixel)
{
    return 0xff000000u | ((pixel & 31u) << 19) | (((pixel >> 5) & 63u) << 10) |
           (((pixel >> 11) & 31u) << 3);
}

#if defined(__ARM_NEON) || defined(__aarch64__)
static inline uint32x4_t argb4(uint16x4_t pixels)
{
    const uint32x4_t p=vmovl_u16(pixels);
    return vorrq_u32(vdupq_n_u32(0xff000000u),
        vorrq_u32(vshlq_n_u32(vandq_u32(p,vdupq_n_u32(31)),19),
        vorrq_u32(vshlq_n_u32(vandq_u32(p,vdupq_n_u32(0x7e0)),5),
                  vshrq_n_u32(vandq_u32(p,vdupq_n_u32(0xf800)),8))));
}
#endif

// Preserve the existing BGR565 expansion exactly, including its low zero bits.
static inline void argb_row(uint32_t *destination,const uint16_t *source,int count)
{
    int x=0;
#if defined(__ARM_NEON) || defined(__aarch64__)
    for(;x+8<=count;x+=8){
        const auto p=vld1q_u16(source+x);
        vst1q_u32(destination+x,argb4(vget_low_u16(p)));
        vst1q_u32(destination+x+4,argb4(vget_high_u16(p)));
    }
#endif
    for(;x<count;++x)destination[x]=argb(source[x]);
}

static inline void argb_double_row(uint32_t *destination,const uint16_t *source,int count)
{
    int x=0;
#if defined(__ARM_NEON) || defined(__aarch64__)
    for(;x+8<=count;x+=8){
        const auto p=vld1q_u16(source+x);
        const auto a=argb4(vget_low_u16(p)),b=argb4(vget_high_u16(p));
        const auto aa=vzipq_u32(a,a),bb=vzipq_u32(b,b);
        vst1q_u32(destination+2*x,aa.val[0]);vst1q_u32(destination+2*x+4,aa.val[1]);
        vst1q_u32(destination+2*x+8,bb.val[0]);vst1q_u32(destination+2*x+12,bb.val[1]);
    }
#endif
    for(;x<count;++x)destination[2*x]=destination[2*x+1]=argb(source[x]);
}

static inline const uint16_t *source_row(const CameraView &view, const uint16_t *pcie,
                                         const uint16_t *udp, int y)
{
    const int row = view.network_source ? y : tile_height - 1 - y;
    return (view.network_source ? udp : pcie) + (view.source_y + row) * 640 + view.source_x;
}

// Fuse BGR565 expansion and the dashboard's exact 320x240 -> 364x273
// nearest-neighbor mapping. The former 1280x480 ARGB intermediate is not
// needed in steady overview. Source reads stay within each physical tile.
static inline void dashboard_overview(uint32_t *pixels,int stride,int left,int top,
                                     const uint16_t *pcie,const uint16_t *udp,
                                     bool refresh_pcie=true,bool refresh_udp=true)
{
    struct Mapping {
        int x[364],base[45];uint8_t index[45][16];
        Mapping(){
            for(int i=0;i<364;++i)x[i]=i*320/364;
            for(int g=0;g<45;++g){
                base[g]=std::min(x[g*8],304);
                for(int j=0;j<8;++j){
                    index[g][j*2]=uint8_t((x[g*8+j]-base[g])*2);
                    index[g][j*2+1]=index[g][j*2]+1;
                }
            }
        }
    };
    static const Mapping map;
    for(const auto &view:camera_views){
        if (view.network_source ? !refresh_udp : !refresh_pcie)
            continue;
        uint32_t *tile=pixels+(top+view.row*273)*stride+left+view.column*364;
        int previous=-1;
        for(int y=0;y<273;++y){
            uint32_t *row=tile+y*stride;const int sy=y*240/273;
            if(sy==previous){std::copy_n(row-stride,364,row);continue;}
            previous=sy;const uint16_t *source=source_row(view,pcie,udp,sy);int x=0;
#if defined(__aarch64__)
            for(int g=0;g<45;++g,x+=8){
                uint8x16x2_t table;
                table.val[0]=vld1q_u8(reinterpret_cast<const uint8_t*>(source+map.base[g]));
                table.val[1]=vld1q_u8(reinterpret_cast<const uint8_t*>(source+map.base[g]+8));
                const auto p=vreinterpretq_u16_u8(vqtbl2q_u8(table,vld1q_u8(map.index[g])));
                vst1q_u32(row+x,argb4(vget_low_u16(p)));
                vst1q_u32(row+x+4,argb4(vget_high_u16(p)));
            }
#endif
            for(;x<364;++x)row[x]=argb(source[map.x[x]]);
        }
    }
}

static inline void overview(uint32_t *pixels, const uint16_t *pcie, const uint16_t *udp,
                            const CameraView *target = nullptr, float dim = 1.f)
{
    // The six views overwrite the first three columns. Clear only the spare
    // fourth column instead of writing the entire canvas twice every frame.
    for(int y=0;y<height;++y)std::fill_n(pixels+y*width+3*tile_width,tile_width,0xff000000u);
    for (const CameraView &view : camera_views)
    {
        const bool darken = target && target->mode != view.mode;
        for (int y = 0; y < tile_height; ++y)
        {
            const uint16_t *source = source_row(view, pcie, udp, y);
            uint32_t *row =
                pixels + (view.row * tile_height + y) * width + view.column * tile_width;
            if(!darken){argb_row(row,source,tile_width);continue;}
            for (int x = 0; x < tile_width; ++x)
            {
                const uint32_t color = argb(source[x]);
                if (!darken)
                    row[x] = color;
                else
                    row[x] = 0xff000000u | (uint32_t(((color >> 16) & 255) * dim) << 16) |
                             (uint32_t(((color >> 8) & 255) * dim) << 8) |
                             uint32_t((color & 255) * dim);
            }
        }
    }
}

static inline void focus(uint32_t *pixels, const uint16_t *pcie, const uint16_t *udp,
                         const CameraView &view)
{
    for (int y = 0; y < height; ++y)
    {
        std::fill_n(pixels+y*width,320,0xff000000u);
        std::fill_n(pixels+y*width+960,320,0xff000000u);
        uint32_t *row=pixels+y*width+320;
        if(y&1)std::copy_n(row-width,640,row);
        else argb_double_row(row,source_row(view,pcie,udp,y/2),tile_width);
    }
}
}
#endif
