#include "../camera_dma_alignment.h"
#include <cassert>
#include <cstdio>
#include <vector>
int main()
{
    std::vector<uint16_t> canonical(640*480), shifted(canonical.size());
    for(int y=0;y<480;++y)for(int x=0;x<640;++x)
    {
        int camera=(y<240?(x<320?0:1):(x<320?2:0));
        unsigned texture=((y%240)*3+x%320)%16;
        canonical[y*640+x]=camera==0?uint16_t((12<<11)|(24<<5)|texture):
                            camera==1?uint16_t((31<<11)|(40<<5)|texture):uint16_t((2<<11)|(12<<5)|(16+texture));
    }
    for(int y : {238,478})for(int x=0;x<640;++x)
        if(x%320>=168)canonical[y*640+x]=(x&2)?65535:0;
    for(int phase=0;phase<480;++phase)
    {
        for(int y=0;y<480;++y)
            for(int x=0;x<640;++x)shifted[y*640+x]=canonical[((y+phase)%480)*640+x];
        int discard=-1;assert(camera_dma_alignment(shifted.data(),640,480,&discard));
        assert(discard==(480-phase)%480);
    }
    std::fill(shifted.begin(),shifted.end(),1234);int discard=99;
    assert(!camera_dma_alignment(shifted.data(),640,480,&discard) && discard==99);
    assert(!camera_dma_alignment(nullptr,640,480,&discard));
    std::puts("DMA ALIGN PASS: all 480 starting phases, no-marker rejection and invalid arguments");
}
