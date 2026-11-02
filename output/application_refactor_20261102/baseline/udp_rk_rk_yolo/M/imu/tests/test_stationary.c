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
    steering_angle_t a;imu_stationary_t h;double angle;float rate;steering_imu_sample_t s;
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
        steering_imu_sample_t s=sample(i,.28f);
        assert(steering_angle_update_with_hold(&a,&h,&s,&angle,&rate)==STEER_OK);
        if(i>=6010)assert(!h.holding);
    }
    assert(fabs((angle-held)-2.5)<.03);
    double moved=angle;
    for(unsigned i=7000;i<7500;++i){
        steering_imu_sample_t s=sample(i,.03f);
        assert(steering_angle_update_with_hold(&a,&h,&s,&angle,&rate)==STEER_OK);
    }
    assert(h.holding && fabs(angle-moved)<.01 && fabs(angle)>2);
    /* Modest thermal residual and isolated gyro/accel spikes must not chatter. */
    init(&a,&h);
    for(unsigned i=0;i<6000;++i){
        steering_imu_sample_t n=sample(i,.08f+.025f*sinf(i*.3f));
        if(i>300 && (i%300==0 || i%300==1))n.gyro_dps[2]=.25f;
        if(i>300 && i%701==0)n.accel_mps2[0]=.3f;
        assert(steering_angle_update_with_hold(&a,&h,&n,&angle,&rate)==STEER_OK);
        if(i==300)held=angle;
        if(i>300)assert(h.holding && angle==held && rate==0);
    }
    assert(h.transitions==1);
    /* Both signs of a genuine low constant turn still release within 100ms. */
    for(unsigned sign=0;sign<2;++sign){
        init(&a,&h);
        for(unsigned i=0;i<300;++i){s=sample(i,0);assert(steering_angle_update_with_hold(&a,&h,&s,&angle,&rate)==STEER_OK);}
        assert(h.holding);
        for(unsigned i=300;i<350;++i){
            s=sample(i,sign?.25f:-.25f);
            assert(steering_angle_update_with_hold(&a,&h,&s,&angle,&rate)==STEER_OK);
            if(i>=310)assert(!h.holding);
        }
        assert(fabs(angle)>.09);
    }
    /* Rebuild the earlier held stream for the duplicate fault check. */
    init(&a,&h);
    for(unsigned i=7000;i<7500;++i){s=sample(i,0);assert(steering_angle_update_with_hold(&a,&h,&s,&angle,&rate)==STEER_OK);}
    /* Temperature-like bias drift is learned only while already stationary. */
    init(&a,&h);
    for(unsigned i=0;i<10000;++i){
        float bias=.12f*fminf(1.f,i/6000.f);
        s=sample(i,bias+.02f*sinf(i*.2f));
        assert(steering_angle_update_with_hold(&a,&h,&s,&angle,&rate)==STEER_OK);
        if(i==300)held=angle;
        if(i>300)assert(h.holding && angle==held);
    }
    assert(h.residual_bias[2]>.11f && h.residual_bias[2]<.13f && h.transitions==1);
    for(unsigned i=10000;i<10100;++i){
        s=sample(i,.37f); // actual .25 + persistent sensor bias .12
        assert(steering_angle_update_with_hold(&a,&h,&s,&angle,&rate)==STEER_OK);
        if(i>=10010)assert(!h.holding);
    }
    assert(fabs((angle-held)-.25)<.03);
    assert(h.residual_bias[2]<.13f); // turn is not absorbed as bias
    init(&a,&h);
    for(unsigned i=7000;i<7500;++i){s=sample(i,0);assert(steering_angle_update_with_hold(&a,&h,&s,&angle,&rate)==STEER_OK);}
    /* Stationary evidence must not conceal duplicated data or faulted state. */
    s=sample(7499,0);
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
        if(i>=310)assert(!h.holding);
    }
    assert(fabs(angle-.25)<.03);
    puts("PASS stationary hold: noisy static drift bounded, nonzero angle retained, slow/fast signed motion preserved, bounded release, vibration and stale data rejected");
    return 0;
}
