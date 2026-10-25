// Build with -DEXPECT_BUG to reproduce the pre-fix serial mismatch.
// Without it, this validates the final production renderer.
#define main gui_main
#ifdef EXPECT_BUG
#include "perf_shm_completion_before_gcfix_20261025.cpp"
#else
#include "../udp_rk_rk_yolo/S/x11/csrc/main.cpp"
#endif
#undef main
#include <cassert>

int main() {
    setenv("VISION_PARTIAL_UPLOAD", "0", 1); // Isolate upload serial tagging.
    start_fullscreen = 0;
    RenderEngine engine = {};
    assert(!render_engine_init(&engine));
    assert(engine.use_shm_completion);
    XSetForeground(engine.display, engine.gc, 0x123456);
    XSetBackground(engine.display, engine.gc, 0x654321);
    assert(!present_dashboard(&engine));
    XSync(engine.display, False);
    XEvent event;
    assert(XCheckTypedEvent(engine.display, engine.shm_completion_type, &event));
    const XShmCompletionEvent* completion = (const XShmCompletionEvent*)&event;
    unsigned long actual = completion->serial;
    fprintf(stderr, "DIRTY_GC tagged_serial=%lu actual_serial=%lu difference=%ld\n",
            engine.shm_pending_serial, actual, (long)actual - (long)engine.shm_pending_serial);
    XPutBackEvent(engine.display, &event);
#ifdef EXPECT_BUG
    assert(actual != engine.shm_pending_serial);
    assert(!present_dashboard(&engine));
    assert(!engine.use_shm_completion && engine.shm_completion_disabled);
    render_engine_cleanup(&engine);
    puts("OLD_DIRTY_GC_SERIAL_BUG_REPRODUCED");
#else
    assert(actual == engine.shm_pending_serial);
    for (int i = 0; i < 150; ++i) {
        XSetForeground(engine.display, engine.gc, (i * 17 + 3) & 0xffffff);
        XSetBackground(engine.display, engine.gc, (i * 31 + 9) & 0xffffff);
        assert(!present_dashboard(&engine));
        assert(engine.use_shm_completion && !engine.shm_completion_disabled && engine.shm_pending);
    }
    assert(engine.profile_upload_completions >= 150);
    render_engine_cleanup(&engine);
    puts("FIXED_DIRTY_GC_COMPLETION_PASS frames150");
#endif
}
