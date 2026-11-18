#ifndef M_HDMI_FRONT_CAPTURE_H
#define M_HDMI_FRONT_CAPTURE_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include "hdmi_signal_detail.h"

namespace hdmi_front
{
constexpr int WIDTH = 320;
constexpr int HEIGHT = 240;
constexpr uint64_t TIMEOUT_NS = 500000000ULL;

struct ContentRect
{
    int x, y, width, height;
};
ContentRect fitted_content(unsigned width, unsigned height);
bool convert_yuyv(const uint8_t *input, size_t bytes, unsigned width, unsigned height,
                  unsigned stride, bool full_range, bool bt709, uint8_t *bgr);
void apply_bgr(const uint8_t *tile, uint8_t *mosaic, int mosaic_width);

class Capture
{
  public:
    explicit Capture(const char *device);
    ~Capture();
    Capture(const Capture &) = delete;
    Capture &operator=(const Capture &) = delete;

    bool enabled() const { return !device_.empty(); }
    ContentRect content_rect() const { return fitted_content(width_, height_); }
    const uint8_t *latest(uint64_t now, uint64_t &captured);
    SignalDetail &signal_detail() { return detail_; }

  private:
    struct Buffer
    {
        void *address;
        size_t size;
    };

    bool start();
    void close_device();
    bool signal_present();

    std::string device_;
    int fd_ = -1;
    unsigned width_ = 0, height_ = 0, stride_ = 0, good_frames_ = 0;
    bool full_range_ = false, bt709_ = false, streaming_ = false, supports_timings_ = false;
    unsigned signal_width_ = 0, signal_height_ = 0;
    uint64_t pixel_clock_ = 0;
    uint64_t retry_ns_ = 0, signal_check_ns_ = 0, last_frame_ns_ = 0, opened_ns_ = 0;
    std::vector<Buffer> buffers_;
    std::vector<uint8_t> tile_;
    SignalDetail detail_;
};
}
#endif
