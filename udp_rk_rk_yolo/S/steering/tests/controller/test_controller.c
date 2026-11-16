#include "steering_feedback.h"
#include "steering_api.h"
#include "steering_keepalive.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
static uint64_t clock_ns=1000000000,enabled;
static steer_direction_t direction;
static float angle,response_scale=1;
static int blocked,stale,simulated,fail_release,discrete_plant,first_response_zero;
static int predictive_plant;static int32_t reported_rate;
static int replay_right;
static unsigned last_width_us;
static unsigned starts,heartbeats;static uint32_t session=7;
static uint64_t now(void *c){(void)c;return clock_ns;}
static steer_result_t output(void *c,steer_direction_t d){
    (void)c;
    if(d==STEER_RELEASE){
        if(direction!=STEER_RELEASE){
            last_width_us=(unsigned)((clock_ns-enabled)/1000);
            if(replay_right&&direction==STEER_RIGHT)
                angle-=last_width_us==30000?(starts==1?.896f:8.099f):1.8f;
            else if(predictive_plant)angle+=(direction==STEER_LEFT?1.f:-1.f)*5; /* identified 100ms tail at 50dps */
            else if(first_response_zero)first_response_zero=0;
            else angle+=discrete_plant?(direction==STEER_LEFT?2.f:-4.f)*response_scale:(direction==STEER_LEFT?1.f:-1.f)*(float)((clock_ns-enabled)*1e-9)*500*response_scale;
        }
        direction=d;return fail_release?STEER_ERR_IO:STEER_OK;
    }
    direction=d;enabled=clock_ns;++starts;return STEER_OK;
}
static steer_result_t feedback(void *c,steering_feedback_t *f){
    (void)c;if(stale)return STEER_ERR_STALE;
    *f=(steering_feedback_t){0};f->flags=simulated?5:1;f->session_id=session;
    f->sequence=(uint32_t)(clock_ns/10000000);f->sample_time_ns=clock_ns;
    f->angle_mdeg=(int32_t)lroundf(angle*1000);f->rate_mdps=reported_rate;return STEER_OK;
}
static steer_result_t permission(void *c,steer_direction_t d){(void)c;(void)d;return blocked?STEER_ERR_BUSY:STEER_OK;}
static steering_response_model_t model;
static void setup_unreferenced(void){
    steering_shutdown();clock_ns+=1000000000;angle=0;direction=STEER_RELEASE;starts=0;response_scale=1;
    replay_right=0;last_width_us=0;
    blocked=stale=simulated=fail_release=discrete_plant=first_response_zero=predictive_plant=0;reported_rate=0;session=7;memset(&model,0,sizeof(model));model.count=2;
    for(unsigned i=0;i<2;++i){steering_response_cell_t *c=&model.cells[i];c->direction=i?STEER_RIGHT:STEER_LEFT;
        c->duration_us=10000;c->n=c->moved=c->observations=3;c->min_deg=c->max_deg=c->mean_deg=5;
        c->start_min_deg=-18;c->start_max_deg=18;for(unsigned j=0;j<3;++j)c->responses_deg[j]=5;}
    steering_controller_io_t io={NULL,{NULL,output,now},feedback,permission,&model,4,100,2,0,0,0};
    assert(steering_bind(io)==STEER_OK);steering_config_t cfg={-18,18,1,100,60};
    assert(steering_init(&cfg)==STEER_OK);assert(steering_set_target_deg(10)==STEER_ERR_NOT_READY);
}
static void setup(void){
    setup_unreferenced();
    assert(steering_confirm_reference(7,0)==STEER_OK);
}
static void advance(unsigned ms){for(unsigned i=0;i<ms;++i){clock_ns+=1000000;
    if(predictive_plant&&direction!=STEER_RELEASE){angle+=direction==STEER_LEFT?.05f:-.05f;reported_rate=direction==STEER_LEFT?50000:-50000;}
    else if(predictive_plant)reported_rate=0;
    (void)steering_tick();}}
