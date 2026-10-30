#include "imu_stationary.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
static steering_imu_sample_t sample(unsigned i,float z)
{
    steering_imu_sample_t s={0};s.session_id=1;s.sequence=i+1;
    s.sample_time_ns=1000000000ULL+10000000ULL*i;
    s.flags=STEER_FEEDBACK_VALID|STEER_FEEDBACK_SIMULATED;
    s.accel_mps2[2]=9.80665f;s.gyro_dps[2]=z;return s;
}
static void init(steering_angle_t *a,imu_stationary_t *h)
{
    float axis[3]={0,0,1},bias[3]={0};
    assert(steering_angle_init(a,axis,bias,100)==STEER_OK);imu_stationary_init(h);
}
int main(void)
{
    steering_angle_t a;imu_stationary_t h;double angle;float rate;
    init(&a,&h);
    for(unsigned i=0;i<6000;++i){
        steering_imu_sample_t s=sample(i,.03f+.04f*sinf(i*.31f));
        s.accel_mps2[0]=.01f*sinf(i*.2f);
        assert(steering_angle_update_with_hold(&a,&h,&s,&angle,&rate)==STEER_OK);
    }
    assert(h.holding && fabs(angle)<.1 && rate==0);
    double held=angle;
    /* Slow 0.25 deg/s after a hold must release without a 2-second delay. */
    for(unsigned i=6000;i<7000;++i){
        steering_imu_sample_t s=sample(i,.25f);
        assert(steering_angle_update_with_hold(&a,&h,&s,&angle,&rate)==STEER_OK);
        if(i>=6001)assert(!h.holding);
    }
    assert(fabs((angle-held)-2.5)<.01);
    double moved=angle;
    for(unsigned i=7000;i<7500;++i){
        steering_imu_sample_t s=sample(i,0);
        assert(steering_angle_update_with_hold(&a,&h,&s,&angle,&rate)==STEER_OK);
    }
    assert(h.holding && fabs(angle-moved)<.01 && fabs(angle)>2);
    /* Stationary evidence must not conceal duplicated data or faulted state. */
    steering_imu_sample_t s=sample(7499,0);
    assert(steering_angle_update_with_hold(&a,&h,&s,&angle,&rate)==STEER_ERR_STALE);
    assert(!h.holding);
    float speeds[]={.25f,1,30,-30};
    for(unsigned k=0;k<4;++k){
        init(&a,&h);
        for(unsigned i=0;i<1000;++i){
            s=sample(i,speeds[k]);
            assert(steering_angle_update_with_hold(&a,&h,&s,&angle,&rate)==STEER_OK);
            assert(!h.holding);
        }
        assert(fabs(angle-speeds[k]*9.99)<.0001);
    }
    /* Quiet gyro alone is insufficient when the accelerometer is vibrating. */
    init(&a,&h);
    for(unsigned i=0;i<400;++i){
        s=sample(i,.03f);s.accel_mps2[0]=(i%2)?1:-1;
        assert(steering_angle_update_with_hold(&a,&h,&s,&angle,&rate)==STEER_OK);
        assert(!h.holding);
    }
    /* A tilted mounting distributes angular velocity over all three axes. */
    const float n[3]={.577350269f,.577350269f,.577350269f},zero[3]={0};
    assert(steering_angle_init(&a,n,zero,100)==STEER_OK);imu_stationary_init(&h);
    for(unsigned i=0;i<400;++i){
        s=sample(i,0);
        for(unsigned k=0;k<3;++k){s.accel_mps2[k]=9.80665f*n[k];s.gyro_dps[k]=i<300?0:.25f*n[k];}
        assert(steering_angle_update_with_hold(&a,&h,&s,&angle,&rate)==STEER_OK);
        if(i==299)assert(h.holding);
        if(i>=301)assert(!h.holding);
    }
    assert(fabs(angle-.25)<.01);
    puts("PASS stationary hold: noisy static drift bounded, nonzero angle retained, slow/fast signed motion preserved, immediate release, vibration and stale data rejected");
    return 0;
}
