#ifndef TRAFFIC_STRUCTURE_H
#define TRAFFIC_STRUCTURE_H
#include "traffic_vision.h"
namespace traffic {
// Large diagram/HUD signals can be outside the COCO detector's training
// distribution. This independent locator requires a dark housing plus TWO
// separate unlit circular rims at equal spacing beside a round bright lamp.
// A colored spot, sign, rectangle or single lamp cannot qualify on color alone.
inline std::vector<Box> locate_three_lamp_housings(const uint8_t*rgb,int w,int h,int stride){
 std::vector<Box> out;if(!rgb||w!=320||h!=240||stride<w*3)return out;
 struct Seed{double x,y,r;unsigned area;};std::vector<Seed> seeds;
 std::vector<uint8_t> mask(w*h),seen(w*h);std::vector<int> queue;queue.reserve(w*h/4);
 for(int y=0;y<h;++y)for(int x=0;x<w;++x){const auto*p=rgb+y*stride+x*3;int hi=std::max(p[0],std::max(p[1],p[2])),lo=std::min(p[0],std::min(p[1],p[2]));mask[y*w+x]=hi>=120&&(hi-lo)*100>=hi*48;}
 for(int at=0;at<w*h;++at){if(!mask[at]||seen[at])continue;queue.clear();queue.push_back(at);seen[at]=1;
  int x0=at%w,x1=x0,y0=at/w,y1=y0;double sx=0,sy=0;
  for(size_t k=0;k<queue.size();++k){int i=queue[k],x=i%w,y=i/w;x0=std::min(x0,x);x1=std::max(x1,x);y0=std::min(y0,y);y1=std::max(y1,y);sx+=x;sy+=y;
   const int dx[]={-1,1,0,0},dy[]={0,0,-1,1};for(int n=0;n<4;++n){int nx=x+dx[n],ny=y+dy[n];if(nx<0||ny<0||nx>=w||ny>=h)continue;int j=ny*w+nx;if(mask[j]&&!seen[j]){seen[j]=1;queue.push_back(j);}}
  }
  int bw=x1-x0+1,bh=y1-y0+1;unsigned area=(unsigned)queue.size();
  if(area<60||area>2500||bw<9||bh<9||bw>64||bh>64||std::max(bw,bh)>1.5*std::min(bw,bh)||area<.55*bw*bh||area>.94*bw*bh)continue;
  seeds.push_back({sx/area,sy/area,(bw+bh)*.25,area});
 }
 std::sort(seeds.begin(),seeds.end(),[](const Seed&a,const Seed&b){return a.area>b.area;});if(seeds.size()>6)seeds.resize(6);
 static const auto directions=[](){std::array<std::array<double,2>,32>d={};for(unsigned i=0;i<32;++i){double a=i*6.283185307179586/32;d[i]={{std::cos(a),std::sin(a)}};}return d;}();
 const auto rim=[&](double cx,double cy,double radius){
  double best=0;
  for(double scale:{.95,1.05,1.15}){double r=radius*scale;if(cx-r*1.21<0||cx+r*1.21>=w||cy-r*1.21<0||cy+r*1.21>=h)continue;
   unsigned edges=0,neutral=0;double contrast=0;
   for(const auto&v:directions){
    const auto sample=[&](double rr){int x=(int)std::lround(cx+v[0]*rr),y=(int)std::lround(cy+v[1]*rr);return rgb+y*stride+x*3;};
    const auto*a=sample(r*.75),*b=sample(r),*c=sample(r*1.2);
    double delta=(luminance(a)+luminance(c))*.5-luminance(b);contrast+=delta;edges+=delta>=6.;
    int hi=std::max(a[0],std::max(a[1],a[2])),lo=std::min(a[0],std::min(a[1],a[2]));neutral+=hi<=120&&hi-lo<=45;
   }
   if(edges>=18&&neutral>=24&&contrast>=32*4.5)best=std::max(best,edges/32.);
  }
  return best;
 };
 for(const auto&s:seeds){Box best;double merit=0;
  for(int vertical=0;vertical<2;++vertical)for(int active=0;active<3;++active)
   for(double ratio:{2.2,2.4,2.6,2.8})for(double tilt:{0.,-.06,.06,-.12,.12}){
    double cx[3],cy[3];for(int j=0;j<3;++j){double d=(j-active)*s.r*ratio;cx[j]=s.x+(vertical?d*tilt:d);cy[j]=s.y+(vertical?d:d*tilt);}
    double x0=*std::min_element(cx,cx+3)-1.35*s.r,y0=*std::min_element(cy,cy+3)-1.35*s.r;
    Box b((float)x0,(float)y0,(float)(*std::max_element(cx,cx+3)+1.35*s.r-x0),(float)(*std::max_element(cy,cy+3)+1.35*s.r-y0),.65f);
    if(b.x<8||b.y<2||b.x+b.w>312||b.y+b.h>238||b.y+b.h*.5>=h*.88||!signal_housing(rgb,w,h,stride,b))continue;
    double score=1.;for(int j=0;j<3&&score>0;++j)if(j!=active)score=std::min(score,rim(cx[j],cy[j],s.r));
    if(score>merit){merit=score;b.score=(float)(.45+.3*score);best=b;}
   }
  if(merit){bool duplicate=false;for(const auto&b:out)if(overlap(best,b)>.4f)duplicate=true;if(!duplicate)out.push_back(best);}
 }
 return out;
}
}
#endif
