#include "traffic_service.h"
#include "traffic_light_ipc.h"
#include "traffic_offline_guard.h"
#include <chrono>
#include <cstdio>
#include <fstream>
#include <thread>
#include <sched.h>
// Explicit saved-sample validation only. No PCIe, FSPI or vehicle commands.
// Traffic metadata is now consumed by the S GO gate. Default to a private IPC
// namespace; live publication is reserved for a controlled stationary test.
static std::vector<uint8_t> load(const char* path,int expected_w,int expected_h){
    std::ifstream f(path,std::ios::binary);std::string magic;int w=0,h=0,max=0;f>>magic>>w>>h>>max;f.get();
    if(magic!="P6"||w!=expected_w||h!=expected_h||max!=255)return {};
    std::vector<uint8_t> rgb((size_t)w*h*3);f.read((char*)rgb.data(),rgb.size());if((size_t)f.gcount()!=rgb.size())return {};return rgb;
}
int main(int argc,char** argv){
    if(traffic_capture_is_running()){fprintf(stderr,"Stop the capture/NPU app before this saved-only test.\n");return 2;}
    const char* live=getenv("TRAFFIC_REPLAY_LIVE_CONTROL_TEST");
    if(!live||strcmp(live,"1")) {
        if(unshare(CLONE_NEWIPC)){perror("Private replay IPC namespace (run as root)");return 2;}
        puts("Saved replay: private IPC; no live traffic-control metadata published.");
    } else puts("Stationary integration test: publishing live traffic-control metadata.");
    bool middle=argc==6&&!std::strcmp(argv[5],"--middle-bgr");
    if(argc!=4&&!middle){fprintf(stderr,"Saved-only: traffic_replay model.rknn red.ppm green.ppm [yellow.ppm --middle-bgr]\n");return 2;}
    int width=middle?640:320,height=middle?480:240;
    auto red=load(argv[2],width,height),green=load(argv[3],width,height);
    auto yellow=middle?load(argv[4],width,height):std::vector<uint8_t>();
    if(red.empty()||green.empty()||(middle&&yellow.empty()))return 2;
    std::vector<uint8_t> gray((size_t)width*height*3,110);
    traffic::Service service(argv[1],middle?traffic::InputView(320,240,traffic::PixelOrder::BGR):traffic::InputView());
    TrafficLightIPC ipc={-1,nullptr};uint64_t id=0;
    std::vector<const std::vector<uint8_t>*> frames={&red,&green};std::vector<unsigned> expected={1,3};
    if(middle){frames.push_back(&yellow);expected.push_back(2);}frames.push_back(&gray);expected.push_back(0);bool pass=true;
    for(size_t phase=0;phase<frames.size();++phase){
        printf("SAVED_REPLAY_PHASE %s\n",traffic_state_name(expected[phase]));fflush(stdout);unsigned confirmed=0;
        auto until=std::chrono::steady_clock::now()+std::chrono::seconds(4);
        while(std::chrono::steady_clock::now()<until){
            uint64_t now=traffic_clock_ns();service.submit_raw(frames[phase]->data(),width,height,++id,now);
            traffic::LaneBudget lane;lane.source=id;lane.captured=lane.completed=now;lane.fresh=true;
            service.after_lane(lane);TrafficLightData data={};traffic_ipc_open(&ipc,0);
            if(!traffic_ipc_read(&ipc,&data)&&traffic_fresh(&data,traffic_clock_ns())&&data.state==expected[phase])++confirmed;
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
        printf("SAVED_REPLAY_RESULT %s confirmed_samples=%u\n",traffic_state_name(expected[phase]),confirmed);fflush(stdout);
        if(confirmed<5)pass=false;
    }
    traffic_ipc_close(&ipc);puts(pass?"SAVED_TRAFFIC_SERVICE_PASS":"SAVED_TRAFFIC_SERVICE_FAIL");return pass?0:1;
}
