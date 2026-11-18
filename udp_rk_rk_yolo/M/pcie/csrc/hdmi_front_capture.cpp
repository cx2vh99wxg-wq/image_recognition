#include "hdmi_front_capture.h"

#include <algorithm>
#include <cstring>

namespace hdmi_front
{

ContentRect fitted_content(unsigned width, unsigned height)
{
    if (!width || !height || width > 4096 || height > 2160)
        return {};
    const int w = std::min<unsigned>(WIDTH, HEIGHT * width / height);
    const int h = std::min<unsigned>(HEIGHT, WIDTH * height / width);
    return {(WIDTH - w) / 2, (HEIGHT - h) / 2, w, h};
}

bool convert_yuyv(const uint8_t *input, size_t bytes, unsigned width, unsigned height,
                  unsigned stride, bool full_range, bool bt709, uint8_t *bgr)
{
    if (!input || !bgr || width < 2 || (width & 1) || !height || width > 4096 || height > 2160 ||
        stride < width * 2 || uint64_t(stride) * (height - 1) + width * 2 > bytes)
        return false;

    const auto content = fitted_content(width, height);
    const unsigned fit_w = content.width, fit_h = content.height;
    if (fit_w < 2 || fit_h < 2)
        return false;
    std::memset(bgr, 0, WIDTH * HEIGHT * 3);
    const YuvMatrix matrix(full_range,bt709);
    for (unsigned y = 0; y < fit_h; ++y)
    {
        const uint8_t *row = input + size_t((2 * y + 1) * height / (2 * fit_h)) * stride;
        uint8_t *out = bgr + ((content.y + y) * WIDTH + content.x) * 3;
        for (unsigned x = 0; x < fit_w; ++x)
        {
            const unsigned sx = (2 * x + 1) * width / (2 * fit_w);
            const uint8_t *pair = row + (sx & ~1u) * 2;
            matrix.decode(pair,sx&1,out[2],out[1],out[0]);
            out+=3;
        }
    }
    return true;
}

void apply_bgr(const uint8_t *tile, uint8_t *mosaic, int mosaic_width)
{
    for (int y = 0; y < HEIGHT; ++y)
        std::memcpy(mosaic + y * mosaic_width * 3, tile + y * WIDTH * 3, WIDTH * 3);
}

}

#ifdef __linux__
#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

