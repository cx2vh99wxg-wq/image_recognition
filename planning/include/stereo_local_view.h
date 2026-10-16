#ifndef STEREO_LOCAL_VIEW_H
#define STEREO_LOCAL_VIEW_H

#include <stddef.h>
#include <stdint.h>

/* stereo_j8 DDR canvas: CAM1 at (0,0), CAM2 at (0,240).
 * Each 320x240 source is enlarged to its own 640x480 X11 window. */
#define STEREO_VIEW_W 640
#define STEREO_VIEW_H 480
#define STEREO_VIEW_PIXELS (320u * 240u)
#define STEREO_VIEW_BYTES ((size_t)STEREO_VIEW_W * STEREO_VIEW_H * 4u)

typedef struct {
    uint32_t nonblack_pixels;
    uint32_t fnv1a;
} stereo_view_stats_t;

/* Output is B,G,R,0 in 32-bit little-endian TrueColor X11 format. */
int stereo_local_view_render(const uint8_t *frame565, int eye,
                             uint8_t *bgrx, size_t capacity,
                             stereo_view_stats_t *stats);

#endif
