#include "lane_curve_geometry.h"
#include <cassert>
#include <cmath>
#include <cstdio>
static YOLOLineStyleResult corridor(float heading) {
    YOLOLineStyleResult r={};r.width=320;r.height=240;r.road_area=6000;r.object_count=2;
    r.left.fit_a=70;r.right.fit_a=250;
    r.left.fit_b=-85+heading;r.right.fit_b=85+heading;
    r.left.first_y=r.right.first_y=140;r.left.last_y=r.right.last_y=220;
    return r;
}
int main(){
    auto straight=corridor(0);
    assert(m_lane::estimate_ego_curve(straight).type==0&&m_lane::estimate_ego_curve(straight).confidence>=60);
    // Check both sides of 40 degrees before rounding the transmitted angle.
    constexpr float radians_per_degree=.017453292519943295f;
    const float below=240.f*.52f*std::tan(39.9f*radians_per_degree);
    const float above=240.f*.52f*std::tan(40.1f*radians_per_degree);
    for(float heading:{below,-below}){
        auto low=m_lane::estimate_ego_curve(corridor(heading));
        assert(low.type==0&&low.confidence>=60&&low.angle==0);
    }
    auto gentle=corridor(-20);
    gentle.left.fit_c=gentle.right.fit_c=9; // midpoint curvature is about 0.00108 px^-1
    auto gentle_result=m_lane::estimate_ego_curve(gentle);
    assert(gentle_result.type==0&&gentle_result.confidence>=60&&gentle_result.angle==0);
    auto above_curvature_limit=gentle;
    above_curvature_limit.left.fit_c=above_curvature_limit.right.fit_c=12; // about 0.00142 px^-1
    auto moderate=m_lane::estimate_ego_curve(above_curvature_limit);
    assert(moderate.type==2&&moderate.confidence>=60&&moderate.angle==13);
    auto left=corridor(above);auto right=corridor(-above);
    auto l=m_lane::estimate_ego_curve(left);auto r=m_lane::estimate_ego_curve(right);
    assert(l.type==1&&l.confidence>=60&&l.angle==40);
    assert(r.type==2&&r.confidence>=60&&r.angle==40);
    auto translated=right;translated.left.fit_a+=20;translated.right.fit_a+=20;
    assert(m_lane::estimate_ego_curve(translated).angle==r.angle);
    // The shared 140..220 interval has midpoint y=180, where this curve gives 14 degrees.
    auto bent=corridor(16);
    bent.left.fit_c=bent.right.fit_c=60;
    auto full=m_lane::estimate_ego_curve(bent);
    assert(full.type==2&&full.angle==14);
    // Coefficients from the saved, independently viewed straight-road frame.
    auto real_straight=straight;real_straight.road_area=12108;real_straight.object_count=3;
    real_straight.left.fit_a=-23.535509f;real_straight.left.fit_b=-158.304489f;
    real_straight.right.fit_a=266.366333f;real_straight.right.fit_b=144.099197f;
    real_straight.left.first_y=116;real_straight.left.last_y=183;
    real_straight.right.first_y=115;real_straight.right.last_y=181;
    auto observed=m_lane::estimate_ego_curve(real_straight);
    assert(observed.type==0&&observed.confidence>=60&&std::abs(observed.shift_px-3.6993f)<.01f);
    auto bad=left;bad.left.last_y=bad.left.first_y;assert(!m_lane::estimate_ego_curve(bad).confidence);
    bad=left;bad.left.last_y=bad.right.last_y=150;assert(!m_lane::estimate_ego_curve(bad).confidence);
    bad=left;bad.right.fit_a=bad.left.fit_a;bad.right.fit_b=bad.left.fit_b;assert(!m_lane::estimate_ego_curve(bad).confidence);
    bad=left;bad.left.fit_c=NAN;assert(!m_lane::estimate_ego_curve(bad).confidence);
    m_lane::EgoCurveConfirm confirm;auto raw=m_lane::estimate_ego_curve(left);
    assert(!confirm.update(raw,1,1000000000).confidence);
    assert(!confirm.update(raw,1,1000000000).confidence); // duplicates never confirm
    assert(!confirm.update(raw,2,1300000000).confidence);
    assert(confirm.update(raw,3,1600000000).confidence>=60);
    assert(!confirm.update(m_lane::estimate_ego_curve(right),4,1900000000).confidence);
    assert(!confirm.update(m_lane::estimate_ego_curve(right),5,2200000000).confidence);
    assert(confirm.update(m_lane::estimate_ego_curve(right),6,2500000000).confidence>=60);
    assert(!confirm.update(raw,7,3200000000).confidence); // source gap resets evidence
    assert(!confirm.update(raw,8,3100000000).confidence); // backwards source time
    puts("PASS ego curve geometry: curvature gate, signed heading, geometry rejection, unique-source confirmation and expiry");
}
