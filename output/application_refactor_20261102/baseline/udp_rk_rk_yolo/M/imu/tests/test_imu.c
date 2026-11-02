#include "steering_angle.h"
#include "mpu9250_transport.h"
#include "imu_source.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

static steer_result_t identity_read(void *context, uint8_t reg, uint8_t *out, size_t size)
{
    assert(reg == 0x75 && size == 1); *out = *(uint8_t *)context; return STEER_OK;
}
static steering_imu_sample_t sample(uint32_t seq, uint64_t time, float rate)
{
    steering_imu_sample_t s = {0};
    s.session_id = 1; s.sequence = seq; s.sample_time_ns = time;
    s.flags = STEER_FEEDBACK_VALID | STEER_FEEDBACK_SIMULATED;
    s.gyro_dps[2] = rate; return s;
}
int main(void)
{
    assert(mpu9250_capture_timing(1000000000,1020000000)==STEER_OK);
    assert(mpu9250_capture_timing(1000000000,1020000001)==STEER_ERR_NOT_READY);
    assert(mpu9250_capture_timing(1000000000,999999999)==STEER_ERR_STALE);
    assert(mpu9250_capture_timing(0,1000000000)==STEER_ERR_STALE);
    const float axis[3] = {0, 0, -2}, bias[3] = {0, 0, 1};
    steering_angle_t a; double angle; float rate;
    assert(steering_angle_init(&a, axis, bias, 100) == STEER_OK);
    steering_imu_sample_t s = sample(1, 1000000000, 31);
    assert(steering_angle_update(&a, &s, &angle, &rate) == STEER_OK && angle == 0 && rate == -30);
    /* Variable sample spacing with known 30 deg/s rotation; sign and bias tested. */
    s = sample(2, 1010000000, 31);
    assert(steering_angle_update(&a, &s, &angle, &rate) == STEER_OK);
    s = sample(3, 1030000000, 31);
    assert(steering_angle_update(&a, &s, &angle, &rate) == STEER_OK);
    assert(fabs(angle + .9) < 1e-6);
    assert(steering_angle_update(&a, &s, &angle, &rate) == STEER_ERR_STALE);
    ++s.sequence; s.sample_time_ns += 10000000;
    assert(steering_angle_update(&a, &s, &angle, &rate) == STEER_ERR_STALE);
    assert(steering_angle_init(&a, axis, bias, 100) == STEER_OK);
    s = sample(UINT32_MAX, 1000000000, 31);
    assert(steering_angle_update(&a, &s, &angle, &rate) == STEER_OK);
    s = sample(0, 1010000000, 31);
    assert(steering_angle_update(&a, &s, &angle, &rate) == STEER_OK); /* Wrap, not rewind. */
    s = sample(1, 1200000000, 31);
    assert(steering_angle_update(&a, &s, &angle, &rate) == STEER_ERR_STALE);
    assert(steering_angle_init(&a, axis, bias, 100) == STEER_OK);
    s = sample(1, 1000000000, 31);
    assert(steering_angle_update(&a, &s, &angle, &rate) == STEER_OK);
    s.session_id = 2; ++s.sequence; s.sample_time_ns += 10000000;
    assert(steering_angle_update(&a, &s, &angle, &rate) == STEER_ERR_STALE);
    uint8_t id = 0x71, out = 0;
    mpu9250_bus_t bus = {&id, identity_read, NULL};
    assert(mpu9250_read_identity(&bus, &out) == STEER_OK && out == 0x71);
    id = 0x70;
    assert(mpu9250_read_identity(&bus, &out) == STEER_ERR_DEVICE && out == 0x71);
    /* Unit tests never open the real device, including when run on M itself. */
    assert(mpu9250_fpga_open(NULL) == STEER_ERR_ARGUMENT);
    imu_source_t source;
    assert(imu_source_open(&source, IMU_SOURCE_MOCK, NAN) == STEER_ERR_ARGUMENT);
    assert(imu_source_open(&source, IMU_SOURCE_MOCK, 30) == STEER_OK);
    assert(imu_source_read(&source, &s) == STEER_OK);
    assert((s.flags & STEER_FEEDBACK_SIMULATED) && !(s.flags & STEER_FEEDBACK_ZERO_VALID));
    imu_source_close(&source);
    puts("PASS IMU: variable dt, axis/bias, wrap, duplicate/gap/session faults, identity and simulation isolation");
    return 0;
}
