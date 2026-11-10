#ifndef S_PEDESTRIAN_GAIT_H
#define S_PEDESTRIAN_GAIT_H
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>
namespace pedestrian_scene {
// Small CPU-generated, shaded 3D atlas. Generated once before display starts;
// a video frame only scales/blends a cached sprite, never renders geometry.
struct GaitAtlas {
 static constexpr int W=64,H=128,FRAMES=8,DIRECTIONS=8;
 std::vector<uint32_t>pixels;
 GaitAtlas():pixels(W*H*FRAMES*DIRECTIONS){
  for(int d=0;d<DIRECTIONS;++d)for(int f=0;f<FRAMES;++f)render(&pixels[(d*FRAMES+f)*W*H],d,f);
 }
 static void render(uint32_t*out,int direction,int frame){
  std::array<double,W*H>depth;depth.fill(-100.);
  const double yaw=direction*.7853981633974483,c=std::cos(yaw),s=std::sin(yaw),swing=std::sin(frame*.7853981633974483);
  auto volume=[&](double x,double y,double z,double rx,double ry,double rz){
   const double px=x*c+z*s,pz=-x*s+z*c,py=y*.96-pz*.22;
   const double ax=std::hypot(rx*c,rz*s),ay=std::hypot(ry*.96,std::hypot(rx*s,rz*c)*.22);
   const double az=std::hypot(rx*s,rz*c);
   int l=std::max(0,int(std::floor((px-ax)/.64*W+W*.5))),r=std::min(W-1,int(std::ceil((px+ax)/.64*W+W*.5)));
   int t=std::max(0,int(std::floor((1.04-py-ay)/1.10*H))),b=std::min(H-1,int(std::ceil((1.04-py+ay)/1.10*H)));
   for(int iy=t;iy<=b;++iy)for(int ix=l;ix<=r;++ix){
    const double dx=(((ix+.5)/W-.5)*.64-px)/ax,dy=(1.04-(iy+.5)/H*1.10-py)/ay,q=dx*dx+dy*dy;
    if(q>1.)continue;
    const double dz=std::sqrt(1.-q),front=-pz+az*dz;const int k=iy*W+ix;if(front<depth[k])continue;depth[k]=front;
    const double nx=dx/ax,ny=dy/ay,nz=dz/az,n=std::sqrt(nx*nx+ny*ny+nz*nz);
    const unsigned shade=unsigned(145+109*std::max(0.,(-.38*nx+.35*ny+.86*nz)/n));
    const unsigned alpha=unsigned(std::min(1.,(1.-q)*18.)*255);
    out[k]=(alpha<<24)|(shade<<16)|(shade<<8)|shade;
   }
  };
  auto limb=[&](double x1,double y1,double z1,double x2,double y2,double z2,double radius){
   for(int j=0;j<=5;++j){double t=j/5.;volume(x1+(x2-x1)*t,y1+(y2-y1)*t,z1+(z2-z1)*t,radius,radius*1.15,radius);}
  };
  volume(0,.64,0,.13,.18,.08);volume(0,.445,0,.105,.07,.07);
  volume(0,.80,0,.034,.046,.034);volume(0,.90,0,.069,.083,.066);
  volume(0,.89,.064,.020,.027,.022); // face/nose gives the walking direction a visible front
  for(int side:{-1,1}){
   const double step=swing*side,hip=side*.063,knee_z=step*.085,ankle_z=step*.16,lift=std::max(0.,step)*.045;
   limb(hip,.43,0,hip,.25+lift,knee_z,.047);
   limb(hip,.25+lift,knee_z,hip,.058+lift,ankle_z,.035);
   volume(hip,.027+lift,ankle_z+.025,.044,.026,.066);
   const double arm=side*.165;
   limb(arm,.73,0,arm,.57,-step*.065,.038);
   limb(arm,.57,-step*.065,arm,.43,-step*.14,.032);
   volume(arm,.405,-step*.14,.035,.037,.032);
  }
 }
 const uint32_t*frame(unsigned direction,unsigned phase)const{return &pixels[((direction%8)*8+phase%8)*W*H];}
};
inline const GaitAtlas& gait_atlas(){static const GaitAtlas atlas;return atlas;}
}
#endif
