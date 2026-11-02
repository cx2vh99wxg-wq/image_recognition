/* Read-only, exclusive-NPU regression on saved raw camera pixels. No SHM/SPI. */
#include "yolo_integration.h"
#include "shared_memory.h"
#include "line_style_analyzer.h"
#include "lane_curve_geometry.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <ctime>
/* Resolve the unused legacy overlay publisher without allowing this read-only
 * executable to initialize or write any shared-memory segment. */
extern "C" int curve_detection_update(uint32_t,CurveType,uint32_t,uint32_t){return -1;}
static uint64_t clock_ns(){timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return (uint64_t)t.tv_sec*1000000000ULL+t.tv_nsec;}
static std::vector<uint8_t> read_frame(const char* name,int& w,int& h){
 FILE* f=fopen(name,"rb");if(!f)return {};char tag[3];int max;
 if(fscanf(f,"%2s %d %d %d",tag,&w,&h,&max)!=4||tag[0]!='P'||tag[1]!='6'||w!=640||h!=480||max!=255){fclose(f);return {};}
 fgetc(f);std::vector<uint8_t> p((size_t)w*h*3);if(fread(p.data(),1,p.size(),f)!=p.size())p.clear();fclose(f);return p;
}
int main(int argc,char** argv){
 if(argc!=5||std::system("pgrep -x pcie_capture >/dev/null")!=256)return 2;
 YOLOEngine engine={};if(yolo_engine_init(&engine,argv[1],nullptr))return 2;
 unsigned expected[]={1,2,0};bool all=true;uint64_t source=0;
 for(int k=0;k<3;++k){
  int w=0,h=0;auto image=read_frame(argv[k+2],w,h);if(image.empty()){all=false;break;}
  m_lane::LineStyleAnalyzer lines;m_lane::EgoCurveConfirm confirm;m_lane::EgoCurve curve;double expansion=0;
  for(int n=0;n<3;++n){
   uint64_t captured=clock_ns();yolopv2_result_t masks={};
   if(yolo_engine_detect(&engine,image.data(),w,h,&masks)){all=false;break;}
   auto lane=lines.analyze_front_camera(masks.lane_line_data,masks.width,masks.height,image.data(),w,h,nullptr,masks.drivable_area_data);
   auto raw=m_lane::estimate_ego_curve(lane);curve=confirm.update(raw,++source,captured);
   double first=std::max(lane.left.first_y,lane.right.first_y),last=std::min(lane.left.last_y,lane.right.last_y);
   expansion=(m_lane::curve_x(lane.right,last,lane.height)-m_lane::curve_x(lane.left,last,lane.height))-
             (m_lane::curve_x(lane.right,first,lane.height)-m_lane::curve_x(lane.left,first,lane.height));
  }
  bool good=curve.confidence>=60&&curve.type==expected[k];all=all&&good;
  printf("QA_CASE {\"case\":%d,\"expected\":%u,\"type\":%u,\"confidence\":%u,\"shift\":%.5f,\"span\":%.2f,\"width_growth\":%.3f,\"passed\":%s}\n",
   k,expected[k],curve.type,curve.confidence,curve.shift_px,curve.span_px,expansion,good?"true":"false");
 }
 yolo_engine_cleanup(&engine);printf("SAVED_INPUT_QA_COMPLETE passed=%d no_SHM_or_actuator=1\n",all?1:0);return all?0:1;
}
