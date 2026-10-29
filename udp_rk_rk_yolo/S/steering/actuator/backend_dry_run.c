#include "steering_backends.h"
#include "steering_platform.h"
#include <stdio.h>
static uint64_t now(void *context) { (void)context; return steering_now_ns(); }
static steer_result_t set_direction(void *context, steer_direction_t direction)
{
    (void)context;
    switch (direction) {
    case STEER_RELEASE: puts("[DRY RUN] release reg2=1 and reg3=1 (no device access)"); break;
    case STEER_LEFT: puts("[DRY RUN] right released, left requested: reg3=1, reg2=0"); break;
    case STEER_RIGHT: puts("[DRY RUN] left released, right requested: reg2=1, reg3=0"); break;
    default: return STEER_ERR_ARGUMENT;
    }
    return STEER_OK;
}
steering_actuator_io_t steering_dry_run_io(void)
{
    return (steering_actuator_io_t){NULL, set_direction, now};
}
