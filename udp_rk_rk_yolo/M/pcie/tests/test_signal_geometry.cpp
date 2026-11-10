#include "traffic_signal_geometry.h"
#include <cassert>
#include <cstdio>
using namespace traffic;
using Image=std::vector<uint8_t>;
static void fill(Image&im,int x0,int y0,int x1,int y1,int r,int g,int b){for(int y=y0;y<y1;++y)for(int x=x0;x<x1;++x){auto*p=im.data()+(y*320+x)*3;p[0]=r;p[1]=g;p[2]=b;}}
int main(){
    Image im(320*240*3,230);Box b(100,40,20,60,.6f),out;
    fill(im,100,40,120,100,130,140,155);
    fill(im,104,75,116,90,20,250,100);fill(im,108,79,112,83,250,255,250);
    assert(exposed_signal_housing(im.data(),320,240,960,b));
    assert(refine_signal_box(im.data(),320,240,960,b,out));
    assert(signal_frame_housing(im.data(),320,240,960,out));
    Box weak=b;weak.score=.10f;assert(!refine_signal_box(im.data(),320,240,960,weak,out));
    Image sign(320*240*3,230);fill(sign,100,40,120,100,20,20,230);fill(sign,108,79,112,83,255,255,255);
    assert(!exposed_signal_housing(sign.data(),320,240,960,b));
    Image rectangle(320*240*3,230);fill(rectangle,100,40,120,100,10,240,10);
    assert(!refine_signal_box(rectangle.data(),320,240,960,b,out));
    Image one(320*240*3,230);fill(one,100,40,110,100,40,40,40);fill(one,108,79,112,83,255,255,255);
    assert(!exposed_signal_housing(one.data(),320,240,960,b));
    Box a(40,50,8,20,.8f),c(80,52,9,20,.2f),ped(130,100,10,22,.8f);
    assert(same_signal_bar(a,c)&&!same_signal_bar(a,ped));
    assert(!same_signal_bar(a,Box(42,50,8,20,.8f)));
    const std::array<float,3> merits={{.1f,.3f,.2f}};
    assert(next_signal_window(0,0,merits,2)==2);
    for(unsigned seen=1;seen<7;++seen)for(unsigned cursor=0;cursor<12;++cursor)
        assert(!(seen&(1u<<next_signal_window(seen,cursor,merits,0))));
    assert(next_signal_window(7,3,merits,0)==1);
    assert(next_signal_window(7,8,merits,0)!=1);
    const std::array<float,3> empty={{0,0,0}};
    for(unsigned n=0;n<9;++n)assert(next_signal_window(7,n,empty,0)==n%3);
    assert(signal_scout_delay(empty,1)==100000000ULL&&signal_scout_delay(empty,7)==600000000ULL&&signal_scout_delay(merits,7)==600000000ULL);
    assert(signal_relevance(a,160)>signal_relevance(Box(a.x,a.y+80,a.w,a.h,a.score),160));
    puts("SIGNAL GEOMETRY PASS: bounded semantic alignment, weak-score rejection, exposed shell, blue/flat/one-sided negatives, aligned-bar ambiguity");
}