namespace hdmi_front
{
namespace
{
int query(int fd, unsigned long command, void *argument)
{
    int result;
    do {
        result = ioctl(fd, command, argument);
    } while (result < 0 && errno == EINTR);
    return result;
}
}

Capture::Capture(const char *device) : device_(device ? device : "")
{
    if (enabled())
        tile_.resize(WIDTH * HEIGHT * 3);
}

Capture::~Capture() { close_device(); }

void Capture::close_device()
{
    auto type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (streaming_)
        query(fd_, VIDIOC_STREAMOFF, &type);
    for (const auto &buffer : buffers_)
        munmap(buffer.address, buffer.size);
    buffers_.clear();
    if (fd_ >= 0)
        close(fd_);
    fd_ = -1;
    streaming_ = false;
    good_frames_ = 0;
    last_frame_ns_ = 0;
    signal_check_ns_ = 0;
}

bool Capture::start()
{
    fd_ = open(device_.c_str(), O_RDWR | O_NONBLOCK | O_CLOEXEC);
    if (fd_ < 0)
        return false;
    v4l2_capability capability = {};
    if (query(fd_, VIDIOC_QUERYCAP, &capability) < 0)
        return false;
    const uint32_t caps = (capability.capabilities & V4L2_CAP_DEVICE_CAPS) ?
                          capability.device_caps : capability.capabilities;
    if (!(caps & V4L2_CAP_VIDEO_CAPTURE) || !(caps & V4L2_CAP_STREAMING))
        return false;

    v4l2_dv_timings timings = {};
    supports_timings_ = query(fd_, VIDIOC_QUERY_DV_TIMINGS, &timings) == 0;
    if (supports_timings_ && (timings.type != V4L2_DV_BT_656_1120 || timings.bt.interlaced ||
                              query(fd_, VIDIOC_S_DV_TIMINGS, &timings) < 0))
        return false;
    if (!supports_timings_ && (errno == ENOLINK || errno == ENOLCK || errno == ERANGE))
        return false;
    signal_width_ = timings.bt.width;
    signal_height_ = timings.bt.height;
    pixel_clock_ = timings.bt.pixelclock;

    v4l2_format format = {};
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    auto &pixel = format.fmt.pix;
    pixel.width = supports_timings_ ? timings.bt.width : 640;
    pixel.height = supports_timings_ ? timings.bt.height : 480;
    pixel.pixelformat = V4L2_PIX_FMT_YUYV;
    pixel.field = V4L2_FIELD_NONE;
    if (query(fd_, VIDIOC_S_FMT, &format) < 0 || pixel.pixelformat != V4L2_PIX_FMT_YUYV ||
        pixel.width < 2 || pixel.width > 4096 || (pixel.width & 1) ||
        !pixel.height || pixel.height > 2160 || pixel.bytesperline < pixel.width * 2 ||
        pixel.field != V4L2_FIELD_NONE)
        return false;
    width_ = pixel.width;
    height_ = pixel.height;
    stride_ = pixel.bytesperline;
    full_range_ = pixel.quantization == V4L2_QUANTIZATION_FULL_RANGE ||
                  (pixel.quantization == V4L2_QUANTIZATION_DEFAULT && pixel.colorspace == V4L2_COLORSPACE_JPEG);
    bt709_ = pixel.ycbcr_enc == V4L2_YCBCR_ENC_709 ||
             (pixel.ycbcr_enc == V4L2_YCBCR_ENC_DEFAULT && pixel.colorspace == V4L2_COLORSPACE_REC709);

    v4l2_streamparm rate = {};
    rate.type = format.type;
    rate.parm.capture.timeperframe.numerator = 1;
    rate.parm.capture.timeperframe.denominator = 30;
    if (!supports_timings_)
        query(fd_, VIDIOC_S_PARM, &rate);

    v4l2_requestbuffers request = {};
    request.type = format.type;
    request.memory = V4L2_MEMORY_MMAP;
    request.count = 4;
    if (query(fd_, VIDIOC_REQBUFS, &request) < 0 || request.count < 2 || request.count > 32)
        return false;
    buffers_.reserve(request.count);
    for (unsigned i = 0; i < request.count; ++i)
    {
        v4l2_buffer buffer = {};
        buffer.type = format.type;
        buffer.memory = request.memory;
        buffer.index = i;
        if (query(fd_, VIDIOC_QUERYBUF, &buffer) < 0)
            return false;
        void *address = mmap(nullptr, buffer.length, PROT_READ, MAP_SHARED, fd_, buffer.m.offset);
        if (address == MAP_FAILED)
            return false;
        buffers_.push_back({address, buffer.length});
        if (query(fd_, VIDIOC_QBUF, &buffer) < 0)
            return false;
    }
    auto type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (query(fd_, VIDIOC_STREAMON, &type) < 0)
        return false;
    streaming_ = true;
    std::fprintf(stderr, "[HDMI] %s streaming %ux%u YUYV, timing probe=%d\n",
                 device_.c_str(), width_, height_, supports_timings_);
    return true;
}

bool Capture::signal_present()
{
    if (supports_timings_)
    {
        v4l2_dv_timings timings = {};
        return query(fd_, VIDIOC_QUERY_DV_TIMINGS, &timings) == 0 &&
               timings.type == V4L2_DV_BT_656_1120 && !timings.bt.interlaced &&
               timings.bt.width == signal_width_ && timings.bt.height == signal_height_ &&
               timings.bt.pixelclock == pixel_clock_;
    }
    v4l2_input input = {};
    if (query(fd_, VIDIOC_G_INPUT, &input.index) < 0 || query(fd_, VIDIOC_ENUMINPUT, &input) < 0)
        return true;
    return !(input.status & (V4L2_IN_ST_NO_SIGNAL | V4L2_IN_ST_NO_H_LOCK |
                             V4L2_IN_ST_NO_V_LOCK | V4L2_IN_ST_NO_SYNC));
}

const uint8_t *Capture::latest(uint64_t now, uint64_t &captured)
{
    captured = 0;
    if (!enabled())
        return nullptr;
    if (fd_ < 0)
    {
        if (now < retry_ns_)
            return nullptr;
        const bool first_open = !retry_ns_;
        retry_ns_ = now + 1000000000ULL;
        if (!start())
        {
            if (first_open)
                std::fprintf(stderr, "[HDMI] %s unavailable or unsupported, using camera\n",
                             device_.c_str());
            close_device();
            return nullptr;
        }
        opened_ns_ = now;
    }
    if (now >= signal_check_ns_)
    {
        signal_check_ns_ = now + 100000000ULL;
        if (!signal_present())
        {
            close_device();
            return nullptr;
        }
    }

    v4l2_buffer latest = {};
    bool have_latest = false;
    for (size_t i = 0; i < buffers_.size(); ++i)
    {
        v4l2_buffer next = {};
        next.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        next.memory = V4L2_MEMORY_MMAP;
        if (query(fd_, VIDIOC_DQBUF, &next) < 0)
        {
            if (errno == EAGAIN)
                break;
            close_device();
            return nullptr;
        }
        if (next.index >= buffers_.size() ||
            (have_latest && query(fd_, VIDIOC_QBUF, &latest) < 0))
        {
            close_device();
            return nullptr;
        }
        latest = next;
        have_latest = true;
    }
    if (have_latest)
    {
        const auto &buffer = buffers_[latest.index];
        uint64_t timestamp = now;
        if ((latest.flags & V4L2_BUF_FLAG_TIMESTAMP_MASK) == V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC)
        {
            timestamp = uint64_t(latest.timestamp.tv_sec) * 1000000000ULL +
                        uint64_t(latest.timestamp.tv_usec) * 1000ULL;
            timestamp = std::min(now, timestamp);
        }
        if (timestamp > last_frame_ns_ && now - timestamp < TIMEOUT_NS &&
            !(latest.flags & V4L2_BUF_FLAG_ERROR) && latest.bytesused <= buffer.size &&
            convert_yuyv(static_cast<const uint8_t *>(buffer.address), latest.bytesused,
                         width_, height_, stride_, full_range_, bt709_, tile_.data()))
        {
            if (!last_frame_ns_ || timestamp - last_frame_ns_ >= TIMEOUT_NS)
                good_frames_ = 0;
            good_frames_ = std::min(2u, good_frames_ + 1);
            last_frame_ns_ = timestamp;
            detail_.capture(static_cast<const uint8_t *>(buffer.address),width_,height_,stride_,
                            full_range_,bt709_);
        }
        if (query(fd_, VIDIOC_QBUF, &latest) < 0)
        {
            close_device();
            return nullptr;
        }
    }
    if (now - (last_frame_ns_ ? last_frame_ns_ : opened_ns_) >= TIMEOUT_NS)
    {
        close_device();
        return nullptr;
    }
    if (good_frames_ < 2)
        return nullptr;
    captured = last_frame_ns_;
    return tile_.data();
}
}
#endif
