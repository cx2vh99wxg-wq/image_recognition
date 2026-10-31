/* Real saved raw-sample replay. No hardware, network or actuator access. */
#include "imu_stationary.h"
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    FILE *csv=fopen(argv[1],"r"),*cal=fopen(argv[2],"r");
    if(!csv||!cal)return 2;
    float axis[3],bias[3];char line[2048],magic[16];
    if(fscanf(cal,"%15s axis %f %f %f bias %f %f %f",magic,
       &axis[0],&axis[1],&axis[2],&bias[0],&bias[1],&bias[2])!=7||strcmp(magic,"IMUCAL1"))return 2;
    fclose(cal);steering_angle_t a;imu_stationary_t h;imu_stationary_init(&h);
    if(steering_angle_init(&a,axis,bias,100)!=STEER_OK)return 2;
    if(!fgets(line,sizeof(line),csv))return 2;
    unsigned samples=0,held=0,post=0,post_held=0,changes=0;int previous=0;
    double angle=0,initial=0,last=0,lo=1e9,hi=-1e9;uint64_t start=0;
    while(fgets(line,sizeof(line),csv)){
        char *col[20],*save=NULL;unsigned count=0;
        for(char *p=strtok_r(line,",",&save);p&&count<20;p=strtok_r(NULL,",",&save))col[count++]=p;
        if(count<12)return 2;
        steering_imu_sample_t s={0};s.session_id=(uint32_t)strtoul(col[1],NULL,10);
        s.sequence=(uint32_t)strtoul(col[2],NULL,10);s.sample_time_ns=strtoull(col[3],NULL,10);
        s.flags=(uint32_t)strtoul(col[4],NULL,10);
        for(unsigned k=0;k<3;++k){s.accel_mps2[k]=strtof(col[5+k],NULL);s.gyro_dps[k]=strtof(col[8+k],NULL);}
        float rate=0;
        if(steering_angle_update_with_hold(&a,&h,&s,&angle,&rate)!=STEER_OK)return 1;
        if(!samples){start=s.sample_time_ns;initial=angle;}
        ++samples;held+=(unsigned)h.holding;
        if(previous!=h.holding)++changes;
        previous=h.holding;last=(s.sample_time_ns-start)*1e-9;
        if(last>=5){++post;post_held+=(unsigned)h.holding;lo=fmin(lo,angle);hi=fmax(hi,angle);}
    }
    fclose(csv);
    printf("REPLAY samples=%u seconds=%.3f hold_fraction=%.6f post5_hold_fraction=%.6f transitions=%u angle_change=%.6f post5_range_deg=%.6f initial_deg=%.6f final_deg=%.6f\n",
        samples,last,samples?(double)held/samples:0,post?(double)post_held/post:0,changes,
        angle-initial,post?hi-lo:0,initial,angle);
    return samples?0:1;
}