static steering_state_t get(void){steering_state_t s;assert(steering_get_state(&s)==STEER_OK);return s;}
static steer_result_t beat(void *c){(void)c;++heartbeats;return STEER_OK;}
static void fast_setup(void){
    setup();steering_shutdown();predictive_plant=1;
    model.burst_max_us=250000;model.burst_session=7;model.burst_zero_raw_deg=0;
    for(unsigned d=0;d<2;++d){model.burst_coast_ms[d]=100;model.burst_guard_ms[d]=120;model.burst_coast_cap_deg[d]=5;model.burst_guard_cap_deg[d]=6;}
    steering_controller_io_t binding={NULL,{NULL,output,now},feedback,permission,&model,20,100,2,10,0,0};
    assert(steering_bind(binding)==STEER_OK);steering_config_t cfg={-18,18,3,100,250};
    assert(steering_init(&cfg)==STEER_OK);
    assert(steering_confirm_reference(8,0)==STEER_ERR_STALE&&steering_confirm_reference(7,.5f)==STEER_ERR_STALE);
    assert(steering_confirm_reference(7,0)==STEER_OK);
}
static void road_right_setup(int wait_for_lane_target)
{
    setup();
    steering_shutdown();
    replay_right=1;
    for (unsigned i=0; i<2; ++i)
    {
        steering_response_cell_t *cell=&model.cells[i];
        const float travel=i?1.8f:4.6f;
        cell->direction=STEER_RIGHT;
        cell->duration_us=i?25000:30000;
        cell->min_deg=cell->max_deg=cell->mean_deg=travel;
        cell->partitioned=1;
        cell->zone_low_deg=-18;
        cell->zone_high_deg=18;
        cell->peak_low_deg=-travel;
        for (unsigned j=0; j<3; ++j) cell->responses_deg[j]=travel;
    }
    steering_controller_io_t binding={NULL,{NULL,output,now},feedback,permission,&model,3,300,2,0,1,wait_for_lane_target};
    steering_config_t cfg={-18,18,2,100,60};
    assert(steering_bind(binding)==STEER_OK);
    assert(steering_init(&cfg)==STEER_OK);
    assert(steering_confirm_reference(7,0)==STEER_OK);
}
static void test_right_road_correction(void)
{
    road_right_setup(0);
    angle=8.392f;
    advance(350);
    assert(steering_set_target_deg(-2.8f)==STEER_OK);
    advance(40);
    assert(starts==1&&last_width_us==30000&&!steering_pulse_active());
    advance(500);
    assert(starts==1);
    advance(52);
    advance(30);
    assert(starts==2&&last_width_us==30000&&!steering_pulse_active());
    assert(fabsf(angle+.603f)<.02f);

    /* The live lane briefly disappeared and returned with a changed goal. */
    assert(steering_cancel()==STEER_OK);
    assert(steering_set_target_deg(-4.f)==STEER_OK);
    reported_rate=1400;
    advance(510);
    assert(starts==2);
    reported_rate=0;
    advance(290);
    assert(starts==2);
    advance(50);
    assert(starts==3&&last_width_us==25000&&!steering_pulse_active());
    advance(600);
    assert(get().status==STEERING_SETTLED&&fabsf(angle+2.403f)<.02f);

    road_right_setup(0);
    model.cells[1].n=0;
    assert(steering_set_target_deg(-4.f)==STEER_OK);
    advance(450);
    assert(starts==0&&get().status==STEERING_RESOLUTION_LIMIT);

    road_right_setup(1);
    model.cells[1].n=0;
    assert(steering_set_target_deg(-4.f)==STEER_OK);
    advance(15000); /* Waiting must not expire into a fault or retry the pulse. */
    assert(starts==0&&get().status==STEERING_WAIT_TARGET);
    assert(steering_set_target_deg(-12.f)==STEER_OK);
    advance(2);
    assert(starts==1&&direction==STEER_RIGHT);
    assert(steering_cancel()==STEER_OK&&direction==STEER_RELEASE);
    puts("PASS right road replay: live angle responses, target replacement, fresh stable wait, 30ms to 25ms and no unsafe fallback");
}

