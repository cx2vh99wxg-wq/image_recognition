#ifndef VISION_WORKER_H
#define VISION_WORKER_H
#include "vision_frame.h"
typedef struct vision_worker vision_worker_t;
typedef struct { const char *person_model,*lane_model; int master,interval,fps; } vision_worker_cfg_t;
int vision_worker_start(vision_worker_t **out,const vision_worker_cfg_t *cfg);
int vision_worker_snapshot(vision_worker_t *worker,VisionFrame *frame,uint64_t *capture_us);
void vision_worker_stop(vision_worker_t *worker);
#endif
