#include "steering_predictive_stop.h"
#include <assert.h>
#include <stdio.h>
int main(void){
    assert(steering_predictive_release(0,0,STEER_LEFT,12,3,100,150,18)==BURST_CONTINUE);
    assert(steering_predictive_release(6,60,STEER_LEFT,12,3,100,150,18)==BURST_TARGET_RELEASE);
    assert(steering_predictive_release(6,-60,STEER_RIGHT,0,3,100,150,18)==BURST_TARGET_RELEASE);
    assert(steering_predictive_release(-6,-60,STEER_RIGHT,-12,3,100,150,18)==BURST_TARGET_RELEASE);
    assert(steering_predictive_release(9,80,STEER_LEFT,15,3,100,150,18)==BURST_GUARD_RELEASE);
    assert(steering_predictive_release(0,-10,STEER_LEFT,12,3,100,150,18)==BURST_WRONG_DIRECTION);
    assert(steering_predictive_release(0,0,STEER_LEFT,12,3,160,150,18)==BURST_BAD_PREDICTION);
    /* A measured delayed acceleration after release needs a larger safety
     * horizon than the target estimator. Never clamp that guard down. */
    assert(steering_predictive_release(-2,-40,STEER_RIGHT,-15,3,172,400,18)==BURST_GUARD_RELEASE);
    assert(steering_predictive_release(0,0,STEER_RIGHT,-15,3,172,501,18)==BURST_BAD_PREDICTION);
    assert(steering_predictive_release_capped(2,80,STEER_LEFT,15,3,138,220,18,6,10)==BURST_CONTINUE);
    assert(steering_predictive_release_capped(8,80,STEER_LEFT,15,3,138,220,18,6,10)==BURST_GUARD_RELEASE);
    assert(steering_predictive_release_capped(6,80,STEER_LEFT,12,3,138,220,18,6,10)==BURST_TARGET_RELEASE);
    puts("PASS predictive single-action release: signed target, RF/coast allowance, inward centering, guard and wrong-direction rejection");return 0;
}
