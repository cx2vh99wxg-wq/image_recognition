#ifndef M_FRONT_ACTOR_PROJECTION_H
#define M_FRONT_ACTOR_PROJECTION_H

#include "ground_projection.h"
#include "person_road_association.h"
#include "../../../common/perception/include/display_people.h"

namespace traffic
{
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
