#include "../camera_valid_rows.h"
#include "../frame_pixels.h"
#include <cassert>
#include <cstdio>
#include <vector>

// Exercise the FPGA's invalid transport region with patterns that are NOT
// all recognizable as BBWW. Valid scene pixels must be bit-for-bit intact.
static void check(int width, int height, int tile_height, int pattern)
{
    const size_t size=(size_t)width*height;
    std::vector<uint16_t> storage(size+32,0xA55A);
    uint16_t *image=storage.data()+16;
    for(int y=0;y<height;++y)
        for(int x=0;x<width;++x)
            image[(size_t)y*width+x]=uint16_t((y*937+x*31+17)%65534+1);
    const int tile_width=width/2;
    for(int end=tile_height;end<=height;end+=tile_height)
        for(int tile=0;tile<2;++tile)
        {
            for(int x=0;x<tile_width;++x)
                image[(size_t)(end-1)*width+tile*tile_width+x]=0xFFFF;
            for(int x=tile_width/2;x<tile_width;++x)
            {
                const int i=x-tile_width/2;
                uint16_t value=(i&2)?65535:0;
                if(pattern==1 && i<18)value=0x7BEF; // Real 142/160 recurrence.
                if(pattern==2 && i<38)value=0x7BEF; // Real ~122/160 recurrence.
                if(pattern==3)value=0;
                if(pattern==4)value=65535;
                if(pattern==5)value=uint16_t(i*3517+pattern*193); // Stale DMA words.
                image[(size_t)(end-2)*width+tile*tile_width+x]=value;
            }
        }
    const auto original=storage;
    camera_extend_padding_rows(image,width,height,tile_height);
    for(int y=0;y<height;++y)
        for(int x=0;x<width;++x)
        {
            int source_y=y;
            if(y%tile_height==tile_height-1)--source_y;
            if(source_y%tile_height==tile_height-2 && x%tile_width>=tile_width/2)
                --source_y;
            const int source_x=(tile_width>=16 && x%tile_width<8)?x-x%tile_width+8:x;
            assert(image[(size_t)y*width+x]==original[16+(size_t)source_y*width+source_x]);
        }
    const auto repaired=storage;
    camera_extend_padding_rows(image,width,height,tile_height);
    assert(storage==repaired); // Re-applying cannot progressively erode edges.
    for(int i=0;i<16;++i)
        assert(storage[i]==0xA55A && storage[size+16+i]==0xA55A);
    std::vector<uint8_t> bgr(size*3);
    std::vector<uint16_t> repacked(size);
    frame_unpack_rgb565(image,bgr.data(),size);
    frame_pack_rgb565(bgr.data(),repacked.data(),size);
    for(size_t i=0;i<size;++i)assert(repacked[i]==image[i]);
}

int main()
{
    for(int p=0;p<6;++p)
    {
        check(640,480,240,p);
        check(64,48,24,p);
        check(8,6,3,p);
    }
    std::vector<uint16_t> unchanged(640*480,0x1234);
    const auto original=unchanged;
    camera_extend_padding_rows(nullptr,640,480,240);
    camera_extend_padding_rows(unchanged.data(),640,480,1);
    camera_extend_padding_rows(unchanged.data(),639,480,240);
    camera_extend_padding_rows(unchanged.data(),640,479,240);
    assert(unchanged==original);
    std::puts("CAMERA GUARD PASS: full/partial BBWW, black, white, stale words; valid pixels preserved; all tiles, RGB565 round-trip, idempotence and bounds");
}
