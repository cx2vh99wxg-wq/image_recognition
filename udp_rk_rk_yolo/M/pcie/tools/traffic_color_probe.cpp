#include "traffic_vision.h"
#include <fstream>
#include <cstdio>
#include <cstdlib>
int main(int argc,char** argv){
    if(argc!=6)return 2;
    std::ifstream f(argv[1],std::ios::binary);std::string magic;int w=0,h=0,max=0;f>>magic>>w>>h>>max;f.get();
    if(magic!="P6"||w<1||h<1||w>1920||h>1080||max!=255)return 2;
    std::vector<uint8_t> rgb((size_t)w*h*3);f.read((char*)rgb.data(),rgb.size());if((size_t)f.gcount()!=rgb.size())return 2;
    traffic::Box b(atof(argv[2]),atof(argv[3]),atof(argv[4]),atof(argv[5]),1);auto c=traffic::classify_color(rgb.data(),w,h,w*3,b);
    printf("{\"state\":%u,\"quality\":%u,\"reason\":%u,\"pixels\":%u}\n",c.state,c.quality,c.reason,c.pixels);
}
