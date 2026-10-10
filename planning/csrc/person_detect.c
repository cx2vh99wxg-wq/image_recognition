#include "person_detect.h"
#include "rknn_vision.h"
#include <stdlib.h>
#include <string.h>
struct person_detect_ctx { int w,h; rknn_vision_t model; };
int person_detect_init(person_detect_ctx_t **ctx,const char *path,int w,int h)
{
    if (!ctx || w<=0 || h<=0) return -1;
    *ctx=NULL;
    person_detect_ctx_t *p=calloc(1,sizeof(*p));
    if (!p) return -1;
    p->w=w; p->h=h;
    if (rknn_vision_init(&p->model,path?path:"model/yolov5s-640-640.rknn",0)<0) { free(p); return -1; }
    *ctx=p; return 0;
}
int person_detect_boxes(person_detect_ctx_t *ctx,const uint8_t *rgb,yolo_boxes_t *boxes)
{
    return ctx ? rknn_vision_run(&ctx->model,rgb,ctx->w,ctx->h,0,boxes,NULL,NULL) : -1;
}
int person_detect_run(person_detect_ctx_t *ctx,const uint8_t *rgb,uint32_t frame,
                      uint64_t now,PersonState *out)
{
    if (!out) return -1;
    memset(out,0,sizeof(*out));
    yolo_boxes_t boxes;
    if (person_detect_boxes(ctx,rgb,&boxes)<0) return -1;
    out->version=PERSON_VERSION; out->frame_id=frame; out->timestamp_us=now;
    out->box_x=out->box_y=out->box_w=out->box_h=-1;
    out->person_count=(uint32_t)boxes.count;
    if (boxes.count) {
        const yolo_box_t *b=&boxes.boxes[0];
        out->detected=1; out->confidence=(uint32_t)(b->score*100+.5f);
        out->box_x=(int)b->x1; out->box_y=(int)b->y1;
        out->box_w=(int)(b->x2-b->x1); out->box_h=(int)(b->y2-b->y1);
    }
    return 0;
}
void person_detect_deinit(person_detect_ctx_t *ctx)
{
    if (ctx) { rknn_vision_close(&ctx->model); free(ctx); }
}
