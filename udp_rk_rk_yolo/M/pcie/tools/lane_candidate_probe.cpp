// Offline comparison uses the production input correction, mask/IPM mapping,
// point selector, paint evidence and heading geometry. Never publishes SHM or controls.
#include "yolo_integration.h"
#include "shared_memory.h"
#include "traffic_offline_guard.h"
#include "front_radial_preview.h"
#include "lane_marking_profile.h"
#include "ego_lane_selector.h"
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <chrono>

extern "C" int curve_detection_update(uint32_t,CurveType,uint32_t,uint32_t){return -1;}
static void save(const std::string &path,const void *bytes,size_t count){
    std::ofstream out(path,std::ios::binary);out.write((const char*)bytes,count);
    if(!out)throw std::runtime_error("cannot save "+path);
}
int main(int argc,char **argv){
    if(argc!=5 || traffic_capture_is_running())return 2;
    YOLOEngine engine={};
    try{
        const std::string folder=argv[2],prefix=argv[3];
        std::ifstream file(folder+"/m_lane_snapshot.ppm",std::ios::binary);
        std::string magic;int w=0,h=0,max=0;file>>magic>>w>>h>>max;file.get();
        if(magic!="P6"||w!=640||h!=480||max!=255)throw std::runtime_error("need saved raw 640x480 BGR mosaic");
        std::vector<uint8_t> raw(w*h*3),input(320*240*3),rgb(input.size()),bird(input.size()),green(320*240),coverage(320*240);
        file.read((char*)raw.data(),raw.size());if((size_t)file.gcount()!=raw.size())return 2;
        const auto lens=m_lane::FrontRadial::for_camera(nullptr,(folder+"/front_radial.conf").c_str());
        const auto ground=m_lane::GroundProjection::for_camera(nullptr,(folder+"/front_ipm_test.conf").c_str());
        m_lane::FrontRadialPreview front(640,480,lens,nullptr,true),ipm(640,480,lens,&ground);
        if(!front.enabled()||!ipm.enabled())throw std::runtime_error("missing production calibration");
        front.render(raw.data(),input.data());ipm.render(raw.data(),bird.data());ipm.coverage_mask(coverage.data());
        auto pull=ipm.compose_masks_from(*front.mask_restore());
        // Snapshot PPM payload is production BGR. Calibration PPM must actually be RGB.
        for(size_t p=0;p<input.size();p+=3){rgb[p]=input[p+2];rgb[p+1]=input[p+1];rgb[p+2]=input[p];}
        {std::ofstream f(prefix+".input.ppm",std::ios::binary);f<<"P6\n320 240\n255\n";f.write((char*)rgb.data(),rgb.size());}
        if(std::string(argv[1])=="--input-only")return 0;
        if(yolo_engine_init(&engine,argv[1],nullptr))throw std::runtime_error("model initialization");
        if(engine.model_width!=320||engine.model_height!=240||engine.io_num.n_output!=2)throw std::runtime_error("candidate does not match production two-head contract");
        yolopv2_result_t output={};const int repeats=std::max(1,std::min(100,std::atoi(argv[4])));
        std::vector<double> times;double run=0;
        for(int n=0;n<repeats+3;++n){
            const auto start=std::chrono::steady_clock::now();
            if(yolo_engine_detect(&engine,input.data(),320,240,&output))throw std::runtime_error("model inference");
            if(n>=3){times.push_back(std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count());run+=engine.timing.run_ns*1e-6;}
        }
        const size_t area=320*240;
        save(prefix+".lane.f32",output.lane_line_data,area*sizeof(float));save(prefix+".road.f32",output.drivable_area_data,2*area*sizeof(float));
        std::vector<float> lane(area);
        if(!pull.remap(output.lane_line_data,output.drivable_area_data,320,240,lane.data(),green.data()))throw std::runtime_error("IPM mask mapping");
        const auto points=m_lane::select_point_lanes(lane.data(),320,240);
        const auto style=m_lane::marking_profile(lane.data(),points);
        const auto result=m_lane::EgoLaneSelector::describe_points(style,bird.data(),true,coverage.data());
        const auto curve=m_lane::estimate_ego_curve(points,true);
        save(prefix+".bird_lane.f32",lane.data(),area*sizeof(float));save(prefix+".bird_road.u8",green.data(),area);
        std::sort(times.begin(),times.end());
        std::printf("CANDIDATE {\"median_ms\":%.4f,\"p95_ms\":%.4f,\"run_ms\":%.4f,\"curve\":%u,\"confidence\":%u,\"angle\":%u,\"left\":%u,\"right\":%u,\"left_reason\":%u,\"right_reason\":%u,\"sampled\":%d,\"left_points\":%zu,\"right_points\":%zu}\n",
            times[times.size()/2],times[std::min(times.size()-1,size_t(times.size()*.95))],run/repeats,curve.type,curve.confidence,curve.angle,result.left.type,result.right.type,result.left.reason,result.right.reason,points.sampled,points.left.size(),points.right.size());
        yolo_engine_cleanup(&engine);return 0;
    }catch(const std::exception &e){std::fprintf(stderr,"%s\n",e.what());yolo_engine_cleanup(&engine);return 1;}
}
