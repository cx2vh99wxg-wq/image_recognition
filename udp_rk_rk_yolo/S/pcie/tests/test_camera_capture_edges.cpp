#include "../include/camera_capture_edges.h"
#include "../include/camera_orientation.h"
#include <cassert>
#include <cstdio>
#include <vector>
int main()
{
    std::vector<uint16_t> data(640*480+32,0xA55A);
    uint16_t *image=data.data()+16;
    for(int y=0;y<480;++y)for(int x=0;x<640;++x)image[y*640+x]=uint16_t(y*91+x*13);
    auto original=data;
    repair_s_camera0_edges(image,640,480);
    for(int y=0;y<480;++y)for(int x=0;x<640;++x)
    {
        bool camera0=(x/320)==(y/240);
        int sx=camera0 && x%320<9?x-x%320+9:x;
        int sy=camera0?y-y%240+(y%240+3)%240:y;
        assert(image[y*640+x]==original[16+sy*640+sx]);
    }
    for(int i=0;i<16;++i)assert(data[i]==0xA55A && data[data.size()-1-i]==0xA55A);
    // S displays a vertical flip after tile rotation: the net mapping is
    // horizontal mirroring. Check the actual rear-center source boundary.
    auto restored=data;
    rotate_rgb565_tiles_180(image,640,480,320,240);
    for(int y=0;y<240;++y)for(int x=0;x<320;++x)
        assert(image[(479-y)*640+320+x]==restored[16+(240+y)*640+639-x]);
    std::vector<uint8_t> expected(640*480*3), fused(expected.size());
    frame_unpack_rgb565(image,expected.data(),640*480);
    unpack_rgb565_tiles_180(original.data()+16,fused.data(),640,480,320,240,true);
    assert(fused==expected);
    repair_s_camera0_edges(nullptr,640,480);
    std::puts("CAMERA0 EDGES PASS: exact three-row rotation, own nine-column fill, other views unchanged, guards and final rear-center mapping");
}
