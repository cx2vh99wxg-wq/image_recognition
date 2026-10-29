#include "imu_source.h"
#include "steering_platform.h"
#include <math.h>
#include <string.h>

steer_result_t imu_source_open(imu_source_t *source, imu_source_kind_t kind, float rate)
{
    if (!source || (kind != IMU_SOURCE_MOCK && kind != IMU_SOURCE_FPGA) ||
        !isfinite(rate) || fabsf(rate) > 2000) return STEER_ERR_ARGUMENT;
    memset(source, 0, sizeof(*source)); source->kind = kind; source->mock_rate_dps = rate;
    if (kind == IMU_SOURCE_FPGA) {
        steer_result_t rc = mpu9250_fpga_open(&source->bus);
        if (rc != STEER_OK) return rc;
        /* Register configuration, data-ready/FIFO handling, scaling and complete
         * timestamped motion snapshots still need B's implementation. */
        mpu9250_fpga_close(&source->bus);
        return STEER_ERR_NOT_READY;
    }
    source->opened = 1;
    return STEER_OK;
}
steer_result_t imu_source_read(imu_source_t *source, steering_imu_sample_t *sample)
{
    if (!source || !sample) return STEER_ERR_ARGUMENT;
    if (!source->opened || source->kind != IMU_SOURCE_MOCK) return STEER_ERR_NOT_READY;
    uint64_t now = steering_now_ns();
    if (!now) return STEER_ERR_IO;
    *sample = (steering_imu_sample_t){0};
    sample->session_id = 1; sample->sequence = ++source->sequence;
    sample->flags = STEER_FEEDBACK_VALID | STEER_FEEDBACK_SIMULATED;
    sample->sample_time_ns = now;
    sample->accel_mps2[2] = 9.80665f;
    sample->gyro_dps[2] = source->mock_rate_dps;
    sample->temperature_c = 25.0f;
    return STEER_OK;
}
void imu_source_close(imu_source_t *source)
{
    if (!source) return;
    mpu9250_fpga_close(&source->bus); source->opened = 0;
}
