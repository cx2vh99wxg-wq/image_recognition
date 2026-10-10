#include "rknn_vision.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

static int shape(const rknn_tensor_attr *a, int *w, int *h, int *c)
{
    if (a->n_dims != 4 || a->dims[0] != 1) return -1;
    if (a->fmt == RKNN_TENSOR_NCHW) {
        *c=a->dims[1]; *h=a->dims[2]; *w=a->dims[3];
    } else if (a->fmt == RKNN_TENSOR_NHWC) {
        *h=a->dims[1]; *w=a->dims[2]; *c=a->dims[3];
    } else return -1;
    return *w>0 && *h>0 && *w<=2048 && *h<=2048 && *c>0 &&
        (size_t)*w * *h * *c == a->n_elems ? 0 : -1;
}
void rknn_vision_close(rknn_vision_t *m)
{
    if (!m) return;
    if (m->ctx) rknn_destroy(m->ctx);
    free(m->pixels);
    memset(m,0,sizeof(*m));
}
int rknn_vision_init(rknn_vision_t *m, const char *path, int multitask)
{
    if (!m || !path) return -1;
    memset(m,0,sizeof(*m));
    m->drive=m->lane=-1;
    for (int i=0;i<3;i++) m->heads[i]=-1;
    FILE *f=fopen(path,"rb");
    if (!f) return -1;
    if (fseek(f,0,SEEK_END)) { fclose(f); return -1; }
    long size=ftell(f);
    if (size<=0 || (unsigned long)size>UINT_MAX || fseek(f,0,SEEK_SET)) { fclose(f); return -1; }
    void *blob=malloc((size_t)size);
    if (!blob) { fclose(f); return -1; }
    size_t got=fread(blob,1,(size_t)size,f); fclose(f);
    int rc=got==(size_t)size ? rknn_init(&m->ctx,blob,(uint32_t)size,0,NULL) : -1;
    free(blob);
    if (rc<0) goto fail;
    rknn_input_output_num io={0};
    if (rknn_query(m->ctx,RKNN_QUERY_IN_OUT_NUM,&io,sizeof(io))<0 || io.n_input!=1 ||
        io.n_output!=(multitask?5u:3u)) goto fail;
    m->count=(int)io.n_output; m->multitask=multitask; m->logits=multitask;
    if (rknn_query(m->ctx,RKNN_QUERY_INPUT_ATTR,&m->input,sizeof(m->input))<0) goto fail;
    int c;
    if (shape(&m->input,&m->width,&m->height,&c)<0 || c!=3 || m->width%32 || m->height%32) goto fail;
    for (int i=0;i<m->count;i++) {
        rknn_tensor_attr *a=&m->output[i]; a->index=(uint32_t)i;
        int w,h;
        if (rknn_query(m->ctx,RKNN_QUERY_OUTPUT_ATTR,a,sizeof(*a))<0 || shape(a,&w,&h,&c)<0) goto fail;
        fprintf(stderr,"[RKNN] output %d %s: %dx%dx%d format=%d type=%d zp=%d scale=%g\n",
                i,a->name,w,h,c,a->fmt,a->type,a->zp,a->scale);
        if (multitask && w==m->width && h==m->height && c==1) m->lane=i;
        else if (multitask && w==m->width && h==m->height && c==2) m->drive=i;
        else {
            int level=-1;
            for (int l=0;l<3;l++) if (w*(8<<l)==m->width && h*(8<<l)==m->height) level=l;
            if (level<0 || c!=255 || m->heads[level]>=0) goto fail;
            m->heads[level]=i;
        }
    }
    for (int l=0;l<3;l++) if (m->heads[l]<0) goto fail;
    if (multitask && (m->lane<0 || m->drive<0)) goto fail;
    memcpy(m->anchors,multitask?yolop2_anchors:yolo5_anchors,sizeof(m->anchors));
    const char *override=getenv(multitask?"YOLOP_ANCHORS":"YOLO5_ANCHORS");
    if (override && yolo_load_anchors(override,m->anchors)<0) goto fail;
    fprintf(stderr,"[RKNN] anchors: %s%s\n",override?override:"reference defaults",
            override?"":" (export anchor grid unavailable; verify box sizes on target)");
    m->pixels=malloc((size_t)m->width*m->height*3);
    if (!m->pixels) goto fail;
    fprintf(stderr,"[RKNN] %s %dx%d, RGB uint8 letterbox; detection logits=%d\n",path,m->width,m->height,m->logits);
    return 0;
fail:
    fprintf(stderr,"[RKNN] model initialization/profile mismatch: %s (no empty-detection fallback)\n",path);
    rknn_vision_close(m); return -1;
}
static float value(const float *p,const rknn_tensor_attr *a,int x,int y,int ch)
{
    int w,h,c;
    if (shape(a,&w,&h,&c)<0) return NAN;
    return p[a->fmt==RKNN_TENSOR_NHWC ? ((size_t)y*w+x)*c+ch : (size_t)ch*w*h+y*w+x];
}
int rknn_vision_run(rknn_vision_t *m,const uint8_t *rgb,int w,int h,
                    int filter,yolo_boxes_t *boxes,float *lane,float *drive)
{
    if (!m || !m->ctx || !boxes || (m->multitask && (!lane || !drive))) return -1;
    letterbox_t map;
    if (letterbox_rgb(rgb,w,h,m->pixels,m->width,m->height,&map)<0) return -1;
    rknn_input input={0}; input.type=RKNN_TENSOR_UINT8; input.fmt=RKNN_TENSOR_NHWC;
    input.size=(uint32_t)m->width*m->height*3; input.buf=m->pixels;
    if (rknn_inputs_set(m->ctx,1,&input)<0 || rknn_run(m->ctx,NULL)<0) return -1;
    rknn_output outputs[5]; memset(outputs,0,sizeof(outputs));
    for (int i=0;i<m->count;i++) { outputs[i].index=(uint32_t)i; outputs[i].want_float=1; }
    if (rknn_outputs_get(m->ctx,(uint32_t)m->count,outputs,NULL)<0) return -1;
    int rc=-1;
    for (int i=0;i<m->count;i++)
        if (!outputs[i].buf || outputs[i].size != m->output[i].n_elems*sizeof(float)) goto done;
    yolo_head_t heads[3];
    for (int l=0;l<3;l++) {
        int i=m->heads[l]; rknn_tensor_attr *a=&m->output[i];
        heads[l].data=outputs[i].buf; heads[l].count=a->n_elems;
        if (shape(a,&heads[l].w,&heads[l].h,&heads[l].channels)<0) goto done;
        heads[l].nhwc=(a->fmt==RKNN_TENSOR_NHWC);
    }
    rc=yolo_decode(heads,m->anchors,m->logits,filter,.35f,.45f,&map,boxes);
    if (rc<0 || !m->multitask) goto done;
    for (int y=0;y<h;y++) for (int x=0;x<w;x++) {
        int mx=map.px+(int)((x+.5f)*map.sx), my=map.py+(int)((y+.5f)*map.sy);
        if (mx>=m->width) mx=m->width-1;
        if (my>=m->height) my=m->height-1;
        float ll=value(outputs[m->lane].buf,&m->output[m->lane],mx,my,0);
        float bg=value(outputs[m->drive].buf,&m->output[m->drive],mx,my,0);
        float fg=value(outputs[m->drive].buf,&m->output[m->drive],mx,my,1);
        if (!isfinite(ll)||!isfinite(bg)||!isfinite(fg)||ll<0||ll>1||bg<0||bg>1||fg<0||fg>1) { rc=-1; goto done; }
        lane[y*w+x]=ll; drive[y*w+x]=bg; drive[(size_t)w*h+y*w+x]=fg;
    }
done:
    rknn_outputs_release(m->ctx,(uint32_t)m->count,outputs);
    return rc;
}
