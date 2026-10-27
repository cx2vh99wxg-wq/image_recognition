#include "yolo_integration.h"
#include "line_style_analyzer.h"
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>
// Offline only: does not publish shared memory, send UDP or open SPI/PCIe.
int main(int argc,char** argv) {
    if(argc!=4){fprintf(stderr,"Usage: line_style_model_probe model.rknn raw_640x480.ppm output_prefix\n");return 2;}
    std::ifstream f(argv[2],std::ios::binary);std::string magic;int w=0,h=0,max=0;
    if(!(f>>magic>>w>>h>>max)||magic!="P6"||w!=640||h!=480||max!=255)return 2;
    f.get();std::vector<uint8_t> rgb((size_t)w*h*3);f.read((char*)rgb.data(),rgb.size());if((size_t)f.gcount()!=rgb.size())return 2;
    YOLOEngine engine={};if(yolo_engine_init(&engine,argv[1],nullptr))return 1;
    yolopv2_result_t masks={};if(yolo_engine_detect(&engine,rgb.data(),w,h,&masks)){yolo_engine_cleanup(&engine);return 1;}
    std::string prefix=argv[3];
    FILE* fp=fopen((prefix+"_mask.pgm").c_str(),"wb");if(!fp)return 1;
    fprintf(fp,"P5\n%d %d\n255\n",masks.width,masks.height);
    for(int i=0;i<masks.width*masks.height;++i)fputc(masks.lane_line_data[i]>.3f?255:0,fp);
    fclose(fp);
    fp=fopen((prefix+"_lane.f32").c_str(),"wb");if(fp){fwrite(masks.lane_line_data,sizeof(float),masks.width*masks.height,fp);fclose(fp);}
    fp=fopen((prefix+"_drivable.f32").c_str(),"wb");if(fp){fwrite(masks.drivable_area_data,sizeof(float),2*masks.width*masks.height,fp);fclose(fp);}
    fp=fopen((prefix+"_green.pgm").c_str(),"wb");if(fp){fprintf(fp,"P5\n%d %d\n255\n",masks.width,masks.height);for(int i=0;i<masks.width*masks.height;++i)fputc(masks.drivable_area_data[masks.width*masks.height+i]>masks.drivable_area_data[i]?255:0,fp);fclose(fp);}
    m_lane::LineStyleAnalyzer analyzer;auto result=analyzer.analyze_front_camera(masks.lane_line_data,masks.width,masks.height,rgb.data(),w,h,nullptr,masks.drivable_area_data);
    for(auto p:{std::make_pair("LEFT",result.left),std::make_pair("RIGHT",result.right)})
        printf("PROBE %s %s reason=%s duty=%u spans=%u gaps=%u coverage=%u bottom=%.1f\n",p.first,line_style_name(p.second.type),line_reason_name(p.second.reason),p.second.duty,p.second.spans,p.second.gaps,p.second.coverage,p.second.x_bottom);
    yolo_engine_cleanup(&engine);return 0;
}
