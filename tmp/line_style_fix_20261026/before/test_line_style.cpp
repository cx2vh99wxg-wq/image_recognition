#include "line_style_analyzer.h"
#include "line_style_wire.h"
#include <cassert>
#include <cstdio>
static void scene(std::vector<float>& mask,std::vector<uint8_t>& rgb,int left,int right,bool fill=false,bool bend=false) {
    const int w=320,h=240;mask.assign(w*h,0);rgb.assign(w*h*3,45);
    for(int y=85;y<238;++y)for(int side=0;side<2;++side) {
        int kind=side?right:left;if(!kind)continue;
        double s=(y-72)/156.0;
        int x=160+(side?1:-1)*std::lround(100*s)+(bend?(int)(22*s*s):0);
        bool on=kind==LINE_SOLID||((y-85)%42<23);
        for(int dx=-3;dx<=3;++dx)if(x+dx>=0&&x+dx<w) {
            if(on||fill)mask[y*w+x+dx]=.9;
            if(on)for(int c=0;c<3;++c)rgb[(y*w+x+dx)*3+c]=220;
        }
    }
}
int main() {
    m_lane::LineStyleAnalyzer a;std::vector<float> mask;std::vector<uint8_t> rgb;
    for(bool bend:{false,true})for(bool fill:{false,true})for(int l:{1,2})for(int r:{1,2}) {
        scene(mask,rgb,l,r,fill,bend);auto raw=a.analyze(mask.data(),320,240,rgb.data(),320,240);
        printf("scene bend=%d filled=%d expected=%d/%d got=%u/%u duty=%u/%u spans=%u/%u\n",bend,fill,l,r,raw.left.type,raw.right.type,raw.left.duty,raw.right.duty,raw.left.spans,raw.right.spans);
        assert(raw.left.type==(unsigned)l&&raw.right.type==(unsigned)r);
    }
    scene(mask,rgb,0,0);auto empty=a.analyze(mask.data(),320,240,rgb.data(),320,240);assert(!empty.left.type&&!empty.right.type);
    scene(mask,rgb,1,1);std::fill(rgb.begin(),rgb.end(),45);auto shadow=a.analyze(mask.data(),320,240,rgb.data(),320,240);assert(!shadow.left.type&&!shadow.right.type);
    scene(mask,rgb,1,2);auto raw=a.analyze(mask.data(),320,240,rgb.data(),320,240);
    assert(!a.confirm(raw,1,1000000000).left.type);
    assert(!a.confirm(raw,1,1000000000).left.type); // duplicated display is not confirmation
    assert(!a.confirm(raw,2,1100000000).left.type);
    assert(a.confirm(raw,3,1200000000).left.type==LINE_SOLID);
    assert(!a.confirm(raw,4,1800000000).left.type); // capture gap resets temporal state
    LineStyleData d={};d.session=9;d.source_id=10;d.left=1;d.right=2;d.left_quality=90;line_set_deadline(&d,1500000000);
    uint8_t wire[line_wire::SIZE];assert(line_wire::encode(wire,d,1100000000));
    line_wire::Receiver receiver;LineStyleData got={};assert(receiver.ingest(wire,sizeof(wire),1100000000,1100000000,got));
    assert(got.left==1&&got.right==2&&line_deadline(&got)==1500000000);
    assert(receiver.ingest(wire,sizeof(wire),1200000000,1200000000,got));assert(line_deadline(&got)==1500000000);
    assert(!receiver.ingest(wire,sizeof(wire),1600000000,1600000000,got));
    wire[20]^=1;assert(!receiver.ingest(wire,sizeof(wire),1200000000,1200000000,got));
    puts("line styles / RGB gaps / temporal identity / metadata expiry PASS");
}
