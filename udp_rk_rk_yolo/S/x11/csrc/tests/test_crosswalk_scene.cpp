#include "crosswalk_scene.h"
#include <cassert>
#include <cstdio>
#include <vector>
int main(){const unsigned width=320,height=480,stride=336,guard=128;const uint32_t base=0xff183448;
 std::vector<uint32_t>canvas(guard+stride*height+guard,base);crosswalk_display::Geometry g;g.count=4;g.quality=80;
 for(unsigned i=0;i<4;++i){crosswalk_display::Quad q={{80+i*35,150,87+i*35,150,87+i*35,195,80+i*35,195}};g.stripes[i]=q;}
 auto first=canvas;crosswalk_scene::draw(canvas.data()+guard,stride,width,height,g,0);unsigned changed=0;
 for(unsigned i=0;i<canvas.size();++i){if(canvas[i]==base)continue;++changed;assert(i>=guard&&i<guard+stride*height);unsigned x=(i-guard)%stride,y=(i-guard)/stride;assert(x<width&&y<height*.60&&y>height*.25);}
 assert(changed>40);auto straight=canvas;canvas=first;crosswalk_scene::draw(canvas.data()+guard,stride,width,height,g,20);assert(canvas!=straight);
 auto shifted=g;for(unsigned i=0;i<shifted.count;++i)for(unsigned j=0;j<4;++j)shifted.stripes[i].xy[j*2]+=25;
 canvas=first;crosswalk_scene::draw(canvas.data()+guard,stride,width,height,shifted,0);assert(canvas==straight);
 for(unsigned i=0;i<shifted.count;++i)for(unsigned j=0;j<4;++j)shifted.stripes[i].xy[j*2+1]-=40;
 canvas=first;crosswalk_scene::draw(canvas.data()+guard,stride,width,height,shifted,0);assert(canvas!=straight);
 unsigned left=0,right=0;for(unsigned y=0;y<height;++y)for(unsigned x=0;x<width;++x)if(straight[guard+y*stride+x]!=base){if(x<width/2)++left;else ++right;}
 assert(left>20&&right>20);
 g.count=0;canvas=first;crosswalk_scene::draw(canvas.data()+guard,stride,width,height,g,20);assert(canvas==first);
 puts("CROSSWALK_SCENE_PASS: bounded white paint, angled placement, empty clears, no car/control mutation");
}
