// Exercise the actual decoder, with float/INT8 and NCHW/NHWC RKNN tensors.
#include "traffic_detector.h"
#include "rknn_api.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <fstream>
using namespace traffic;
static bool quant=false,nhwc=false;
static int mode=0,calls=0;
static std::vector<float> floats[3];
static std::vector<int8_t> ints[3];
extern "C" {
int rknn_init(rknn_context *c,void*,uint32_t,uint32_t,rknn_init_extend*){*c=1;return 0;}
int rknn_destroy(rknn_context){return 0;}
int rknn_query(rknn_context,rknn_query_cmd cmd,void *out,uint32_t){
    if(cmd==RKNN_QUERY_IN_OUT_NUM){auto *n=(rknn_input_output_num*)out;n->n_input=1;n->n_output=3;return 0;}
    auto *a=(rknn_tensor_attr*)out;a->n_dims=4;a->dims[0]=1;
    a->fmt=nhwc?RKNN_TENSOR_NHWC:RKNN_TENSOR_NCHW;
    if(cmd==RKNN_QUERY_INPUT_ATTR){a->type=RKNN_TENSOR_UINT8;a->dims[1]=nhwc?320:3;a->dims[2]=320;a->dims[3]=nhwc?3:320;return 0;}
    assert(cmd==RKNN_QUERY_OUTPUT_ATTR&&a->index<3);const int d=40>>a->index;
    a->dims[1]=nhwc?d:255;a->dims[2]=d;a->dims[3]=nhwc?255:d;
    a->type=quant?RKNN_TENSOR_INT8:RKNN_TENSOR_FLOAT32;
    a->qnt_type=quant?RKNN_TENSOR_QNT_AFFINE_ASYMMETRIC:RKNN_TENSOR_QNT_NONE;a->zp=-128;a->scale=1.f/255;return 0;
}
int rknn_inputs_set(rknn_context,uint32_t n,rknn_input *p){assert(n==1&&p->fmt==RKNN_TENSOR_NHWC);return 0;}
int rknn_run(rknn_context,rknn_run_extend*){++calls;return 0;}
int rknn_outputs_get(rknn_context,uint32_t n,rknn_output *out,rknn_output_extend*){
    assert(n==3);
    for(int k=0;k<3;++k){const int d=40>>k,area=d*d;floats[k].assign(255*area,0.f);
        const auto set=[&](int channel,int pos,float value){floats[k][nhwc?pos*255+channel:channel*area+pos]=value;};
        if(k==0){const int pos=28*d+30;for(int ch=0;ch<4;++ch)set(ch,pos,.5f);set(4,pos,.8f);set(5,pos,.8f);set(14,pos,.7f);}
        if(k==1){const int pos=6*d+5,base=85;
            // The 320x240 image has 40 padded rows above it in the NN input.
            set(base,pos,.5f);set(base+1,pos,.75f);set(base+2,pos,std::sqrt(20.f/(4*62)));set(base+3,pos,std::sqrt(60.f/(4*45)));
            set(base+4,pos,.8f);set(base+14,pos,mode==3?.1f:.7f);
            set(base+5+2,pos,mode==1?.9f:mode==2?.7f:.2f);
        }
        out[k].want_float=!quant;
        if(quant){ints[k].resize(floats[k].size());for(size_t i=0;i<ints[k].size();++i)ints[k][i]=(int8_t)(std::lround(floats[k][i]*255)-128);out[k].buf=ints[k].data();out[k].size=ints[k].size();}
        else{out[k].buf=floats[k].data();out[k].size=floats[k].size()*sizeof(float);}
    }return 0;
}
int rknn_outputs_release(rknn_context,uint32_t,rknn_output*){return 0;}
}
int main(){
    const char *model="build/signal-competition.fixture";{std::ofstream f(model);f<<"fixture";}
    std::vector<uint8_t> rgb(320*240*3,110);
    for(int y=41;y<=103;++y)for(int x=77;x<=99;++x)for(int c=0;c<3;++c)rgb[(y*320+x)*3+c]=20;
    for(int y=47;y<=57;++y)for(int x=83;x<=93;++x)if((x-88)*(x-88)+(y-52)*(y-52)<=16){auto *p=&rgb[(y*320+x)*3];p[0]=240;p[1]=p[2]=10;}
    unsigned tests=0;
    for(bool q:{false,true})for(bool f:{false,true}){
        quant=q;nhwc=f;mode=0;Detector d;std::string error;assert(d.init(model,&error));
        for(int scenario=0;scenario<4;++scenario){mode=scenario;calls=0;std::vector<Box> signals;PersonResult people;
            assert(d.detect(rgb.data(),320,240,960,signals,&error,60,&people));
            const bool expected=scenario==0||scenario==2;
            assert(calls==1&&people.count==1&&people.candidates.size()==1);
            if(signals.empty()==expected){std::printf("SIGNAL FAIL quant=%d nhwc=%d scenario=%d boxes=%zu\n",q,f,scenario,signals.size());std::fflush(stdout);}
            assert(signals.size()==(expected?1u:0u));
            if(expected)assert(classify_color(rgb.data(),320,240,960,signals[0]).state==TRAFFIC_RED);
            ++tests;
        }
    }
    std::remove(model);std::printf("SIGNAL COMPETITION PASS: %u cases; top-class/ties/strong competitor/low score, INT8/float, NHWC/NCHW, person preservation and one NPU call\n",tests);
}
