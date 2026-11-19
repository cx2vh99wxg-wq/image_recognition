#ifndef PERCEPTION_RIDER_GEOMETRY_H
#define PERCEPTION_RIDER_GEOMETRY_H
#include <algorithm>
#include <cmath>
namespace rider_geometry {
template<class PersonBox, class CycleBox>
inline bool seated(const PersonBox &p, const CycleBox &b)
{
    const double cx = p.x + p.w * .5, foot = p.y + p.h;
    const double shared = std::min<double>(p.x + p.w, b.x + b.w) - std::max<double>(p.x, b.x);
    const bool aligned_feet=std::abs(foot-(b.y+b.h))<=.20*b.h;
    const bool lower_wheel_box=shared>=.80*b.w && p.w<=1.95*b.w && p.h>=1.30*b.h;
    const double inset=lower_wheel_box && aligned_feet ? 0. : .10;
    return (shared>=.65*p.w || lower_wheel_box) &&
           cx>=b.x+inset*b.w && cx<=b.x+(1.-inset)*b.w &&
           p.y < b.y + .22 * b.h && foot >= b.y + .18 * b.h && foot <= b.y + 1.12 * b.h;
}
}
#endif
