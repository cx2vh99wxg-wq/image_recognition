#include "lane_curve_geometry.h"
#include "ground_projection.h"
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
int main(int argc, char **argv){
    const auto radial=m_lane::FrontRadial::from_text("-0.1673");
    for (m_lane::ImagePoint source : {m_lane::ImagePoint{0,0}, {159.5,119.5}, {319,239}, {41,185}})
    {
        auto point=source;
        assert(radial.undistort(point,320,240));
        assert(radial.distort(point,320,240));
        assert(std::hypot(point.x-source.x,point.y-source.y)<1e-9);
    }
    double original_error=0, corrected_error=0;
    // Measurements from the approved 320x240 still, independent of the board.
    m_lane::ImagePoint edge[]={{80,31.26},{125,28.38},{155,27.58},{200,29.34},{245,33.14}};
    auto a=edge[0], b=edge[4], ca=a, cb=b;
    assert(radial.undistort(ca,320,240)&&radial.undistort(cb,320,240));
    for (auto point : edge)
    {
        original_error+=std::abs(point.y-a.y-(point.x-a.x)*(b.y-a.y)/(b.x-a.x));
        assert(radial.undistort(point,320,240));
        corrected_error+=std::abs(point.y-ca.y-(point.x-ca.x)*(cb.y-ca.y)/(cb.x-ca.x));
    }
    assert(corrected_error < original_error * .30);
    assert(!m_lane::FrontRadial::from_text("garbage").valid());
    assert(!m_lane::FrontRadial::from_text("nan").valid());
    auto straight=corridor(0);
    assert(m_lane::estimate_ego_curve(straight).type==0&&m_lane::estimate_ego_curve(straight).confidence>=60);
    auto radial_heading=m_lane::estimate_ego_curve_mapped(straight,[&](m_lane::ImagePoint &point){
        return radial.undistort(point,straight.width,straight.height);
    });
    assert(radial_heading.type==0&&radial_heading.confidence>=60);
    assert(!m_lane::estimate_ego_curve_mapped(straight,[](m_lane::ImagePoint &){ return false; }).confidence);
    auto cropped=m_lane::estimate_ego_curve_mapped(straight,[](m_lane::ImagePoint &point){
        return point.y>=155&&point.y<=205;
    },true);
    assert(cropped.type==0&&cropped.confidence==75&&std::abs(cropped.span_px-50)<1e-5);
    assert(!m_lane::estimate_ego_curve_mapped(straight,[](m_lane::ImagePoint &point){
        return point.y<170||point.y>185;
    },true).confidence);
    assert(!m_lane::estimate_ego_curve_mapped(straight,[](m_lane::ImagePoint &point){
        return point.y>=170&&point.y<=184;
    },true).confidence);
    const m_lane::GroundProjection ground(".30,.40,.70,.40,.90,.95,.10,.95,2");
    assert(ground.valid()&&ground.enabled());
    assert(!m_lane::GroundProjection(".3,.4,.7,.4,.1,.95,.9,.95,2").valid());
    assert(!m_lane::GroundProjection().enabled());
    assert(!m_lane::GroundProjection::for_camera("", "unused").valid());
    if (argc == 2)
    {
        const auto from_file = m_lane::GroundProjection::for_camera(nullptr, argv[1]);
        assert(from_file.valid() && from_file.enabled() && from_file.aspect() == 2.);
    }
    m_lane::ImagePoint outside{159.5, 0.};
    assert(!ground.ground_at(outside, 320, 240));
    for (m_lane::ImagePoint unit : {m_lane::ImagePoint{0,0}, {1,0}, {1,1}, {0,1}, {.3,.7}})
    {
        auto image=unit;
        assert(ground.image_at(image,320,240));
        assert(radial.distort(image,320,240));
        assert(radial.undistort(image,320,240));
        assert(ground.ground_at(image,320,240));
        assert(std::abs(image.x-(unit.x-.5)*160)<1e-8);
        assert(std::abs(image.y-unit.y*320)<1e-8);
    }
    for (double slope : {0., -.25, .25})
    {
        auto scene=straight;
        for (int side=0;side<2;++side)
        {
            const double offset=side ? .67 : .33;
            m_lane::ImagePoint far{offset+slope*.8,.10}, near{offset-slope*.8,.90};
            assert(ground.image_at(far,320,240)&&ground.image_at(near,320,240));
            const double s=(near.x-far.x)/(near.y-far.y);
            auto &edge=side ? scene.right : scene.left;
            edge.first_y=(unsigned)std::ceil(far.y);
            edge.last_y=(unsigned)std::floor(near.y);
            edge.fit_a=far.x+s*(240*.95-far.y);
            edge.fit_b=s*240*.52;
        }
        auto bird=m_lane::estimate_ego_curve_mapped(scene,[&](m_lane::ImagePoint &point){
            return ground.ground_at(point,320,240);
        },true);
        assert(bird.confidence>=60&&bird.type==(slope<0?1u:slope>0?2u:0u));
        assert(bird.angle==(slope?14u:0u));
    }
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
    assert(confirm.update(raw,2,1300000000).confidence>=60);
    assert(confirm.update(raw,3,1600000000).confidence>=60);
    assert(!confirm.update(m_lane::estimate_ego_curve(right),4,1900000000).confidence);
    assert(confirm.update(m_lane::estimate_ego_curve(right),5,2200000000).confidence>=60);
    assert(confirm.update(m_lane::estimate_ego_curve(right),6,2500000000).confidence>=60);
    assert(!confirm.update(raw,7,3200000000).confidence); // source gap resets evidence
    assert(!confirm.update(raw,8,3100000000).confidence); // backwards source time
    // Compare signed, unrounded predictions with an inclusive 1.5-degree limit.
    m_lane::EgoCurveConfirm pair;
    auto prediction = [](double degrees) {
        m_lane::EgoCurve p;
        p.confidence=85; p.signed_angle_deg=degrees;
        p.type=degrees<0?1u:degrees>0?2u:0u;
        p.angle=(unsigned)std::lround(std::abs(degrees));
        return p;
    };
    assert(!pair.update(prediction(5),1,1000000000).confidence);
    assert(pair.update(prediction(6.5),2,1200000000).confidence==85);
    assert(!pair.update(prediction(8.01),3,1400000000).confidence);
    assert(pair.update(prediction(8.2),4,1600000000).confidence==85);
    assert(!pair.update(prediction(-8.2),5,1800000000).confidence); // signs matter
    assert(pair.update(prediction(-8.3),6,2000000000).confidence==85);
    auto invalid=prediction(-8.3);invalid.confidence=0;
    assert(!pair.update(invalid,7,2200000000).confidence);
    assert(!pair.update(prediction(-8.3),8,2400000000).confidence);
    assert(pair.update(prediction(-8.3),9,2600000000).confidence==85);
    assert(!pair.update(prediction(0),10,2800000000).confidence);
    assert(pair.update(prediction(0),11,3000000000).confidence==85);
    assert(pair.update(prediction(.8),12,3200000000).confidence==85); // no same-category rule
    assert(pair.update(prediction(-.7),13,3400000000).confidence==85);
    assert(!pair.update(prediction(-.7),14,4100000000).confidence); // >500ms resets
    assert(pair.update(prediction(-.7),15,4300000000).confidence==85);
    puts("PASS ego curve geometry: curvature gate, signed sub-degree adjacent pairs, 1.5 threshold, duplicates, invalid data and expiry");
}
