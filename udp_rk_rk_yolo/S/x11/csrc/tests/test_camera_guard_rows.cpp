#include "../../../../common/image/camera_valid_rows.h"
#include "../../../pcie/include/camera_orientation.h"
#include "six_camera_render.h"
#include <cassert>
#include <cstdio>
#include <vector>

int main()
{
    std::vector<uint16_t> image(640*480), original;
    for(int y=0;y<480;++y)
        for(int x=0;x<640;++x)
            image[y*640+x]=uint16_t(1+(y%240)*80+x%320);
    auto clean=image;
    camera_extend_padding_rows(clean.data(),640,480,240);
    for(int y : {238,478})
        for(int x=0;x<640;++x)
            assert(clean[y*640+x]==image[(y-(x%320>=160))*640+(x%320<8?x-x%320+8:x)]);
    // Simulate transport padding and spill from the preceding camera.
    for(int y=0;y<480;++y)
        for(int x=0;x<8;++x)
        {
            image[y*640+x]=65535;
            image[y*640+320+x]=image[y*640+312+x];
        }
    for(int y : {239,479})
        for(int x=0;x<640;++x)
            image[y*640+x]=(x%16==0)?65535:0;
    for(int y : {238,478})
        for(int x=0;x<640;++x)
            if(x%320>=168)
                image[y*640+x]=(x&2)?65535:0;
    original=image;
    camera_extend_padding_rows(image.data(),640,480,240);
    for(int y=0;y<480;++y)
        for(int x=0;x<640;++x)
        {
            int source_y=y;
            if(y%240==239)--source_y;
            if(source_y%240==238 && x%320>=160)--source_y;
            const int source_x=x%320<8?x-x%320+8:x;
            assert(image[y*640+x]==original[source_y*640+source_x]);
        }
    auto valid=image;
    rotate_rgb565_tiles_180(image.data(),640,480,320,240);
    std::vector<uint32_t> output(1280*480);
    six_camera::overview(output.data(),image.data(),valid.data());
    for(const auto &view : camera_views)
        for(int x : {0,100,160,319})
        {
            const int source_x=view.source_x+(view.network_source?x:319-x);
            const int sy=view.source_y+238;
            assert(output[(view.row*240+239)*1280+view.column*320+x]==
                   six_camera::argb(valid[sy*640+source_x]));
        }
    camera_extend_padding_rows(image.data(),640,480,1);
    std::puts("CAMERA GUARD PASS: 8-pixel prefix + invalid tail repaired regardless of contents; valid half unchanged; S six-view edges correct");
}
