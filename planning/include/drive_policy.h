#ifndef DRIVE_POLICY_H
#define DRIVE_POLICY_H
#include "vision_frame.h"
typedef struct {
    int red_latched,person_latched,zebra_seen;
    unsigned green_frames,clear_frames;
    uint32_t last_m,last_s,clear_m,clear_s;
    uint64_t coast_until;
} drive_policy_t;
ControlCommand drive_policy_step(drive_policy_t *p,const VisionResult *m,uint64_t mt,
                                 const VisionResult *s,uint64_t st,uint64_t now);
#endif
