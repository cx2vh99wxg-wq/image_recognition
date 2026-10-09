#ifndef STUB_XLIB_H
#define STUB_XLIB_H
#include <stddef.h>
typedef struct _XDisplay Display;
typedef struct { int dummy; } Visual;
typedef unsigned long long Window;   /* 与 aarch64 上 unsigned long 同宽，保证指针/整数转换语义一致 */
typedef unsigned long long XID;
typedef unsigned long long KeySym;
typedef struct _XGC { int dummy; } *GC;   /* real Xlib: GC is a pointer type */
typedef struct { char *data; int width, height; } XImage;
typedef struct { int type; unsigned long long serial; unsigned int keycode, state; } XKeyEvent;
typedef union { int type; XKeyEvent xkey; } XEvent;
typedef struct { int type; } XErrorEvent;
#define KeyPress 2
#define KeyPressMask (1L<<0)
#define ExposureMask (1L<<15)
#define StructureNotifyMask (1L<<17)
#define ZPixmap 2
#define CurrentTime 0L
#define RevertToParent 2
#define BlackPixel(d,s) ((void)(d), (void)(s), 0)
#define WhitePixel(d,s) ((void)(d), (void)(s), 0xFFFFFF)
#define DefaultScreen(d) ((void)(d), 0)
#define RootWindow(d,s) ((void)(d), (void)(s), 0)
#define DefaultVisual(d,s) ((void)(d), (void)(s), (Visual*)0)
Display *XOpenDisplay(const char *);
Window XCreateSimpleWindow(Display*, Window, int,int, unsigned,unsigned, unsigned, unsigned long, unsigned long);
int XStoreName(Display*, Window, const char*);
int XSelectInput(Display*, Window, long);
int XMapWindow(Display*, Window);
int XRaiseWindow(Display*, Window);
int XSetInputFocus(Display*, Window, int, long);
int XFlush(Display*);
XImage *XCreateImage(Display*, Visual*, unsigned, int, int, char*, unsigned, unsigned, int, int);
GC XCreateGC(Display*, Window, unsigned long, void*);
int XPutImage(Display*, Window, GC, XImage*, int,int,int,int, unsigned,unsigned);
int XFreeGC(Display*, GC);
int XDestroyImage(XImage*);
int XCloseDisplay(Display*);
int (*XSetErrorHandler(int (*)(Display*, XErrorEvent*)))(Display*, XErrorEvent*);
int XPending(Display*);
int XNextEvent(Display*, XEvent*);
KeySym XLookupKeysym(XKeyEvent*, int);
#endif
