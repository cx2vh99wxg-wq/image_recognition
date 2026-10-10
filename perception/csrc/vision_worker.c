#include "vision_worker.h"
#include "pcie_capture.h"
#include "rknn_vision.h"
#include "lane_detect.h"
#include "turn_decide.h"
#include "lane_mark.h"
#include "zebra_detect.h"
#include "traffic_light.h"
#include "time_util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#if defined(__linux__)
#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>
struct vision_worker {
    vision_worker_cfg_t cfg;
    frame_grabber_t grab;
    rknn_vision_t person,road;
    pthread_t capture_thread,inference_thread;
    pthread_mutex_t lock;
    atomic_int run;
    int capture_started,inference_started,available;
    uint8_t latest[IMG_FRAME_BYTES];
    uint32_t frame_id;
    uint64_t captured,analyzed;
    VisionResult result;
};
static void *capture_loop(void *arg)
{
    vision_worker_t *w=arg;
    uint8_t *frame=malloc(IMG_FRAME_BYTES);
    if (!frame) { atomic_store(&w->run,0); return NULL; }
    while (atomic_load(&w->run)) {
        uint64_t started=now_us_mono();
        if (pcie_capture_grab(&w->grab,frame)<0) { atomic_store(&w->run,0); break; }
        pthread_mutex_lock(&w->lock);
        memcpy(w->latest,frame,IMG_FRAME_BYTES); w->captured=now_us_mono();
        w->frame_id++; w->available=1;
        pthread_mutex_unlock(&w->lock);
        uint64_t elapsed=now_us_mono()-started,period=1000000u/(unsigned)w->cfg.fps;
        if (elapsed<period) usleep((useconds_t)(period-elapsed));
    }
    free(frame); return NULL;
}
static void add_boxes(VisionResult *v,const yolo_boxes_t *boxes,int camera,int persons)
{
    for (int i=0;i<boxes->count;i++) {
        const yolo_box_t *b=&boxes->boxes[i];
        if (persons && b->class_id!=0) continue;
        if (persons) v->person_mask|=1u<<camera;
        if (v->box_count==VISION_BOXES) continue;
        VisionBox box={(int16_t)b->x1,(int16_t)b->y1,(int16_t)b->x2,(int16_t)b->y2,
                      (uint16_t)(b->score*100+.5f),(uint16_t)(persons?0:1),(uint16_t)camera,0};
        if (box.x2>box.x1 && box.y2>box.y1) v->boxes[v->box_count++]=box;
    }
}
static void road_geometry(VisionResult *v,const lane_seg_t *seg)
{
    for (int side=0;side<2;side++) {
        int previous=-1;
        for (int i=VISION_POINTS-1;i>=0;i--) {
            int y=80+i*5,lo=side?160:0,hi=side?320:160,best=-1;
            float score=LM_THRESH;
            if (previous>=0) { lo=previous-35; hi=previous+36; if(lo<0)lo=0; if(hi>320)hi=320; }
            for (int x=lo;x<hi;x++) if (seg->lane_prob[y*320+x]>score) {score=seg->lane_prob[y*320+x];best=x;}
            if (best>=0) {
                int a=best,b=best;
                while(a>lo && seg->lane_prob[y*320+a-1]>LM_THRESH)a--;
                while(b+1<hi && seg->lane_prob[y*320+b+1]>LM_THRESH)b++;
                best=(a+b)/2;
            }
            v->lane_x[side][i]=(int16_t)best;
            if (best>=0) previous=best;
        }
        /* Interpolate short gaps in the path; annotation still uses the measured
         * solid/dashed classification, and never draws an unknown line type. */
        for (int i=1;i<VISION_POINTS;i++) if(v->lane_x[side][i]<0) {
            int j=i+1; while(j<VISION_POINTS && v->lane_x[side][j]<0)j++;
            int a=v->lane_x[side][i-1];
            if(a>=0 && j<VISION_POINTS && j-i<=8)
                for(int k=i;k<j;k++)v->lane_x[side][k]=(int16_t)(a+(v->lane_x[side][j]-a)*(k-i+1)/(j-i+1));
        }
    }
    for(int y=0;y<60;y++)for(int x=0;x<80;x++) {
        size_t p=(size_t)(y*4)*320+x*4;
        if(seg->drivable[320*240+p]>seg->drivable[p])v->drivable[(y*80+x)/8]|=(uint8_t)(1u<<((y*80+x)%8));
    }

}
static void *inference_loop(void *arg)
{
    vision_worker_t *w=arg;
    uint8_t *frame=malloc(IMG_FRAME_BYTES),*rgb=malloc(320*240*3);
    lane_seg_t seg={0};
    if(!frame||!rgb||lane_seg_alloc(&seg,320,240)<0){atomic_store(&w->run,0);goto done;}
    uint32_t last=0;
    while(atomic_load(&w->run)) {
        uint32_t fid; uint64_t captured; int ready;
        pthread_mutex_lock(&w->lock);
        fid=w->frame_id;captured=w->captured;ready=w->available;
        if(ready && (uint32_t)(fid-last)>=(unsigned)w->cfg.interval)memcpy(frame,w->latest,IMG_FRAME_BYTES);
        pthread_mutex_unlock(&w->lock);
        if(!ready || (uint32_t)(fid-last)<(unsigned)w->cfg.interval){usleep(5000);continue;}
        last=fid;
        VisionResult v;memset(&v,0,sizeof(v));memset(v.lane_x,0xff,sizeof(v.lane_x));
        v.version=VISION_VERSION;v.analysis_frame_id=fid;v.front_camera=1;v.healthy=1;
        for(int cam=0;cam<3;cam++) {
            yolo_boxes_t boxes;
            vision_crop_rgb(frame,cam,rgb);
            if(rknn_vision_run(&w->person,rgb,320,240,0,&boxes,NULL,NULL)<0){v.healthy=0;break;}
            add_boxes(&v,&boxes,cam,1);
            if(v.person_mask){
                /* Publish the hazard as soon as any camera detects a person;
                 * do not wait for remaining cameras / multi-task inference. */
                pthread_mutex_lock(&w->lock);w->result.person_mask|=v.person_mask;pthread_mutex_unlock(&w->lock);
            }
        }
        if(w->cfg.master && v.healthy) {
            yolo_boxes_t boxes;
            vision_crop_rgb(frame,1,rgb);
            if(rknn_vision_run(&w->road,rgb,320,240,-1,&boxes,seg.lane_prob,seg.drivable)<0) v.healthy=0;
            else {
                add_boxes(&v,&boxes,1,0);
                if(turn_decide(&seg,320,240,&v.lane)<0 || lane_marks_analyze(&seg,320,240,&v.marks)<0 ||
                   zebra_detect_rows(rgb,320,240,&v.zebra,v.zebra_rows,&v.zebra_count)<0 || traffic_light_detect(rgb,320,240,&v.light)<0) v.healthy=0;
                road_geometry(&v,&seg);
                v.lane.version=LANERESULT_VERSION;v.lane.frame_id=fid;
                v.light.version=TL_VERSION;v.marks.version=LANEMARK_VERSION;v.zebra.version=ZEBRA_VERSION;
            }
        }
        pthread_mutex_lock(&w->lock);w->result=v;w->analyzed=captured;pthread_mutex_unlock(&w->lock);
        fprintf(stderr,"[VISION] analysis=%u healthy=%u people=0x%x light=%s lane=%d/%d zebra=%u latency=%.1fms\n",
                fid,v.healthy,v.person_mask,(const char *[]) {"UNKNOWN","RED","YELLOW","GREEN"}[v.light.state],v.marks.left_type,v.marks.right_type,v.zebra.detected,
                (now_us_mono()-captured)/1000.0);
    }
done:
    lane_seg_free(&seg);free(frame);free(rgb);return NULL;
}
int vision_worker_start(vision_worker_t **out,const vision_worker_cfg_t *cfg)
{
    if(!out||!cfg||cfg->fps<1||cfg->fps>30||(cfg->interval!=1&&cfg->interval!=3&&cfg->interval!=5))return -1;
    *out=NULL;vision_worker_t *w=calloc(1,sizeof(*w));if(!w)return -1;
    w->cfg=*cfg;w->grab.fd=-1;pthread_mutex_init(&w->lock,NULL);atomic_init(&w->run,1);
    if(rknn_vision_init(&w->person,cfg->person_model,0)<0 ||
       (cfg->master&&rknn_vision_init(&w->road,cfg->lane_model,1)<0)||
       pcie_capture_open(&w->grab,640,480,PCIE_LEAD_PIXELS)<0||pcie_capture_start(&w->grab)<0)goto fail;
    if(pthread_create(&w->capture_thread,NULL,capture_loop,w))goto fail;
    w->capture_started=1;
    if(pthread_create(&w->inference_thread,NULL,inference_loop,w))goto fail;
    w->inference_started=1;*out=w;return 0;
fail: vision_worker_stop(w);return -1;
}
int vision_worker_snapshot(vision_worker_t *w,VisionFrame *f,uint64_t *captured)
{
    if(!w||!f||!captured)return -1;
    pthread_mutex_lock(&w->lock);
    int ok=w->available&&atomic_load(&w->run);
    if(ok) {
        memcpy(f->image,w->latest,IMG_FRAME_BYTES); f->result=w->result;
        f->result.version=VISION_VERSION;f->result.frame_id=w->frame_id;*captured=w->captured;
        uint64_t age=w->analyzed?(now_us_mono()-w->analyzed)/1000:UINT32_MAX;
        f->result.age_ms=age>UINT32_MAX?UINT32_MAX:(uint32_t)age;
    }
    pthread_mutex_unlock(&w->lock);return ok?0:-1;
}
void vision_worker_stop(vision_worker_t *w)
{
    if(!w)return;
    atomic_store(&w->run,0);
    if(w->capture_started)pthread_join(w->capture_thread,NULL);
    if(w->inference_started)pthread_join(w->inference_thread,NULL);
    pcie_capture_stop(&w->grab);pcie_capture_close(&w->grab);
    rknn_vision_close(&w->person);rknn_vision_close(&w->road);pthread_mutex_destroy(&w->lock);free(w);
}
#else
int vision_worker_start(vision_worker_t **o,const vision_worker_cfg_t *c){(void)o;(void)c;return -1;}
int vision_worker_snapshot(vision_worker_t *w,VisionFrame *f,uint64_t *t){(void)w;(void)f;(void)t;return -1;}
void vision_worker_stop(vision_worker_t *w){(void)w;}
#endif
