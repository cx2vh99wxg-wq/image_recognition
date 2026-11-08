// Offline RKNN timing and tensor export only. No SHM, UDP, GPIO or motion.
#include "rknn_api.h"
#include "traffic_offline_guard.h"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <vector>

static void check(int status,const char *what) {
    if(status<0)throw std::runtime_error(std::string(what)+": "+std::to_string(status));
}
static std::vector<char> read(const char *path) {
    std::ifstream f(path,std::ios::binary|std::ios::ate);
    if(!f||f.tellg()<=0)throw std::runtime_error("missing input file");
    std::vector<char> result(static_cast<size_t>(f.tellg()));f.seekg(0);
    if(!f.read(result.data(),result.size()))throw std::runtime_error("short input read");
    return result;
}
static double median(std::vector<double> v) { std::sort(v.begin(),v.end());return v[v.size()/2]; }
static double elapsed(std::chrono::steady_clock::time_point a,std::chrono::steady_clock::time_point b) {
    return std::chrono::duration<double,std::milli>(b-a).count();
}
int main(int argc,char **argv) {
    if(argc!=6||traffic_capture_is_running())return 2;
    rknn_context ctx=0;
    try {
        auto model=read(argv[1]),image=read(argv[2]);
        check(rknn_init(&ctx,model.data(),model.size(),RKNN_FLAG_PRIOR_LOW,nullptr),"init");
        rknn_input_output_num io={};check(rknn_query(ctx,RKNN_QUERY_IN_OUT_NUM,&io,sizeof io),"io");
        rknn_tensor_attr input_attr={};check(rknn_query(ctx,RKNN_QUERY_INPUT_ATTR,&input_attr,sizeof input_attr),"input");
        if(io.n_input!=1||input_attr.n_elems!=image.size()||io.n_output>32)throw std::runtime_error("input/output contract");
        const int repeats=std::stoi(argv[4]);if(repeats<3||repeats>100)throw std::runtime_error("repeat range");
        const bool floats=std::string(argv[5])=="float";
        rknn_input input={};input.buf=image.data();input.size=image.size();input.type=RKNN_TENSOR_UINT8;input.fmt=RKNN_TENSOR_NHWC;
        std::vector<rknn_output> outputs(io.n_output);
        std::ofstream meta(std::string(argv[3])+".json");meta<<std::setprecision(9)<<"{\"outputs\":[";
        for(unsigned i=0;i<io.n_output;++i) {
            rknn_tensor_attr a={};a.index=i;check(rknn_query(ctx,RKNN_QUERY_OUTPUT_ATTR,&a,sizeof a),"output attr");
            if(i)meta<<",";
            meta<<"{\"index\":"<<i<<",\"format\":"<<int(a.fmt)<<",\"type\":"<<int(a.type)
                <<",\"scale\":"<<a.scale<<",\"zero_point\":"<<a.zp<<",\"shape\":[";
            for(unsigned d=0;d<a.n_dims;++d){if(d)meta<<",";meta<<a.dims[d];}meta<<"]}";
            outputs[i].index=i;outputs[i].want_float=floats;
        }
        meta<<"],\"float_outputs\":"<<floats<<"}";meta.close();
        std::vector<double> set,run,get,total;
        for(int n=0;n<repeats+3;++n) {
            auto a=std::chrono::steady_clock::now();check(rknn_inputs_set(ctx,1,&input),"set");
            auto b=std::chrono::steady_clock::now();check(rknn_run(ctx,nullptr),"run");
            auto c=std::chrono::steady_clock::now();check(rknn_outputs_get(ctx,io.n_output,outputs.data(),nullptr),"get");
            auto d=std::chrono::steady_clock::now();
            if(n>=3){set.push_back(elapsed(a,b));run.push_back(elapsed(b,c));get.push_back(elapsed(c,d));total.push_back(elapsed(a,d));}
            if(n==repeats+2)for(const auto &out:outputs) {
                std::ofstream f(std::string(argv[3])+"."+std::to_string(out.index)+(floats?".f32":".raw"),std::ios::binary);
                if(!f.write(static_cast<const char*>(out.buf),out.size))throw std::runtime_error("tensor write");
            }
            check(rknn_outputs_release(ctx,io.n_output,outputs.data()),"release");
        }
        std::printf("DETECTOR_BENCH {\"set_ms\":%.4f,\"run_ms\":%.4f,\"get_ms\":%.4f,\"total_ms\":%.4f,\"repeats\":%d,\"float_outputs\":%d}\n",
                    median(set),median(run),median(get),median(total),repeats,floats);
        rknn_destroy(ctx);return 0;
    }catch(const std::exception &e){std::fprintf(stderr,"%s\n",e.what());if(ctx)rknn_destroy(ctx);return 1;}
}
