/* Synthetic tensors, not a claim about NPU accuracy. Exercises the production
 * RKNN adapter too: metadata query, uint8 input, output lifetime, segmentation. */
#include "rknn_vision.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int multi=0,bad_shape=0,bad_output=0,releases=0;
int rknn_init(rknn_context *c,void *p,uint32_t n,uint32_t flags,rknn_init_extend *e){(void)p;(void)n;(void)flags;(void)e;*c=1;return 0;}
int rknn_destroy(rknn_context c){(void)c;return 0;}
int rknn_query(rknn_context c,rknn_query_cmd cmd,void *out,uint32_t bytes){
 (void)c;(void)bytes;
 if(cmd==RKNN_QUERY_IN_OUT_NUM){rknn_input_output_num *n=out;n->n_input=1;n->n_output=multi?5:3;return 0;}
 rknn_tensor_attr *a=out;unsigned idx=a->index;memset(a,0,sizeof(*a));a->index=idx;
 a->n_dims=4;a->dims[0]=1;a->fmt=RKNN_TENSOR_NCHW;a->type=RKNN_TENSOR_FLOAT32;
 int w=640,h=multi?480:640,ch=3;
 if(cmd==RKNN_QUERY_OUTPUT_ATTR){
  if(multi&&idx<2)ch=idx==0?2:1;
  else {int head=(int)idx-(multi?2:0);w>>=(3+head);h>>=(3+head);ch=bad_shape?254:255;}
 }
 a->dims[1]=ch;a->dims[2]=h;a->dims[3]=w;a->n_elems=w*h*ch;a->size=a->n_elems*4;return 0;
}
int rknn_inputs_set(rknn_context c,uint32_t n,rknn_input *i){(void)c;assert(n==1&&i->fmt==RKNN_TENSOR_NHWC&&i->type==RKNN_TENSOR_UINT8);return 0;}
int rknn_run(rknn_context c,rknn_run_extend *e){(void)c;(void)e;return 0;}
int rknn_outputs_get(rknn_context c,uint32_t n,rknn_output *o,rknn_output_extend *e){
 (void)e;
 for(unsigned i=0;i<n;i++){
  rknn_tensor_attr a={0};a.index=i;rknn_query(c,RKNN_QUERY_OUTPUT_ATTR,&a,sizeof(a));
  float *p=malloc(a.size);assert(p);o[i].buf=p;o[i].size=a.size;
  for(unsigned j=0;j<a.n_elems;j++)p[j]=multi&&i>=2?-20:0;
  if(multi&&i<2){unsigned plane=640*480;for(unsigned j=0;j<plane;j++){p[j]=i==0?.1f:.8f;if(i==0)p[plane+j]=.9f;}}
 }
 if(bad_output)((float *)o[n-1].buf)[0]=NAN;
 return 0;
}
int rknn_outputs_release(rknn_context c,uint32_t n,rknn_output *o){(void)c;for(unsigned i=0;i<n;i++)free(o[i].buf);releases++;return 0;}
static void decoder_test(void){
 uint8_t *src=malloc(320*240*3),*dst=malloc(640*640*3);assert(src&&dst);memset(src,64,320*240*3);
 letterbox_t map;assert(letterbox_rgb(src,320,240,dst,640,640,&map)==0);
 assert(map.py==80&&map.px==0&&map.sx==2&&map.sy==2&&dst[0]==114&&dst[80*640*3]==64);
 yolo_head_t head[3];float *mem[3];
 for(int i=0;i<3;i++){int w=80>>i;mem[i]=calloc(w*w*255,sizeof(float));head[i]=(yolo_head_t){mem[i],(size_t)w*w*255,w,w,255,0};}
 int cell=20*80+30,plane=80*80;
 /* Two anchors describe identical 10x13 boxes: NMS must retain only best. */
 for(int a=0;a<2;a++){
  int b=a*85;float *p=mem[0];p[(b+0)*plane+cell]=p[(b+1)*plane+cell]=.5f;
  p[(b+2)*plane+cell]=.5f*sqrtf(10/yolo5_anchors[a*2]);
  p[(b+3)*plane+cell]=.5f*sqrtf(13/yolo5_anchors[a*2+1]);
  p[(b+4)*plane+cell]=.9f;p[(b+5)*plane+cell]=a?.8f:.95f;
 }
 yolo_boxes_t boxes;assert(yolo_decode(head,yolo5_anchors,0,0,.35f,.45f,&map,&boxes)==0&&boxes.count==1);
 assert(fabsf(boxes.boxes[0].x1-119.5f)<.01f&&fabsf(boxes.boxes[0].y1-38.75f)<.01f);
 /* Transpose to NHWC, numerical output must stay identical. */
 for(int l=0;l<3;l++){int cells=head[l].w*head[l].h;float *a=malloc(head[l].count*4);
  for(int c=0;c<255;c++)for(int j=0;j<cells;j++)a[j*255+c]=mem[l][c*cells+j];
  free(mem[l]);mem[l]=a;head[l].data=a;head[l].nhwc=1;}
 assert(yolo_decode(head,yolo5_anchors,0,0,.35f,.45f,&map,&boxes)==0&&boxes.count==1);
 assert(yolo_decode(head,yolo5_anchors,0,2,.35f,.45f,&map,&boxes)==0&&boxes.count==0);
 for(int l=0;l<3;l++)for(size_t i=0;i<head[l].count;i++) {
  float v=mem[l][i];mem[l][i]=v<=0?-30:logf(v/(1-v));
 }
 assert(yolo_decode(head,yolo5_anchors,1,0,.35f,.45f,&map,&boxes)==0&&boxes.count==1);
 assert(fabsf(boxes.boxes[0].x1-119.5f)<.01f);
 mem[2][0]=NAN;assert(yolo_decode(head,yolo5_anchors,0,0,.35f,.45f,&map,&boxes)<0);
 for(int l=0;l<3;l++)free(mem[l]);
 free(src);free(dst);puts("YOLO: letterbox/coordinate restoration, NCHW/NHWC, confidence, class filter, NMS, NaN PASS");
}
int main(void){
 decoder_test();const char *path="tmp/fake-profile.rknn";FILE *f=fopen(path,"wb");assert(f);fputc(0,f);fclose(f);
 uint8_t *rgb=calloc(320*240*3,1);float *lane=malloc(320*240*4),*drive=malloc(320*240*8);assert(rgb&&lane&&drive);
 for(multi=0;multi<2;multi++){
  rknn_vision_t m;yolo_boxes_t boxes;assert(rknn_vision_init(&m,path,multi)==0);
  assert(rknn_vision_run(&m,rgb,320,240,0,&boxes,lane,drive)==0&&boxes.count==0);
  if(multi)assert(fabsf(lane[320*239]-.8f)<.001f&&fabsf(drive[320*240+50]-.9f)<.001f);
  bad_output=1;assert(rknn_vision_run(&m,rgb,320,240,0,&boxes,lane,drive)<0);bad_output=0;
  rknn_vision_close(&m);bad_shape=1;assert(rknn_vision_init(&m,path,multi)<0);bad_shape=0;
 }
 assert(releases==4);remove(path);free(rgb);free(lane);free(drive);
 puts("RKNN adapter: both repository profiles, segmentation scales, shape failure, output release PASS");return 0;
}
