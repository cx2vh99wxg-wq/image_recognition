/* Fake registers/plant only. The real runtime+UDP cache are exercised, no SPI. */
#include "steering_runtime.h"
#include "steering_feedback.h"
#include "steering_feedback_wire.h"
#include "steering_platform.h"
#include "fspi_module.h"
#include "../../../../common/perception/include/curve_heading.h"
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
static uint64_t lane_deadline;
static uint8_t fpga_autopilot_status;
static int read_lane(void *context,uint32_t *id,unsigned *kind,unsigned *quality,
                     unsigned *road_angle,uint64_t *deadline_ns){
    (void)context;assert(!active);++lane_reads;
    *deadline_ns=lane_deadline ? lane_deadline : steering_now_ns()+CURVE_RESULT_TTL_NS;
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
int fspi_read_autopilot_status(uint8_t *status)
{
    *status=fpga_autopilot_status;
    return 0;
}
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
static void start_authorized(void)
{
    assert(steering_runtime_init(0)==STEER_OK);
    fpga_autopilot_status=0x0d;
    steering_runtime_set_hmi_request(5);
}

static void feed_until(uint64_t duration)
{
    const uint64_t end=steering_now_ns()+duration;
    do {
        feed();steering_runtime_tick();steering_sleep_ms(1);
    } while (steering_now_ns()<end);
}

static void test_output_gate(void)
{
    assert(steering_runtime_init(0)==STEER_OK);
    fpga_autopilot_status=0x0d;
    feed_until(350000000ULL);
    steering_runtime_lane(1,++frame,1,90,20);
    feed_until(40000000ULL);
    assert(!starts && !active); /* No software approval at startup. */

    steering_runtime_set_hmi_request(5);
    steering_runtime_lane(1,++frame,1,90,20);
    fpga_autopilot_status=1; /* OFF after the last approved poll, before the FSPI write. */
    feed_until(40000000ULL);
    assert(!starts && !active);
    steering_runtime_set_hmi_request(0);

    fpga_autopilot_status=0x0e;
    steering_runtime_set_hmi_request(6);
    steering_runtime_lane(1,++frame,1,90,20);
    for (unsigned i=0; i<50 && !active; ++i) {
        feed();steering_runtime_tick();steering_sleep_ms(1);
    }
    assert(starts==1 && active==2);
    steering_runtime_set_hmi_request(0);
    assert(!active && (pins.contacts&CONTACT_LEFT_N) && (pins.contacts&CONTACT_RIGHT_N));
    steering_runtime_lane(1,++frame,2,90,20);
    feed_until(50000000ULL);
    assert(starts==1 && !active);
    steering_runtime_close();
    starts=0;angle=0;
    puts("PASS output gate: OFF ignores lane, rechecks FPGA before a write, ON drives and OFF releases");
}

static void test_wait_keeps_approval(void)
{
    steering_state_t state;
    angle=2.8f;
    start_authorized();
    feed_until(350000000ULL);
    steering_runtime_lane(1,++frame,1,90,20); /* +5 deg; no safe candidate from +2.8. */
    feed_until(40000000ULL);
    steering_get_state(&state);
    assert(state.status==STEERING_WAIT_TARGET && steering_runtime_available() && !starts);
    steering_runtime_lane(1,++frame,1,90,curve_encode_image_heading(8));
    for (unsigned i=0; i<50 && !active; ++i) {
        feed();steering_runtime_tick();steering_sleep_ms(1);
    }
    assert(active==2 && starts==1);
    steering_runtime_set_hmi_request(0);
    assert(!active);
    steering_runtime_close();
    starts=0;angle=0;
    puts("PASS wait target: no safe candidate keeps approval; a new feasible target drives");
}

static void test_pulse_limit_keeps_approval(void)
{
    assert(!setenv("STEERING_MAX_CORRECTION_PULSES","1",1));
    start_authorized();
    feed_until(350000000ULL);
    steering_runtime_lane(1,++frame,1,90,curve_encode_image_heading(10));
    feed_until(380000000ULL);
    steering_state_t state;
    steering_get_state(&state);
    assert(state.status==STEERING_PULSE_LIMIT && steering_runtime_available() && starts==1 && !active);
    steering_runtime_cancel(); /* A dropout must not reset the pulse latch. */
    steering_runtime_lane(1,++frame,2,90,curve_encode_image_heading(8));
    feed_until(30000000ULL);
    assert(steering_runtime_available() && starts==1 && !active);
    steering_runtime_set_hmi_request(0);
    fpga_autopilot_status=0x0e;
    steering_runtime_set_hmi_request(6);
    steering_runtime_lane(1,++frame,2,90,curve_encode_image_heading(8));
    for (unsigned i=0; i<50 && !active; ++i) {
        feed();steering_runtime_tick();steering_sleep_ms(1);
    }
    assert(starts==2 && active==3);
    steering_runtime_set_hmi_request(0);
    steering_runtime_close();
    assert(!unsetenv("STEERING_MAX_CORRECTION_PULSES"));
    starts=0;angle=0;
    puts("PASS pulse limit: approval stays active; no retry until a fresh HMI authorization");
}

int main(int argc,char **argv){
    assert(argc==4);setenv("STEERING_RESPONSE_TABLE",argv[1],1);setenv("STEERING_REFERENCE_FILE",argv[2],1);setenv("STEERING_FEEDBACK_PORT",argv[3],1);
    setenv("STEERING_PRIORITY","0",1);setenv("STEERING_CURVE_GAIN","0.25",1);setenv("STEERING_LANE_TARGET_LIMIT_DEG","12",1);
    pins.contacts=CONTACT_GO_N|CONTACT_BACK_N|CONTACT_LEFT_N|CONTACT_RIGHT_N|CONTACT_STOP_N;
    pins.gates=GATE_RESET_RELEASED|GATE_FS_ARMED;
    test_output_gate();
    test_wait_keeps_approval();
    test_pulse_limit_keeps_approval();
    start_authorized();feed();steering_runtime_tick();
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
    start_authorized();feed();steering_runtime_tick();
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
    start_authorized();feed();steering_runtime_tick();
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
        start_authorized();feed();steering_runtime_tick();
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
                uint64_t unused_deadline;
                read_lane(NULL,&id,&kind,&quality,&road,&unused_deadline);
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
    start_authorized();feed();steering_runtime_tick();
    start=steering_now_ns();
    while(steering_now_ns()-start<450000000){feed();steering_runtime_poll_lane(0,read_lane,NULL);steering_runtime_tick();steering_sleep_ms(1);}
    assert(!active&&starts==0);steering_runtime_close();
    lane_reads=0;assert(steering_runtime_init(1)==STEER_OK);feed();steering_runtime_tick();
    steering_runtime_poll_lane(1,read_lane,NULL);assert(!lane_reads&&!starts);steering_runtime_close();
    puts("PASS short-valid-lane regression: 100ms polling misses 60ms windows, 10ms poll acts, expiry releases, denial and bench isolation retained; FAKE IO only");
    start_authorized();feed();steering_runtime_tick();
    lane_frame=4950;lane_confidence=90;lane_deadline=steering_now_ns()-1;
    steering_runtime_poll_lane(1,read_lane,NULL);
    steering_get_state(&state);assert(!isfinite(state.target_deg)&&!starts&&!active);
    steering_sleep_ms(11);feed();
    ++lane_frame;lane_deadline=steering_now_ns()+25000000;
    steering_runtime_poll_lane(1,read_lane,NULL);
    steering_get_state(&state);assert(fabsf(state.target_deg-5.f)<.01f);
    steering_sleep_ms(30);feed();steering_runtime_tick();
    steering_get_state(&state);
    assert(state.status==STEERING_CANCELLED&&!isfinite(state.error_deg)&&!starts&&!active);
    lane_deadline=0;steering_runtime_close();
    puts("PASS expired road input: original receiver deadline reaches controller, no stale pulse; FAKE IO only");
    start_authorized();feed();steering_runtime_tick();
    steering_runtime_lane(1,4999,1,90,curve_encode_image_heading(45));
    steering_get_state(&state);assert(fabsf(state.target_deg-12.f)<.01f);
    steering_runtime_close();
    setenv("STEERING_LANE_TARGET_LIMIT_DEG","16",1);
    start_authorized();feed();steering_runtime_tick();
    const struct {unsigned type, heading; float target;} headings[]={
        {2,10,-10.f},{1,15,15.f},{2,18,-15.2f},{1,30,16.f},{2,90,-16.f},
    };
    for(unsigned i=0;i<sizeof(headings)/sizeof(headings[0]);++i){
        unsigned wire_angle=curve_encode_image_heading(headings[i].heading);
        assert(curve_is_image_heading(wire_angle));
        assert(curve_angle_degrees(wire_angle)==headings[i].heading);
        steering_runtime_lane(1,5000+i,headings[i].type,90,wire_angle);
        steering_get_state(&state);
        assert(fabsf(state.target_deg-headings[i].target)<.01f);
    }
    assert(!active&&starts==0);steering_runtime_close();
    setenv("STEERING_LANE_TARGET_LIMIT_DEG","12",1);
    puts("PASS image heading: one-to-one, compressed, signed and bounded targets; FAKE IO only");
    /* A new M boot must remain visibly blocked by the old physical reference.
     * Sending plausible curve/angle data must not silently rebind that zero. */
    starts=0;active=0;angle=0;feedback_session=8;
    start_authorized();
    for(unsigned i=0;i<10;++i){
        feed();steering_runtime_tick();
        steering_runtime_lane(1,++frame,1,90,25);
        steering_sleep_ms(1);
    }
    assert(!active&&starts==0);
    steering_runtime_close();
    puts("PASS new IMU session: old physical reference blocks all automatic pulses; FAKE IO only");
    puts("PASS runtime integration: lane angle mapping, duplicate frame/goal suppression, stale feedback, S/R rearm, STOP release; FAKE IO only");return 0;
}
