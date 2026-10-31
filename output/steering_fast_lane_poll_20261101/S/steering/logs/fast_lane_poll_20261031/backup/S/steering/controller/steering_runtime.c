#define _POSIX_C_SOURCE 200809L
#include "steering_runtime.h"
#include "steering_feedback.h"
#include "steering_keepalive.h"
#include "steering_platform.h"
#include "fspi_module.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/resource.h>
static steering_response_model_t model;
static steering_keepalive_t keepalive;
static ControlSnapshot snapshot;
static uint64_t snapshot_ns,lane_ns;
static uint32_t reference_session,last_frame,blocked_frame;
static uint64_t reference_notice_ns;
static uint32_t reference_notice_session;
static steer_result_t reference_notice_result;
static float zero_raw,curve_gain=.25f,target_limit=12,last_target,target_tolerance=2,center_tolerance=3;
static int initialized,confirmed,bench,have_lane,have_frame;
static int scheduling_changed,memory_locked,previous_policy;
static struct sched_param previous_schedule;
static uint64_t now_ns(void *p){(void)p;return steering_now_ns();}
int steering_runtime_requested(void){const char *s=getenv("STEERING_CONTROL_MODE");return s&&!strcmp(s,"closed-loop");}
static steer_result_t output(void *ctx,steer_direction_t d)
{
    (void)ctx;snapshot_ns=0;
    if(d==STEER_RELEASE){int a=-1,b=-1;for(int i=0;i<3&&(a||b);++i){if(a)a=fspi_write_direction_once(2,1);if(b)b=fspi_write_direction_once(3,1);}return a||b?STEER_ERR_IO:STEER_OK;}
    if(d!=STEER_LEFT&&d!=STEER_RIGHT)return STEER_ERR_ARGUMENT;
    if(fspi_write_direction_once(d==STEER_LEFT?3:2,1))return STEER_ERR_IO;
    return fspi_write_direction_once(d==STEER_LEFT?2:3,0)?STEER_ERR_IO:STEER_OK;
}
static steer_result_t read_feedback(void *ctx,steering_feedback_t *f){(void)ctx;return steering_feedback_read(f);}
static steer_result_t permission(void *ctx,steer_direction_t d)
{
    (void)ctx;uint64_t now=steering_now_ns();
    uint64_t deadline=steering_pulse_deadline_ns();
    /* An RX/status transaction can block several milliseconds in the SPI
     * driver. Starting one just before release can stretch a 30ms contact
     * beyond its timing envelope. Reuse the recent snapshot in the final
     * 6ms; the ordinary 5ms refresh makes its maximum holding time 11ms.
     * The FPGA still applies STOP/manual arbitration to the physical output.
     * Feedback freshness and the actuator deadline are checked every tick. */
    int near_release=deadline&&deadline>=now&&deadline-now<=6000000&&snapshot_ns&&now>=snapshot_ns&&now-snapshot_ns<12000000;
    if(!near_release&&(!snapshot_ns||now<snapshot_ns||now-snapshot_ns>=5000000)){
        if(fspi_read_control_snapshot(&snapshot))return STEER_ERR_IO;
        snapshot_ns=now;
    }
    if(!(snapshot.gates&GATE_RESET_RELEASED)||!(snapshot.gates&GATE_FS_ARMED)||(snapshot.gates&GATE_STOP)||!(snapshot.contacts&CONTACT_STOP_N)||
       snapshot.hmi==0xcc||snapshot.hmi==0xdd||snapshot.hmi==0xee)return STEER_ERR_BUSY;
    if(bench&&((snapshot.gates&GATE_AUTO)||!(snapshot.contacts&CONTACT_GO_N)||!(snapshot.contacts&CONTACT_BACK_N)))return STEER_ERR_BUSY;
    if(steering_pulse_active()&&d!=STEER_RELEASE){
        unsigned own=d==STEER_LEFT?CONTACT_LEFT_N:CONTACT_RIGHT_N,other=d==STEER_LEFT?CONTACT_RIGHT_N:CONTACT_LEFT_N;
        if((snapshot.contacts&own)||!(snapshot.contacts&other))return STEER_ERR_BUSY;
    }else if((snapshot.contacts&(CONTACT_LEFT_N|CONTACT_RIGHT_N))!=(CONTACT_LEFT_N|CONTACT_RIGHT_N))return STEER_ERR_BUSY;
    /* Lane solid-line permissions apply to HMI lane changes in FPGA. Curvature
     * following stays inside the lane, so it must not reinterpret those bits. */
    return STEER_OK;
}
static steer_result_t heartbeat(void *ctx)
{
    (void)ctx;ControlSnapshot s;
    if(fspi_read_control_snapshot(&s))return STEER_ERR_IO;
    steering_state_t st;steering_get_state(&st);
    printf("[STEER_HEARTBEAT] seq=%llu status=%d angle=%.3f target=%.3f feedback=%d no_motion_pulse=1\n",
        (unsigned long long)keepalive.count+1,st.status,st.angle_deg,st.target_deg,st.feedback_valid);
    return STEER_OK;
}
static float env_float(const char *name,float fallback){const char *s=getenv(name);if(!s)return fallback;char *e;float n=strtof(s,&e);return *e?NAN:n;}
steer_result_t steering_runtime_init(int bench_mode)
{
    const char *table=getenv("STEERING_RESPONSE_TABLE"),*ref=getenv("STEERING_REFERENCE_FILE");
    if(!table||!ref)return STEER_ERR_NOT_READY;
    steer_result_t rc=steering_response_load(&model,table);if(rc!=STEER_OK)return rc;
    FILE *f=fopen(ref,"r");char magic[48],extra;unsigned declared;
    if(!f)return STEER_ERR_IO;
    int ok=fscanf(f,"%47s %u %f %u",magic,&reference_session,&zero_raw,&declared)==4&&
        !strcmp(magic,"STEER_PHYSICAL_REFERENCE_V1")&&reference_session&&isfinite(zero_raw)&&declared==1;
    if(ok&&fscanf(f," %c",&extra)==1)ok=0;
    fclose(f);if(!ok)return STEER_ERR_ARGUMENT;
    if(model.burst_max_us&&(model.burst_session!=reference_session||fabsf(model.burst_zero_raw_deg-zero_raw)>.001f))return STEER_ERR_STALE;
    curve_gain=env_float("STEERING_CURVE_GAIN",.25f);target_limit=env_float("STEERING_LANE_TARGET_LIMIT_DEG",12);
    target_tolerance=env_float("STEERING_TARGET_TOLERANCE_DEG",model.burst_max_us?3:2);center_tolerance=env_float("STEERING_CENTER_TOLERANCE_DEG",3);
    float priority_f=env_float("STEERING_PRIORITY",0);
    float pulse_count_f=env_float("STEERING_MAX_CORRECTION_PULSES",20);
    float port_f=env_float("STEERING_FEEDBACK_PORT",54321);
    if(!isfinite(curve_gain)||curve_gain<=0||curve_gain>2||!isfinite(target_limit)||target_limit<1||target_limit>16||
       !isfinite(priority_f)||priority_f<0||priority_f>30||floorf(priority_f)!=priority_f||
       !isfinite(target_tolerance)||target_tolerance<.1f||target_tolerance>3||!isfinite(center_tolerance)||center_tolerance<.1f||center_tolerance>3||
       !isfinite(pulse_count_f)||pulse_count_f<1||pulse_count_f>20||floorf(pulse_count_f)!=pulse_count_f||
       !isfinite(port_f)||port_f<1||port_f>65535||floorf(port_f)!=port_f)return STEER_ERR_ARGUMENT;
    unsigned priority=(unsigned)priority_f;
    rc=steering_feedback_open("0.0.0.0","192.168.100.10",(unsigned)port_f,100);if(rc!=STEER_OK)return rc;
    steering_controller_io_t io={NULL,{NULL,output,now_ns},read_feedback,permission,&model,(unsigned)pulse_count_f,300,3,priority};
    rc=steering_bind(io);if(rc==STEER_OK){steering_config_t cfg={-18,18,target_tolerance,100,model.burst_max_us?model.burst_max_us/1000:60};rc=steering_init(&cfg);}
    if(rc!=STEER_OK){steering_feedback_close();return rc;}
    rc=output(NULL,STEER_RELEASE);if(rc!=STEER_OK){steering_shutdown();steering_feedback_close();return rc;}
    if(priority){
        struct rlimit limit;if(!getrlimit(RLIMIT_MEMLOCK,&limit)){limit.rlim_cur=limit.rlim_max;(void)setrlimit(RLIMIT_MEMLOCK,&limit);}
        memory_locked=mlockall(MCL_CURRENT)==0;
        previous_policy=sched_getscheduler(0);(void)sched_getparam(0,&previous_schedule);
        struct sched_param param={0};param.sched_priority=(int)priority;
        if(!memory_locked||previous_policy<0||sched_setscheduler(0,SCHED_FIFO,&param)){
            if(memory_locked)(void)munlockall();
            memory_locked=0;steering_shutdown();steering_feedback_close();return STEER_ERR_IO;
        }
        scheduling_changed=1;
    }
    steering_keepalive_init(&keepalive,1000);bench=bench_mode;initialized=1;confirmed=have_lane=have_frame=0;snapshot_ns=0;blocked_frame=0;
    reference_notice_ns=0;reference_notice_session=0;reference_notice_result=STEER_OK;
    printf("[STEER_INIT] table=%s reference_session=%u zero_raw=%.3f max_pulse_us=%u priority=%u target_tolerance=%.1f center_tolerance=%.1f; awaiting fresh matching session\n",table,reference_session,zero_raw,model.burst_max_us?model.burst_max_us:60000,priority,target_tolerance,center_tolerance);
    return STEER_OK;
}
void steering_runtime_cancel(void){if(initialized)(void)steering_cancel();have_lane=0;blocked_frame=last_frame;}
void steering_runtime_tick(void)
{
    if(!initialized)return;
    if(!confirmed){
        steering_feedback_t f={0};
        steer_result_t rc=steering_feedback_read(&f);
        if(rc==STEER_OK&&f.session_id==reference_session){
            rc=steering_confirm_reference(reference_session,zero_raw);
            if(rc==STEER_OK){confirmed=1;puts("[STEER_REFERENCE] explicitly confirmed current session; physical zero offset bound");return;}
        }
        uint64_t now=steering_now_ns();
        if(!reference_notice_ns||now-reference_notice_ns>=5000000000ULL||
           f.session_id!=reference_notice_session||rc!=reference_notice_result){
            const char *reason=rc!=STEER_OK?"feedback_not_ready":
                f.session_id!=reference_session?"imu_session_changed":"reference_not_ready";
            printf("[STEER_WAIT_REFERENCE] reason=%s expected_session=%u received_session=%u feedback=%s automatic_steering=blocked physical_center_confirmation_required=1\n",
                reason,reference_session,f.session_id,steer_result_name(rc));
            reference_notice_ns=now;reference_notice_session=f.session_id;reference_notice_result=rc;
        }
        return;
    }
    uint64_t now=steering_now_ns();
    if(!bench&&have_lane&&now-lane_ns>=500000000){steering_runtime_cancel();puts("[STEER_CANCEL] lane result expired");}
    (void)steering_tick();steering_state_t st;steering_get_state(&st);
    if(st.status==STEERING_CANCELLED&&st.last_error!=STEER_OK){have_lane=0;blocked_frame=last_frame;}
    /* S/R clears a controller fault only; it never binds a new M session. */
    if(!steering_pulse_active()&&(!snapshot_ns||now-snapshot_ns>=5000000)){
        if(!fspi_read_control_snapshot(&snapshot))snapshot_ns=now;
    }
    if(snapshot_ns&&now-snapshot_ns<10000000&&snapshot.hmi==0xee&&(st.status==STEERING_FAULT||st.status==STEERING_RESOLUTION_LIMIT))steering_runtime_cancel();
    static int previous=-1;
    /* A line-buffered terminal or SD-card write can block longer than a short
     * pulse. BEGIN was emitted before enable; defer state output until release. */
    if(!steering_pulse_active()&&previous!=(int)st.status){printf("[STEER_STATE] state=%d name=%s angle=%.3f target=%.3f error=%.3f result=%s\n",st.status,steering_status_name(st.status),st.angle_deg,st.target_deg,st.error_deg,steer_result_name(st.last_error));previous=st.status;}
    (void)steering_keepalive_tick(&keepalive,now,bench||steering_pulse_active(),heartbeat,NULL);
}
void steering_runtime_lane(int valid,uint32_t frame,unsigned type,unsigned confidence,unsigned road_angle)
{
    if(!initialized||!confirmed)return;
    if(!valid||confidence<60||type>2||road_angle>90){
        if(have_lane){
            steering_runtime_cancel();
            printf("[STEER_CANCEL] reason=lane_invalid_or_expired frame=%u valid=%d type=%u confidence=%u road_angle=%u\n",frame,valid,type,confidence,road_angle);
        }
        return;
    }
    if(have_frame&&frame==last_frame)return;
    have_frame=1;last_frame=frame;lane_ns=steering_now_ns();
    if(frame==blocked_frame)return;
    float target=type==0?0:fminf(target_limit,road_angle*curve_gain)*(type==1?1:-1);
    if(type&&road_angle==0){steering_runtime_cancel();return;}
    if(have_lane&&((model.burst_max_us&&type&&(target*last_target>0)&&fabsf(target-last_target)<target_tolerance)||
       (!model.burst_max_us&&fabsf(target-last_target)<1)||fabsf(target-last_target)<.001f))return;
    steer_result_t rc=steering_set_target_with_tolerance_deg(target,type==0?center_tolerance:target_tolerance);
    printf("[STEER_LANE] frame=%u type=%u confidence=%u road_angle=%u target_deg=%.3f result=%s\n",frame,type,confidence,road_angle,target,steer_result_name(rc));
    if(rc==STEER_OK){have_lane=1;last_target=target;}
}
void steering_runtime_wait(void){
    uint64_t wake=steering_now_ns()+500000,deadline=steering_pulse_deadline_ns();
    if(deadline&&deadline<wake)wake=deadline;
    struct timespec t={(time_t)(wake/1000000000),(long)(wake%1000000000)};
    (void)clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&t,NULL);
}
void steering_runtime_close(void){
    steering_shutdown();steering_feedback_close();initialized=confirmed=have_lane=0;
    if(scheduling_changed)(void)sched_setscheduler(0,previous_policy,&previous_schedule);
    if(memory_locked)(void)munlockall();
    scheduling_changed=memory_locked=0;
}
