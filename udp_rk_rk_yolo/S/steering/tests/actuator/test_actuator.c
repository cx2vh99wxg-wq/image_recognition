#include "steering_actuator.h"
#include <assert.h>
#include <stdio.h>

typedef struct {
    uint64_t time_ns;
    unsigned calls;
    int fail_release, fail_enable;
    steer_direction_t actual;
} fake_t;
static uint64_t now(void *context) { return ((fake_t *)context)->time_ns; }
static steer_result_t set(void *context, steer_direction_t direction)
{
    fake_t *f = context; ++f->calls;
    if (direction == STEER_RELEASE && f->fail_release) return STEER_ERR_IO;
    f->actual = direction;
    /* A failed write may still have activated a contact. */
    return direction != STEER_RELEASE && f->fail_enable ? STEER_ERR_IO : STEER_OK;
}
int main(void)
{
    fake_t f = {UINT64_C(1000000000), 0, 0, 0, STEER_RELEASE};
    steering_actuator_t a;
    steering_actuator_io_t io = {&f, set, now};
    assert(steering_actuator_init(&a, io, 200) == STEER_OK);
    assert(f.calls == 0);
    assert(steering_actuator_start(&a, STEER_LEFT, 0) == STEER_ERR_ARGUMENT);
    assert(steering_actuator_start(&a, STEER_LEFT, 201) == STEER_ERR_ARGUMENT);
    assert(steering_actuator_start(&a, STEER_RELEASE, 10) == STEER_ERR_ARGUMENT);
    assert(f.calls == 0);
    assert(steering_actuator_start(&a, STEER_LEFT, 20) == STEER_OK);
    assert(f.actual == STEER_LEFT && a.status == ACTUATOR_ACTIVE);
    unsigned calls = f.calls;
    assert(steering_actuator_start(&a, STEER_RIGHT, 20) == STEER_ERR_BUSY);
    assert(f.calls == calls && f.actual == STEER_LEFT);
    f.time_ns = a.deadline_ns - 1;
    assert(steering_actuator_tick(&a) == STEER_OK && f.actual == STEER_LEFT);
    ++f.time_ns;
    assert(steering_actuator_tick(&a) == STEER_OK && f.actual == STEER_RELEASE);
    assert(a.status == ACTUATOR_IDLE);

    assert(steering_actuator_start(&a, STEER_RIGHT, 40) == STEER_OK);
    f.time_ns += 5000000;
    assert(steering_actuator_stop(&a) == STEER_OK && f.actual == STEER_RELEASE);
    assert(a.release_end_ns < a.deadline_ns); /* Explicit cancellation. */

    f.fail_enable = 1;
    assert(steering_actuator_start(&a, STEER_LEFT, 20) == STEER_ERR_IO);
    assert(a.status == ACTUATOR_FAULT && f.actual == STEER_RELEASE);
    calls = f.calls;
    assert(steering_actuator_start(&a, STEER_RIGHT, 20) == STEER_ERR_IO);
    assert(f.calls == calls);
    f.fail_enable = 0;
    assert(steering_actuator_stop(&a) == STEER_OK);
    assert(steering_actuator_start(&a, STEER_LEFT, 20) == STEER_OK);
    f.fail_release = 1; f.time_ns = a.deadline_ns;
    assert(steering_actuator_tick(&a) == STEER_ERR_IO && a.status == ACTUATOR_FAULT);
    f.fail_release = 0;
    assert(steering_actuator_stop(&a) == STEER_OK && f.actual == STEER_RELEASE);
    assert(steering_actuator_start(&a, STEER_RIGHT, 20) == STEER_OK);
    f.time_ns = 0;
    assert(steering_actuator_tick(&a) == STEER_ERR_IO && f.actual == STEER_RELEASE);
    assert(steering_actuator_stop(&a) == STEER_OK);
    assert(steering_actuator_start(&a, STEER_LEFT, 20) == STEER_ERR_IO);
    assert(a.status == ACTUATOR_FAULT && f.actual == STEER_RELEASE);
    puts("PASS actuator: bounds, deadline, mutual exclusion, cancellation, uncertain writes, release faults");
    return 0;
}
