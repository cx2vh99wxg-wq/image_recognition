#include "ego_lane_selector.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <chrono>
#include <cstdlib>
static bool load(const char* path,std::vector<uint8_t>& p,int& w,int& h,const char* magic,int channels) {
    std::ifstream f(path,std::ios::binary);std::string kind;int maximum;
    if(!(f>>kind>>w>>h>>maximum)||kind!=magic||w<=0||h<=0||w>4096||h>4096||maximum!=255)return false;
    f.get();p.resize((size_t)w*h*channels);f.read((char*)p.data(),p.size());return (size_t)f.gcount()==p.size();
}
/* Analyze exported raw image/mask pairs without cameras, RKNN or FSPI.
 * Prints single-image evidence; does not fake three fresh video results. */
int main(int argc,char** argv) {
    if(argc!=6){fprintf(stderr,"Usage: line_style_replay original.ppm lane_mask.pgm [evidence.csv [benchmark_repeats]]\n");return 2;}
    std::vector<uint8_t> image,mask;int iw,ih,mw,mh;
    if(!load(argv[1],image,iw,ih,"P6",3)||!load(argv[2],mask,mw,mh,"P5",1)){fprintf(stderr,"Need P6 RGB image and P5 binary lane mask, max=255\n");return 1;}
    std::vector<float> lane(mask.size());for(size_t i=0;i<mask.size();++i)lane[i]=mask[i]/255.f;
    
    std::vector<uint8_t> green;int gw,gh;
    if(!load(argv[5],green,gw,gh,"P5",1)||gw!=mw||gh!=mh)return 3;
    m_lane::EgoLaneSelector analyzer;

    m_lane::LineStyleTrace trace;
    auto result=analyzer.analyze(lane.data(),green.data(),image.data(),mw,mh,&trace);
    printf("EGO reference=%.2f near_y=%.2f road=%u objects=%zu selected=%d/%d\n",trace.reference_x,trace.near_y,trace.road_area,trace.objects.size(),trace.selected_left,trace.selected_right);
    for(size_t n=0;n<trace.objects.size();++n){const auto& t=trace.objects[n];printf("OBJECT %zu fit=%.3f,%.3f,%.3f y=%d..%d NN=%u\n",n,t.a,t.b,t.c,t.first,t.last,t.nn_points);}
    for(auto p:{std::make_pair("LEFT",result.left),std::make_pair("RIGHT",result.right)})
        printf("%s %s quality=%u reason=%s duty=%u spans=%u gaps=%u coverage=%u bottom_x=%.1f fit=%.3f,%.3f,%.3f y=%u..%u\n",p.first,line_style_name(p.second.type),p.second.quality,line_reason_name(p.second.reason),p.second.duty,p.second.spans,p.second.gaps,p.second.coverage,p.second.x_bottom,p.second.fit_a,p.second.fit_b,p.second.fit_c,p.second.first_y,p.second.last_y);
    if(argc>=4){FILE* f=fopen(argv[3],"w");if(!f)return 1;fprintf(f,"side,y,x,paint,filtered\n");
        for(auto pair:{std::make_pair("LEFT",&trace.left),std::make_pair("RIGHT",&trace.right)})for(const auto& p:*pair.second)
            fprintf(f,"%s,%d,%d,%d,%d\n",pair.first,p.y,p.x,p.raw,p.filtered);
        fclose(f);
    }
    if(argc==5) {
        int repeats=std::max(1,std::min(1000,atoi(argv[4])));uint32_t guard=0;
        auto start=std::chrono::steady_clock::now();
        for(int n=0;n<repeats;++n){lane[0]=(n&1);auto r=analyzer.analyze(lane.data(),green.data(),image.data(),mw,mh);guard+=r.left.type+r.right.type;}
        double ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count()/repeats;
        printf("PERF postprocess_ms=%.3f repeats=%d check=%u\n",ms,repeats,guard);
    }
}
