from pathlib import Path
import re, subprocess
w=Path(__file__).resolve().parent
root=w.parents[2]/'udp_rk_rk_yolo'
names=['rgb565_to_rgb888','rgb888_to_rgb565','rearrange_columns','shift_columns_up','rgb565_to_rgb888_yolo']
prefix='''
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <vector>
static bool fail;
static size_t allocated;
static void *test_malloc(size_t size) {
    allocated=size;
    return fail ? nullptr : malloc(size);
}
#define malloc test_malloc
#include "frame_pixels.h"
'''
def functions(main,yolo):
    return '\n'.join(re.search(rf'^void {name}\([^;]*?\)\n\{{.*?\n\}}',source,re.M|re.S)[0]
                     for name,source in zip(names,[main]*4+[yolo]))
test=r'''
#undef malloc
int main() {
    std::vector<uint16_t> pixels(65536);
    for (unsigned i=0;i<pixels.size();++i) pixels[i]=i;
    std::vector<uint8_t> a(pixels.size()*3),b(a.size());
    old::rgb565_to_rgb888(pixels.data(),a.data(),pixels.size());
    next::rgb565_to_rgb888(pixels.data(),b.data(),pixels.size());
    assert(a==b);
    memset(b.data(),0,b.size());
    old::rgb565_to_rgb888_yolo(pixels.data(),a.data(),256,256);
    next::rgb565_to_rgb888_yolo(pixels.data(),b.data(),256,256);
    assert(a==b);
    next::rgb565_to_rgb888_yolo(pixels.data(),b.data(),-1,5);
    assert(a==b);
    next::rgb565_to_rgb888_yolo(pixels.data(),b.data(),0,5);
    assert(a==b);

    std::vector<uint16_t> before(pixels.size()),after(before.size());
    for(unsigned r=0;r<256;++r) {
        for(unsigned gb=0;gb<65536;++gb) {
            a[gb*3]=r; a[gb*3+1]=gb>>8; a[gb*3+2]=gb;
        }
        old::rgb888_to_rgb565(a.data(),before.data(),before.size());
        next::rgb888_to_rgb565(a.data(),after.data(),after.size());
        assert(before==after);
    }

    unsigned cases=0;
    for(int width=1;width<=16;++width)
      for(int height=1;height<=11;++height)
        for(int first=-1;first<=width;++first)
          for(int last=-1;last<=width;++last)
            for(int shift=-1;shift<=height;++shift) {
                // uint16_t storage also supplies the original S path's alignment.
                uint16_t image[16*11*2], expected[16*11*2];
                for(unsigned i=0;i<sizeof(image)/sizeof(*image);++i) image[i]=(i*4591+123)&0xffff;
                memcpy(expected,image,sizeof(image));
                fail=false;
                allocated=0;
                old::shift_columns_up((uint8_t *)expected,width,height,first,last,shift);
                size_t old_size=allocated;
                allocated=0;
                next::shift_columns_up((uint8_t *)image,width,height,first,last,shift);
                assert(old_size==allocated && memcmp(image,expected,sizeof(image))==0);
                allocated=0;
                old::rearrange_columns(expected,width,first,last);
                old_size=allocated;
                allocated=0;
                next::rearrange_columns(image,width,first,last);
                assert(old_size==allocated && memcmp(image,expected,sizeof(image))==0);
                ++cases;
            }
    fail=true;
    uint16_t image[18]={1,2,3,4,5,6};
    uint16_t expected[18];
    memcpy(expected,image,sizeof image);
    old::shift_columns_up((uint8_t *)expected,3,3,0,2,1);
    next::shift_columns_up((uint8_t *)image,3,3,0,2,1);
    old::rearrange_columns(expected,3,0,1);
    next::rearrange_columns(image,3,0,1);
    assert(memcmp(expected,image,sizeof image)==0 && image[0]==1 && image[1]==2);
    printf("%u geometry cases; RGB565 and RGB888 exhaustive; allocation failure checked\n",cases);
}
'''
for board in 'MS':
    base=functions((w/f'{board}_main.cpp').read_text(encoding='utf-8'),(w/f'{board}_yolo.c').read_text(encoding='utf-8'))
    source=root/board/'pcie/csrc'
    current=functions((source/'main.cpp').read_text(encoding='utf-8'),(source/'yolo_integration.c').read_text(encoding='utf-8'))
    generated=w/f'test_{board}.cpp'
    generated.write_text(prefix+'\nnamespace old {\n'+base+'\n}\nnamespace next {\n'+current+'\n}\n'+test,encoding='utf-8')
    exe=generated.with_suffix('.exe')
    subprocess.run(['g++','-std=c++11','-O2','-Wall','-I'+str(root/'common/image'),str(generated),'-o',str(exe)],check=True)
    run=subprocess.run([str(exe)],capture_output=True,check=True)
    (w/f'{board}_comparison.log').write_bytes(run.stdout)
    print(board,run.stdout.decode().splitlines()[-1],flush=True)
