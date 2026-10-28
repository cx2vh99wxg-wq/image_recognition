#ifndef TRAFFIC_DETECTOR_H
#define TRAFFIC_DETECTOR_H
#include "traffic_vision.h"
#include <memory>
#include <string>
namespace traffic {
class Detector {
    struct Impl;std::unique_ptr<Impl> impl_;
public:
    Detector();~Detector();Detector(const Detector&)=delete;Detector& operator=(const Detector&)=delete;
    bool init(const char* path,std::string* error=nullptr);
    bool detect(const uint8_t* rgb,int w,int h,int stride,std::vector<Box>& boxes,std::string* error=nullptr);
    int input_size()const;
    unsigned last_passes()const;
};
}
#endif
