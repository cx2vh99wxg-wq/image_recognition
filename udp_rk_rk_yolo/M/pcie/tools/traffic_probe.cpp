#include "traffic_detector.h"
#include "traffic_offline_guard.h"
#include <fstream>
#include <cstdio>
#include <chrono>
#include <cstdlib>
// Saved RGB only. No shared memory, UDP, PCIe, FSPI or control publication.
int main(int argc,char** argv){
    if(traffic_capture_is_running()){fprintf(stderr,"Stop the capture/NPU app before this offline test.\n");return 2;}
    if(argc<3||argc>4){fprintf(stderr,"Usage: traffic_probe model.rknn raw_rgb.ppm [repeats]\n");return 2;}
    std::ifstream f(argv[2],std::ios::binary);std::string magic;int w=0,h=0,max=0;f>>magic>>w>>h>>max;f.get();
    if(magic!="P6"||w<4||h<5||w>1920||h>1080||max!=255)return 2;
    std::vector<uint8_t> rgb((size_t)w*h*3);f.read((char*)rgb.data(),rgb.size());if((size_t)f.gcount()!=rgb.size())return 2;
    traffic::Detector detector;std::string error;if(!detector.init(argv[1],&error)){fprintf(stderr,"%s\n",error.c_str());return 1;}
    int repeats=argc==4?std::max(1,std::min(50,atoi(argv[3]))):1;double elapsed=0;std::vector<traffic::Box> boxes;
    for(int n=0;n<repeats;++n){auto begin=std::chrono::steady_clock::now();if(!detector.detect(rgb.data(),w,h,w*3,boxes,&error)){fprintf(stderr,"%s\n",error.c_str());return 1;}elapsed+=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-begin).count();}
    printf("{\"input_size\":%d,\"width\":%d,\"height\":%d,\"repeats\":%d,\"npu_passes\":%u,\"mean_ms\":%.3f,\"boxes\":[",detector.input_size(),w,h,repeats,detector.last_passes(),elapsed/repeats);
    bool first=true;for(const auto& b:boxes){auto c=traffic::classify_color(rgb.data(),w,h,w*3,b);if(!first)printf(",");first=false;printf("{\"xywh\":[%.3f,%.3f,%.3f,%.3f],\"score\":%.6f,\"color\":\"%s\",\"quality\":%u,\"reason\":%u}",b.x,b.y,b.w,b.h,b.score,traffic_state_name(c.state),c.quality,c.reason);}
    puts("]}");return 0;
}
