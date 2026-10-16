/* M-board camera bring-up: PCIe -> two local X11 windows, no UDP or RKNN. */
#include "driving_config.h"
#include "pcie_capture.h"
#include "stereo_local_view.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static volatile sig_atomic_t running = 1;
static void on_stop(int signum) { (void)signum; running = 0; }

typedef struct {
    Window window;
    GC gc;
    XImage *image;
} eye_window_t;

static void close_eye(Display *display, eye_window_t *eye)
{
    if (eye->gc) XFreeGC(display, eye->gc);
    if (eye->image) XDestroyImage(eye->image); /* also releases image->data */
    /* XCloseDisplay releases the window, including when the user closed it. */
    memset(eye, 0, sizeof(*eye));
}

static int open_eye(Display *display, int screen, eye_window_t *eye, int index)
{
    char title[80];
    const int x = index ? STEREO_VIEW_W + 20 : 0;
    eye->window = XCreateSimpleWindow(display, RootWindow(display, screen), x, 0,
                                      STEREO_VIEW_W, STEREO_VIEW_H, 1,
                                      BlackPixel(display, screen), BlackPixel(display, screen));
    if (!eye->window) return -1;
    snprintf(title, sizeof(title), "M FPGA OV5640 CAM%d (PCIe local)", index + 1);
    XStoreName(display, eye->window, title);
    XSelectInput(display, eye->window, ExposureMask | StructureNotifyMask);

    eye->image = XCreateImage(display, DefaultVisual(display, screen),
                              (unsigned)DefaultDepth(display, screen), ZPixmap, 0, NULL,
                              STEREO_VIEW_W, STEREO_VIEW_H, 32, 0);
    if (!eye->image) return -1;
    if (eye->image->bits_per_pixel != 32 || eye->image->byte_order != LSBFirst ||
        eye->image->red_mask != 0x00ff0000ul ||
        eye->image->green_mask != 0x0000ff00ul ||
        eye->image->blue_mask != 0x000000fful ||
        eye->image->bytes_per_line != STEREO_VIEW_W * 4) {
        fprintf(stderr, "X11 pixel format unsupported: depth=%d bits=%d masks=%lx/%lx/%lx\n",
                eye->image->depth, eye->image->bits_per_pixel,
                eye->image->red_mask, eye->image->green_mask, eye->image->blue_mask);
        return -1;
    }
    eye->image->data = calloc(1, STEREO_VIEW_BYTES);
    if (!eye->image->data) return -1;
    eye->gc = XCreateGC(display, eye->window, 0, NULL);
    if (!eye->gc) return -1;
    XMapWindow(display, eye->window);
    return 0;
}

static int save_snapshot(const char *prefix, const eye_window_t *eye, int index)
{
    char path[1024];
    int n = snprintf(path, sizeof(path), "%s-cam%d.ppm", prefix, index + 1);
    if (n < 0 || (size_t)n >= sizeof(path)) return -1;
    FILE *fp = fopen(path, "wb");
    if (!fp) { perror(path); return -1; }
    if (fprintf(fp, "P6\n%d %d\n255\n", STEREO_VIEW_W, STEREO_VIEW_H) < 0) {
        fclose(fp); return -1;
    }
    const uint8_t *bgrx = (const uint8_t *)eye->image->data;
    for (size_t i = 0; i < (size_t)STEREO_VIEW_W * STEREO_VIEW_H; ++i) {
        const uint8_t rgb[3] = {bgrx[i * 4 + 2], bgrx[i * 4 + 1], bgrx[i * 4]};
        if (fwrite(rgb, 1, sizeof(rgb), fp) != sizeof(rgb)) {
            perror(path); fclose(fp); return -1;
        }
    }
    if (fclose(fp) != 0) { perror(path); return -1; }
    printf("snapshot: %s\n", path);
    return 0;
}

static void usage(const char *name)
{
    fprintf(stderr, "Usage: %s [--frames N] [--snapshot PATH_PREFIX]\n", name);
}

