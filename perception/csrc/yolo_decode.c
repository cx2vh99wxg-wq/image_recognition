#include "yolo_decode.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const float yolo5_anchors[18]={10,13,16,30,33,23,30,61,62,45,59,119,116,90,156,198,373,326};
/* YOLOPv2/YOLOv7 reference profile. Override with the export's anchor grid. */
const float yolop2_anchors[18]={12,16,19,36,40,28,36,75,76,55,72,146,142,110,192,243,459,401};
int yolo_load_anchors(const char *path,float a[18]) {
    FILE *f=fopen(path,"r"); if(!f)return -1;
    float tmp[18]; int ok=1;
    for(int i=0;i<18;i++)if(fscanf(f,"%f",&tmp[i])!=1 || !isfinite(tmp[i]) || tmp[i]<=0) {ok=0;break;}
    fclose(f); if(!ok)return -1; memcpy(a,tmp,sizeof(tmp));return 0;
}
int letterbox_rgb(const uint8_t *src,int sw,int sh,uint8_t *dst,int dw,int dh,letterbox_t *m) {
    if(!src||!dst||!m||sw<1||sh<1||dw<1||dh<1||sw>8192||sh>8192||dw>8192||dh>8192)return -1;
    float scale=fminf((float)dw/sw,(float)dh/sh);
    *m=(letterbox_t){.sw=sw,.sh=sh,.dw=dw,.dh=dh,.rw=(int)lroundf(sw*scale),.rh=(int)lroundf(sh*scale)};
    if(m->rw<1)m->rw=1;
    if(m->rh<1)m->rh=1;
    m->px=(dw-m->rw)/2;m->py=(dh-m->rh)/2;m->sx=(float)m->rw/sw;m->sy=(float)m->rh/sh;
    memset(dst,114,(size_t)dw*dh*3);
    for(int y=0;y<m->rh;y++)for(int x=0;x<m->rw;x++) {
        float fx=fmaxf(0,(x+.5f)/m->sx-.5f),fy=fmaxf(0,(y+.5f)/m->sy-.5f);
        int x0=(int)fx,y0=(int)fy,x1=x0+1<sw?x0+1:x0,y1=y0+1<sh?y0+1:y0;
        float ax=fx-x0,ay=fy-y0;
        for(int c=0;c<3;c++) {
            float top=src[((size_t)y0*sw+x0)*3+c]*(1-ax)+src[((size_t)y0*sw+x1)*3+c]*ax;
            float bot=src[((size_t)y1*sw+x0)*3+c]*(1-ax)+src[((size_t)y1*sw+x1)*3+c]*ax;
            dst[((size_t)(y+m->py)*dw+x+m->px)*3+c]=(uint8_t)lroundf(top*(1-ay)+bot*ay);
        }
    }
    return 0;
}
static float prob(float v,int logits) {
    if(!isfinite(v))return -1;
    if(logits)return v>=0?1.f/(1.f+expf(-v)):expf(v)/(1.f+expf(v));
    return v>=-.001f && v<=1.001f?fminf(1,fmaxf(0,v)):-1;
}
static float at(const yolo_head_t *h,int cell,int c) {
    return h->data[h->nhwc?(size_t)cell*h->channels+c:(size_t)c*h->w*h->h+cell];
}
static float iou(const yolo_box_t *a,const yolo_box_t *b) {
    float inter=fmaxf(0,fminf(a->x2,b->x2)-fmaxf(a->x1,b->x1))*fmaxf(0,fminf(a->y2,b->y2)-fmaxf(a->y1,b->y1));
    float u=(a->x2-a->x1)*(a->y2-a->y1)+(b->x2-b->x1)*(b->y2-b->y1)-inter;
    return u>0?inter/u:0;
}
static int cmp_box(const void *a,const void *b) {
    float x=((const yolo_box_t*)a)->score,y=((const yolo_box_t*)b)->score;return x>y?-1:x<y;
}
int yolo_decode(const yolo_head_t heads[3],const float anchors[18],int logits,int filter,float th,float nms,
                const letterbox_t *m,yolo_boxes_t *out) {
    enum { MAX_CANDIDATES=1024 };
    if(!heads||!anchors||!m||!out||m->sx<=0||m->sy<=0||th<=0||th>1||nms<0||nms>1)return -1;
    memset(out,0,sizeof(*out));yolo_box_t *cand=malloc(sizeof(*cand)*MAX_CANDIDATES);if(!cand)return -1;
    int count=0;
    for(int l=0;l<3;l++) {
        const yolo_head_t *h=&heads[l];int stride=8<<l,attrs=h->channels/3;
        if(!h->data||attrs<6||h->channels%3||h->w*stride!=m->dw||h->h*stride!=m->dh||
           h->count!=(size_t)h->w*h->h*h->channels||filter>=attrs-5) {free(cand);return -1;}
        /* A malformed/model-profile-mismatched tensor must not become an empty
         * scene that authorises motion. Validate even low-confidence cells. */
        for(size_t j=0;j<h->count;j++) if(prob(h->data[j],logits)<0) {free(cand);return -1;}
        for(int a=0;a<3;a++)for(int y=0;y<h->h;y++)for(int x=0;x<h->w;x++) {
            int cell=y*h->w+x,base=a*attrs;
            float obj=prob(at(h,cell,base+4),logits);if(obj<th)continue;
            float best=0;int cls=-1;
            for(int c=0;c<attrs-5;c++) {
                if(filter>=0&&filter!=c)continue;
                float p=prob(at(h,cell,base+5+c),logits);if(p>best){best=p;cls=c;}
            }
            float score=obj*best;if(cls<0||score<th)continue;
            float px=prob(at(h,cell,base),logits),py=prob(at(h,cell,base+1),logits);
            float pw=prob(at(h,cell,base+2),logits),ph=prob(at(h,cell,base+3),logits);
            if(px<0||py<0||pw<0||ph<0)continue;
            float cx=(px*2-.5f+x)*stride,cy=(py*2-.5f+y)*stride;
            float w=pw*pw*4*anchors[l*6+a*2],hh=ph*ph*4*anchors[l*6+a*2+1];
            yolo_box_t b={fmaxf(0,(cx-w/2-m->px)/m->sx),fmaxf(0,(cy-hh/2-m->py)/m->sy),
                fminf((float)m->sw,(cx+w/2-m->px)/m->sx),fminf((float)m->sh,(cy+hh/2-m->py)/m->sy),score,cls};
            if(b.x2<=b.x1||b.y2<=b.y1)continue;
            if(count<MAX_CANDIDATES)cand[count++]=b;
            else {int worst=0;for(int k=1;k<count;k++)if(cand[k].score<cand[worst].score)worst=k;
                  if(b.score>cand[worst].score)cand[worst]=b;}
        }
    }
    qsort(cand,(size_t)count,sizeof(*cand),cmp_box);
    for(int i=0;i<count&&out->count<YOLO_MAX_BOXES;i++) {
        int keep=1;for(int j=0;j<out->count;j++)if(cand[i].class_id==out->boxes[j].class_id&&iou(&cand[i],&out->boxes[j])>nms){keep=0;break;}
        if(keep)out->boxes[out->count++]=cand[i];
    }
    free(cand);return 0;
}
