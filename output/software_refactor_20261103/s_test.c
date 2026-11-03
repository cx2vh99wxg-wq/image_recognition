#include "yolo_integration.h"
#include "person_alert_effects.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
void apply_person_alert_effects(uint8_t *rgb,int w,int h,const detect_result_group_t *r) {
    if(r->count) { assert(w>0 && h>0); rgb[0]=(uint8_t)r->count; }
}
int main(int argc,char **argv) {
    assert(argc==2);
    FILE *fp=fopen(argv[1],"wb"); assert(fp);
    static const char *names[]={"traffic light","fire hydrant","stop sign","parking meter",
        "sports ball","baseball bat","baseball glove","wine glass","hot dog","potted plant",
        "dining table","cell phone","hair drier","teddy bear","person","bottle","chair","table","other"};
    uint8_t rgb[83*61*3];
    for (int w=1;w<=83;w+=4) for(int h=1;h<=61;h+=6)
      for(int trial=0;trial<44;++trial) {
        detect_result_group_t r={0}; r.count=3;
        for(int j=0;j<3;++j) {
          detect_result_t *d=&r.results[j];
          int x=(trial*13+j*5)%(w+12)-6, y=(trial*23+j*7)%(h+12)-6;
          d->box=(BOX_RECT){x,y,x+(trial*3+j)%92-2,y+(trial*17+j)%63-2};
          d->prop=(float)((trial*17+j*23)%101)/100;
          strcpy(d->name,names[(trial+j)%19]);
        }
        memset(rgb,85,sizeof(rgb));
        draw_detection_results(rgb,w,h,&r);
        fwrite(rgb,1,w*h*3,fp);
      }
    fclose(fp);
}
