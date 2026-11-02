/* Bench-only simulated lane input -> real runtime/FSPI/IMU. No perception or HMI writes. */
#define _POSIX_C_SOURCE 200809L
#include "steering_api.h"
#include "steering_platform.h"
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef STEERING_WITH_FSPI
#include "steering_runtime.h"
#include "fspi_module.h"
#endif
static volatile sig_atomic_t cancelled;
static void cancel_handler(int n){(void)n;cancelled=1;}
int main(int argc,char **argv)
{
    const char *model=NULL,*reference=NULL;float target=NAN,current=0,tolerance=1;int hardware=0,fixed=0;
    for(int i=1;i<argc;++i){
        if(!strcmp(argv[i],"--fixed-chassis")){fixed=1;continue;}
        if(i+1>=argc)return 2;
        const char *k=argv[i],*v=argv[++i];char *e;
        if(!strcmp(k,"--model"))model=v;
        else if(!strcmp(k,"--reference"))reference=v;
        else if(!strcmp(k,"--backend")){if(strcmp(v,"fspi")&&strcmp(v,"dry-run"))return 2;hardware=!strcmp(v,"fspi");}
        else if(!strcmp(k,"--target-deg")){target=strtof(v,&e);if(*e)return 2;}
        else if(!strcmp(k,"--current-deg")){current=strtof(v,&e);if(*e)return 2;}
        else if(!strcmp(k,"--tolerance-deg")){tolerance=strtof(v,&e);if(*e)return 2;}
        else return 2;
    }
    if(!model||!isfinite(target)||!isfinite(current)||fabsf(target)>16)return 2;
    if(!isfinite(tolerance)||tolerance<.1f||tolerance>3)return 2;
    unsigned road_angle=(unsigned)lroundf(fabsf(target)*4);
    if(fabsf(road_angle-fabsf(target)*4)>.001f)return 2;
    if(!hardware){
        steering_response_model_t table;steering_response_choice_t d={0};
        if(steering_response_load(&table,model)!=STEER_OK)return 1;
        steer_result_t rc=steering_response_choose(&table,current,target,tolerance,-18,18,60000,2,0,&d);
        printf("PLAN_ONLY_SIMULATED result=%s direction=%d duration_us=%u no_device_access=1\n",steer_result_name(rc),d.direction,d.duration_us);
        return rc==STEER_OK?0:1;
    }
#ifdef STEERING_WITH_FSPI
    if(!fixed||!reference||access("/tmp/steering-bench.active",F_OK)){fputs("Explicit fixed-chassis isolation/reference required\n",stderr);return 2;}
    setvbuf(stdout,NULL,_IOLBF,0);signal(SIGINT,cancel_handler);signal(SIGTERM,cancel_handler);
    if(fspi_init())return 1;
    setenv("STEERING_RESPONSE_TABLE",model,1);setenv("STEERING_REFERENCE_FILE",reference,1);
    char limit[32],band[32];snprintf(limit,sizeof(limit),"%.3f",fmaxf(15,fabsf(target)));snprintf(band,sizeof(band),"%.3f",tolerance);
    setenv("STEERING_CURVE_GAIN","0.25",1);setenv("STEERING_LANE_TARGET_LIMIT_DEG",limit,1);
    setenv("STEERING_TARGET_TOLERANCE_DEG",band,1);setenv("STEERING_CENTER_TOLERANCE_DEG",band,1);
    steer_result_t rc=steering_runtime_init(1);int done=0,requested=0;
    uint64_t next_lane=0;uint32_t frame=0;unsigned type=target==0?0:target>0?1:2;
    puts("LANE_BENCH input=SIMULATED_LANE_INPUT feedback=REAL_M_UDP actuator=REAL_FSPI perception_and_hmi_unchanged=1");
    uint64_t end=steering_now_ns()+UINT64_C(25000000000);
    if(rc==STEER_OK)while(!cancelled&&steering_now_ns()<end){
        steering_runtime_tick();steering_state_t s;steering_get_state(&s);
        uint64_t now=steering_now_ns();
        if(s.feedback_valid&&now>=next_lane){
            steering_runtime_lane(1,++frame,type,90,road_angle);next_lane=now+100000000;
            steering_get_state(&s);requested=isfinite(s.target_deg)&&fabsf(s.target_deg-target)<.01f;
        }
        if(requested&&(s.status==STEERING_SETTLED||s.status==STEERING_FAULT||s.status==STEERING_RESOLUTION_LIMIT||s.status==STEERING_CANCELLED)){
            printf("GOAL_RESULT status=%d target_deg=%.3f measured_deg=%.3f error_deg=%.3f tolerance_deg=%.1f result=%s state_name=%s\n",s.status,target,s.angle_deg,s.error_deg,tolerance,steer_result_name(s.last_error),steering_status_name(s.status));
            done=s.status==STEERING_SETTLED;break;
        }
        steering_runtime_wait();
    }
    steering_runtime_close();fspi_cleanup();return done?0:1;
#else
    (void)reference;(void)fixed;(void)cancel_handler;fputs("FSPI backend requires Linux hardware build\n",stderr);return 2;
#endif
}
