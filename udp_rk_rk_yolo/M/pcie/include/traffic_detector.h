#ifndef TRAFFIC_DETECTOR_H
#define TRAFFIC_DETECTOR_H
#include "traffic_vision.h"
#include <memory>
#include <string>
namespace traffic
{
struct PersonResult { Box best; unsigned count = 0; }; // threshold-passing person anchors (not a head count)
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
                PersonResult *persons = nullptr);
    int input_size() const;
    unsigned last_passes() const;
};
}
#endif
