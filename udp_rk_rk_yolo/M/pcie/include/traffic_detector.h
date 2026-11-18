#ifndef TRAFFIC_DETECTOR_H
#define TRAFFIC_DETECTOR_H
#include "traffic_vision.h"
#include <memory>
#include <string>
namespace traffic
{
struct PersonResult {
    Box best;
    unsigned count = 0;
    std::vector<Box> candidates; // all threshold-passing anchors, not a head count
};
struct VehicleDetection {
    unsigned coco_class;
    Box box;
};
inline bool seated_on_cycle(const Box &person, const Box &cycle)
{
    const float center = person.x + person.w * .5f;
    const float foot = person.y + person.h;
    const float shared = std::min(person.x + person.w, cycle.x + cycle.w) -
                         std::max(person.x, cycle.x);
    return shared >= .65f * person.w &&
           center >= cycle.x + .12f * cycle.w && center <= cycle.x + .88f * cycle.w &&
           person.y < cycle.y + .22f * cycle.h &&
           foot >= cycle.y + .18f * cycle.h && foot <= cycle.y + 1.12f * cycle.h;
}
struct DetectorTiming {
    uint64_t prepare=0, inputs=0, run=0, outputs=0, decode=0, release=0, filter=0;
};
struct DetectorSchedule {
    unsigned passes=0, view_cursor=0, locked_view=0, hits=0;
    DetectorTiming timing;
};
class Detector
{
    struct Impl;
    std::unique_ptr<Impl> impl_;

  public:
    Detector();
    ~Detector();
    Detector(const Detector &) = delete;
    Detector &operator=(const Detector &) = delete;
    bool init(const char *path, std::string *error = nullptr);
    bool detect(const uint8_t *rgb, int w, int h, int stride, std::vector<Box> &boxes,
                std::string *error = nullptr, unsigned budget_ms = default_budget_ms,
                PersonResult *persons = nullptr, bool full_view = false,
                std::vector<Box> *signal_hints = nullptr,
                std::vector<VehicleDetection> *vehicles = nullptr);
    int input_size() const;
    unsigned last_passes() const;
    DetectorTiming last_timing() const;
    DetectorSchedule schedule() const;
    void restore_schedule(const DetectorSchedule &state) noexcept;
};
}
#endif
