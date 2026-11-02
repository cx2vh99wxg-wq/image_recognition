#include "steering_sequence.h"
#include <assert.h>
#include <stdio.h>
static uint64_t stamp;static steer_direction_t pins;static unsigned enables;static int fail_release;
static uint64_t clock_now(void *p){(void)p;return stamp;}
static steer_result_t output(void *p,steer_direction_t d){(void)p;if(d!=STEER_RELEASE){assert(pins==STEER_RELEASE);++enables;}pins=d;return fail_release&&d==STEER_RELEASE?STEER_ERR_IO:STEER_OK;}
int main(void){
    steering_sequence_t s;steering_actuator_io_t io={0,output,clock_now};stamp=1000000;
    assert(steering_sequence_init(&s,io)==STEER_OK);
    assert(steering_sequence_start(&s,STEER_LEFT,60001,0,0)==STEER_ERR_ARGUMENT);
    assert(steering_sequence_start(&s,STEER_LEFT,30000,20000,1999)==STEER_ERR_ARGUMENT);
    assert(steering_sequence_start(&s,STEER_LEFT,30000,17000,5000)==STEER_OK);
    assert(steering_sequence_start(&s,STEER_RIGHT,1,0,0)==STEER_ERR_BUSY);
    stamp+=30000000;assert(steering_sequence_tick(&s)==STEER_OK&&pins==STEER_RELEASE&&s.status==SEQUENCE_GAP);
    stamp+=4999000;assert(steering_sequence_tick(&s)==STEER_OK&&pins==STEER_RELEASE);
    stamp+=1000;assert(steering_sequence_tick(&s)==STEER_OK&&pins==STEER_RIGHT);
    stamp+=17000000;assert(steering_sequence_tick(&s)==STEER_OK&&pins==STEER_RELEASE&&s.completed==2&&s.status==SEQUENCE_DONE);
    assert(s.records[0].direction==STEER_LEFT&&s.records[1].direction==STEER_RIGHT&&enables==2);
    assert(steering_sequence_start(&s,STEER_RIGHT,10000,20000,5000)==STEER_OK);
    assert(steering_sequence_cancel(&s)==STEER_OK&&pins==STEER_RELEASE);
    assert(steering_sequence_start(&s,STEER_RIGHT,10000,20000,5000)==STEER_OK);
    stamp+=13000000;assert(steering_sequence_tick(&s)==STEER_ERR_IO&&pins==STEER_RELEASE&&s.completed==1);
    assert(s.status==SEQUENCE_FAULT);fail_release=1;assert(steering_sequence_cancel(&s)==STEER_ERR_IO);
    fail_release=0;assert(steering_sequence_init_limit(&s,io,251)==STEER_ERR_ARGUMENT);
    assert(steering_sequence_init_limit(&s,io,120)==STEER_OK);
    assert(steering_sequence_start(&s,STEER_LEFT,120001,0,0)==STEER_ERR_ARGUMENT);
    assert(steering_sequence_start(&s,STEER_LEFT,100000,0,0)==STEER_OK);
    stamp+=100000000;assert(steering_sequence_tick(&s)==STEER_OK&&s.completed==1&&pins==STEER_RELEASE);
    puts("PASS bounded sequence: dead-time, mutual exclusion, deadline/cancel faults, original60ms and explicit long-action cap");return 0;
}
