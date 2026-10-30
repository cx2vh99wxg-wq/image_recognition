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
        source->session_id=(uint32_t)steering_now_ns()^(uint32_t)(steering_now_ns()>>32);
        if(!source->session_id)source->session_id=1;
    }
    source->opened = 1;
    return STEER_OK;
}
steer_result_t imu_source_read(imu_source_t *source, steering_imu_sample_t *sample)
{
    if (!source || !sample) return STEER_ERR_ARGUMENT;
    if (!source->opened) return STEER_ERR_NOT_READY;
    if(source->kind==IMU_SOURCE_FPGA) {
        fpga_imu_snapshot_t raw;uint64_t host=0;
        steer_result_t rc=mpu9250_fpga_snapshot(&source->bus,&raw,&host);
        if(rc!=STEER_OK)return rc;
        if(raw.error||raw.identity!=0x71||(raw.flags&7)!=7||!raw.sample_us)
            return STEER_ERR_NOT_READY;
        if(raw.now_us-raw.sample_us>50000)return STEER_ERR_STALE;
        source->captured_age_ns=(raw.now_us-raw.sample_us)*1000;
        source->capture_host_ns=host;
        if(source->anchored&&raw.generation!=source->generation)return STEER_ERR_STALE;
        if(source->anchored&&raw.sequence==source->sequence)return STEER_ERR_BUSY;
        if(!source->anchored) {
            source->anchor_fpga_us=raw.now_us;source->anchor_host_ns=host;
            source->generation=raw.generation;source->anchored=1;
        }
        int64_t delta_us=(int64_t)raw.sample_us-(int64_t)source->anchor_fpga_us;
        int64_t stamp=(int64_t)source->anchor_host_ns+delta_us*1000;
        if(stamp<=0)return STEER_ERR_STALE;
        steering_imu_sample_t s={0};s.session_id=source->session_id;
        s.sequence=raw.sequence;s.flags=STEER_FEEDBACK_VALID;s.sample_time_ns=(uint64_t)stamp;
        for(unsigned k=0;k<3;++k){s.accel_mps2[k]=raw.accel[k]*(9.80665f/16384.f);s.gyro_dps[k]=raw.gyro[k]/131.f;}
        s.temperature_c=raw.temperature/333.87f+21.f;
        source->sequence=raw.sequence;*sample=s;return STEER_OK;
    }
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
