#ifndef TRAFFIC_SERVICE_H
#define TRAFFIC_SERVICE_H
#include "traffic_vision.h"
#include <memory>
namespace traffic {
/* CPU color tracking has its own latest-frame mailbox. Only after_lane(),
 * called by the existing lane worker, may execute detector inference. */
class Service:public NpuTask {
    struct Impl;std::unique_ptr<Impl> impl_;
public:
    explicit Service(const char* model_path,InputView view=InputView());~Service();
    Service(const Service&)=delete;Service& operator=(const Service&)=delete;
    void submit_raw(const uint8_t* rgb,int width,int height,uint64_t source,uint64_t captured);
    void after_lane(const LaneBudget& lane)override;
};
}
#endif