static void test_right_road_calibrated_zone(void)
{
    road_right_setup(1);
    steering_response_cell_t *long_pulse = &model.cells[0];
    long_pulse->n = long_pulse->moved = 25;
    long_pulse->zone_low_deg = -6.f;
    long_pulse->zone_high_deg = -2.f;
    long_pulse->start_min_deg = -5.972f;
    long_pulse->start_max_deg = -2.068f;
    long_pulse->peak_low_deg = -5.935f;
    model.cells[1].n = 0;
    angle = -2.309f;
    advance(350);

    /* These two successive lane targets appeared in the same live session. */
    assert(steering_set_target_deg(-8.4f) == STEER_OK);
    advance(2);
    assert(starts == 0 && get().status == STEERING_WAIT_TARGET);
    assert(steering_set_target_deg(-9.2f) == STEER_OK);
    advance(2);
    assert(starts == 1 && direction == STEER_RIGHT);
    advance(30);
    assert(last_width_us == 30000 && direction == STEER_RELEASE);

    road_right_setup(1);
    long_pulse->n = long_pulse->moved = 25;
    long_pulse->zone_low_deg = 6.f;
    long_pulse->zone_high_deg = 10.f;
    model.cells[1].n = 0;
    angle = 7.496f;
    advance(350);
    assert(steering_set_target_deg(0.f) == STEER_OK);
    advance(2);
    assert(starts == 0 && get().status == STEERING_WAIT_TARGET);

    road_right_setup(1);
    long_pulse->n = 0;
    model.cells[1].zone_low_deg = -2.f;
    model.cells[1].zone_high_deg = 2.f;
    angle = .551f;
    advance(350);
    assert(steering_set_target_deg(-4.8f) == STEER_OK);
    advance(2);
    assert(starts == 0 && get().status == STEERING_WAIT_TARGET);
    puts("PASS right road calibration: usable 30ms correction, retained outer-zone and 25ms guards");
}

