/* Fake registers/plant only. The real runtime+UDP cache are exercised, no SPI. */
#include "steering_runtime.h"
#include "steering_feedback.h"
#include "steering_feedback_wire.h"
#include "steering_platform.h"
#include "fspi_module.h"
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
static uint32_t seq,frame;
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
int fspi_read_control_snapshot(ControlSnapshot *out){*out=pins;return 0;}
static void feed(void){
    steering_feedback_t s={7,++seq,1,(int32_t)lroundf(angle*1000),0,0,steering_now_ns()};uint8_t wire[48];
    assert(steering_feedback_encode(wire,48,&s)==STEER_OK);
    assert(steering_feedback_ingest(wire,48,steering_now_ns())==STEER_OK);
}
int main(int argc,char **argv){
    assert(argc==4);setenv("STEERING_RESPONSE_TABLE",argv[1],1);setenv("STEERING_REFERENCE_FILE",argv[2],1);setenv("STEERING_FEEDBACK_PORT",argv[3],1);
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
    puts("PASS runtime integration: lane angle mapping, duplicate frame/goal suppression, stale feedback, S/R rearm, STOP release; FAKE IO only");return 0;
}
