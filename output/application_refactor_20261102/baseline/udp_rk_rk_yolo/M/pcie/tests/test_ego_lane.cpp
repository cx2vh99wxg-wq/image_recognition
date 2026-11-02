#include "line_style_analyzer.h"
#include "line_style_wire.h"
#include <cassert>
#include <cstdio>
#include <cstring>
namespace {
const int W=320,H=240;
struct Scene { std::vector<float> lane;std::vector<uint8_t> green,rgb;Scene():lane(W*H,0),green(W*H,0),rgb(W*H*3,45){} };
static Scene inset(unsigned left=LINE_SOLID,unsigned middle=LINE_DASHED,bool high_duty=false) {
    Scene s;const int tip=100;
    for(int y=tip;y<230;++y)for(int x=0;x<W;++x) {
        int d=y-tip;double a=60-1.7*d,c=60+2.4*d;
        if(x>=a&&x<=c&&y<=140+x*.2){s.green[y*W+x]=255;for(int k=0;k<3;++k)s.rgb[(y*W+x)*3+k]=90;}
    }
    for(int y=tip+3;y<225;++y)for(int line=0;line<3;++line) {
        double d=y-tip;int x=(int)std::lround(60+(line==0?-1.7:line==1?.7:2.4)*d);
        unsigned kind=line==0?left:line==1?middle:unsigned(LINE_SOLID);
        bool on=kind==LINE_SOLID||(high_duty?((y-tip)%24<20):((y-tip)%16<8));
        if(y>148+x*.2)continue;
        for(int dx=-3;dx<=3;++dx)if(x+dx>=0&&x+dx<W) {
            s.lane[y*W+x+dx]=1;
            if(on)for(int k=0;k<3;++k)s.rgb[(y*W+x+dx)*3+k]=230;
        }
    }
    return s;
}
static Scene mirror(const Scene& a) {
    Scene b;
    for(int y=0;y<H;++y)for(int x=0;x<W;++x){int from=y*W+x,to=y*W+W-1-x;b.lane[to]=a.lane[from];b.green[to]=a.green[from];for(int k=0;k<3;++k)b.rgb[to*3+k]=a.rgb[from*3+k];}
    return b;
}
static Scene full(unsigned left,unsigned right,bool curved) {
    Scene s;
    for(int y=80;y<236;++y) {
        double d=y-70,bend=curved?22*(d/166)*(d/166):0;
        for(int x=0;x<W;++x)if(x>=160-1.2*d+bend&&x<=160+1.2*d+bend){s.green[y*W+x]=255;for(int k=0;k<3;++k)s.rgb[(y*W+x)*3+k]=80;}
        for(int line=0;line<3;++line) {
            int x=(int)std::lround(160+(line==0?-.65:line==1?.65:-1.15)*d+bend);
            unsigned kind=line==0?left:line==1?right:unsigned(LINE_DASHED);
            bool on=kind==LINE_SOLID||((y-80)%36<19);
            for(int dx=-3;dx<=3;++dx)if(x+dx>=0&&x+dx<W){s.lane[y*W+x+dx]=1;if(on)for(int k=0;k<3;++k)s.rgb[(y*W+x+dx)*3+k]=220;}
        }
    }
    return s;
}
static YOLOLineStyleResult classify(const Scene& s) { return m_lane::EgoLaneSelector().analyze(s.lane.data(),s.green.data(),s.rgb.data(),W,H); }
static YOLOLineStyleResult known() {
    YOLOLineStyleResult r={};r.width=W;r.height=H;r.road_area=8000;r.object_count=3;r.reference_x=60;r.reference_y=100;
    r.left.type=r.left.raw_type=LINE_SOLID;r.right.type=r.right.raw_type=LINE_DASHED;
    r.left.quality=r.right.quality=85;r.left.first_y=r.right.first_y=120;r.left.last_y=r.right.last_y=180;
    r.left.fit_a=20;r.right.fit_a=200;return r;
}
static void tracking() {
    m_lane::LineStyleAnalyzer analyzer;auto raw=known();
    assert(!analyzer.confirm(raw,1,1000000000ULL).left.type);
    assert(!analyzer.confirm(raw,2,1280000000ULL).left.type);
    auto confirmed=analyzer.confirm(raw,3,1560000000ULL);
    assert(confirmed.left.type==LINE_SOLID&&confirmed.left.evidence_ns==1560000000ULL);
    auto gap=raw;gap.left.type=gap.left.raw_type=0;gap.left.reason=LINE_AMBIGUOUS;
    auto held=analyzer.confirm(gap,4,1840000000ULL);
    assert(held.left.type==LINE_SOLID&&held.left.reason==LINE_TRACK_HELD&&held.left.evidence_ns==1560000000ULL);
    auto d=line_result_data(&held,42,4,1840000000ULL);
    assert(line_deadline(&d)==2560000000ULL&&d.left==LINE_SOLID);
    held=analyzer.confirm(gap,5,2120000000ULL);d=line_result_data(&held,42,5,2120000000ULL);
    assert(held.left.type==LINE_SOLID&&line_deadline(&d)==2560000000ULL);
    uint8_t packet[line_wire::SIZE];assert(line_wire::encode(packet,d,2130000000ULL));
    line_wire::Receiver receiver;LineStyleData received={};
    assert(receiver.ingest(packet,sizeof(packet),2130000000ULL,2130000000ULL,received));
    assert(line_deadline(&received)==2560000000ULL);
    auto expired=analyzer.confirm(gap,6,2400000000ULL);assert(!expired.left.type);
    assert(!analyzer.confirm(raw,6,2400000000ULL).left.type);
    assert(!analyzer.confirm(raw,7,2680000000ULL).left.type);
    assert(!analyzer.confirm(raw,8,2960000000ULL).left.type);
    confirmed=analyzer.confirm(raw,9,3240000000ULL);assert(confirmed.left.type==LINE_SOLID);
    auto other=raw;other.left.fit_a=125;
    assert(!analyzer.confirm(other,10,3520000000ULL).left.type); // a different boundary cannot inherit the old label
    m_lane::LineStyleAnalyzer blind;
    blind.confirm(raw,1,1000000000ULL);blind.confirm(raw,2,1280000000ULL);blind.confirm(raw,3,1560000000ULL);
    auto missing=raw;missing.left={};missing.right={};missing.object_count=1;
    assert(blind.confirm(missing,4,1840000000ULL).left.reason==LINE_TRACK_HELD);
    assert(!blind.confirm(missing,5,2120000000ULL).left.type);
    m_lane::LineStyleAnalyzer absent;
    absent.confirm(raw,1,1000000000ULL);absent.confirm(raw,2,1280000000ULL);absent.confirm(raw,3,1560000000ULL);
    missing.road_area=0;missing.object_count=0;
    assert(!absent.confirm(missing,4,1840000000ULL).left.type);
    m_lane::LineStyleAnalyzer translated;
    translated.confirm(raw,1,1000000000ULL);translated.confirm(raw,2,1280000000ULL);translated.confirm(raw,3,1560000000ULL);
    auto shifted=raw;shifted.reference_y+=35;shifted.left.first_y+=35;shifted.left.last_y+=35;shifted.right.first_y+=35;shifted.right.last_y+=35;
    assert(translated.confirm(shifted,4,1840000000ULL).left.type==LINE_SOLID);
    assert(sizeof(LineStyleData)==56&&sizeof(LineStyleSegment)==64&&line_wire::SIZE==60);
    puts("ego tracking: identity, missing evidence, expiry, wire lifetime and translated ROI PASS");
}
}
int main() {
    Scene s=inset();auto a=classify(s);printf("inset three lines: %s/%s\n",line_style_name(a.left.type),line_style_name(a.right.type));
    assert(a.left.type==LINE_SOLID&&a.right.type==LINE_DASHED);
    a=classify(mirror(s));assert(a.left.type==LINE_DASHED&&a.right.type==LINE_SOLID);
    for(bool bend:{false,true})for(unsigned l:{LINE_SOLID,LINE_DASHED})for(unsigned r:{LINE_SOLID,LINE_DASHED}) {
        Scene f=full(l,r,bend);a=classify(f);printf("full curve=%d expected=%u/%u got=%u/%u\n",bend,l,r,a.left.type,a.right.type);
        assert(a.left.type==l&&a.right.type==r);
    }
    Scene empty;assert(!classify(empty).left.type);
    auto no_lane=s;std::fill(no_lane.lane.begin(),no_lane.lane.end(),0);a=classify(no_lane);assert(!a.left.type&&!a.right.type);
    auto no_rgb=s;std::fill(no_rgb.rgb.begin(),no_rgb.rgb.end(),60);a=classify(no_rgb);assert(!a.left.type&&!a.right.type);
    auto no_road=s;std::fill(no_road.green.begin(),no_road.green.end(),0);assert(!classify(no_road).left.type);
    a=classify(inset(LINE_DASHED));assert(a.left.type!=LINE_SOLID);
    a=classify(inset(LINE_DASHED,LINE_DASHED,true));assert(a.left.type!=LINE_SOLID); // dense repeated gaps are not a solid line
    m_lane::LineStyleAnalyzer analyzer;
    std::vector<float> mask(640*480,0),drive(640*480*2,0);std::vector<uint8_t> rgb(640*480*3,0);
    for(int y=0;y<H;++y)for(int x=0;x<W;++x){size_t source=y*W+x,target=y*640+x;mask[target]=s.lane[source];drive[target]=s.green[source]?0:1;drive[640*480+target]=s.green[source]?1:0;for(int k=0;k<3;++k)rgb[target*3+k]=s.rgb[source*3+k];}
    a=analyzer.analyze_front_camera(mask.data(),640,480,rgb.data(),640,480,nullptr,drive.data());
    assert(a.left.type==LINE_SOLID&&a.right.type==LINE_DASHED);
    tracking();puts("ego lane ownership / line style / negative evidence PASS");
}
