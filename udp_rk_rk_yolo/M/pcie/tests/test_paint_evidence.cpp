#include "ego_lane_selector.h"
#include "line_style_analyzer.h"
#include <cassert>
#include <chrono>
#include <cstdio>
#include <utility>
#include <vector>

static const int W=320,H=240;
struct Scene {
    std::vector<uint8_t> rgb,coverage;
    m_lane::PointLanePair pair;
    Scene():rgb(W*H*3),coverage(W*H,1){pair.width=W;pair.height=H;pair.first=20;pair.last=225;pair.candidates=2;}
};
static Scene scene(unsigned left,unsigned right,int background,int paint,int offset,double slope,bool yellow=false) {
    Scene s;std::fill(s.rgb.begin(),s.rgb.end(),background);
    for(int y=s.pair.first;y<=s.pair.last;++y)for(int side=0;side<2;++side){
        const int x=(int)std::lround((side?225:95)+slope*(y-120));
        (side?s.pair.right:s.pair.left).push_back({double(x),0.,y,3,true});
        const bool on=(side?right:left)==LINE_SOLID || (y-20)%48<26;
        if(on)for(int dx=-1;dx<=1;++dx){
            auto *p=s.rgb.data()+(y*W+x+offset+dx)*3;
            p[0]=yellow?0:paint;p[1]=yellow?paint*85/100:paint;p[2]=paint;
        }
    }
    return s;
}
static YOLOLineStyleResult classify(const Scene &s){return m_lane::EgoLaneSelector::describe_points(s.pair,s.rgb.data(),true,s.coverage.data());}
int main(){
    unsigned correct=0,total=0;
    for(const auto &level:{std::pair<int,int>{45,210},{10,35},{5,20},{65,90}})
      for(int offset:{-4,0,4})for(double slope:{-.45,0.,.45})for(bool yellow:{false,true})
        for(unsigned l:{LINE_SOLID,LINE_DASHED})for(unsigned r:{LINE_SOLID,LINE_DASHED}){
            auto s=scene(l,r,level.first,level.second,offset,slope,yellow);
            const auto result=classify(s);correct+=result.left.type==l;correct+=result.right.type==r;total+=2;
#ifndef PAINT_BASELINE_MEASURE
            if(result.left.type!=l||result.right.type!=r)std::printf("FAIL paint bg=%d paint=%d offset=%d slope=%.2f yellow=%d expected=%u/%u got=%u/%u duty=%u/%u reason=%u/%u\n",level.first,level.second,offset,slope,yellow,l,r,result.left.type,result.right.type,result.left.duty,result.right.duty,result.left.reason,result.right.reason);
            assert(result.left.type==l&&result.right.type==r);
#endif
        }
    std::printf("PAINT_CASES correct=%u total=%u (controlled images, not real-road accuracy)\n",correct,total);
    auto s=scene(LINE_SOLID,LINE_SOLID,45,210,0,0.);
    // Geometry is visible, but a wide missing image region cannot prove SOLID
    // or become a dark dash gap. Uniform bright/dark images are not paint.
    for(int y=90;y<140;++y)for(int x=0;x<W;++x)s.coverage[y*W+x]=0;
#ifndef PAINT_BASELINE_MEASURE
    auto occluded=classify(s);assert(!occluded.left.type&&!occluded.right.type);
#endif
    std::fill(s.coverage.begin(),s.coverage.end(),1);
    for(int gray:{0,45,230}){
        std::fill(s.rgb.begin(),s.rgb.end(),gray);const auto r=classify(s);assert(!r.left.type&&!r.right.type);
    }
    for(size_t i=0;i<s.rgb.size();i+=3){s.rgb[i]=0;s.rgb[i+1]=76;s.rgb[i+2]=90;}
    assert(!classify(s).left.type); // a uniformly yellow surface has no paint ridge
    std::fill(s.coverage.begin(),s.coverage.end(),0);assert(!classify(s).left.type);
    s=scene(LINE_SOLID,LINE_DASHED,10,35,0,.3,true);
    const auto raw=classify(s);
#ifndef PAINT_BASELINE_MEASURE
    m_lane::LineStyleAnalyzer confirm;
    assert(confirm.confirm(raw,1,1000000000ULL,&s.pair).left.type==LINE_SOLID);
    assert(!confirm.confirm(raw,1,1000000000ULL,&s.pair).left.type);
#else
    (void)raw;
#endif
    auto dense=scene(LINE_SOLID,LINE_SOLID,45,210,0,.45);
    for(int y=20;y<=225;++y)if((y-20)%24>=20){
        const int x=(int)std::lround(95+.45*(y-120));
        for(int dx=-1;dx<=1;++dx)for(int c=0;c<3;++c)dense.rgb[(y*W+x+dx)*3+c]=45;
    }
    assert(classify(dense).left.type==LINE_DASHED && classify(dense).right.type==LINE_SOLID);
#ifndef PAINT_BASELINE_MEASURE
    auto screen=scene(LINE_SOLID,LINE_SOLID,45,210,0,0.);
    for(int y=55;y<=184;++y)if(y<=76 || (y>=161 && y<=184))
        for(int dx=-1;dx<=1;++dx)for(int c=0;c<3;++c)
            screen.rgb[(y*W+95+dx)*3+c]=53; // screen glow across a real gap
    const auto restored=classify(screen);
    assert(restored.left.type==LINE_DASHED && restored.left.gaps>=2);
    assert(restored.right.type==LINE_SOLID);
    for(int y=55;y<=184;++y)if(y<=76 || (y>=161 && y<=184))
        for(int x=80;x<=110;++x)screen.coverage[y*W+x]=0;
    assert(classify(screen).left.type==LINE_UNKNOWN); // the same interval with no visible road
    // IPM black wedges and frame edges must not erase a visible solid line.
    // Missing background is never synthesized into a dark dash gap.
    auto edge=scene(LINE_SOLID,LINE_DASHED,45,210,0,0.);
    for(int y=20;y<=225;++y)for(int x=0;x<91;++x)edge.coverage[y*W+x]=0;
    assert(classify(edge).left.type==LINE_SOLID && classify(edge).right.type==LINE_DASHED);
    for(int y=85;y<145;++y)for(int x=91;x<105;++x)edge.coverage[y*W+x]=0;
    assert(!classify(edge).left.type);
    auto border=scene(LINE_SOLID,LINE_SOLID,45,210,0,0.);
    for(auto &point:border.pair.left){
        for(int dx=-1;dx<=1;++dx)for(int c=0;c<3;++c)border.rgb[(point.y*W+(int)point.x+dx)*3+c]=45;
        point.x=12;
        for(int dx=-1;dx<=1;++dx)for(int c=0;c<3;++c)border.rgb[(point.y*W+12+dx)*3+c]=210;
    }
    assert(classify(border).left.type==LINE_SOLID);
    auto glare=scene(LINE_SOLID,LINE_DASHED,75,220,0,0.);
    // Distant bright roadside material is not the immediate road background.
    for(int y=20;y<=225;++y)for(int x=103;x<=112;++x)for(int c=0;c<3;++c)glare.rgb[(y*W+x)*3+c]=245;
    assert(classify(glare).left.type==LINE_SOLID && classify(glare).right.type==LINE_DASHED);
    auto gradient=scene(LINE_SOLID,LINE_DASHED,45,245,0,0.);
    for(int y=20;y<=225;++y)for(int x=70;x<=115;++x)if(std::abs(x-95)>1)
        for(int c=0;c<3;++c)gradient.rgb[(y*W+x)*3+c]=std::max(100,std::min(240,218-(x-95)*2));
    assert(classify(gradient).left.type==LINE_SOLID);
    for(int y=20;y<=225;++y)for(int x=70;x<=115;++x)
        for(int c=0;c<3;++c)gradient.rgb[(y*W+x)*3+c]=std::max(100,std::min(240,218-(x-95)*2));
    assert(!classify(gradient).left.type); // gradient without a ridge is not paint
    auto speck=scene(LINE_DASHED,LINE_DASHED,45,210,0,0.);
    for(int y:{55,103,151,199})for(int x:{95,225})for(int c=0;c<3;++c)speck.rgb[(y*W+x)*3+c]=53;
    m_lane::LanePaintEvidence observed(speck.rgb.data(),W,H,speck.coverage.data());
    assert(observed.sample(95,55,0.,3)==m_lane::PAINT_WEAK);
    assert(classify(speck).left.type==LINE_DASHED && classify(speck).right.type==LINE_DASHED);
    auto clipped=scene(LINE_DASHED,LINE_DASHED,45,210,0,0.);
    clipped.pair.last=71;
    for(auto *line:{&clipped.pair.left,&clipped.pair.right})while(line->back().y>71)line->pop_back();
    assert(classify(clipped).left.type==LINE_DASHED && classify(clipped).right.type==LINE_DASHED);
    // A missing row inside the sole observed gap cannot be bridged as a speck.
    for(int x=0;x<W;++x)clipped.coverage[55*W+x]=0;
    assert(m_lane::LanePaintEvidence(clipped.rgb.data(),W,H,clipped.coverage.data()).sample(95,55,0.,3)==m_lane::PAINT_MISSING);
    assert(!classify(clipped).left.type);
#endif
    const auto begin=std::chrono::steady_clock::now();unsigned checksum=0;
    for(int n=0;n<200;++n){const auto r=classify(s);checksum+=r.left.type+r.right.type;}
    std::printf("PAINT_PAIR mean_ms=%.4f checksum=%u\n",std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-begin).count()/200,checksum);
    std::puts("PAINT EVIDENCE PASS: white/yellow, shadow, lateral error, oblique lines, true gaps, coverage barriers, no-paint and immediate confirmation");
}
