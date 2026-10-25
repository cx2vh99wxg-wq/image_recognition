#define _POSIX_C_SOURCE 200809L
#include "person_alert_effects.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
void baseline_draw(uint8_t*,int,int,const BOX_RECT*,AlertEffectState*);
void baseline_apply(uint8_t*,int,int,const detect_result_group_t*);
extern AlertEffectState baseline_state;
int __wrap_gettimeofday(struct timeval* tv,void* zone) {
    (void)zone;tv->tv_sec=100;tv->tv_usec=100000;return 0;
}
static unsigned seed=73;
static unsigned random_value(void) {seed=seed*1664525+1013904223;return seed;}
static double seconds(void) {struct timespec ts;clock_gettime(CLOCK_MONOTONIC,&ts);return ts.tv_sec+ts.tv_nsec/1e9;}
static void compare(BOX_RECT box,int width,int height,int blink) {
    size_t n=(size_t)width*height*3;
    uint8_t* a=malloc(n),*b=malloc(n);assert(a&&b);
    for(size_t k=0;k<n;++k)a[k]=random_value()>>24;
    memcpy(b,a,n);
    AlertEffectState sa={100100,blink,1},sb=sa;
    baseline_draw(a,width,height,&box,&sa);
    draw_person_alert_effect(b,width,height,&box,&sb);
    assert(memcmp(&sa,&sb,sizeof(sa))==0);
    if(memcmp(a,b,n)!=0) {
        for(size_t k=0;k<n;++k)if(a[k]!=b[k]) {
            fprintf(stderr,"Mismatch image=%dx%d box=%d,%d,%d,%d blink=%d pixel=%zu/%zu channel=%zu before=%u after=%u\n",
                width,height,box.left,box.top,box.right,box.bottom,blink,k/3%width,k/3/width,k%3,a[k],b[k]);break;
        }
        abort();
    }
    free(a);free(b);
}
static void benchmark(BOX_RECT box,const char* name) {
    size_t n=640*480*3;uint8_t *a=malloc(n),*b=malloc(n);assert(a&&b);
    memset(a,123,n);memcpy(b,a,n);
    const int repeats=160;
    AlertEffectState sa={100100,1,1},sb=sa;
    double start=seconds();for(int n=0;n<repeats;++n)baseline_draw(a,640,480,&box,&sa);double old=seconds()-start;
    start=seconds();for(int n=0;n<repeats;++n)draw_person_alert_effect(b,640,480,&box,&sb);double current=seconds()-start;
    assert(memcmp(a,b,n)==0);
    printf("%s baseline_ms=%.3f candidate_ms=%.3f speedup=%.2f\n",name,old*1000/repeats,current*1000/repeats,old/current);
    free(a);free(b);
}
static void compare_multi(void) {
    size_t bytes=640*480*3;uint8_t *a=malloc(bytes),*b=malloc(bytes);assert(a&&b);
    detect_result_group_t group={0};group.count=12;
    for(int n=0;n<group.count;++n) {
        strcpy(group.results[n].name,n%4==0?"car":"person");
        group.results[n].box.left=40+n*30;group.results[n].box.right=400+n*15;
        group.results[n].box.top=20+n*10;group.results[n].box.bottom=420+n*4;
    }
    for(int blink=0;blink<2;++blink) {
        for(size_t n=0;n<bytes;++n)a[n]=(uint8_t)(random_value()>>24);
        memcpy(b,a,bytes);
        baseline_state=(AlertEffectState){100100,blink,1};g_alert_state=baseline_state;
        baseline_apply(a,640,480,&group);apply_person_alert_effects(b,640,480,&group);
        assert(memcmp(a,b,bytes)==0);
    }
    free(a);free(b);
}
int main(int argc,char** argv) {
    (void)argv;
#define BOX(l,t,r,b) { .left=(l), .top=(t), .right=(r), .bottom=(b) }
    BOX_RECT cases[]={
        BOX(0,0,639,479),BOX(40,40,600,440),BOX(160,80,480,400),BOX(0,0,40,100),
        BOX(520,10,639,400),BOX(0,300,240,479),BOX(320,240,600,479),
        BOX(300,220,340,260),BOX(320,240,320,240),BOX(-200,-200,-100,-100),
        BOX(-30,-50,700,550),BOX(600,450,800,650),BOX(100,100,20,20),
        BOX(79,79,560,400),BOX(80,80,559,399),BOX(-80,120,400,400)
    };
    for(size_t n=0;n<sizeof(cases)/sizeof(cases[0]);++n)for(int blink=0;blink<=1;++blink)compare(cases[n],640,480,blink);
    for(int n=0;n<180;++n) {
        BOX_RECT box;
        box.left=(int)(random_value()%900)-120;box.top=(int)(random_value()%700)-110;
        box.right=box.left+(int)(random_value()%700);box.bottom=box.top+(int)(random_value()%500);
        compare(box,n%3==0?800:640,n%3==0?600:480,n%2);
    }
    compare_multi();
    puts("212 boxes / all corners / both blink phases / overlap / offscreen / degenerates pixel-identical PASS");
    if(argc>1) {
        benchmark((BOX_RECT)BOX(160,80,480,400),"medium person");
        benchmark((BOX_RECT)BOX(40,40,600,440),"large person");
        benchmark((BOX_RECT)BOX(250,130,350,330),"small person");
        benchmark((BOX_RECT)BOX(0,0,639,479),"full frame person");
    }
}
