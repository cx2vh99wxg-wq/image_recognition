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
    auto straight=corridor(0);auto left=corridor(50);auto right=corridor(-50);
    assert(m_lane::estimate_ego_curve(straight).type==0&&m_lane::estimate_ego_curve(straight).confidence>=60);
    assert(m_lane::estimate_ego_curve(left).type==1&&m_lane::estimate_ego_curve(left).confidence>=60);
    assert(m_lane::estimate_ego_curve(right).type==2&&m_lane::estimate_ego_curve(right).confidence>=60);
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
    puts("PASS ego curve geometry: paired boundaries, signed heading, absent/short/crossing/NaN rejection, unique-source confirmation and expiry");
}
