#include "crosswalk_detector.h"
#include <cassert>
#include <cstdio>
#include <chrono>
#include <fstream>
static void fill(std::vector<uint8_t>&im,int x0,int y0,int x1,int y1,int value){for(int y=y0;y<y1;++y)for(int x=x0;x<x1;++x)for(unsigned c=0;c<3;++c)im[(y*320+x)*3+c]=value;}
int main(int argc,char**argv){
 crosswalk::Detector detector;std::vector<uint8_t>im(320*240*3,55),coverage(320*240,1);
 for(int i=0;i<8;++i)fill(im,88+i*18,160,94+i*18,200,210);
 auto g=detector.detect(im.data(),coverage.data());assert(g.count>=4);
 fill(im,88,190,226,195,220);assert(detector.detect(im.data(),coverage.data()).count>=4);
 crosswalk_display::Snapshot s={};s.session=7;s.source=9;s.count=g.count;s.quality=g.quality;std::copy_n(g.stripes,g.count,s.stripes);crosswalk_display::expires(s,1000000000ULL);assert(crosswalk_display::valid(s));
 uint8_t wire[crosswalk_display::WIRE_SIZE];assert(crosswalk_display::encode(wire,s,500000000ULL));crosswalk_display::Receiver rx;crosswalk_display::Snapshot got={};assert(rx.ingest(wire,sizeof(wire),10,10,got));assert(got.count==s.count);
 assert(rx.ingest(wire,sizeof(wire),20,20,got)&&crosswalk_display::deadline(got)==500000010ULL);assert(!rx.ingest(wire,sizeof(wire),500000011ULL,500000011ULL,got));
 wire[45]^=1;assert(!rx.ingest(wire,sizeof(wire),10,10,got));wire[45]^=1;
 s.source=8;assert(crosswalk_display::encode(wire,s,500000000ULL));assert(!rx.ingest(wire,sizeof(wire),10,10,got));
 s.source=10;s.count=0;s.quality=0;assert(crosswalk_display::encode(wire,s,500000000ULL)&&rx.ingest(wire,sizeof(wire),20,20,got)&&!got.count);
 // Flat white, a stop bar, two/dashed lane boundaries and uncovered paint are negative.
 std::fill(im.begin(),im.end(),200);assert(!detector.detect(im.data(),coverage.data()).count);
 std::fill(im.begin(),im.end(),55);fill(im,40,175,280,185,220);assert(!detector.detect(im.data(),coverage.data()).count);
 std::fill(im.begin(),im.end(),55);for(int x:{90,220})for(int y:{145,175,205})fill(im,x,y,x+6,y+18,220);assert(!detector.detect(im.data(),coverage.data()).count);
 std::fill(coverage.begin(),coverage.end(),0);assert(!detector.detect(im.data(),coverage.data()).count);std::fill(coverage.begin(),coverage.end(),1);
 if(argc==3){std::ifstream f(argv[1],std::ios::binary);std::string magic;int w,h,v;f>>magic>>w>>h>>v;f.get();assert(magic=="P6"&&w==320&&h==240&&v==255);f.read((char*)im.data(),im.size());assert(f);
  for(size_t i=0;i<im.size();i+=3)std::swap(im[i],im[i+2]);
  std::ifstream c(argv[2],std::ios::binary);c>>magic>>w>>h>>v;c.get();assert(magic=="P5"&&w==320&&h==240);c.read((char*)coverage.data(),coverage.size());assert(c);g=detector.detect(im.data(),coverage.data());
  printf("CROSSWALK_RESULT {\"count\":%u,\"quality\":%u,\"quads\":[",g.count,g.quality);for(unsigned i=0;i<g.count;++i){printf("%s[",i?",":"");for(unsigned j=0;j<8;++j)printf("%s%u",j?",":"",g.stripes[i].xy[j]);printf("]");}puts("]}");}
 auto started=std::chrono::steady_clock::now();for(unsigned i=0;i<100;++i)g=detector.detect(im.data(),coverage.data());printf("CROSSWALK_PASS mean_ms=%.3f\n",std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-started).count()/100);
}
