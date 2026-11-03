#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
int main(int argc,char **argv) {
    assert(argc==2);
    FILE *out=fopen(argv[1],"wb");
    assert(out);
    std::vector<uint16_t> source(640*480);
    for(size_t i=0;i<source.size();++i) source[i]=(uint16_t)(i*4517+127);
    std::vector<uint8_t> rgb(65536*3);
    for(unsigned i=0;i<65536;++i) source[i]=(uint16_t)i;
    rgb565_to_rgb888(source.data(),rgb.data(),65536);
    fwrite(rgb.data(),1,rgb.size(),out);
    std::vector<uint32_t> screen(1280*480);
    for(int y : {0,240}) for(int x : {0,320,640,960}) {
        std::fill(screen.begin(),screen.end(),0xff17372a);
        render_camera_block(screen.data(),source.data(),x,y,83,-100);
        fwrite(screen.data(),4,screen.size(),out);
        for(float scale : {0.0f,0.67f,1.0f,1.375f,2.0f,2.43f}) {
            for(int left : {-19,640,1250}) {
                std::fill(screen.begin(),screen.end(),0xff17272a);
                render_scaled_camera_block(screen.data(),source.data(),x,y,scale,left,150);
                fwrite(screen.data(),4,screen.size(),out);
            }
        }
    }
    std::vector<uint8_t> artwork(57*83*4);
    for(unsigned i=0;i<artwork.size();++i) artwork[i]=i*137+11;
    for(int i=0;i<33;++i) {
        for(size_t j=0;j<screen.size();++j) screen[j]=(uint32_t)(j*48317+i*354);
        const int x=(i*91)%1410-100, y=(i*47)%620-100;
        draw_lane_lines(screen.data(),x,y,(i%4+1)*77,(i%6+1)*55,(CurveType)(i%5),58+i%6);
        VehicleLayerState state={};
        state.png=artwork.data(); state.png_width=57; state.png_height=83;
        state.person=i%4!=0; state.blink=i%2;
        state.box_left=(i*91)%640; state.box_top=(i*83)%480;
        state.box_right=state.box_left+44; state.box_bottom=state.box_top+90;
        render_car_png_with_state(screen.data(),x,y,(i%4+1)*77,(i%6+1)*55,state);
        fwrite(screen.data(),4,screen.size(),out);
    }
    fclose(out);
}
