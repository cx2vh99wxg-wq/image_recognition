#ifndef STEERING_BACKENDS_H
#define STEERING_BACKENDS_H
#include "steering_actuator.h"
typedef struct { int initialized, lock_fd; } steering_fspi_t;
steering_actuator_io_t steering_dry_run_io(void);
steer_result_t steering_fspi_open(steering_fspi_t *backend);
steering_actuator_io_t steering_fspi_io(steering_fspi_t *backend);
void steering_fspi_close(steering_fspi_t *backend);
#endif
