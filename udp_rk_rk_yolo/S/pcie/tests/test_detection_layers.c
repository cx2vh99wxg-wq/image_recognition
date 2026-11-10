#include "yolo_integration.h"
#include "person_alert_effects.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
int person_alert_cache_wait(unsigned timeout_ms);
int test_gettimeofday(struct timeval *tv, void *zone) {
    (void)zone;tv->tv_sec=1000;tv->tv_usec=0;return 0;
}
int main(void) {
    const size_t bytes=640*480*3,guard=32;
    unsigned char *a=malloc(bytes+guard*2),*b=malloc(bytes+guard*2);
    assert(a&&b);
    for(int scene=0;scene<8;++scene) {
        detect_result_group_t raw={0};raw.count=scene;
        for(int i=0;i<raw.count;++i) {
            detect_result_t*r=&raw.results[i];strcpy(r->name,i%3==2?"car":"person");r->prop=.8f;
            r->box=(BOX_RECT){10+i*37,90+i*37,20+i*29,140+i*29};
        }
        const detect_result_group_t before=raw;
        for(int phase=0;phase<2;++phase) {
            memset(a,89,bytes+guard*2);g_alert_state=(AlertEffectState){1000000,phase,1};
            draw_detection_results(a+guard,640,480,&raw);assert(person_alert_cache_wait(5000));
            memset(a,89,bytes+guard*2);draw_detection_results(a+guard,640,480,&raw);
            memset(b,89,bytes+guard*2);draw_detection_alerts(b+guard,640,480,&raw);
            draw_detection_boxes(b+guard,640,480,&raw);
            assert(!memcmp(a,b,bytes+guard*2));assert(!memcmp(&raw,&before,sizeof(raw)));
            for(size_t k=0;k<guard;++k)assert(a[k]==89&&a[guard+bytes+k]==89);
        }
    }
    free(a);free(b);puts("DETECTION LAYERS PASS: identical fresh warnings and boxes; guarded pixels and raw metadata unchanged");
}
