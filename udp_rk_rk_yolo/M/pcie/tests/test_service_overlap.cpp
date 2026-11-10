// Reuse fake RKNN tensors; no real NPU/device/control transport is opened.
#define main decoder_fixture_main
#include "test_signal_competition.cpp"
#undef main
#include "traffic_service.h"
#include "traffic_light_ipc.h"
#include "front_person_ipc.h"
#include <sched.h>
#include <thread>
#include <chrono>
int main(){
    if(unshare(CLONE_NEWIPC)){std::perror("private IPC namespace (run as root)");return 2;}
    const char*path="build/service-overlap.fixture";{std::ofstream f(path);f<<"fixture";}
    quant=false;nhwc=false;mode=0;
    std::vector<uint8_t> rgb(320*240*3,110);
    for(int y=41;y<=103;++y)for(int x=77;x<=99;++x)for(int c=0;c<3;++c)rgb[(y*320+x)*3+c]=20;
    for(int y=47;y<=57;++y)for(int x=83;x<=93;++x)if((x-88)*(x-88)+(y-52)*(y-52)<=16){auto*p=&rgb[(y*320+x)*3];p[0]=240;p[1]=p[2]=10;}
    std::vector<float> road(2*320*240);std::fill(road.begin()+320*240,road.end(),1.f);
    traffic::Service service(path);FrontPersonIPC ipc={-1,nullptr};
    auto read=[&]{FrontPersonData p={};assert(!front_person_ipc_open(&ipc,0)&&!front_person_ipc_read(&ipc,&p));return p;};
    auto lane=[&](unsigned id){traffic::LaneBudget b;b.source=id;b.captured=b.completed=traffic_clock_ns();b.fresh=true;
        b.front.pixels=rgb.data();b.front.road=road.data();b.front.width=320;b.front.height=240;b.front.source=id;b.front.captured=b.captured;return b;};
    auto b=lane(1);calls=0;service.prepare_lane(b);assert(calls==1&&!read().source_id);
    b.fresh=false;service.after_lane(b);assert(calls==1&&!read().source_id);
    b=lane(2);calls=0;service.prepare_lane(b);b.duration=350000001ULL;service.after_lane(b);assert(calls==1&&!read().source_id);
    b=lane(3);calls=0;service.prepare_lane(b);assert(calls==1&&!read().source_id);service.after_lane(b);
    auto p=read();assert(calls==1&&p.source_id==3&&p.road_source_id==3&&(p.flags&FRONT_PERSON_VALID)&&p.risk_count);
    b=lane(4);calls=0;service.prepare_lane(b);service.after_lane(b);assert(calls==0&&read().source_id==3);
    std::this_thread::sleep_for(std::chrono::milliseconds(110));
    b=lane(5);calls=0;service.prepare_lane(b);assert(calls==1&&read().source_id==3);
    b=lane(6);service.after_lane(b);p=read();assert(calls==2&&p.source_id==6&&p.road_source_id==6&&p.risk_count);
    front_person_ipc_close(&ipc);std::remove(path);
    puts("SERVICE OVERLAP PASS: private IPC, no early brake publication, stale/budget discard, one accepted inference, original rate gate, identity mismatch fallback, same-frame road association");
}
