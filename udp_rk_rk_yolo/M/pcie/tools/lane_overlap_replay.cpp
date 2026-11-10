// Saved raw camera frame, production models and private IPC. No FSPI/GPIO.
#include "latest_lane_worker.h"
#include "shared_memory.h"
#include "traffic_service.h"
#include "traffic_frame_input.h"
#include "traffic_offline_guard.h"
#include "front_person_ipc.h"
#include <fstream>
#include <cassert>
#include <sched.h>
extern "C" int curve_detection_update(uint32_t,CurveType,uint32_t,uint32_t){return -1;}
static uint64_t hash(const void*p,size_t n){uint64_t h=1469598103934665603ULL;auto*b=(const uint8_t*)p;for(size_t i=0;i<n;++i)h=(h^b[i])*1099511628211ULL;return h;}
struct Output {
    std::shared_ptr<const m_lane::Snapshot> snapshot;
    FrontPersonData person={};
};
static Output once(const char*model,const char*detector,const std::vector<uint8_t>&pixels,bool overlap){
    assert(!setenv("VISION_LANE_NPU_OVERLAP",overlap?"1":"0",1));
    const auto lens=m_lane::FrontRadial::for_camera(nullptr,"front_radial.conf");
    const auto ground=m_lane::GroundProjection::for_camera(nullptr,"front_ipm_test.conf");
    m_lane::FrontRadialPreview front(640,480,lens,nullptr,true),bird(640,480,lens,&ground);
    assert(front.enabled()&&bird.enabled());
    YOLOEngine engine={};assert(!yolo_engine_init(&engine,model,nullptr));
    std::vector<uint8_t> corrected(320*240*3);front.render(pixels.data(),corrected.data());
    yolopv2_result_t warm={};assert(!yolo_engine_detect(&engine,corrected.data(),320,240,&warm));
    Output out;
    {
        traffic::Service service(detector,traffic::InputView(0,0,traffic::PixelOrder::BGR),&front);
        m_lane::LatestWorker worker(&engine,640,480,&service,&bird,&front);
        FrontPersonIPC ipc={-1,nullptr};const uint64_t captured=m_lane::clock_ns();
        worker.submit(pixels.data(),71,captured);
        for(unsigned n=0;n<2000;++n){
            auto s=worker.latest();FrontPersonData person={};
            if(!front_person_ipc_open(&ipc,0)&&!front_person_ipc_read(&ipc,&person)&&person.source_id==71&&s&&s->source_id==71){out.snapshot=s;out.person=person;break;}
            assert(!worker.failed());std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        worker.stop();front_person_ipc_close(&ipc);assert(out.snapshot&&out.snapshot->valid);
    }
    yolo_engine_cleanup(&engine);return out;
}
static bool boundary(const YOLOLineBoundary&a,const YOLOLineBoundary&b){
    return a.type==b.type&&a.quality==b.quality&&a.duty==b.duty&&a.spans==b.spans&&a.gaps==b.gaps&&a.coverage==b.coverage&&a.reason==b.reason&&a.raw_type==b.raw_type&&a.first_y==b.first_y&&a.last_y==b.last_y&&a.x_bottom==b.x_bottom&&a.x_top==b.x_top&&a.fit_a==b.fit_a&&a.fit_b==b.fit_b&&a.fit_c==b.fit_c;
}
int main(int argc,char**argv){
    if(argc!=4||traffic_capture_is_running())return 2;
    if(unshare(CLONE_NEWIPC)){perror("private IPC namespace");return 2;}
    assert(!setenv("LANE_CURVE_MODE","ego-boundaries",1));assert(!setenv("LANE_CANDIDATE_MODE","bottom-up",1));unsetenv("LANE_GROUND_RECT");unsetenv("LANE_STYLE_DUMP_DIR");
    std::ifstream f(argv[1],std::ios::binary);std::string magic;int w,h,max;f>>magic>>w>>h>>max;f.get();assert(magic=="P6"&&w==640&&h==480&&max==255);
    std::vector<uint8_t> raw(w*h*3);f.read((char*)raw.data(),raw.size());assert((size_t)f.gcount()==raw.size());
    auto a=once(argv[2],argv[3],raw,false),b=once(argv[2],argv[3],raw,true);const auto&x=*a.snapshot;const auto&y=*b.snapshot;
    assert(x.lane==y.lane&&x.drivable==y.drivable&&x.width==y.width&&x.height==y.height);
    assert(x.curve.curve_type==y.curve.curve_type&&x.curve.confidence==y.curve.confidence&&x.curve.angle==y.curve.angle);
    assert(boundary(x.line_style.left,y.line_style.left)&&boundary(x.line_style.right,y.line_style.right));
    assert(x.ipm_points.left_x==y.ipm_points.left_x&&x.ipm_points.right_x==y.ipm_points.right_x&&x.ipm_points.sampled==y.ipm_points.sampled);
    const auto&u=a.person;const auto&v=b.person;
    assert(u.source_id==v.source_id&&u.flags==v.flags&&u.count==v.count&&u.quality==v.quality&&u.x==v.x&&u.y==v.y&&u.width==v.width&&u.height==v.height&&u.risk_count==v.risk_count&&u.outside_count==v.outside_count&&u.unknown_count==v.unknown_count&&u.road_pixels==v.road_pixels&&u.road_source_id==v.road_source_id);
    const auto m=x.masks(),n=y.masks();const size_t area=(size_t)m.width*m.height;
    assert(m.width==n.width&&m.height==n.height&&!memcmp(m.lane_line_data,n.lane_line_data,area*sizeof(float))&&!memcmp(m.drivable_area_data,n.drivable_area_data,2*area*sizeof(float)));
    printf("OVERLAP_REAL_PASS {\"type\":%u,\"angle\":%u,\"confidence\":%u,\"left\":%u,\"right\":%u,\"person_flags\":%u,\"risk\":%u,\"outside\":%u,\"unknown\":%u,\"lane_hash\":\"%016llx\",\"road_hash\":\"%016llx\",\"restored_after_engine_destroy\":true}\n",x.curve.curve_type,x.curve.angle,x.curve.confidence,x.line_style.left.type,x.line_style.right.type,u.flags,u.risk_count,u.outside_count,u.unknown_count,(unsigned long long)hash(x.lane.data(),x.lane.size()*4),(unsigned long long)hash(x.drivable.data(),x.drivable.size()*4));
}
