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
    if(argc!=5)return 2;
    const bool cpu_only=std::string(argv[1])=="--tensor-replay"||std::string(argv[1])=="--input-only";
    if(!cpu_only && traffic_capture_is_running())return 2;
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
        const bool tensor_replay=std::string(argv[1])=="--tensor-replay";
        if(!tensor_replay && yolo_engine_init(&engine,argv[1],nullptr))throw std::runtime_error("model initialization");
        if(!tensor_replay && (engine.model_width!=320||engine.model_height!=240||engine.io_num.n_output!=2))throw std::runtime_error("candidate does not match production two-head contract");
        yolopv2_result_t output={};const int repeats=std::max(1,std::min(100,std::atoi(argv[4])));
        const bool twin_softmax=std::getenv("LANE_CANDIDATE_TWIN_SOFTMAX")!=nullptr;
        std::vector<float> external_lane(320*240),external_road(2*320*240);
        if(tensor_replay){
            for(auto item:{std::make_pair("lane",&external_lane),std::make_pair("road",&external_road)}){
                std::ifstream input(prefix+".tensor."+item.first+".f32",std::ios::binary);
                const size_t bytes=item.second->size()*sizeof(float);input.read((char*)item.second->data(),bytes);
                if((size_t)input.gcount()!=bytes || input.peek()!=EOF)throw std::runtime_error("invalid external tensor size");
                for(float value:*item.second)if(!std::isfinite(value))throw std::runtime_error("non-finite external tensor");
            }
            output={external_lane.data(),external_road.data(),320,240};
        }
        std::vector<double> times;double run=0;
        for(int n=0;n<(tensor_replay?1:repeats+3);++n){
            const auto start=std::chrono::steady_clock::now();
            if(!tensor_replay && yolo_engine_detect(&engine,input.data(),320,240,&output))throw std::runtime_error("model inference");
            // Explicit offline adapter: the author's TwinLiteNet+ uses argmax
            // over two logits. Never silently apply YOLOPv2's 0.3 gate to it.
            if(twin_softmax)for(size_t p=0;p<320*240;++p){
                const float score=output.lane_line_data[p];
                if(!std::isfinite(score)||score<0.f||score>1.f)throw std::runtime_error("invalid Twin softmax output");
                output.lane_line_data[p]=score>.5f?1.f:0.f;
            }
            if(tensor_replay)times.push_back(0.); // External inference is timed by its caller, never reported as NPU time.
            else if(n>=3){times.push_back(std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count());run+=engine.timing.run_ns*1e-6;}
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
        std::printf("CANDIDATE {\"median_ms\":%.4f,\"p95_ms\":%.4f,\"run_ms\":%.4f,\"curve\":%u,\"confidence\":%u,\"angle\":%u,\"left\":%u,\"right\":%u,\"left_reason\":%u,\"right_reason\":%u,\"sampled\":%d,\"left_points\":%zu,\"right_points\":%zu,\"twin_softmax_adapter\":%d,\"tensor_replay\":%d}\n",
            times[times.size()/2],times[std::min(times.size()-1,size_t(times.size()*.95))],run/repeats,curve.type,curve.confidence,curve.angle,result.left.type,result.right.type,result.left.reason,result.right.reason,points.sampled,points.left.size(),points.right.size(),twin_softmax,tensor_replay);
        yolo_engine_cleanup(&engine);return 0;
    }catch(const std::exception &e){std::fprintf(stderr,"%s\n",e.what());yolo_engine_cleanup(&engine);return 1;}
}
