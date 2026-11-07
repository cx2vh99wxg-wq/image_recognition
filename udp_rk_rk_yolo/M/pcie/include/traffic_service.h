#ifndef TRAFFIC_SERVICE_H
#define TRAFFIC_SERVICE_H
#include "traffic_vision.h"
#include <memory>
namespace m_lane
{
class FrontRadialPreview;
}
namespace traffic
{
/* CPU 颜色跟踪使用独立的最新帧邮箱；只有既有车道工作线程调用的
 * after_lane() 可以提交检测器推理。 */
class Service : public NpuTask
{
    struct Impl;
    std::unique_ptr<Impl> impl_;

  public:
    // The caller keeps correction alive until this service has stopped.
    explicit Service(const char *model_path, InputView view = InputView(),
                     const m_lane::FrontRadialPreview *correction = nullptr);
    ~Service();
    Service(const Service &) = delete;
    Service &operator=(const Service &) = delete;
    void submit_raw(const uint8_t *rgb, int width, int height, uint64_t source, uint64_t captured);
    void after_lane(const LaneBudget &lane) override;
};
}
#endif
