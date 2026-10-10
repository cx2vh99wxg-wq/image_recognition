#ifndef VISION_FRAME_H
#define VISION_FRAME_H
#include <stddef.h>
#include "driving_config.h"
#define VISION_VERSION 1u
#define VISION_BOXES 48
#define VISION_POINTS 32
#define VISION_MAX_AGE_MS 2500u
#define VISION_STREAM_TIMEOUT_MS 500u
/* Fixed-width little-endian wire fields, M/S RK3568 ABI. No pointers/floats. */
typedef struct { int16_t x1,y1,x2,y2; uint16_t score,kind,camera,reserved; } VisionBox;
typedef struct {
    uint32_t version,frame_id,analysis_frame_id,age_ms;
    uint32_t healthy,person_mask,front_camera,box_count;
    LaneResult lane;
    TrafficLightResult light;
    LaneMarkResult marks;
    ZebraResult zebra;
    VisionBox boxes[VISION_BOXES];
    int16_t lane_x[2][VISION_POINTS]; /* -1 means absent; rows y=80+i*5 */
    uint16_t zebra_rows[16];
    uint32_t zebra_count;
    uint8_t drivable[80*60/8];
    uint32_t reserved;
} VisionResult;
typedef struct { uint8_t image[IMG_FRAME_BYTES]; VisionResult result; } VisionFrame;
_Static_assert(sizeof(VisionResult)==1824,"VisionResult ABI");
_Static_assert(sizeof(VisionFrame)==616224,"VisionFrame ABI");
int vision_validate(const VisionResult *v);
void vision_crop_rgb(const uint8_t *board565,int camera,uint8_t *rgb);
void vision_annotate(uint8_t *board565,const VisionResult *v);
/* All ages use local elapsed durations; M/S boot clocks are never compared. */
int vision_fresh(const VisionResult *v,uint64_t arrived,uint64_t now);
#endif
