#include "imu_stationary.h"
#include <math.h>
#include <string.h>
void imu_stationary_init(imu_stationary_t *s) { if(s)memset(s,0,sizeof(*s)); }
static int update(imu_stationary_t *s,const steering_imu_sample_t *v,const float bias[3])
{
    if(s->previous_time && (v->sample_time_ns<=s->previous_time ||
       v->sample_time_ns-s->previous_time>50000000)){
        float learned[3];memcpy(learned,s->residual_bias,sizeof(learned));
        imu_stationary_init(s);memcpy(s->residual_bias,learned,sizeof(learned));
    }
    double dt=s->previous_time?(v->sample_time_ns-s->previous_time)*1e-9:.01;
    s->previous_time=v->sample_time_ns;
    float x[6];double norm=0,raw_rate2=0,filtered_rate2=0;
    for(unsigned k=0;k<3;++k){
        x[k]=v->gyro_dps[k]-bias[k];x[k+3]=v->accel_mps2[k];
        if(!isfinite(x[k])||!isfinite(x[k+3])){imu_stationary_init(s);return 0;}
        norm+=(double)x[k+3]*x[k+3];
        float corrected=x[k]-s->residual_bias[k];raw_rate2+=(double)corrected*corrected;
        /* 30ms low-pass affects motion classification only, never integration.
         * Two raw noisy samples no longer masquerade as sustained rotation. */
        float alpha=(float)(dt/(.03+dt));
        s->filtered_rate[k]=s->filter_ready?s->filtered_rate[k]+alpha*(corrected-s->filtered_rate[k]):corrected;
        filtered_rate2+=(double)s->filtered_rate[k]*s->filtered_rate[k];
    }
    s->filter_ready=1;
    unsigned p=s->next;
    for(unsigned k=0;k<6;++k){
        if(s->count==IMU_STATIONARY_WINDOW){
            double old=s->value[p][k];s->sum[k]-=old;s->square[k]-=old*old;
        }
        s->value[p][k]=x[k];s->sum[k]+=x[k];s->square[k]+=(double)x[k]*x[k];
    }
    s->stamp[p]=v->sample_time_ns;s->next=(p+1)%IMU_STATIONARY_WINDOW;
    if(s->count<IMU_STATIONARY_WINDOW)++s->count;
    s->fast_count=filtered_rate2>.18*.18?s->fast_count+1:0;
    if(s->fast_count>3)s->fast_count=3;
    int moving=raw_rate2>1.0 || s->fast_count>=3 || fabs(sqrt(norm)-9.80665)>.7;
    int quiet=s->count==IMU_STATIONARY_WINDOW &&
        v->sample_time_ns-s->stamp[s->next]>=1800000000 &&
        v->sample_time_ns-s->stamp[s->next]<=2500000000 &&
        fabs(sqrt(norm)-9.80665)<.35 && !moving;
    double mean_rate2=0;
    for(unsigned k=0;k<6 && quiet;++k){
        double mean=s->sum[k]/s->count;
        double variance=fmax(0,s->square[k]/s->count-mean*mean);
        if(k<3){
            double residual=mean-s->residual_bias[k];mean_rate2+=residual*residual;
            double limit=s->holding?.16:.12;
            if(variance>limit*limit)quiet=0;
        }else {
            double limit=s->holding?.08:.05;
            if(variance>limit*limit || fabs(x[k]-mean)>(s->holding?.25:.15))quiet=0;
        }
    }
    double mean_limit=s->holding?.15:.10;
    if(mean_rate2>mean_limit*mean_limit)quiet=0;
    int before=s->holding;
    if(moving){s->holding=0;s->quiet_count=s->bad_count=0;}
    else if(s->holding){
        s->bad_count=quiet?0:s->bad_count+1;
        if(s->bad_count>=15){s->holding=0;s->quiet_count=0;}
    }else {
        s->quiet_count=quiet?s->quiet_count+1:0;
        if(s->quiet_count>=10){s->holding=1;s->bad_count=0;}
    }
    if(before!=s->holding)++s->transitions;
    if(before && s->holding && quiet){
        /* Adapt only after independent stationary evidence, never on entry or
         * during motion. 5s time constant, hard 0.20 dps vector bound relative
         * to the fixed calibration. No angle/zero-point correction. */
        float next[3];double size=0;
        for(unsigned k=0;k<3;++k){
            next[k]=s->residual_bias[k]+(float)(dt/(5+dt))*(float)(s->sum[k]/s->count-s->residual_bias[k]);
            size+=(double)next[k]*next[k];
        }
        double scale=size>.20*.20?.20/sqrt(size):1;
        for(unsigned k=0;k<3;++k)s->residual_bias[k]=(float)(next[k]*scale);
    }
    return s->holding;
}
steer_result_t steering_angle_update_with_hold(steering_angle_t *a,imu_stationary_t *h,
    const steering_imu_sample_t *v,double *relative,float *rate)
{
    if(!a||!h)return STEER_ERR_ARGUMENT;
    double before=a->angle_deg;
    if(!v)return STEER_ERR_ARGUMENT;
    steering_imu_sample_t corrected=*v;
    for(unsigned k=0;k<3;++k)corrected.gyro_dps[k]-=h->residual_bias[k];
    steer_result_t rc=steering_angle_update(a,&corrected,relative,rate);
    if(rc!=STEER_OK){h->holding=0;return rc;}
    if(update(h,v,a->bias_dps)){
        a->angle_deg=before;a->previous_rate_dps=0;
        *relative=before;*rate=0;
    }
    return STEER_OK;
}
