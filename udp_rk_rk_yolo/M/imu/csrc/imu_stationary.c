#include "imu_stationary.h"
#include <math.h>
#include <string.h>
void imu_stationary_init(imu_stationary_t *s) { if(s)memset(s,0,sizeof(*s)); }
static int update(imu_stationary_t *s,const steering_imu_sample_t *v,const float bias[3])
{
    if(s->previous_time && (v->sample_time_ns<=s->previous_time ||
       v->sample_time_ns-s->previous_time>50000000))imu_stationary_init(s);
    s->previous_time=v->sample_time_ns;
    float x[6];double norm=0,gyro_norm=0;
    for(unsigned k=0;k<3;++k){
        x[k]=v->gyro_dps[k]-bias[k];x[k+3]=v->accel_mps2[k];
        if(!isfinite(x[k])||!isfinite(x[k+3])){imu_stationary_init(s);return 0;}
        norm+=(double)x[k+3]*x[k+3];
        gyro_norm+=(double)x[k]*x[k];
    }
    unsigned p=s->next;
    for(unsigned k=0;k<6;++k){
        if(s->count==IMU_STATIONARY_WINDOW){
            double old=s->value[p][k];s->sum[k]-=old;s->square[k]-=old*old;
        }
        s->value[p][k]=x[k];s->sum[k]+=x[k];s->square[k]+=(double)x[k]*x[k];
    }
    s->stamp[p]=v->sample_time_ns;s->next=(p+1)%IMU_STATIONARY_WINDOW;
    if(s->count<IMU_STATIONARY_WINDOW)++s->count;
    double rate_norm=sqrt(gyro_norm);
    s->fast_count=rate_norm>=.20 ? s->fast_count+1 : 0;
    if(s->fast_count>2)s->fast_count=2;
    int quiet=s->count==IMU_STATIONARY_WINDOW &&
        v->sample_time_ns-s->stamp[s->next]>=1800000000 &&
        v->sample_time_ns-s->stamp[s->next]<=2500000000 &&
        fabs(sqrt(norm)-9.80665)<.35 && rate_norm<.5 && s->fast_count<2;
    for(unsigned k=0;k<6 && quiet;++k){
        double mean=s->sum[k]/s->count;
        double variance=fmax(0,s->square[k]/s->count-mean*mean);
        if(k<3){
            if(fabs(mean)>(s->holding?.08:.06)||variance>.12*.12)quiet=0;
        }else if(variance>.035*.035 || fabs(x[k]-mean)>.12)quiet=0;
    }
    s->holding=quiet;return quiet;
}
steer_result_t steering_angle_update_with_hold(steering_angle_t *a,imu_stationary_t *h,
    const steering_imu_sample_t *v,double *relative,float *rate)
{
    if(!a||!h)return STEER_ERR_ARGUMENT;
    double before=a->angle_deg;
    steer_result_t rc=steering_angle_update(a,v,relative,rate);
    if(rc!=STEER_OK){h->holding=0;return rc;}
    if(update(h,v,a->bias_dps)){
        a->angle_deg=before;a->previous_rate_dps=0;
        *relative=before;*rate=0;
    }
    return STEER_OK;
}