int main(int argc, char **argv)
{
    unsigned long max_frames = 0;
    const char *snapshot = NULL;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--frames") == 0 && i + 1 < argc) {
            char *end = NULL;
            errno = 0;
            max_frames = strtoul(argv[++i], &end, 10);
            if (errno || !end || *end || !max_frames) { usage(argv[0]); return 2; }
        } else if (strcmp(argv[i], "--snapshot") == 0 && i + 1 < argc) {
            snapshot = argv[++i];
        } else {
            usage(argv[0]); return 2;
        }
    }

    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "XOpenDisplay failed: run from the M-board graphical desktop as its user; DISPLAY=%s\n",
                getenv("DISPLAY") ? getenv("DISPLAY") : "(unset)");
        return 1;
    }
    eye_window_t eyes[2] = {{0}, {0}};
    int result = 1;
    uint8_t *frame = NULL;
    frame_grabber_t grabber;
    memset(&grabber, 0, sizeof(grabber));
    grabber.fd = -1;
    int mapped = 0;

    if (open_eye(display, DefaultScreen(display), &eyes[0], 0) ||
        open_eye(display, DefaultScreen(display), &eyes[1], 1)) goto done;
    XFlush(display);
    frame = malloc(IMG_FRAME_BYTES);
    if (!frame) { perror("malloc frame"); goto done; }
    if (pcie_capture_open(&grabber, IMG_WIDTH, IMG_HEIGHT, PCIE_LEAD_PIXELS) != 0) goto done;
    if (pcie_capture_start(&grabber) != 0) goto done;
    mapped = 1;

    signal(SIGINT, on_stop);
    signal(SIGTERM, on_stop);
    printf("M local PCIe preview: CAM1 and CAM2, separate 640x480 windows; no UDP. Press Ctrl-C to stop.\n");
    printf("Each camera uses a 320x240 region from the FPGA's contiguous 640x480 DDR frame.\n");
    fflush(stdout);
    unsigned long frames = 0;
    int saved = 0;
    while (running && (!max_frames || frames < max_frames)) {
        while (XPending(display)) {
            XEvent event;
            XNextEvent(display, &event);
            if (event.type == DestroyNotify) running = 0;
        }
        if (!running) break;
        if (pcie_capture_grab(&grabber, frame) != 0) {
            fprintf(stderr, "PCIe frame read failed after %lu successful frames\n", frames);
            goto done;
        }
        stereo_view_stats_t stats[2];
        for (int i = 0; i < 2; ++i) {
            if (stereo_local_view_render(frame, i, (uint8_t *)eyes[i].image->data,
                                         STEREO_VIEW_BYTES, &stats[i]) != 0) goto done;
            XPutImage(display, eyes[i].window, eyes[i].gc, eyes[i].image,
                      0, 0, 0, 0, STEREO_VIEW_W, STEREO_VIEW_H);
        }
        XFlush(display);
        ++frames;
        if (frames == 1 || frames % 5 == 0) {
            printf("frame=%lu pad-white=%u/%u pad-nonzero=%u/%u CAM1 nonblack=%u/%u hash=%08x CAM2 nonblack=%u/%u hash=%08x\n",
                   frames, grabber.lead_white_pixels, (unsigned)(PCIE_LEAD_PIXELS * IMG_HEIGHT),
                   grabber.lead_nonzero_pixels, (unsigned)(PCIE_LEAD_PIXELS * IMG_HEIGHT),
                   stats[0].nonblack_pixels, STEREO_VIEW_PIXELS, stats[0].fnv1a,
                   stats[1].nonblack_pixels, STEREO_VIEW_PIXELS, stats[1].fnv1a);
            fflush(stdout);
        }
        if (snapshot && !saved && frames >= 5) {
            if (save_snapshot(snapshot, &eyes[0], 0) ||
                save_snapshot(snapshot, &eyes[1], 1)) goto done;
            saved = 1;
        }
        struct timespec pause_time = {.tv_sec = 0, .tv_nsec = 200000000L};
        while (running && nanosleep(&pause_time, &pause_time) == -1 && errno == EINTR) {}
    }
    result = 0;

done:
    if (mapped) pcie_capture_stop(&grabber);
    pcie_capture_close(&grabber);
    free(frame);
    if (eyes[0].window) close_eye(display, &eyes[0]);
    if (eyes[1].window) close_eye(display, &eyes[1]);
    XCloseDisplay(display);
    return result;
}
