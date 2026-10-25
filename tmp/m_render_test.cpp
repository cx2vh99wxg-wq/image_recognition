#define main captured_program_main
#include "../udp_rk_rk_yolo/M/pcie/csrc/main.cpp"
#undef main
#include <cassert>
static void exercise(bool shared) {
    RenderEngine engine={};engine.shminfo.shmid=-1;
    engine.display=XOpenDisplay(nullptr);assert(engine.display);
    int screen=DefaultScreen(engine.display);
    engine.window=XCreateSimpleWindow(engine.display,RootWindow(engine.display,screen),0,0,
        DISPLAY_WIDTH,DISPLAY_HEIGHT,0,0,0);
    engine.gc=XCreateGC(engine.display,engine.window,0,nullptr);
    engine.pixmap=XCreatePixmap(engine.display,engine.window,DISPLAY_WIDTH,DISPLAY_HEIGHT,
        DefaultDepth(engine.display,screen));
    if(shared) {
        assert(try_shared_ximage(&engine,screen)==1);
        assert(engine.ximage->obdata==(XPointer)&engine.shminfo);
    } else {
        engine.display_buffer=(char*)malloc(DISPLAY_WIDTH*DISPLAY_HEIGHT*4);
        engine.ximage=XCreateImage(engine.display,DefaultVisual(engine.display,screen),
            DefaultDepth(engine.display,screen),ZPixmap,0,engine.display_buffer,
            DISPLAY_WIDTH,DISPLAY_HEIGHT,32,DISPLAY_WIDTH*4);
    }
    engine.is_init=1;
    std::vector<uint8_t> pixels(DISPLAY_WIDTH*DISPLAY_HEIGHT*3);
    for(unsigned f=0;f<25;++f) {
        for(size_t n=0;n<pixels.size();++n)pixels[n]=(n*19+f*29)%256;
        assert(render_engine_render_frame_with_detections(&engine,pixels.data())==0);
        XSync(engine.display,False);
        XImage* roundtrip=XGetImage(engine.display,engine.pixmap,0,0,DISPLAY_WIDTH,DISPLAY_HEIGHT,AllPlanes,ZPixmap);
        assert(roundtrip);
        for(int y=0;y<DISPLAY_HEIGHT;y+=11) for(int x=0;x<DISPLAY_WIDTH;x+=7) {
            unsigned long got=XGetPixel(roundtrip,x,y);
            size_t n=((size_t)y*DISPLAY_WIDTH+x)*3;
            unsigned long expected=(unsigned long)pixels[n]|((unsigned long)pixels[n+1]<<8)|((unsigned long)pixels[n+2]<<16);
            assert((got&0xffffff)==expected);
        }
        XDestroyImage(roundtrip);
    }
    render_engine_cleanup(&engine);render_engine_cleanup(&engine);
    assert(!engine.display&&!engine.display_buffer&&!engine.ximage);
    printf("M renderer %s pixel/fence/double-cleanup PASS\n",shared?"MIT-SHM":"XPutImage fallback");
}
int main() { exercise(true);exercise(false); }
