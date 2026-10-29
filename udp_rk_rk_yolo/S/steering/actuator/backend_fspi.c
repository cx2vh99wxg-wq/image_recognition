#include "steering_backends.h"
#include "steering_platform.h"

#ifdef STEERING_WITH_FSPI
#ifndef __linux__
#error "The FSPI backend requires Linux; build without STEERING_WITH_FSPI for dry run."
#endif
#include "fspi_module.h"
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>

static steer_result_t set_direction(void *context, steer_direction_t direction)
{
    steering_fspi_t *backend = context;
    if (!backend || !backend->initialized) return STEER_ERR_NOT_READY;
    if (direction == STEER_RELEASE) {
        /* Never short-circuit: try the other release after a write failure. */
        int left = fspi_write_data(2, 1);
        int right = fspi_write_data(3, 1);
        return left || right ? STEER_ERR_IO : STEER_OK;
    }
    if (direction != STEER_LEFT && direction != STEER_RIGHT) return STEER_ERR_ARGUMENT;
    uint16_t active = direction == STEER_LEFT ? 2 : 3;
    uint16_t opposite = direction == STEER_LEFT ? 3 : 2;
    if (fspi_write_data(opposite, 1)) return STEER_ERR_IO;
    return fspi_write_data(active, 0) ? STEER_ERR_IO : STEER_OK;
}
steer_result_t steering_fspi_open(steering_fspi_t *backend)
{
    if (!backend) return STEER_ERR_ARGUMENT;
    backend->initialized = 0; backend->lock_fd = -1;
    int fd = open("/tmp/steering-actuator.lock", O_CREAT | O_RDWR | O_NOFOLLOW, 0600);
    if (fd < 0) return STEER_ERR_IO;
    if (flock(fd, LOCK_EX | LOCK_NB)) { close(fd); return STEER_ERR_BUSY; }
    if (fspi_init()) { close(fd); return STEER_ERR_IO; }
    backend->initialized = 1; backend->lock_fd = fd;
    return STEER_OK;
}
void steering_fspi_close(steering_fspi_t *backend)
{
    if (!backend) return;
    if (backend->initialized) {
        (void)set_direction(backend, STEER_RELEASE);
        fspi_cleanup(); backend->initialized = 0;
    }
    if (backend->lock_fd >= 0) { close(backend->lock_fd); backend->lock_fd = -1; }
}
#else
static steer_result_t set_direction(void *context, steer_direction_t direction)
{
    (void)context; (void)direction; return STEER_ERR_NOT_READY;
}
steer_result_t steering_fspi_open(steering_fspi_t *backend)
{
    if (backend) { backend->initialized = 0; backend->lock_fd = -1; }
    return backend ? STEER_ERR_NOT_READY : STEER_ERR_ARGUMENT;
}
void steering_fspi_close(steering_fspi_t *backend) { (void)backend; }
#endif

static uint64_t now(void *context) { (void)context; return steering_now_ns(); }
steering_actuator_io_t steering_fspi_io(steering_fspi_t *backend)
{
    return (steering_actuator_io_t){backend, set_direction, now};
}
