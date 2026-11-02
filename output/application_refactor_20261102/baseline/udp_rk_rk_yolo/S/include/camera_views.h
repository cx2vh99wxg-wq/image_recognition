#ifndef CAMERA_VIEWS_H
#define CAMERA_VIEWS_H

#include <stdint.h>

/* Six physical views. Values remain the HMI/FSPI wire codes, not DMA slots.
 * The two 640x480 transport planes retain their existing 2x2 packing for the
 * FPGA, inference and UDP ABI. Their spare top-left tiles are not extra views. */
typedef enum {
    DISPLAY_MODE_REAR_LEFT = 0x22,
    DISPLAY_MODE_REAR_RIGHT = 0x33,
    DISPLAY_MODE_REAR_CENTER = 0x44,
    DISPLAY_MODE_FRONT_RIGHT = 0x66,
    DISPLAY_MODE_FRONT_LEFT = 0x77,
    DISPLAY_MODE_FRONT_CENTER = 0x88,
    DISPLAY_MODE_ALL_CAMERAS = 0x99
} DisplayMode;

typedef struct {
    DisplayMode mode;
    const char* name;
    uint8_t network_source;  /* 0: S/PCIe, 1: M/UDP */
    uint16_t source_x, source_y;
    uint8_t column, row;
    const char* hmi_hotspot;
} CameraView;

#define PHYSICAL_CAMERA_COUNT 6
/* Match the existing, confirmed correct 3x2 overview exactly. All render
 * paths and touch selection use this table instead of independent switches. */
static const CameraView camera_views[PHYSICAL_CAMERA_COUNT] = {
    {DISPLAY_MODE_FRONT_LEFT,   "Front Left",   1,   0, 240, 0, 0, "m4"},
    {DISPLAY_MODE_FRONT_CENTER, "Front Center", 1, 320, 240, 1, 0, "m3"},
    {DISPLAY_MODE_FRONT_RIGHT,  "Front Right",  1, 320,   0, 2, 0, "m5"},
    {DISPLAY_MODE_REAR_LEFT,    "Rear Left",    0,   0, 240, 0, 1, "m2"},
    {DISPLAY_MODE_REAR_CENTER,  "Rear Center",  0, 320, 240, 1, 1, "m0"},
    {DISPLAY_MODE_REAR_RIGHT,   "Rear Right",   0, 320,   0, 2, 1, "m1"}
};

static inline int camera_mode_normalize(uint32_t raw, DisplayMode* mode) {
    /* Old 11/55 buttons select the same physical center, never a seventh
     * or eighth view. AA..FF remain vehicle commands and are rejected. */
    if (raw == 0x11) raw = DISPLAY_MODE_REAR_CENTER;
    if (raw == 0x55) raw = DISPLAY_MODE_FRONT_CENTER;
    if (raw == DISPLAY_MODE_ALL_CAMERAS) {
        if (mode) *mode = DISPLAY_MODE_ALL_CAMERAS;
        return 1;
    }
    for (unsigned i = 0; i < PHYSICAL_CAMERA_COUNT; ++i) {
        if (raw == (uint32_t)camera_views[i].mode) {
            if (mode) *mode = camera_views[i].mode;
            return 1;
        }
    }
    return 0;
}

static inline const CameraView* camera_view_for_mode(uint32_t raw) {
    DisplayMode mode;
    if (!camera_mode_normalize(raw, &mode)) return 0;
    for (unsigned i = 0; i < PHYSICAL_CAMERA_COUNT; ++i)
        if (mode == camera_views[i].mode) return &camera_views[i];
    return 0;
}

static inline DisplayMode camera_mode_or_overview(uint32_t raw) {
    DisplayMode mode;
    return camera_mode_normalize(raw, &mode) ? mode : DISPLAY_MODE_ALL_CAMERAS;
}

static inline const char* camera_mode_name(uint32_t raw) {
    const CameraView* view = camera_view_for_mode(raw);
    return view ? view->name : "Surround View";
}

#endif
