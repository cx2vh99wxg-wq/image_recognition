#include "yolo_integration.h"
#include "traffic_offline_guard.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <vector>
int main(int argc,char **argv) {
    if(argc!=4 || traffic_capture_is_running()) return 2;
    std::ifstream in(argv[2],std::ios::binary);std::string magic;int w=0,h=0,max=0;
    in>>magic>>w>>h>>max;in.get();
    if(magic!="P6"||w!=320||h!=240||max!=255)return 2;
    std::vector<uint8_t> image(w*h*3);in.read((char*)image.data(),image.size());
    if((size_t)in.gcount()!=image.size())return 2;
    // Saved PPM is RGB; the production lane engine receives BGR camera bytes.
    for(size_t i=0;i<image.size();i+=3)std::swap(image[i],image[i+2]);
    YOLOEngine e={};if(yolo_engine_init(&e,argv[1],nullptr))return 3;
    rknn_sdk_version sdk={};rknn_query(e.ctx,RKNN_QUERY_SDK_VERSION,&sdk,sizeof(sdk));
    std::printf("SDK api=%s driver=%s outputs=%u\n",sdk.api_version,sdk.drv_version,e.io_num.n_output);
    for(unsigned i=0;i<e.io_num.n_output;++i){const auto &a=e.output_attrs[i];
        std::printf("ATTR index=%u name=%s type=%d format=%d size=%u elements=%u shape=",i,a.name,a.type,a.fmt,a.size,a.n_elems);
        for(unsigned j=0;j<a.n_dims;++j)std::printf("%u,",a.dims[j]);
        std::puts("");}
    const int repeats=std::max(5,std::min(100,std::atoi(argv[3])));YOLOTiming sum={};
    const char *reference=std::getenv("LANE_PROBE_REFERENCE");
    std::vector<float> expected((size_t)w*h*3);
    bool have_reference=false;
    if(reference){std::ifstream f(reference,std::ios::binary);f.read((char*)expected.data(),expected.size()*sizeof(float));have_reference=(size_t)f.gcount()==expected.size()*sizeof(float);}
    for(int n=0;n<repeats+3;++n){yolopv2_result_t masks={};
        if(yolo_engine_detect(&e,image.data(),w,h,&masks)){yolo_engine_cleanup(&e);return 4;}
        if(n>=3){
            if(reference) {
                const size_t bytes=(size_t)w*h*sizeof(float);
                if(!have_reference && !e.output_mode) {
                    std::memcpy(expected.data(),masks.lane_line_data,bytes);
                    std::memcpy(expected.data()+w*h,masks.drivable_area_data,2*bytes);
                    std::ofstream f(reference,std::ios::binary);f.write((char*)expected.data(),3*bytes);have_reference=true;
                }
                if(!have_reference || std::memcmp(expected.data(),masks.lane_line_data,bytes) ||
                   std::memcmp(expected.data()+w*h,masks.drivable_area_data,2*bytes)) {
                    std::fprintf(stderr,"OUTPUT_EQUIVALENCE_FAIL mode=%u iteration=%d\n",e.output_mode,n);
                    yolo_engine_cleanup(&e);return 5;
                }
            }
            sum.prepare_ns+=e.timing.prepare_ns;sum.inputs_ns+=e.timing.inputs_ns;
            sum.run_ns+=e.timing.run_ns;sum.outputs_ns+=e.timing.outputs_ns;
            sum.copy_ns+=e.timing.copy_ns;sum.release_ns+=e.timing.release_ns;}}
    const double scale=1e-6/repeats;
    std::printf("LANE_PROFILE {\"mode\":%u,\"repeats\":%d,\"prepare_ms\":%.4f,\"inputs_ms\":%.4f,\"run_ms\":%.4f,\"outputs_ms\":%.4f,\"copy_ms\":%.4f,\"release_ms\":%.4f,\"output_count\":%u,\"bitwise_equal\":%d}\n",
        e.output_mode,repeats,sum.prepare_ns*scale,sum.inputs_ns*scale,sum.run_ns*scale,sum.outputs_ns*scale,sum.copy_ns*scale,sum.release_ns*scale,e.requested_output_count,reference&&have_reference);
    yolo_engine_cleanup(&e);return 0;
}
