#ifndef YOLO_DECODE_H
#define YOLO_DECODE_H
#include <stddef.h>
#include <stdint.h>
#define YOLO_MAX_BOXES 64
typedef struct { float x1,y1,x2,y2,score; int class_id; } yolo_box_t;
typedef struct { int count; yolo_box_t boxes[YOLO_MAX_BOXES]; } yolo_boxes_t;
typedef struct { int sw,sh,dw,dh,rw,rh,px,py; float sx,sy; } letterbox_t;
typedef struct { const float *data; size_t count; int w,h,channels,nhwc; } yolo_head_t;
extern const float yolo5_anchors[18];
extern const float yolop2_anchors[18];
int letterbox_rgb(const uint8_t *src,int sw,int sh,uint8_t *dst,int dw,int dh,letterbox_t *map);
/* Three stride-8/16/32 heads; filter=-1 accepts all classes. logits is explicit. */
int yolo_decode(const yolo_head_t heads[3],const float anchors[18],int logits,
                int filter,float threshold,float nms,const letterbox_t *map,yolo_boxes_t *out);
int yolo_load_anchors(const char *path,float anchors[18]);
#endif
