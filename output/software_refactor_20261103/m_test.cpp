#include "yolop_opencv_integration.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <fstream>

static int fail_stage, calls, output_count;
static const unsigned char *input_data;
static std::vector<float> detection(25200*6), road(640*640*2), lane(640*640*2);
int rknn_init(rknn_context *ctx, void *, uint32_t, uint32_t, rknn_init_extend *) { *ctx=1; return 0; }
int rknn_destroy(rknn_context) { return 0; }
int rknn_query(rknn_context, rknn_query_cmd cmd, void *ptr, uint32_t) {
    if(cmd==RKNN_QUERY_IN_OUT_NUM) *static_cast<rknn_input_output_num *>(ptr)={1,3};
    return 0;
}
int rknn_inputs_set(rknn_context, uint32_t n, rknn_input *input) {
    assert(++calls==1 && n==1 && input->index==0 && input->type==RKNN_TENSOR_UINT8);
    assert(input->fmt==RKNN_TENSOR_NHWC && input->size==640*640*3);
    input_data=static_cast<const unsigned char *>(input->buf);
    assert(input_data[0]==42);
    return fail_stage==1 ? -11 : 0;
}
int rknn_run(rknn_context, rknn_run_extend *) {
    assert(++calls==2 && input_data[0]==42);
    return fail_stage==2 ? -12 : 0;
}
int rknn_outputs_get(rknn_context, uint32_t n, rknn_output *out, rknn_output_extend *) {
    assert(++calls==3 && n==unsigned(output_count));
    if(fail_stage==3) return -13;
    std::vector<float> *data[]={&detection,&road,&lane};
    for(unsigned i=0;i<n;++i) {
        assert(out[i].want_float && out[i].index==0 && !out[i].buf);
        out[i].buf=data[i]->data();
        out[i].size=data[i]->size()*sizeof(float);
    }
    return 0;
}
int rknn_outputs_release(rknn_context, uint32_t n, rknn_output *) {
    assert(++calls==4 && n==unsigned(output_count));
    return 0;
}
int main(int argc,char **argv) {
    assert(argc==2);
    std::ofstream("fixture-model")<<"model";
    std::ofstream("fixture-labels")<<"car\n";
    YOLOPEngine engine("fixture-model","fixture-labels");
    assert(engine.isValid());
    auto fp=fopen(argv[1],"wb"); assert(fp);
    for (int i=0;i<12;++i) detection[i]=0.95f;
    for(int i=0;i<640*640;++i) {
        road[640*640+i]=i%3 ? 1.0f : NAN;
        lane[640*640+i]=i%5 ? 0.0f : 1.0f;
    }
    for (int width : {1,17,321,640})
      for (int height : {1,29,239,640})
       for (int mode : {1,3})
        for (fail_stage=0;fail_stage<=3;++fail_stage) {
          std::vector<uint8_t> rgb(width*height*3);
          for(size_t j=0;j<rgb.size();++j) rgb[j]=(42+j*17)%256;
          detect_result_group_t results;
          memset(&results,0x55,sizeof(results));
          output_count=mode; calls=0;
          int ret=mode==1 ? engine.detectOnly(rgb.data(),width,height,&results)
                          : engine.detect(rgb.data(),width,height,&results);
          assert(ret==(fail_stage ? -1 : 0) && calls==(fail_stage ? fail_stage : 4));
          fwrite(&results,1,sizeof(results),fp);
          if (fail_stage) results.count=0; engine.drawResults(rgb.data(),width,height,&results);
          fwrite(rgb.data(),1,rgb.size(),fp);
        }
    fclose(fp);
}

