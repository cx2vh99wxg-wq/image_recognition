#define PERCEPTION_HAVE_GLES 1
#include "gles_presenter.h"
#include <X11/Xutil.h>
#include <cassert>
#include <vector>
#include <string>
#include <unistd.h>
static int capture_w,capture_h;
static bool shader_failure=false,link_failure=false,swap_failure=false;
static std::vector<unsigned char> framebuffer;
extern "C" void __real_glGetShaderiv(GLuint,GLenum,GLint*);
extern "C" void __wrap_glGetShaderiv(GLuint shader,GLenum query,GLint* value) {
    __real_glGetShaderiv(shader,query,value);
    if(shader_failure&&query==GL_COMPILE_STATUS)*value=GL_FALSE;
}
extern "C" void __real_glGetProgramiv(GLuint,GLenum,GLint*);
extern "C" void __wrap_glGetProgramiv(GLuint program,GLenum query,GLint* value) {
    __real_glGetProgramiv(program,query,value);
    if(link_failure&&query==GL_LINK_STATUS)*value=GL_FALSE;
}
extern "C" EGLBoolean __real_eglSwapBuffers(EGLDisplay,EGLSurface);
extern "C" EGLBoolean __wrap_eglSwapBuffers(EGLDisplay display,EGLSurface surface) {
    if(swap_failure)return EGL_FALSE;
    framebuffer.resize((size_t)capture_w*capture_h*4);
    glPixelStorei(GL_PACK_ALIGNMENT,4);
    glReadPixels(0,0,capture_w,capture_h,GL_RGBA,GL_UNSIGNED_BYTE,framebuffer.data());
    assert(glGetError()==GL_NO_ERROR);
    return __real_eglSwapBuffers(display,surface);
}
static void expect_pixel(int x,int y,unsigned r,unsigned g,unsigned b) {
    size_t index=((size_t)y*capture_w+x)*4;
    if(framebuffer[index]!=r||framebuffer[index+1]!=g||framebuffer[index+2]!=b) {
        fprintf(stderr,"Pixel mismatch x=%d y=%d actual=%u,%u,%u expected=%u,%u,%u\n",x,y,
            framebuffer[index],framebuffer[index+1],framebuffer[index+2],r,g,b);abort();
    }
}
static void resize_window(Display* d,Window w,int width,int height) {
    XResizeWindow(d,w,width,height);XSync(d,False);
    XWindowAttributes current={};
    for(int n=0;n<200;++n) {
        XGetWindowAttributes(d,w,&current);
        if(current.width==width&&current.height==height)break;
        usleep(10000);
    }
    assert(current.width==width&&current.height==height);
    capture_w=width;capture_h=height;
}
int main() {
    setvbuf(stdout,NULL,_IOLBF,0);
    setenv("VISION_SWAP_INTERVAL","0",1);
    Display* display=XOpenDisplay(NULL);assert(display);
    int screen=DefaultScreen(display);
    Window window=XCreateSimpleWindow(display,RootWindow(display,screen),0,0,1600,900,0,0,0);
    XMapWindow(display,window);XSync(display,False);
    GlesPresenter presenter;
    assert(presenter.init(display,window,XVisualIDFromVisual(DefaultVisual(display,screen)),1600,900));
    capture_w=1600;capture_h=900;
    std::vector<uint32_t> pixels(1600*900);
    for(int y=0;y<900;++y)for(int x=0;x<1600;++x)
        pixels[(size_t)y*1600+x]=0xff000000u|((unsigned)(x*17+y*3)%256<<16)|((unsigned)(x^y)%256<<8)|((unsigned)(x*7+y*13)%256);
    assert(presenter.present(pixels.data(),1600,900,capture_w,capture_h));
    for(int y=0;y<900;++y)for(int x=0;x<1600;++x) {
        unsigned want=pixels[(size_t)(899-y)*1600+x];
        expect_pixel(x,y,(want>>16)&255,(want>>8)&255,want&255);
    }
    puts("1600x900 nonuniform 1px texture BGRA/orientation byte-identical PASS");
    pixels.resize(64*32);
    for(int y=0;y<32;++y)for(int x=0;x<64;++x)
        pixels[y*64+x]=y<16?(x<32?0xffff0000:0xff00ff00):(x<32?0xff0000ff:0xffffffff);
    resize_window(display,window,320,240);
    assert(presenter.present(pixels.data(),64,32,capture_w,capture_h));
    assert(presenter.present(pixels.data(),64,32,capture_w,capture_h));
    expect_pixel(40,180,255,0,0);expect_pixel(280,180,0,255,0);
    expect_pixel(40,60,0,0,255);expect_pixel(280,60,255,255,255);
    expect_pixel(0,0,5,7,21);expect_pixel(100,239,5,7,21);
    resize_window(display,window,200,300);
    assert(presenter.present(pixels.data(),64,32,capture_w,capture_h));
    assert(presenter.present(pixels.data(),64,32,capture_w,capture_h));
    expect_pixel(25,175,255,0,0);expect_pixel(175,175,0,255,0);
    expect_pixel(25,125,0,0,255);expect_pixel(175,125,255,255,255);
    expect_pixel(100,0,5,7,21);expect_pixel(100,299,5,7,21);
    puts("GPU linear letterbox/canvas resize/window landscape+portrait resize PASS");
    presenter.cleanup();presenter.cleanup();assert(!presenter.ready());
    auto visual=XVisualIDFromVisual(DefaultVisual(display,screen));
    shader_failure=true;assert(!presenter.init(display,window,visual,64,32));
    assert(!presenter.ready()&&std::string(presenter.error()).find("shader")!=std::string::npos);
    shader_failure=false;link_failure=true;assert(!presenter.init(display,window,visual,64,32));
    assert(!presenter.ready());link_failure=false;
    assert(!presenter.init(display,window,0xabcdef,64,32));assert(!presenter.ready());
    assert(presenter.init(display,window,visual,64,32));
    swap_failure=true;assert(!presenter.present(pixels.data(),64,32,capture_w,capture_h));assert(!presenter.ready());
    swap_failure=false;assert(presenter.init(display,window,visual,64,32));
    assert(presenter.present(pixels.data(),64,32,capture_w,capture_h));
    presenter.cleanup();presenter.cleanup();
    XDestroyWindow(display,window);XCloseDisplay(display);
    puts("shader/link/config/swap failure clean fallback + repeated init/cleanup PASS");
}
