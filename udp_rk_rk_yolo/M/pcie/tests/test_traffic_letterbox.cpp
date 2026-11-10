#include "traffic_letterbox.h"
#include <cassert>
#include <cstdio>
#include <chrono>
int main(){
    for(int w:{1,159,160,319,320,640})for(int h:{1,120,160,239,240,320,480})
        for(int pad:{0,17})for(float view:{1.f,.5f,.25f}){
            int stride=w*3+pad;std::vector<uint8_t> raw((size_t)stride*h);
            for(size_t i=0;i<raw.size();++i)raw[i]=(uint8_t)((i*73+i/256)%256);
            auto before=raw;std::vector<uint8_t>a,b;
            auto old=traffic::prepare_letterbox(raw.data(),w,h,stride,320,320,view,a,false);
            auto fast=traffic::prepare_letterbox(raw.data(),w,h,stride,320,320,view,b);
            assert(a==b&&raw==before&&old.scale==fast.scale&&old.x==fast.x&&old.y==fast.y);
        }
    std::vector<uint8_t> raw(320*240*3,127),out;volatile unsigned checksum=0;
    for(bool fast:{false,true}){
        auto start=std::chrono::steady_clock::now();
        for(int i=0;i<100;++i){traffic::prepare_letterbox(raw.data(),320,240,960,320,320,1.f,out,fast);checksum+=out[i];}
        printf("LETTERBOX fast=%d mean_ms=%.4f\n",fast,std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count()/100);
    }
    raw.resize(160*160*3);
    for(bool fast:{false,true}){
        auto start=std::chrono::steady_clock::now();
        for(int i=0;i<100;++i){traffic::prepare_letterbox(raw.data(),160,160,480,320,320,1.f,out,fast);checksum+=out[i];}
        printf("LETTERBOX 2x fast=%d mean_ms=%.4f\n",fast,std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count()/100);
    }
    printf("LETTERBOX PASS: byte-identical generic/unit/exact-2x paths, padded stride, down/up-sampling, three views, raw immutable (%u)\n",checksum);
}
