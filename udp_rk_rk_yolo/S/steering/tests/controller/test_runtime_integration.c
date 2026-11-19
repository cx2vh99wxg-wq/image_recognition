/* Fake registers/plant only. The real runtime+UDP cache are exercised, no SPI. */
#include "steering_runtime.h"
#include "steering_feedback.h"
#include "steering_feedback_wire.h"
#include "steering_platform.h"
#include "fspi_module.h"
#include "steering_center.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
static ControlSnapshot pins;
static uint64_t enabled;
static unsigned starts;static int active;
static float angle;
static uint32_t seq,frame,feedback_session=7;
static int slow_release_window,late_status_reads;
static uint32_t lane_frame;
static unsigned lane_confidence,lane_reads;
static unsigned calibration_requests;
uint32_t __wrap_steering_feedback_center_session(void) { return 4242; }
int __wrap_steering_feedback_request_center(uint32_t session)
{
    assert(session == 4242 && !active);
    ++calibration_requests;
    return 0;
}
static int read_lane(void *context,uint32_t *id,unsigned *kind,unsigned *quality,unsigned *road_angle){
    (void)context;assert(!active);++lane_reads;
    *id=lane_frame;*kind=1;*quality=lane_confidence;*road_angle=20;return 0;
}
int printf(const char *format,...){
    assert(!(active&&strstr(format,"[STEER_STATE]")));
    va_list args;va_start(args,format);int result=vfprintf(stdout,format,args);va_end(args);return result;
}
int fspi_write_data(uint16_t reg,uint8_t value){
    assert(reg==2||reg==3);assert(value<=1);
    unsigned mask=reg==2?CONTACT_LEFT_N:CONTACT_RIGHT_N;
    if(value&&active==(int)reg){angle+=(reg==2?1.f:-1.f)*(float)((steering_now_ns()-enabled)*1e-9)*500;active=0;}
    if(value)pins.contacts|=mask;else {pins.contacts&=(uint8_t)~mask;active=reg;enabled=steering_now_ns();++starts;}
    return 0;
}
int fspi_write_direction_once(uint16_t r,uint8_t v){return fspi_write_data(r,v);}
int fspi_read_control_snapshot(ControlSnapshot *out){
    if(slow_release_window&&active&&steering_now_ns()-enabled>=4000000){
        ++late_status_reads;steering_sleep_ms(7);
    }
    *out=pins;return 0;
}
static void feed(void){
    steering_feedback_t s={feedback_session,++seq,1,(int32_t)lroundf(angle*1000),0,0,steering_now_ns()};uint8_t wire[48];
    assert(steering_feedback_encode(wire,48,&s)==STEER_OK);
    assert(steering_feedback_ingest(wire,48,steering_now_ns())==STEER_OK);
}

static void hold_sr(void)
{
    const uint64_t start = steering_now_ns();
    do
    {
        feed();
        steering_runtime_center_input(0xee, 1, 1);
        steering_runtime_tick();
        steering_sleep_ms(8);
    } while (steering_now_ns() - start < CENTER_STOP_HOLD_NS + 25000000ULL);
    assert(steering_runtime_center_input(0xee, 1, 1) == STEERING_CENTER_RELEASE_STOP);
}

static void check_center_rejections(void)
{
    const uint64_t now = 1000000000ULL;
    SteeringCenter center = {.status = STEERING_CENTER_HOLD_STOP, .session = 8};
    steering_center_key(&center, 0xee, now);
    steering_center_key(&center, 0, now + CENTER_STOP_HOLD_NS - 1);
    steering_center_key(&center, 0x99, now + CENTER_STOP_HOLD_NS);
    assert(center.status == STEERING_CENTER_HOLD_STOP);
    steering_center_key(&center, 0xee, now);
    steering_center_key(&center, 0xee, now + CENTER_STOP_HOLD_NS);
    steering_center_key(&center, 0, now + CENTER_STOP_HOLD_NS);
    steering_center_key(&center, 0x44, now + CENTER_STOP_HOLD_NS + 18000000000ULL);
    assert(center.status == STEERING_CENTER_TAP_OVERVIEW);
    steering_center_key(&center, 0x99, now + CENTER_STOP_HOLD_NS + CENTER_CONFIRM_NS);
    assert(center.status == STEERING_CENTER_HOLD_STOP);

    steering_feedback_t sample = {8, 1, STEER_FEEDBACK_VALID, 0, 0, 0, now};
    assert(steering_center_sample(&center, &sample, now) == 0);
    assert(steering_center_sample(&center, &sample, now + 10000000ULL) == 0 && center.count == 1);
    ++sample.sequence;
    sample.sample_time_ns += 20000000ULL;
    sample.angle_mdeg = 401;
    assert(steering_center_sample(&center, &sample, now + 20000000ULL) < 0);
    sample.angle_mdeg = 0;
    assert(steering_center_sample(&center, &sample, now + CENTER_SAMPLE_GAP_NS) < 0);
}

