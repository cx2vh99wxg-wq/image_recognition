#ifndef S_VEHICLE_LAYER_STATE_H
#define S_VEHICLE_LAYER_STATE_H
#include "../../../include/shared_memory.h"
#include "pedestrian_scene.h"
#include "crosswalk_scene.h"
#include "vehicle_scene.h"
// Pixel cache key only. Raw detection/control metadata keeps its exact values.
struct VehicleLayerState
{
    const uint32_t *artwork;
    uint64_t artwork_revision;
    int person, front_person, box_left, box_top, box_right, box_bottom, blink;
    CurveType curve;
    uint32_t confidence;
    int signed_angle;
    unsigned left_style, right_style, blocked;
    bool warning_red;
    bool front_stop;
    pedestrian_scene::Actors actors;
    vehicle_scene::Geometry vehicles;
    crosswalk_display::Geometry crosswalk;
    unsigned warning_views;
    bool operator==(const VehicleLayerState &other) const
    {
        return artwork == other.artwork && artwork_revision == other.artwork_revision &&
               person == other.person && front_person == other.front_person && box_left == other.box_left && box_top == other.box_top &&
               box_right == other.box_right && box_bottom == other.box_bottom &&
               blink == other.blink && (confidence >= 60) == (other.confidence >= 60) &&
               signed_angle == other.signed_angle && left_style == other.left_style &&
               right_style == other.right_style && blocked == other.blocked &&
               warning_red == other.warning_red && front_stop == other.front_stop && warning_views == other.warning_views &&
               actors == other.actors && vehicles == other.vehicles && crosswalk == other.crosswalk;
    }
};
#endif
