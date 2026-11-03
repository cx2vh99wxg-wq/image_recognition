#include "person_alert_effects.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
static unsigned long tick;
int fixture_gettimeofday(struct timeval *tv, void *tz)
{
    (void)tz;
    tv->tv_sec=tick/1000;
    tv->tv_usec=(tick%1000)*1000;
    return 0;
}
int main(int argc,char **argv)
{
    assert(argc==2);
    FILE *fp=fopen(argv[1],"wb");
    assert(fp);
    const BOX_RECT boxes[]={{0,125,0,188},{410,639,0,199},{30,180,289,479},
                           {470,630,280,455},{200,440,120,360},{-100,100,-80,100},
                           {320,320,240,240},{312,300,248,230},{600,800,430,610},
                           {18,602,28,451}};
    for(int h=480;h<=520;h+=40) for(int w=640;w<=700;w+=60) {
        unsigned char *pixels=malloc(w*h*3);
        assert(pixels);
        for(unsigned i=0;i<sizeof(boxes)/sizeof(*boxes);++i) {
            for(int step=0;step<3;++step) {
                tick=step*ALERT_BLINK_INTERVAL;
                AlertEffectState state={0,1,1};
                for(int n=0;n<w*h*3;++n) pixels[n]=(unsigned char)(n*13+i*17);
                draw_person_alert_effect(pixels,w,h,&boxes[i],&state);
                fwrite(pixels,1,w*h*3,fp);
                fwrite(&state.last_blink_time,sizeof(state.last_blink_time),1,fp);
                fwrite(&state.blink_state,sizeof(state.blink_state),1,fp);
            }
            for(int y=-30;y<=h+30;y+=13) for(int x=-30;x<=w+30;x+=19) {
                FadeInfo info;
                memset(&info,0,sizeof info);
                calculate_fade_info(x,y,&boxes[i],&info);
                fwrite(&info,sizeof info,1,fp);
            }
        }
        memset(pixels,113,w*h*3);
        detect_result_group_t results={0};
        results.count=3;
        results.results[0].box=boxes[0];
        results.results[1].box=boxes[5];
        results.results[2].box=boxes[9];
        strcpy(results.results[0].name,"person");
        strcpy(results.results[1].name,"car");
        strcpy(results.results[2].name,"person");
        alert_effects_init();
        apply_person_alert_effects(pixels,w,h,&results);
        fwrite(pixels,1,w*h*3,fp);
        free(pixels);
    }
    fclose(fp);
}
