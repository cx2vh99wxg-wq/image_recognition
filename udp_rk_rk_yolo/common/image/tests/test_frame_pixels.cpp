#include "../frame_pixels.h"
#ifdef TEST_S_ORIENTATION
#include "../../../S/pcie/include/camera_orientation.h"
#endif
#include <algorithm>
#include <cassert>
#include <chrono>
#include <vector>

#if defined(__GNUC__)
#define SCALAR __attribute__((noinline, optimize("no-tree-vectorize")))
#else
#define SCALAR
#endif
static SCALAR void scalar_unpack(const uint16_t *src, uint8_t *dst, size_t count)
{
    for (size_t i=0;i<count;++i) {
        unsigned p=src[i], r=p>>11, g=(p>>5)&63, b=p&31;
        dst[i*3]=(r<<3)|(r>>2); dst[i*3+1]=(g<<2)|(g>>4); dst[i*3+2]=(b<<3)|(b>>2);
    }
}
static SCALAR void scalar_pack(const uint8_t *src, uint16_t *dst, size_t count)
{
    for (size_t i=0;i<count;++i)
        dst[i]=((src[i*3]&248)<<8)|((src[i*3+1]&252)<<3)|(src[i*3+2]>>3);
}
template<class F> static double bench(F operation)
{
    const auto start=std::chrono::steady_clock::now();
    for(int i=0;i<60;++i) operation();
    return std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count()/60;
}
#ifdef TEST_S_ORIENTATION
static SCALAR void scalar_rotate(uint16_t *pixels)
{
    for(int ty=0;ty<480;ty+=240) for(int tx=0;tx<640;tx+=320)
        for(int y=0;y<120;++y) for(int x=0;x<320;++x)
            std::swap(pixels[(ty+y)*640+tx+x],pixels[(ty+239-y)*640+tx+319-x]);
}
#endif
int main()
{
    const size_t n=65536;
    std::vector<uint16_t> source(n+2), packed(n+2,0), expected(n+2,0);
    std::vector<uint8_t> rgb(n*3+2,0xA5), reference(n*3+2,0xA5);
    for(size_t i=0;i<n;++i) source[i+1]=uint16_t(i);
    frame_unpack_rgb565(source.data()+1,rgb.data()+1,n);
    scalar_unpack(source.data()+1,reference.data()+1,n);
    assert(rgb==reference);
    frame_pack_rgb565(rgb.data()+1,packed.data()+1,n);
    assert(std::equal(source.begin()+1,source.end()-1,packed.begin()+1));
    for(size_t i=0;i<rgb.size();++i) rgb[i]=uint8_t(i*37+i/257);
    frame_pack_rgb565(rgb.data()+1,packed.data()+1,n);
    scalar_pack(rgb.data()+1,expected.data()+1,n);
    assert(packed==expected);
    for(size_t count=0;count<32;++count) {
        std::fill(rgb.begin(),rgb.end(),0xA5); reference=rgb;
        frame_unpack_rgb565(source.data()+1,rgb.data()+1,count);
        scalar_unpack(source.data()+1,reference.data()+1,count);
        assert(rgb==reference);
        std::fill(packed.begin(),packed.end(),0xA5A5); expected=packed;
        frame_pack_rgb565(rgb.data()+1,packed.data()+1,count);
        scalar_pack(rgb.data()+1,expected.data()+1,count);
        assert(packed==expected);
    }
    std::vector<uint16_t> frame(640*480);
    std::vector<uint8_t> colors(frame.size()*3);
    for(size_t i=0;i<frame.size();++i) frame[i]=uint16_t(i*73);
    double old_unpack=bench([&]{scalar_unpack(frame.data(),colors.data(),frame.size());});
    double new_unpack=bench([&]{frame_unpack_rgb565(frame.data(),colors.data(),frame.size());});
    double old_pack=bench([&]{scalar_pack(colors.data(),frame.data(),frame.size());});
    double new_pack=bench([&]{frame_pack_rgb565(colors.data(),frame.data(),frame.size());});
#ifdef TEST_S_ORIENTATION
    auto rotated=frame, correct=frame;
    scalar_rotate(correct.data());
    rotate_rgb565_tiles_180(rotated.data(),640,480,320,240);
    assert(rotated==correct);
    std::vector<uint8_t> fused(colors.size()), separate(colors.size());
    unpack_rgb565_tiles_180(frame.data(),fused.data(),640,480,320,240);
    scalar_unpack(correct.data(),separate.data(),frame.size());
    assert(fused==separate);
    double fused_ms=bench([&]{unpack_rgb565_tiles_180(frame.data(),fused.data(),640,480,320,240);});
    double old_rotate=bench([&]{scalar_rotate(correct.data());});
    double new_rotate=bench([&]{rotate_rgb565_tiles_180(rotated.data(),640,480,320,240);});
    printf("ROTATION PASS scalar_ms=%.3f neon_ms=%.3f fused_unpack_ms=%.3f\n",old_rotate,new_rotate,fused_ms);
#endif
    printf("PIXELS PASS exhaustive=65536 tails=0..31 unpack_ms=%.3f/%.3f pack_ms=%.3f/%.3f neon=%d\n",old_unpack,new_unpack,old_pack,new_pack,
#ifdef FRAME_PIXELS_NEON
        1
#else
        0
#endif
    );
}
