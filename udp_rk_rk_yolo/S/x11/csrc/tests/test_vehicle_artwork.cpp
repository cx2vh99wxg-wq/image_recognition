#include "vehicle_artwork.h"
#include <cassert>
#include <cstdio>

int main()
{
    const int w=60,h=100;
    std::vector<uint8_t> rgba(w*h*4,255);
    for(int y=0;y<h;++y) for(int x=0;x<w;++x)
    {
        auto *p=rgba.data()+(y*w+x)*4;
        p[0]=x==30?255:10;p[1]=20;p[2]=30;
    }
    const auto original=rgba;
    const auto image=vehicle_artwork::prepare(rgba.data(),w,h);
    assert(rgba==original&&image.size()==320*480);
    assert(image[240*320+160]==0xffff141e);
    assert(image[240*320]==image[240*320+15]);
    assert(image[240*320+319]==image[240*320+305]);
    assert(vehicle_artwork::prepare(nullptr,w,h).empty());
    const int stride=336,guard=64;
    std::vector<uint32_t> surface(stride*480+2*guard,0x12345678);
    for(int y=0;y<480;++y)std::copy_n(image.data()+y*320,320,surface.data()+guard+y*stride);
    const auto before=surface;
    vehicle_artwork::person_glow(surface.data()+guard,stride,320,240,true);
    unsigned changed=0;
    for(int y=0;y<480;++y)for(int x=0;x<stride;++x)
    {
        const int i=guard+y*stride+x;
        if(x>=320||x<240||y<160||y>=320)assert(surface[i]==before[i]);
        changed+=surface[i]!=before[i];
    }
    assert(changed>0);
    for(int i=0;i<guard;++i)assert(surface[i]==before[i]&&surface[surface.size()-i-1]==before[surface.size()-i-1]);
    puts("VEHICLE ARTWORK PASS: centered, aspect-preserved, source immutable, bounded cached person glow");
}
