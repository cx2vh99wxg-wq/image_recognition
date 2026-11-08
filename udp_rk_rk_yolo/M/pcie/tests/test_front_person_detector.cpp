// Runs the production decoder with deterministic RKNN head tensors; no device IO.
#include "traffic_detector.h"
#include "rknn_api.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <cstring>
using namespace traffic;
static std::vector<float> heads[3];
static int calls=0,mode=0;
extern "C" {
int rknn_init(rknn_context* c,void*,uint32_t,uint32_t,rknn_init_extend*){*c=1;return 0;}
int rknn_destroy(rknn_context){return 0;}
int rknn_query(rknn_context,rknn_query_cmd cmd,void* out,uint32_t){
 if(cmd==RKNN_QUERY_IN_OUT_NUM){auto* x=(rknn_input_output_num*)out;x->n_input=1;x->n_output=3;return 0;}
 auto* a=(rknn_tensor_attr*)out;a->n_dims=4;a->fmt=RKNN_TENSOR_NCHW;a->type=RKNN_TENSOR_FLOAT32;
 if(cmd==RKNN_QUERY_INPUT_ATTR){a->dims[0]=1;a->dims[1]=3;a->dims[2]=a->dims[3]=320;return 0;}
 assert(cmd==RKNN_QUERY_OUTPUT_ATTR && a->index<3);a->dims[0]=1;a->dims[1]=255;a->dims[2]=a->dims[3]=40>>a->index;return 0;
}
int rknn_inputs_set(rknn_context,uint32_t n,rknn_input* p){assert(n==1 && p->fmt==RKNN_TENSOR_NHWC);return 0;}
int rknn_run(rknn_context,rknn_run_extend*){++calls;return mode==3?-1:0;}
int rknn_outputs_get(rknn_context,uint32_t n,rknn_output* p,rknn_output_extend*){
 assert(n==3);
 for(int k=0;k<3;++k){int dim=40>>k,sz=dim*dim;heads[k].assign(255*sz,0.f);
  if(!k && mode!=2){int pos=32*dim+20; // near bottom, outside signal ROI
   for(int ch=0;ch<4;++ch)heads[k][ch*sz+pos]=.5f;
   heads[k][4*sz+pos]=.5f;heads[k][5*sz+pos]=mode==1?.299f:.3f;
   if(mode==4)heads[k][6*sz+pos]=.9f; // class1 outranks person
  }
  p[k].buf=heads[k].data();p[k].size=heads[k].size()*sizeof(float);
 }
 return 0;
}
int rknn_outputs_release(rknn_context,uint32_t,rknn_output*){return 0;}
}
int main(){
 const char* model="build/front-person-decoder.fixture";{std::ofstream f(model);f<<"stub";}
 Detector d;std::string error;assert(d.init(model,&error));
 std::vector<uint8_t> rgb(320*240*3,114);std::vector<Box> traffic;
 PersonResult people;calls=0;
 assert(d.detect(rgb.data(),320,240,960,traffic,&error,60,&people));
 assert(calls==1 && people.count==1 && people.best.score==.15f && people.best.y>190 && traffic.empty());
 assert(people.candidates.size()==1 && people.candidates[0].score==people.best.score);
 mode=1;assert(d.detect(rgb.data(),320,240,960,traffic,&error,60,&people) && !people.count);
 mode=2;assert(d.detect(rgb.data(),320,240,960,traffic,&error,60,&people) && !people.count);
 mode=4;assert(d.detect(rgb.data(),320,240,960,traffic,&error,60,&people) && !people.count);
 mode=0;assert(d.detect(rgb.data(),320,240,960,traffic,&error) && traffic.empty()); // person isn't a signal
 mode=3;assert(!d.detect(rgb.data(),320,240,960,traffic,&error,60,&people) && !people.count);
 assert(people.candidates.empty());
 std::remove(model);puts("FRONT PERSON DECODER PASS: COCO class0, exact float threshold, full frame, no housing/color filter, one NPU call, clear/error");
}
