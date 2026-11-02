#define main renderer_application_main
#include "../main.cpp"
#undef main
#include <assert.h>

static void paint(std::vector<uint16_t>& plane, int x, int y, uint16_t color) {
    for (int row = 0; row < 240; ++row)
        std::fill_n(plane.data() + (y + row) * 640 + x, 320, color);
}

int main(int argc, char** argv) {
    std::vector<uint16_t> s(640 * 480), m(640 * 480);
    paint(s, 0, 0, 0xffff); paint(m, 0, 0, 0xf81f);
    paint(m, 0, 240, 0x001f); paint(m, 320, 240, 0x07e0); paint(m, 320, 0, 0xf800);
    paint(s, 0, 240, 0x07ff); paint(s, 320, 240, 0xffe0); paint(s, 320, 0, 0x7810);
    const unsigned modes[] = {0x77, 0x88, 0x66, 0x22, 0x44, 0x33};
    const uint32_t colors[] = {0xfff80000, 0xff00fc00, 0xff0000f8,
                               0xfff8fc00, 0xff00fcf8, 0xff800078};
    RenderEngine engine = {};
    engine.headless = engine.is_init = engine.preview_recorded = 1;
    engine.display_buffer = (char*)calloc(1280 * 480, 4);
    engine.vehicle_layer = (uint32_t*)calloc(1280 * 480, 4);
    engine.dashboard = new perception_ui::Dashboard();
    transition_animation_init(&engine.transition);
    const uint8_t *sp = (const uint8_t*)s.data(), *mp = (const uint8_t*)m.data();
    for (int i = 0; i < 6; ++i) {
        assert(render_engine_render_camera_view(&engine, sp, mp, (DisplayMode)modes[i]) == 0);
        assert(((uint32_t*)engine.display_buffer)[120 * 1280 + 640] == colors[i]);
        transition_animation_start(&engine.transition, DISPLAY_MODE_ALL_CAMERAS, (DisplayMode)modes[i]);
        for (int frame = 0; frame < 20; ++frame) {
            assert(render_engine_render_with_transition(&engine, sp, mp, (DisplayMode)modes[i]) == 0);
            for (int n = 0; n < 1280 * 480; ++n) {
                const uint32_t pixel = ((uint32_t*)engine.display_buffer)[n];
                assert(pixel != 0xfff8fcf8 && pixel != 0xfff800f8);
            }
        }
        assert(render_engine_render_camera_view(&engine, sp, mp, (DisplayMode)modes[i]) == 0);
        if (argc == 2) {
            char path[4096];
            snprintf(path, sizeof(path), "%s/view_%02x.ppm", argv[1], modes[i]);
            assert(save_dashboard(&engine, path) == 0);
        }
    }
    assert(render_engine_render_camera_view(&engine, sp, mp, DISPLAY_MODE_ALL_CAMERAS) == 0);
    for (int i = 0; i < 6; ++i)
        assert(((uint32_t*)engine.display_buffer)[(i / 3 * 240 + 120) * 1280 + i % 3 * 320 + 160] == colors[i]);
    if (argc == 2) {
        char path[4096]; snprintf(path, sizeof(path), "%s/overview.ppm", argv[1]);
        assert(save_dashboard(&engine, path) == 0);
    }
    render_engine_cleanup(&engine);
    puts("RENDER INTEGRATION PASS: actual steady rendering and 120 animation frames never expose either spare source; six focused views match panorama");
}