int main(void){
    steering_feedback_t f;assert(steering_feedback_read(&f)==STEER_ERR_NOT_READY&&!f.flags);
    test_right_road_correction();
    test_right_road_calibrated_zone();
    steering_config_t invalid={-30,30,2,100,200};assert(steering_init(&invalid)==STEER_ERR_ARGUMENT);
    setup_unreferenced();
    angle=56;
    assert(steering_confirm_reference(7,0)==STEER_ERR_DEVICE);
    angle=0;
    assert(steering_set_target_deg(10)==STEER_ERR_NOT_READY);
    advance(300);assert(!starts&&direction==STEER_RELEASE);
    assert(steering_confirm_reference(7,0)==STEER_OK);
    assert(steering_set_target_deg(10)==STEER_OK);advance(600);
    assert(get().status==STEERING_SETTLED&&starts==2&&direction==STEER_RELEASE);
    setup();assert(steering_set_target_deg(NAN)==STEER_ERR_ARGUMENT);assert(steering_set_target_deg(19)==STEER_ERR_ARGUMENT);
    assert(steering_set_target_deg(10)==STEER_OK);advance(600);
    assert(get().status==STEERING_SETTLED&&fabsf(angle-10)<.001f&&starts==2&&direction==STEER_RELEASE);
    for(unsigned i=0;i<100;++i)assert(steering_set_target_deg(10)==STEER_OK);
    advance(1000);assert(starts==2);
    /* Real NN results can be usable for less than the settling interval.
     * Quiet feedback acquired before the target must remain usable. */
    setup();advance(300);assert(steering_set_target_deg(10)==STEER_OK);advance(2);
    assert(starts==1&&direction==STEER_LEFT);
    assert(steering_cancel()==STEER_OK);assert(steering_set_target_deg(-5)==STEER_OK);
    advance(90);assert(starts==1); /* Cancellation of motion invalidated history. */
    advance(25);assert(starts==2);
    setup();assert(steering_set_target_deg(-10)==STEER_OK);advance(600);assert(get().status==STEERING_SETTLED&&fabsf(angle+10)<.001f);
    setup();assert(steering_set_target_deg(10)==STEER_OK);advance(105);assert(steering_pulse_active());
    blocked=1;advance(1);assert(direction==STEER_RELEASE&&get().status==STEERING_CANCELLED);
    setup();assert(steering_set_target_deg(10)==STEER_OK);advance(105);stale=1;advance(1);assert(direction==STEER_RELEASE&&!get().feedback_valid);
    setup();assert(steering_set_target_deg(10)==STEER_OK);advance(105);session=8;advance(1);
    assert(direction==STEER_RELEASE&&steering_set_target_deg(10)==STEER_ERR_NOT_READY);
    assert(steering_confirm_reference(7,0)==STEER_ERR_STALE);
    setup();assert(steering_set_target_deg(0)==STEER_OK);advance(200);clock_ns+=110000000;
    assert(!get().feedback_valid&&isnan(get().angle_deg));
    setup();assert(steering_set_target_deg(10)==STEER_OK);advance(50);clock_ns-=10000000;
    assert(steering_tick()==STEER_ERR_STALE&&direction==STEER_RELEASE);
    setup();simulated=1;assert(steering_set_target_deg(10)!=STEER_OK);assert(!starts);
    setup();response_scale=0;assert(steering_set_target_deg(10)==STEER_OK);advance(1000);
    assert(get().status==STEERING_RESOLUTION_LIMIT&&starts==2&&direction==STEER_RELEASE);
    for(unsigned i=0;i<100;++i)(void)steering_set_target_deg(10);
    advance(1000);assert(starts==2);
    setup();response_scale=0;assert(steering_set_target_deg(10)==STEER_OK);
    for(unsigned i=0;i<20;++i){advance(150);(void)steering_set_target_deg(i%2?10:12);}
    assert(starts<=4&&get().status==STEERING_RESOLUTION_LIMIT);
    setup();assert(steering_set_target_deg(1.5f)==STEER_OK);advance(200);assert(get().status==STEERING_RESOLUTION_LIMIT&&!starts);
    setup();angle=2.8f;assert(steering_set_target_with_tolerance_deg(0,3)==STEER_OK);advance(200);
    assert(get().status==STEERING_SETTLED&&!starts&&fabsf(get().angle_deg-2.8f)<.01f);
    assert(steering_set_target_with_tolerance_deg(0,3.1f)==STEER_ERR_ARGUMENT);
    assert(steering_set_target_deg(5)==STEER_OK);advance(200);
    assert(get().status==STEERING_RESOLUTION_LIMIT&&!starts); /* Default 1deg turn tolerance was preserved. */
    setup();discrete_plant=1;angle=-12;
    for(unsigned i=0;i<2;++i){steering_response_cell_t *c=&model.cells[i];
        c->duration_us=i?25000:19000;c->min_deg=c->max_deg=c->mean_deg=i?4:2;
        for(unsigned j=0;j<3;++j)c->responses_deg[j]=c->mean_deg;
        c->partitioned=1;c->zone_low_deg=-15;c->zone_high_deg=-9;
        c->start_min_deg=i?-11:-12.5f;c->start_max_deg=i?-9.1f:-11.5f;
        c->peak_low_deg=i?-5.5f:0;c->peak_high_deg=i?0:2.1f;
    }
    assert(steering_set_target_with_tolerance_deg(-15,2)==STEER_OK);advance(1000);
    assert(get().status==STEERING_SETTLED&&fabsf(angle+14)<.01f&&starts==2&&direction==STEER_RELEASE);
    setup();discrete_plant=1;angle=-8;model.count=3;
    for(unsigned i=0;i<3;++i){steering_response_cell_t *c=&model.cells[i];memset(c,0,sizeof(*c));
        c->direction=i==0?STEER_LEFT:STEER_RIGHT;c->duration_us=i==0?19000:i==1?25000:30000;
        c->n=c->moved=c->observations=3;c->min_deg=c->max_deg=c->mean_deg=i==0?2:4;
        for(unsigned j=0;j<3;++j)c->responses_deg[j]=c->mean_deg;
        c->partitioned=1;c->zone_low_deg=i==2?-10:-15;c->zone_high_deg=i==2?-6:-9;
        c->start_min_deg=i==0?-12.5f:i==1?-11:-9;c->start_max_deg=i==0?-11.5f:i==1?-9.1f:-7;
        c->peak_low_deg=i==0?0:i==1?-5.5f:-4.2f;c->peak_high_deg=i==0?2.1f:0;
    }
    assert(steering_set_target_with_tolerance_deg(-15,2)==STEER_OK);advance(1500);
    assert(get().status==STEERING_SETTLED&&fabsf(angle+14)<.01f&&starts==3&&direction==STEER_RELEASE);
    setup();discrete_plant=first_response_zero=1;
    for(unsigned i=0;i<2;++i){steering_response_cell_t *c=&model.cells[i];c->direction=STEER_LEFT;
        c->duration_us=i?30000:19000;c->min_deg=c->max_deg=c->mean_deg=i?2:2.3f;
        for(unsigned j=0;j<3;++j)c->responses_deg[j]=c->mean_deg;
        c->partitioned=1;c->zone_low_deg=-3;c->zone_high_deg=3;c->start_min_deg=-1;c->start_max_deg=1;
        c->peak_low_deg=0;c->peak_high_deg=c->max_deg+.1f;
    }
    assert(steering_set_target_with_tolerance_deg(2.4f,.5f)==STEER_OK);advance(1000);
    assert(get().status==STEERING_SETTLED&&fabsf(angle-2)<.01f&&starts==2&&direction==STEER_RELEASE);
    setup();steering_shutdown();discrete_plant=1;response_scale=.2f;
    for(unsigned i=0;i<2;++i){steering_response_cell_t *c=&model.cells[i];
        c->duration_us=60000;c->min_deg=c->max_deg=c->mean_deg=2;
        for(unsigned j=0;j<3;++j)c->responses_deg[j]=2;
    }
    steering_controller_io_t bounded_io={NULL,{NULL,output,now},feedback,permission,&model,20,100,2,0,0,0};
    assert(steering_bind(bounded_io)==STEER_OK);steering_config_t bounded_cfg={-18,18,.1f,100,60};
    assert(steering_init(&bounded_cfg)==STEER_OK&&steering_confirm_reference(7,0)==STEER_OK);
    assert(steering_set_target_deg(15)==STEER_OK);advance(3000);
    assert(get().status==STEERING_RESOLUTION_LIMIT&&starts==10&&direction==STEER_RELEASE); /* 600ms total, not twenty 60ms pulses. */
    setup();assert(steering_set_target_deg(10)==STEER_OK);advance(105);clock_ns+=10000000;(void)steering_tick();assert(get().status==STEERING_FAULT&&direction==STEER_RELEASE);
    setup();assert(steering_set_target_deg(10)==STEER_OK);advance(105);fail_release=1;assert(steering_cancel()==STEER_ERR_IO&&get().status==STEERING_FAULT);
    fail_release=0;steering_shutdown();
    fast_setup();assert(steering_set_target_deg(15)==STEER_OK);advance(700);
    assert(get().status==STEERING_SETTLED&&fabsf(angle-15)<.1f&&starts==1&&direction==STEER_RELEASE);
    fast_setup();assert(steering_set_target_deg(-15)==STEER_OK);advance(700);
    assert(get().status==STEERING_SETTLED&&fabsf(angle+15)<.1f&&starts==1&&direction==STEER_RELEASE);
    fast_setup();assert(steering_set_target_deg(15)==STEER_OK);advance(130);blocked=1;advance(1);
    assert(direction==STEER_RELEASE&&get().status==STEERING_CANCELLED);
    fast_setup();assert(steering_set_target_deg(15)==STEER_OK);advance(130);session=8;advance(1);
    assert(direction==STEER_RELEASE&&get().status==STEERING_NOT_READY);
    fast_setup();assert(steering_set_target_deg(16)==STEER_ERR_ARGUMENT&&!starts);
    steering_shutdown();
    steering_keepalive_t k;assert(steering_keepalive_init(&k,1000)==STEER_OK);heartbeats=0;
    assert(steering_keepalive_tick(&k,1000000000,1,beat,NULL)==STEER_OK&&!heartbeats);
    assert(steering_keepalive_tick(&k,1000000000,0,beat,NULL)==STEER_OK&&heartbeats==1);
    for(unsigned i=0;i<100;++i)assert(steering_keepalive_tick(&k,1500000000,0,beat,NULL)==STEER_OK);
    assert(heartbeats==1);assert(steering_keepalive_tick(&k,9000000000,0,beat,NULL)==STEER_OK&&heartbeats==2);
    assert(steering_keepalive_tick(&k,1000000000,0,beat,NULL)==STEER_ERR_STALE);
    puts("PASS closed loop: multi-step signed targets, duplicate goals, session/stale/manual/simulated rejection, no-progress, timing, release failure, non-motion heartbeat");
    return 0;
}
