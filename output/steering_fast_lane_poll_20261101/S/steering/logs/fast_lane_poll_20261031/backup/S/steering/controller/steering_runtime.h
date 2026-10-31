#ifndef STEERING_RUNTIME_H
#define STEERING_RUNTIME_H
#include "steering_api.h"
/* Called only by the already-initialized single FSPI owner. Init failure never
 * permits fallback to legacy pulses when closed-loop mode was requested. */
int steering_runtime_requested(void);
steer_result_t steering_runtime_init(int bench_mode);
void steering_runtime_tick(void);
void steering_runtime_wait(void);
void steering_runtime_lane(int valid,uint32_t frame,unsigned type,unsigned confidence,unsigned road_angle);
void steering_runtime_cancel(void);
void steering_runtime_close(void);
#endif
