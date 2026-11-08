#include "person_road_association.h"
#include <cassert>
#include <chrono>
#include <cstdio>
#include <limits>
using namespace traffic;
int main() {
    std::vector<uint8_t> rgb(320*240*3);
    std::vector<float> mask(320*240*2,0);
    FrontRoadFrame f; f.pixels=rgb.data(); f.road=mask.data();
    f.width=320; f.height=240; f.source=12; f.captured=100;
    auto rect=[&](int x0,int y0,int x1,int y1,float value=1.f) {
        for(int y=y0;y<=y1;++y) for(int x=x0;x<=x1;++x) mask[320*240+y*320+x]=value;
    };
    PersonRoadAssociation a;
    const Box inside(140,80,40,100,.7f), outside(20,80,30,100,.9f);
    assert(!a.prepare(f,13,100)); // different frame
    assert(!a.prepare(f,12,101)); // different timestamp
    assert(!a.prepare(f,12,100)); // empty mask is NOT a clear road
    assert(a.classify(outside)==FootRelation::Unknown);
    rect(100,120,220,239);
    assert(a.prepare(f,12,100));
    assert(a.classify(inside)==FootRelation::Inside);
    assert(a.classify(outside)==FootRelation::Outside);
    assert(a.classify(Box(90,80,10,100,.8))==FootRelation::Unknown); // road edge
    assert(a.classify(Box(140,150,40,90,.8))==FootRelation::Unknown); // cropped feet
    assert(a.classify(Box(140,80,2,100,.8))==FootRelation::Unknown);
    assert(a.classify(Box(-1,80,40,100,.8))==FootRelation::Unknown);
    // Person silhouette has no drivable pixels: support below feet still works.
    rect(140,80,179,179,0); assert(a.prepare(f,12,100));
    assert(a.classify(inside)==FootRelation::Inside);
    PersonResult p;p.candidates={outside,inside};p.count=2;p.best=outside;
    FrontPersonData d={};d.flags=3;d.source_id=12;
    a.annotate(p,f,12,100,d);
    assert(d.count==2 && d.risk_count==1 && d.outside_count==1 && !d.unknown_count);
    assert(front_person_hazard(&d) && d.x==140 && d.road_source_id==12);
    p.candidates={outside};p.count=1;
    d=FrontPersonData{};d.flags=3;a.annotate(p,f,12,100,d);
    assert(d.outside_count==1 && !front_person_hazard(&d));
    d=FrontPersonData{};d.flags=3;a.annotate(p,f,99,100,d);
    assert(d.unknown_count==1 && front_person_hazard(&d));
    mask[0]=std::numeric_limits<float>::quiet_NaN();
    assert(!a.prepare(f,12,100));
    d=FrontPersonData{};d.flags=3;a.annotate(p,f,12,100,d);
    assert(d.unknown_count==1 && front_person_hazard(&d));
    p=PersonResult{};d=FrontPersonData{};d.flags=3;a.annotate(p,f,12,100,d);
    assert(!d.count && !front_person_hazard(&d)); // valid no-person inference
    mask[0]=0;
    auto start=std::chrono::steady_clock::now();
    for(int i=0;i<1000;++i) { assert(a.prepare(f,12,100)); a.classify(inside); }
    const double ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count()/1000;
    std::printf("PERSON ROAD PASS: matched frame, all people, feet, occlusion, outside, edge, crop, empty/NaN/failure; association_mean_ms=%.4f\n",ms);
}
