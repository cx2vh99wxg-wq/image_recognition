#include "six_camera_render.h"
#include <assert.h>
#include <stdio.h>
#include <cstring>
#include <vector>

// Distinct physical signals plus two deliberately different spare-quadrant
// sentinels. Tests must not pass just because the FPGA happens to copy a tile.
static void fill_tile(std::vector<uint16_t>& plane, int x, int y, uint16_t color) {
    for (int row = 0; row < 240; ++row)
        for (int col = 0; col < 320; ++col) plane[(y + row) * 640 + x + col] = color;
}

int main() {
    // Exhaust the 16-bit source space and exercise vector tails. The scalar
    // bit contract is the independent color oracle, not another SIMD path.
    std::vector<uint16_t> codes(65536);
    std::vector<uint32_t> decoded(65536),doubled(2*65536);
    for(unsigned p=0;p<codes.size();++p)codes[p]=p;
    six_camera::argb_row(decoded.data(),codes.data(),codes.size());
    six_camera::argb_double_row(doubled.data(),codes.data(),codes.size());
    for(unsigned p=0;p<codes.size();++p){
        const uint32_t expected=0xff000000u|((p&31)<<19)|(((p>>5)&63)<<10)|(((p>>11)&31)<<3);
        assert(decoded[p]==expected && doubled[2*p]==expected && doubled[2*p+1]==expected);
    }
    for(int n:{1,3,7,9,15}){
        six_camera::argb_row(decoded.data(),codes.data()+123,n);
        six_camera::argb_double_row(doubled.data(),codes.data()+123,n);
        for(int i=0;i<n;++i)assert(decoded[i]==six_camera::argb(codes[123+i]) &&
            doubled[2*i]==decoded[i] && doubled[2*i+1]==decoded[i]);
    }
    std::vector<uint16_t> s(640 * 480), m(640 * 480);
    fill_tile(s, 0, 0, 0xffff);       // spare S: white, must never appear
    fill_tile(m, 0, 0, 0xf81f);       // spare M: magenta, must never appear
    fill_tile(m, 0, 240, 0x001f);     // front-left red in captured BGR565
    fill_tile(m, 320, 240, 0x07e0);   // front-center green
    fill_tile(m, 320, 0, 0xf800);     // front-right blue
    fill_tile(s, 0, 240, 0x07ff);     // rear-left yellow
    fill_tile(s, 320, 240, 0xffe0);   // rear-center cyan
    fill_tile(s, 320, 0, 0x7810);     // rear-right violet
    const unsigned modes[] = {0x77, 0x88, 0x66, 0x22, 0x44, 0x33};
    const char* hotspots[] = {"m4", "m3", "m5", "m2", "m0", "m1"};
    const uint32_t colors[] = {0xfff80000, 0xff00fc00, 0xff0000f8,
                               0xfff8fc00, 0xff00fcf8, 0xff800078};
    assert(PHYSICAL_CAMERA_COUNT == 6);
    std::vector<uint32_t> out(1280 * 480);
    six_camera::overview(out.data(), s.data(), m.data());
    for (int n = 0; n < 6; ++n) {
        const int x = (n % 3) * 320 + 160, y = (n / 3) * 240 + 120;
        assert(out[y * 1280 + x] == colors[n]);
        const CameraView* view = camera_view_for_mode(modes[n]);
        assert(view && !strcmp(view->hmi_hotspot, hotspots[n]));
        six_camera::focus(out.data(), s.data(), m.data(), *view);
        for (int row = 0; row < 480; ++row) {
            assert(out[row * 1280 + 320] == colors[n]);
            assert(out[row * 1280 + 959] == colors[n]);
            assert(out[row * 1280 + 319] == 0xff000000);
            assert(out[row * 1280 + 960] == 0xff000000);
        }
        // The actual zoom background also excludes both spare quadrants.
        for (float dim : {1.f, .7f, .3f}) {
            six_camera::overview(out.data(), s.data(), m.data(), view, dim);
            for (uint32_t pixel : out)
                assert(pixel != 0xfff8fcf8 && pixel != 0xfff800f8);
            for (int row = 0; row < 480; ++row)
                for (int col = 960; col < 1280; ++col)
                    assert(out[row * 1280 + col] == 0xff000000);
        }
        six_camera::overview(out.data(), s.data(), m.data());
    }
    assert(camera_view_for_mode(0x11) == camera_view_for_mode(0x44));
    assert(camera_view_for_mode(0x55) == camera_view_for_mode(0x88));
    assert(camera_mode_or_overview(0) == DISPLAY_MODE_ALL_CAMERAS);
    for (unsigned command : {0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x100}) {
        DisplayMode unchanged = DISPLAY_MODE_FRONT_LEFT;
        assert(!camera_mode_normalize(command, &unchanged));
        assert(unchanged == DISPLAY_MODE_FRONT_LEFT);
    }
    // Keep the existing S-only vertical orientation in every view path.
    const CameraView* rear = camera_view_for_mode(0x22);
    s[240 * 640] = 0x001f;
    s[479 * 640] = 0xf800;
    six_camera::focus(out.data(), s.data(), m.data(), *rear);
    assert(out[320] == 0xff0000f8);
    assert(out[479 * 1280 + 320] == 0xfff80000);
    // A row/column-varying signal catches repeated, flipped and shifted edges.
    for(size_t i=0;i<s.size();++i){s[i]=(i*71u+i/640*977u)&65535;m[i]=(i*113u+i/640*19u)&65535;}
    six_camera::overview(out.data(),s.data(),m.data());
    for(const auto &view:camera_views)for(int y=0;y<240;++y)for(int x=0;x<320;++x){
        const int sy=view.source_y+(view.network_source?y:239-y);
        const auto &plane=view.network_source?m:s;
        assert(out[(view.row*240+y)*1280+view.column*320+x]==six_camera::argb(plane[sy*640+view.source_x+x]));
    }
    for(const auto &view:camera_views){
        six_camera::focus(out.data(),s.data(),m.data(),view);
        for(int y=0;y<480;++y)for(int x=0;x<640;++x){
            const int sy=view.source_y+(view.network_source?y/2:239-y/2);
            const auto &plane=view.network_source?m:s;
            assert(out[y*1280+320+x]==six_camera::argb(plane[sy*640+view.source_x+x/2]));
        }
    }
    puts("SIX CAMERA PASS: six unique sources, HMI positions, focus/overview/zoom background, spare exclusion, center aliases, control-code rejection and S orientation");
}