int main(int argc,char **argv){
    assert(argc==4);setenv("STEERING_RESPONSE_TABLE",argv[1],1);setenv("STEERING_REFERENCE_FILE",argv[2],1);setenv("STEERING_FEEDBACK_PORT",argv[3],1);
    check_center_rejections();
    setenv("STEERING_PRIORITY","0",1);setenv("STEERING_CURVE_GAIN","0.25",1);setenv("STEERING_LANE_TARGET_LIMIT_DEG","12",1);
    pins.contacts=CONTACT_GO_N|CONTACT_BACK_N|CONTACT_LEFT_N|CONTACT_RIGHT_N|CONTACT_STOP_N;
    pins.gates=GATE_RESET_RELEASED|GATE_FS_ARMED;
    assert(steering_runtime_init(0)==STEER_OK);feed();steering_runtime_tick();
    uint64_t start=steering_now_ns(),next_feed=0,next_lane=0;
    while(steering_now_ns()-start<1200000000){
        uint64_t n=steering_now_ns();if(n>=next_feed){feed();next_feed=n+10000000;}
        if(n>=next_lane){steering_runtime_lane(1,++frame,1,90,20);next_lane=n+100000000;}
        steering_runtime_tick();steering_sleep_ms(1);
    }
    steering_state_t state;steering_get_state(&state);assert(state.status==STEERING_SETTLED&&fabsf(state.angle_deg-5)<1&&starts==1);
    steering_sleep_ms(110);steering_runtime_tick();steering_get_state(&state);assert(!state.feedback_valid&&!active);
    feed();pins.hmi=0xee;steering_runtime_tick();steering_sleep_ms(6);feed();steering_runtime_tick();pins.hmi=0;
    feed();steering_runtime_lane(1,++frame,2,90,20);
    start=steering_now_ns();int interrupted=0;
    while(steering_now_ns()-start<1000000000){
        feed();steering_runtime_lane(1,++frame,2,90,20);steering_runtime_tick();
        if(active){pins.gates|=GATE_STOP;interrupted=1;}
        if(interrupted&&!active)break;
        steering_sleep_ms(1);
    }
    assert(interrupted&&!active);steering_runtime_close();
    /* Replay the observed cadence: 3.3Hz NN with only 80ms validity left
     * after inference/transport, then explicit confidence=0 expiry. */
    starts=0;angle=0;active=0;pins.hmi=0;
    pins.contacts=CONTACT_GO_N|CONTACT_BACK_N|CONTACT_LEFT_N|CONTACT_RIGHT_N|CONTACT_STOP_N;
    pins.gates=GATE_RESET_RELEASED|GATE_FS_ARMED;
    assert(steering_runtime_init(0)==STEER_OK);feed();steering_runtime_tick();
    start=steering_now_ns();next_feed=0;
    while(steering_now_ns()-start<450000000){
        uint64_t n=steering_now_ns();if(n>=next_feed){feed();next_feed=n+10000000;}
        steering_runtime_tick();steering_sleep_ms(1);
    }
    start=steering_now_ns();next_feed=next_lane=0;
    while(steering_now_ns()-start<1200000000){
        uint64_t n=steering_now_ns(),elapsed=n-start;
        if(n>=next_feed){feed();next_feed=n+10000000;}
        if(n>=next_lane){
            uint32_t id=1000+(uint32_t)(elapsed/300000000);
            steering_runtime_lane(1,id,1,elapsed%300000000<80000000?90:0,20);
            next_lane=n+10000000;
        }
        steering_runtime_tick();steering_sleep_ms(1);
    }
    assert(starts==1&&fabsf(angle-5)<2&&!active);
    feed();steering_runtime_lane(1,2000,1,90,20);steering_runtime_tick();steering_get_state(&state);
    assert(state.status==STEERING_SETTLED&&starts==1);
    steering_runtime_close();
    /* A status RX started 4-6ms before a 10ms release may block for 7ms.
     * The previous 2ms exclusion window admitted that transaction and
     * stretched the contact past its +2ms limit. Exercise real runtime
     * scheduling with delayed fake SPI, not an adjusted timing tolerance. */
    starts=0;angle=0;active=0;pins.hmi=0;late_status_reads=0;
    pins.contacts=CONTACT_GO_N|CONTACT_BACK_N|CONTACT_LEFT_N|CONTACT_RIGHT_N|CONTACT_STOP_N;
    pins.gates=GATE_RESET_RELEASED|GATE_FS_ARMED;
    assert(steering_runtime_init(0)==STEER_OK);feed();steering_runtime_tick();
    start=steering_now_ns();next_feed=0;slow_release_window=1;
    while(steering_now_ns()-start<1200000000){
        uint64_t n=steering_now_ns();if(n>=next_feed){feed();next_feed=n+10000000;}
        steering_runtime_lane(1,++frame,1,90,20);
        steering_runtime_tick();steering_sleep_ms(1);
    }
    steering_get_state(&state);
    assert(starts==1&&!active&&late_status_reads==0&&state.status==STEERING_SETTLED&&fabsf(angle-5)<1);
    slow_release_window=0;steering_runtime_close();
    puts("PASS delayed SPI regression: release deadline retained with 7ms status RX latency; FAKE IO only");
    puts("PASS intermittent NN regression: 3.3Hz, 80ms valid window, expiry releases, idle IMU history retained; FAKE IO only");
    /* An NN result may be valid only in [120,180)ms of each 300ms period.
     * A 100ms consumer misses it even though a 30fps display can show it.
     * Exercise the production 10ms poll API against the same input/plant. */
    for(int fast=0;fast<2;++fast){
        starts=0;active=0;angle=0;lane_reads=0;
        assert(steering_runtime_init(0)==STEER_OK);feed();steering_runtime_tick();
        start=steering_now_ns();
        while(steering_now_ns()-start<450000000){feed();steering_runtime_tick();steering_sleep_ms(1);}
        start=steering_now_ns();next_lane=0;
        while(steering_now_ns()-start<1200000000){
            uint64_t elapsed=steering_now_ns()-start,phase=elapsed%300000000;
            lane_frame=3000+(uint32_t)(elapsed/300000000);
            lane_confidence=phase>=120000000&&phase<180000000?90:0;
            feed();
            if(fast)steering_runtime_poll_lane(1,read_lane,NULL);
            else if(elapsed>=next_lane){
                uint32_t id;unsigned kind,quality,road;
                read_lane(NULL,&id,&kind,&quality,&road);
                steering_runtime_lane(1,id,kind,quality,road);
                next_lane=(elapsed/100000000+1)*100000000;
            }
            steering_runtime_tick();steering_sleep_ms(1);
        }
        assert(!active);
        if(fast)assert(starts==1&&fabsf(angle-5)<2&&lane_reads>40);
        else assert(starts==0&&fabsf(angle)<.01f);
        steering_runtime_close();
    }
    /* Policy denial and explicit bench mode cannot be bypassed by polling. */
    starts=0;active=0;angle=0;lane_reads=0;lane_frame=4000;lane_confidence=90;
    assert(steering_runtime_init(0)==STEER_OK);feed();steering_runtime_tick();
    start=steering_now_ns();
    while(steering_now_ns()-start<450000000){feed();steering_runtime_poll_lane(0,read_lane,NULL);steering_runtime_tick();steering_sleep_ms(1);}
    assert(!active&&starts==0);steering_runtime_close();
    lane_reads=0;assert(steering_runtime_init(1)==STEER_OK);feed();steering_runtime_tick();
    steering_runtime_poll_lane(1,read_lane,NULL);assert(!lane_reads&&!starts);steering_runtime_close();
    puts("PASS short-valid-lane regression: 100ms polling misses 60ms windows, 10ms poll acts, expiry releases, denial and bench isolation retained; FAKE IO only");
    /* A new M boot must remain visibly blocked by the old physical reference.
     * Sending plausible curve/angle data must not silently rebind that zero. */
    starts=0;active=0;angle=0;feedback_session=8;
    assert(steering_runtime_init(0)==STEER_OK);
    for(unsigned i=0;i<10;++i){
        feed();steering_runtime_tick();
        steering_runtime_lane(1,++frame,1,90,25);
        steering_sleep_ms(1);
    }
    assert(!active&&starts==0);
    angle=3.40f;
    feed();
    assert(steering_runtime_center_input(0x99,1,1)==STEERING_CENTER_HOLD_STOP);
    assert(steering_runtime_center_input(0xee,1,0)==STEERING_CENTER_SWITCH_OFF);
    hold_sr();
    assert(steering_runtime_center_input(0,1,1)==STEERING_CENTER_TAP_OVERVIEW);
    assert(steering_runtime_center_input(0x99,1,1)==STEERING_CENTER_SAMPLING);
    start=steering_now_ns();
    while(steering_runtime_center_sampling()&&steering_now_ns()-start<2500000000ULL){
        feed();
        steering_runtime_center_input(0x99,1,1);
        steering_runtime_tick();
        steering_sleep_ms(8);
    }
    assert(steering_runtime_center_input(0x99,1,1)==STEERING_CENTER_READY);
    steering_get_state(&state);
    assert(!active&&!starts&&state.feedback_valid&&fabsf(state.angle_deg)<.01f);
    FILE *saved=fopen(argv[2],"r");
    unsigned saved_session=0;
    float saved_angle=0.f;
    assert(saved && fscanf(saved,"STEER_PHYSICAL_REFERENCE_V1 %u %f 1",&saved_session,&saved_angle)==2);
    fclose(saved);
    assert(saved_session==8&&fabsf(saved_angle-3.4f)<.001f);
    steering_runtime_close();
    assert(steering_runtime_init(0)==STEER_OK);feed();steering_runtime_tick();steering_get_state(&state);
    assert(!active&&!starts&&state.feedback_valid&&fabsf(state.angle_deg)<.01f);
    assert(steering_runtime_center_input(0xee,1,1)==STEERING_CENTER_HIDDEN);
    start=steering_now_ns();
    while(steering_now_ns()-start<1200000000ULL){
        feed();steering_runtime_lane(1,++frame,1,90,20);steering_runtime_tick();steering_sleep_ms(1);
    }
    steering_get_state(&state);
    assert(!active&&starts==1&&state.status==STEERING_SETTLED&&fabsf(state.angle_deg-5)<1);
    steering_runtime_close();
    puts("PASS new IMU session: no automatic pulses before explicit HMI confirmation; saved zero reloads on S restart");

    /* Same-session range fault must keep the lane inhibited across OFF/ON,
     * feedback recovery and a return into range until the operator saves a zero. */
    starts = 0;
    angle = 3.4f;
    assert(steering_runtime_init(0) == STEER_OK);
    feed();
    steering_runtime_tick();
    angle = -14.8f;
    feed();
    steering_runtime_tick();
    assert(steering_runtime_center_input(0, 1, 0) == STEERING_CENTER_LIMIT_SWITCH_OFF);
    steering_sleep_ms(110);
    assert(steering_runtime_center_input(0, 1, 1) == STEERING_CENTER_LIMIT_WAIT_IMU);

    angle = 4.25f;
    feed();
    steering_runtime_tick();
    assert(steering_runtime_center_input(0xee, 0, 1) == STEERING_CENTER_LIMIT_HOLD_STOP);
    assert(steering_runtime_center_input(0, 1, 0) == STEERING_CENTER_LIMIT_SWITCH_OFF);
    steering_runtime_lane(1, 5000, 1, 90, 20);
    assert(!active && !starts);

    assert(steering_runtime_center_input(0xee, 1, 1) == STEERING_CENTER_LIMIT_HOLD_STOP);
    assert(steering_runtime_center_input(0, 1, 0) == STEERING_CENTER_LIMIT_SWITCH_OFF);
    assert(!steering_runtime_center_sampling() && !starts);

    hold_sr();
    assert(steering_runtime_center_input(0, 1, 1) == STEERING_CENTER_TAP_OVERVIEW);
    assert(steering_runtime_center_input(0x99, 1, 1) == STEERING_CENTER_SAMPLING);
    start = steering_now_ns();
    while (steering_runtime_center_sampling() && steering_now_ns() - start < 2500000000ULL)
    {
        feed();
        steering_runtime_center_input(0x99, 1, 1);
        steering_runtime_lane(1, 5000, 1, 90, 20);
        steering_runtime_tick();
        steering_sleep_ms(8);
    }
    assert(steering_runtime_center_input(0, 1, 1) == STEERING_CENTER_READY);
    steering_get_state(&state);
    assert(!active && !starts && state.feedback_valid && fabsf(state.angle_deg) < .01f);
    saved = fopen(argv[2], "r");
    assert(saved && fscanf(saved, "STEER_PHYSICAL_REFERENCE_V1 %u %f 1", &saved_session, &saved_angle) == 2);
    fclose(saved);
    assert(saved_session == 8 && fabsf(saved_angle - 4.25f) < .001f);

    start = steering_now_ns();
    while (steering_now_ns() - start < 400000000ULL)
    {
        feed();
        steering_runtime_lane(1, 5000, 1, 90, 20);
        steering_runtime_tick();
        steering_sleep_ms(1);
    }
    assert(!active && !starts);
    start = steering_now_ns();
    while (steering_now_ns() - start < 1200000000ULL)
    {
        feed();
        const uint32_t id = 5001 + (uint32_t)((steering_now_ns() - start) / 100000000ULL);
        steering_runtime_lane(1, id, 1, 90, 20);
        steering_runtime_tick();
        steering_sleep_ms(1);
    }
    steering_get_state(&state);
    assert(!active && starts == 1 && state.status == STEERING_SETTLED && fabsf(state.angle_deg - 5) < 1);
    steering_runtime_close();

    angle = 22.5f;
    assert(steering_runtime_init(0) == STEER_OK);
    feed();
    steering_runtime_tick();
    assert(steering_runtime_center_input(0, 1, 1) == STEERING_CENTER_LIMIT_HOLD_STOP);
    angle = 4.25f;
    feed();
    steering_runtime_tick();
    assert(steering_runtime_center_input(0, 1, 1) == STEERING_CENTER_LIMIT_HOLD_STOP);
    assert(!active && starts == 1);
    steering_runtime_close();
    puts("PASS steering limit recovery: same-session manual zero, switch interlock, and fresh lane required; FAKE IO only");

    /* Production cold start accepts the full gesture before M sends angles. */
    assert(setenv("STEERING_STARTUP_CALIBRATION", "1", 1) == 0);
    starts = 0;
    assert(steering_runtime_init(0) == STEER_OK);
    assert(!steering_runtime_autopilot_ready());
    assert(steering_runtime_center_input(0, 1, 1) == STEERING_CENTER_HOLD_STOP);
    start=steering_now_ns();
    do {
        steering_runtime_center_input(0xee, 1, 1);
        steering_runtime_tick();
        steering_sleep_ms(8);
    } while (steering_now_ns()-start<CENTER_STOP_HOLD_NS+25000000ULL);
    assert(steering_runtime_center_input(0, 1, 1) == STEERING_CENTER_TAP_OVERVIEW);
    assert(steering_runtime_center_input(0x99, 1, 1) == STEERING_CENTER_CALIBRATING_IMU);
    assert(calibration_requests==1 && !steering_runtime_autopilot_ready());
    angle=2.0f;
    feed(); /* Still the old session from the file. */
    steering_runtime_tick();
    assert(steering_runtime_center_input(0, 1, 1) == STEERING_CENTER_CALIBRATING_IMU);
    assert(!steering_runtime_autopilot_ready());
    feedback_session=4242;
    feed();
    assert(steering_runtime_center_input(0, 1, 1) == STEERING_CENTER_SAMPLING);
    start=steering_now_ns();
    while(steering_runtime_center_sampling() && steering_now_ns()-start<2500000000ULL)
    {
        feed();
        steering_runtime_center_input(0, 1, 1);
        steering_runtime_lane(1, 6000, 1, 90, 20);
        steering_runtime_tick();
        steering_sleep_ms(8);
    }
    assert(steering_runtime_autopilot_ready() && !starts);
    assert(steering_runtime_center_input(0, 1, 1) == STEERING_CENTER_READY);
    start=steering_now_ns();
    while (steering_now_ns()-start < 1000000000ULL)
    {
        feed();
        const uint32_t id = 6001 + (uint32_t)((steering_now_ns()-start)/100000000ULL);
        steering_runtime_lane(steering_runtime_autopilot_ready(), id, 1, 90, 20);
        steering_runtime_tick();
        steering_sleep_ms(1);
    }
    steering_get_state(&state);
    assert(!active && starts==1 && state.status==STEERING_SETTLED && fabsf(state.angle_deg-5)<1);
    steering_runtime_close();
    assert(steering_runtime_init(0) == STEER_OK);
    feed();steering_runtime_tick();
    assert(!steering_runtime_autopilot_ready());
    steering_runtime_close();
    puts("PASS startup S/R: no initial feedback, reject old session, confirm mechanical zero, require confirmation after restart");
    puts("PASS runtime integration: lane angle mapping, duplicate frame/goal suppression, stale feedback, S/R rearm, STOP release; FAKE IO only");return 0;
}
