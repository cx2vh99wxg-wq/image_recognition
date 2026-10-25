#include "yolo_integration.h"
#include "shared_memory.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
void original_draw(uint8_t*,int,int,const yolopv2_result_t*);
static unsigned updates;
static YOLOCurveResult expected;
int curve_detection_update(uint32_t frame_id,CurveType type,uint32_t confidence,uint32_t angle) {
    (void)frame_id; ++updates;expected.curve_type=type;expected.confidence=confidence;expected.angle=angle;
    return 0;
}
static unsigned state=42;
static unsigned random_value(void) {state=1664525*state+1013904223;return state;}
static void test_size(int width,int height,int mw,int mh) {
    size_t bytes=(size_t)width*height*3,count=(size_t)mw*mh;
    uint8_t *before=malloc(bytes),*old=malloc(bytes),*current=malloc(bytes);
    float *lane=malloc(count*sizeof(float)),*area=malloc(count*2*sizeof(float));
    assert(before&&old&&current&&lane&&area);
    yolopv2_result_t masks={lane,area,mw,mh};
    for(unsigned scenario=0;scenario<5;++scenario) {
        for(size_t n=0;n<bytes;++n)before[n]=(uint8_t)(random_value()>>20);
        for(size_t n=0;n<count;++n) {
            lane[n]=scenario==0?0.0f:scenario==1?0.3f:scenario==2?0.30000004f:(float)(random_value()%101)/100;
            area[n]=(float)(random_value()%100)/100;area[count+n]=(float)(random_value()%100)/100;
        }
        memcpy(old,before,bytes);memcpy(current,before,bytes);
        original_draw(old,width,height,&masks);
        YOLOCurveResult want=expected;unsigned old_updates=updates;
        int pixels=yolo_overlay_lane_and_drivable(current,width,height,&masks);assert(pixels>=0);
        YOLOCurveResult got;
        assert(yolo_analyze_annotated_curve(current,width,height,pixels,&got)==0);
        assert(updates==old_updates);assert(memcmp(old,current,bytes)==0);
        assert(got.curve_type==want.curve_type&&got.confidence==want.confidence&&got.angle==want.angle);
        draw_lane_and_drivable(current,width,height,&masks);assert(updates==old_updates+1);
    }
    free(before);free(old);free(current);free(lane);free(area);
}
int main(void) {
    test_size(640,480,640,480);test_size(320,240,320,240);
    test_size(257,129,320,240);test_size(7,5,7,5);
    fprintf(stderr,"pure overlay byte-identical + curve-identical across 20 cases; no IPC side effect PASS\n");
}
