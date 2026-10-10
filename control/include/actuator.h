#ifndef ACTUATOR_H
#define ACTUATOR_H
#include "driving_types.h"
/* Open-loop steering position estimate, original vehicle timings. Start with
 * wheels mechanically centred. STOP interrupts immediately, no centring wait. */
typedef struct {int32_t position; int direction; uint64_t last;} actuator_t;
void actuator_step(actuator_t *s,ControlCommand cmd,uint64_t now,uint8_t regs[5]);
#endif
