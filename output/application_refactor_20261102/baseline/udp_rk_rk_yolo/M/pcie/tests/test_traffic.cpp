#include "traffic_vision.h"
#include "traffic_light_wire.h"
#include <cassert>
#include <cstdio>
using namespace traffic;
namespace {
const int W=320,H=240;
using Image=std::vector<uint8_t>;
static Image background(){return Image(W*H*3,110);}
static void rectangle(Image& im,int x,int y,int w,int h,unsigned r,unsigned g,unsigned b){for(int yy=y;yy<y+h;++yy)for(int xx=x;xx<x+w;++xx){auto p=&im[(yy*W+xx)*3];p[0]=r;p[1]=g;p[2]=b;}}
static void disk(Image& im,int x,int y,int radius,unsigned r,unsigned g,unsigned b){for(int yy=y-radius;yy<=y+radius;++yy)for(int xx=x-radius;xx<=x+radius;++xx)if((xx-x)*(xx-x)+(yy-y)*(yy-y)<=radius*radius){auto p=&im[(yy*W+xx)*3];p[0]=r;p[1]=g;p[2]=b;}}
static Image signal(unsigned state){auto im=background();rectangle(im,60,40,40,120,20,20,20);if(state==1)disk(im,80,60,11,240,10,10);if(state==2)disk(im,80,100,11,240,180,0);if(state==3)disk(im,80,140,11,0,235,210);return im;}
static Color color(const Image& im){return classify_color(im.data(),W,H,W*3,Box(60,40,40,120,.8));}
}
int main(){
    // M's physical middle camera is bottom-right; display already interprets
    // these bytes as BGR. Other tiles and the legacy source must be untouched.
    const int mosaic_stride=640*3+16;Image mosaic(mosaic_stride*480,0);
    for(int y=0;y<480;++y)for(int x=0;x<640;++x){auto p=&mosaic[y*mosaic_stride+x*3];
        if(x>=320&&y>=240){p[0]=10;p[1]=20;p[2]=240;}else{p[0]=40;p[1]=50;p[2]=60;}}
    auto original=mosaic;Image normalized;
    assert(copy_camera_rgb(mosaic.data(),640,480,mosaic_stride,InputView(320,240,PixelOrder::BGR),normalized));
    for(size_t k=0;k<normalized.size();k+=3)assert(normalized[k]==240&&normalized[k+1]==20&&normalized[k+2]==10);
    assert(mosaic==original);
    assert(copy_camera_rgb(mosaic.data(),640,480,mosaic_stride,InputView(),normalized));
    assert(normalized[0]==40&&normalized[1]==50&&normalized[2]==60);
    assert(!copy_camera_rgb(mosaic.data(),640,480,mosaic_stride,InputView(321,240,PixelOrder::BGR),normalized));
    assert(!copy_camera_rgb(mosaic.data(),640,480,640*3-1,InputView(),normalized));
    for(unsigned state=1;state<=3;++state){auto im=signal(state),before=im;auto r=color(im);assert(r.state==state&&r.quality>=65);assert(im==before);}
    auto both=signal(1);disk(both,80,140,11,0,235,210);assert(color(both).reason==TRAFFIC_AMBIGUOUS);
    auto plain=background();rectangle(plain,60,40,40,120,240,5,5);assert(!color(plain).state);
    auto outside=signal(0);disk(outside,150,60,15,240,0,0);assert(!color(outside).state);
    auto wrong_order=signal(0);disk(wrong_order,80,140,11,240,0,0);assert(!color(wrong_order).state);
    auto white=signal(0);disk(white,80,60,11,255,255,255);assert(!color(white).state);
    auto blue=signal(0);disk(blue,80,60,11,20,20,250);assert(!color(blue).state);
    auto sign=background();rectangle(sign,60,40,40,120,10,90,210);disk(sign,80,100,10,240,195,0);assert(!color(sign).state);
    auto seam=Image(W*H*3,20);rectangle(seam,0,20,8,10,245,10,10);
    assert(!classify_color(seam.data(),W,H,W*3,Box(0,10,8,60,.073)).state);
    assert(!classify_color(seam.data(),W,H,W*3,Box(0,10,40,60,.073)).state);
    auto good=signal(1);assert(classify_color(good.data(),W,H,W*3,Box(60,40,3,3,1)).reason==TRAFFIC_SMALL);
    HousingTracker tracker;Box box(60,40,40,120,.9),tracked;tracker.start(good.data(),W,H,W*3,box);assert(tracker.update(good.data(),W,H,W*3,box,tracked));assert(std::abs(tracked.x-box.x)<1);
    auto green=signal(3);assert(tracker.update(green.data(),W,H,W*3,box,tracked));assert(classify_color(green.data(),W,H,W*3,tracked).state==3);
    Image shifted(W*H*3,110);for(int y=0;y<H;++y)for(int x=4;x<W;++x)for(int c=0;c<3;++c)shifted[(y*W+x)*3+c]=green[(y*W+x-4)*3+c];
    assert(tracker.update(shifted.data(),W,H,W*3,box,tracked));assert(std::abs(tracked.x-64)<2.1);
    Image lost(W*H*3,250);assert(!tracker.update(lost.data(),W,H,W*3,box,tracked));
    ColorConfirmation confirm;
    assert(!confirm.observe(1,1,1000000000ULL));assert(!confirm.observe(1,2,1100000000ULL));assert(confirm.observe(1,3,1200000000ULL)==1);
    assert(!confirm.observe(1,3,1200000000ULL));assert(!confirm.observe(3,4,1300000000ULL));assert(!confirm.observe(3,5,1400000000ULL));assert(confirm.observe(3,6,1500000000ULL)==3);
    assert(!confirm.observe(3,7,2000000000ULL));assert(!confirm.observe(0,8,2100000000ULL));
    Admission admission;LaneBudget lane;lane.source=1;lane.captured=1000000000ULL;lane.completed=1280000000ULL;lane.duration=280000000ULL;lane.fresh=true;
    assert(admission.allow(lane,1280000000ULL));lane.curve_active=true;assert(!admission.allow(lane,1280000000ULL));lane.curve_active=false;
    lane.duration=360000000ULL;assert(!admission.allow(lane,1280000000ULL));lane.duration=280000000ULL;assert(!admission.allow(lane,1400000000ULL));
    admission.completed(1300000000ULL,20000000ULL);assert(!admission.allow(lane,1300000000ULL));lane.captured=2200000000ULL;assert(admission.allow(lane,2480000000ULL));
    for(int i=0;i<3;++i)admission.completed(3000000000ULL+i*11000000000ULL,90000000ULL);
    lane.captured=50000000000ULL;assert(!admission.allow(lane,50280000000ULL));
    TrafficLightData d={};d.session=7;d.source_id=10;d.detector_id=1;d.state=d.raw_state=TRAFFIC_RED;d.flags=TRAFFIC_BOX|TRAFFIC_CONFIRMED;d.color_quality=90;d.detector_quality=75;d.x=60;d.y=40;d.width=40;d.height=120;traffic_set_deadline(&d,1750000000ULL);
    uint8_t packet[traffic_wire::SIZE];assert(traffic_wire::encode(packet,d,1100000000ULL));traffic_wire::Receiver receiver;TrafficLightData received={};
    assert(receiver.ingest(packet,sizeof(packet),4000000000ULL,4000000000ULL,received));assert(received.state==1&&traffic_deadline(&received)==4650000000ULL);
    assert(receiver.ingest(packet,sizeof(packet),4200000000ULL,4200000000ULL,received));assert(traffic_deadline(&received)==4650000000ULL);
    assert(!receiver.ingest(packet,sizeof(packet),4700000000ULL,4700000000ULL,received));
    packet[30]^=1;assert(!receiver.ingest(packet,sizeof(packet),4300000000ULL,4300000000ULL,received));
    assert(!traffic_wire::encode(packet,d,1750000000ULL));d.width=320;assert(!traffic_wire::encode(packet,d,1100000000ULL));
    static_assert(sizeof(TrafficLightData)==64,"traffic data ABI");static_assert(sizeof(TrafficLightSegment)==72,"traffic IPC ABI");
    puts("TRAFFIC TEST PASS: middle-camera ROI/BGR normalization, raw-input immutability, RGB/color ambiguity, housing tracking, distinct-frame confirmation, lane-first admission/backoff, cross-clock wire TTL, duplicate expiry, CRC and bounds");
}
