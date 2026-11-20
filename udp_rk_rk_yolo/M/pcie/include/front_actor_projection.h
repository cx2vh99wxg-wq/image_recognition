#ifndef M_FRONT_ACTOR_PROJECTION_H
#define M_FRONT_ACTOR_PROJECTION_H

#include "ground_projection.h"
#include "vehicle_lane_position.h"
#include "traffic_detector.h"
#include "person_road_association.h"
#include "../../../common/perception/include/display_people.h"
#include "../../../common/perception/include/display_vehicles.h"

namespace traffic
{
inline bool boundary_at_foot(const std::vector<m_lane::LaneRowPoint> &points, int y, double &x)
{
    if (points.size() < 2) return false;
    auto hi = std::lower_bound(points.begin(), points.end(), y,
        [](const m_lane::LaneRowPoint &a, int row) { return a.y < row; });
    if (hi != points.end() && hi->y == y) { x = hi->x; return true; }
    // Interpolate only across a short break in the marking. Farther extrapolation
    // could put a car into an adjoining lane on a curve or behind an occlusion.
    if (hi == points.begin() || hi == points.end() || hi->y - (hi-1)->y > 16) return false;
    const auto &lo = *(hi-1);
    x = lo.x + (hi->x-lo.x)*(y-lo.y)/(hi->y-lo.y);
    return true;
}

inline bool set_lane_position(display_people::Person &actor, const m_lane::PointLanePair &lanes,
                              const std::array<m_lane::MarkingFit,2> *fit=nullptr)
{
    const int y = std::min(239u, actor.y + actor.h);
    double left, right;
    const bool measured=boundary_at_foot(lanes.left,y,left) && boundary_at_foot(lanes.right,y,right) &&
                        right-left>=18;
    if (!measured && (!fit || !m_lane::fit_at_foot(*fit,y,left,right))) return false;
    const double u = (actor.x + actor.w*.5 - left)/(right-left);
    if (u < -2.5 || u > 3.5) return false;
    actor.gx = int32_t(std::lround(u*1000));
    actor.flags |= display_vehicles::LaneRelative;
    return true;
}

inline bool close_car_in_path(const display_people::Person &actor)
{
    // Close 320x240 front-camera reference: a large car filling the lower field.
    if (actor.quality < 25 || actor.w < 96 || actor.h < 48 || actor.y + actor.h < 174)
        return false;
    if (actor.flags & display_vehicles::LaneRelative)
        return actor.gx >= 0 && actor.gx <= 1000;
    if (actor.flags & display_people::OffRoad)
        return false;
    // A close bumper can hide both lines. Require a centered box that
    // visibly covers the entire central travel band in that case.
    const unsigned center_twice = actor.x * 2 + actor.w;
    return actor.w >= 112 && center_twice >= 256 && center_twice <= 384 &&
           actor.x <= 144 && actor.x + actor.w >= 176;
}

inline bool is_visible_rider(const Box &p, const std::vector<VehicleDetection> &vehicles)
{
    for (const auto &v : vehicles) {
        if (v.coco_class != 1 && v.coco_class != 3) continue;
        if (seated_on_cycle(p, v.box))
            return true;
    }
    return false;
}

inline display_people::Person front_actor(const Box &box, FootRelation relation,
                                         const m_lane::GroundProjection &ground, double scale)
{
    auto actor = display_people::box(display_people::Front, int(box.x), int(box.y),
                                    int(std::ceil(box.w)), int(std::ceil(box.h)), box.score);
    if (relation == FootRelation::Inside)
        actor.flags |= display_people::InRoad;
    else if (relation == FootRelation::Outside)
        actor.flags |= display_people::OffRoad;

    // Detector coordinates refer to the resized, center-cropped image.
    // The homography was calibrated in the uncropped undistorted image.
    const double foot_x = std::min(319., double(box.x + box.w * .5f));
    const double foot_y = std::min(239., double(box.y + box.h));
    m_lane::ImagePoint foot = {159.5 + (foot_x - 159.5) / scale,
                               119.5 + (foot_y - 119.5) / scale};
    if (ground.display_ground_at(foot, 320, 240)) {
        actor.flags |= display_people::Ground;
        actor.gx = int32_t(std::lround(foot.x * 1000.));
        actor.gy = int32_t(std::lround(foot.y * 1000.));
    }
    return actor;
}

inline void add_front_person(display_people::Snapshot &scene, const display_people::Person &actor)
{
    // The input is confidence sorted. Adjacent anchors on the same person
    // can differ in box size but still agree on the feet.
    for (unsigned i = 0; i < scene.count; ++i) {
        const auto &accepted = scene.people[i];
        const double dx = (actor.x + actor.w * .5) - (accepted.x + accepted.w * .5);
        const double dy = double(actor.y + actor.h) - (accepted.y + accepted.h);
        if (std::abs(dx) <= std::max(3., std::min(actor.w, accepted.w) * .24) &&
            std::abs(dy) <= std::max(4., std::min(actor.h, accepted.h) * .16) &&
            display_people::overlap(actor, accepted) > .20f)
            return;
    }
    display_people::add(scene, actor);
}
}
#endif
