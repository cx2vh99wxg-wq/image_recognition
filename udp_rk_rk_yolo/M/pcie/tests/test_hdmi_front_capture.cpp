#include "hdmi_front_capture.h"

#include <algorithm>
#include <cassert>
#include <vector>

#ifdef __linux__
#include <cerrno>
#include <cstdarg>
#include <linux/videodev2.h>
#include "../../../common/transport/include/curve_result_wire.h"
#include "../../../common/perception/include/front_person_wire.h"
#include "../../../common/perception/include/line_style_wire.h"
#include "../../../common/perception/include/traffic_light_wire.h"
#include "../../../S/include/front_person_guard.h"
#include "../../../S/include/traffic_go_rule.h"

namespace
{
uint8_t fake_buffers[2][1280 * 360];
unsigned queued_frames = 0;
bool active_signal = true;
bool report_dv = false;
bool available = true;
}

extern "C" int __wrap_open(const char *, int, ...)
{
    if (available)
        return 41;
    errno = ENODEV;
    return -1;
}
extern "C" int __wrap_close(int fd) { assert(fd == 41); return 0; }
extern "C" void *__wrap_mmap(void *, size_t length, int, int, int fd, int64_t offset)
{
    assert(fd == 41 && length == sizeof(fake_buffers[0]) && offset < 2);
    return fake_buffers[offset];
}
extern "C" int __wrap_munmap(void *, size_t) { return 0; }

extern "C" int __wrap_ioctl(int fd, unsigned long request, ...)
{
    assert(fd == 41);
    va_list args;
    va_start(args, request);
    void *data = va_arg(args, void *);
    va_end(args);
    switch (request)
    {
    case VIDIOC_QUERYCAP:
        static_cast<v4l2_capability *>(data)->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
        return 0;
    case VIDIOC_QUERY_DV_TIMINGS:
        if (report_dv && active_signal)
        {
            auto &t = static_cast<v4l2_dv_timings *>(data)->bt;
            t.width = 1280;
            t.height = 720;
            t.pixelclock = 74250000;
            return 0;
        }
        errno = report_dv ? ENOLINK : ENOTTY;
        return -1;
    case VIDIOC_S_DV_TIMINGS:
    case VIDIOC_STREAMON:
    case VIDIOC_STREAMOFF:
    case VIDIOC_S_PARM:
    case VIDIOC_G_INPUT:
    case VIDIOC_QBUF:
        return 0;
    case VIDIOC_ENUMINPUT:
        static_cast<v4l2_input *>(data)->status = active_signal ? 0 : V4L2_IN_ST_NO_SIGNAL;
        return 0;
    case VIDIOC_S_FMT:
    {
        auto &p = static_cast<v4l2_format *>(data)->fmt.pix;
        p.width = 640;
        p.height = 360;
        p.bytesperline = 1280;
        return 0;
    }
    case VIDIOC_REQBUFS:
        static_cast<v4l2_requestbuffers *>(data)->count = 2;
        return 0;
    case VIDIOC_QUERYBUF:
    {
        auto &b = *static_cast<v4l2_buffer *>(data);
        b.length = sizeof(fake_buffers[0]);
        b.m.offset = b.index;
        return 0;
    }
    case VIDIOC_DQBUF:
        if (queued_frames)
        {
            auto &b = *static_cast<v4l2_buffer *>(data);
            b.index = --queued_frames;
            b.bytesused = sizeof(fake_buffers[0]);
            return 0;
        }
        errno = EAGAIN;
        return -1;
    default:
        assert(false);
        return -1;
    }
}

static void test_capture()
{
    for (auto &buffer : fake_buffers)
        for (unsigned row=0; row<360; ++row)
            for (unsigned x=0; x<1280; x+=4)
            {
                buffer[1280*row+x]=buffer[1280*row+x+2]=(row<160?50:170);
                buffer[1280*row+x+1]=buffer[1280*row+x+3]=128;
            }
    uint64_t captured = 0;
    hdmi_front::Capture capture("/dev/hdmi-test");
    assert(!capture.latest(1000000000, captured));
    queued_frames = 1;
    assert(!capture.latest(1033000000, captured));
    queued_frames = 1;
    assert(capture.latest(1066000000, captured) && captured == 1066000000);
    assert(capture.latest(1100000000, captured) && captured == 1066000000);
    assert(!capture.latest(1566000000, captured) && !captured);

    available = false;
    assert(!capture.latest(2000000000, captured));
    available = true;
    report_dv = true;
    queued_frames = 1;
    assert(!capture.latest(3000000000, captured));
    queued_frames = 1;
    assert(capture.latest(3033000000, captured));
    active_signal = false;
    assert(!capture.latest(3100000000, captured));
    assert(!capture.latest(4000000000, captured));
    active_signal = true;
    report_dv = false;
    std::fill(fake_buffers[0], fake_buffers[0]+sizeof(fake_buffers[0]), 128);
    queued_frames = 1;
    assert(!capture.latest(5000000000, captured));
    queued_frames = 1;
    assert(!capture.latest(5033000000, captured) && !captured);
}

static void test_metadata()
{
    const uint64_t now = 6000000000ULL;
    video_v2::Metadata meta;
    meta.session_id = 1;
    meta.curve_type = 2;
    meta.curve_frame_id = 1;
    meta.curve_age_ms = 10;
    meta.curve_angle = 200;
    meta.curve_confidence = 92;
    uint8_t curve_packet[curve_wire::SIZE];
    assert(curve_wire::encode(curve_packet, meta));
    curve_wire::Receiver curve_receiver;
    video_v2::Metadata decoded;
    assert(curve_receiver.ingest(curve_packet, sizeof(curve_packet), 1, now, now, decoded));
    assert(decoded.curve_confidence == meta.curve_confidence);

    LineStyleData line = {};
    line.session = 1;
    line.source_id = 1;
    line.left = LINE_SOLID;
    line_set_deadline(&line, now + LINE_STYLE_TTL_NS);
    uint8_t line_packet[line_wire::SIZE];
    assert(line_wire::encode(line_packet, line, now));
    line_wire::Receiver line_receiver;
    LineStyleData copy = {};
    assert(line_receiver.ingest(line_packet, sizeof(line_packet), now, now, copy));
    assert(copy.left == LINE_SOLID && copy.diagnostic == 0);

    FrontPersonData person = {};
    person.session = 1;
    person.source_id = 1;
    person.flags = FRONT_PERSON_READY | FRONT_PERSON_VALID;
    front_person_set_deadline(&person, now + FRONT_PERSON_TTL_NS);
    uint8_t person_packet[front_person_wire::SIZE];
    assert(front_person_wire::encode(person_packet, person, now));
    front_person_wire::Receiver person_receiver;
    FrontPersonData person_copy = {};
    assert(person_receiver.ingest(person_packet, sizeof(person_packet), now, now, person_copy));
    FrontPersonGuard guard = {};
    assert(front_person_guard_observe(&guard, &person_copy, now, 1) == 0);
    std::vector<uint8_t> playback(320*240*3,0);
    assert(!hdmi_front::has_scene_content(playback.data()));
    for(int y=0;y<240;++y)for(int x=0;x<320;++x) {
        auto *p=&playback[3*(320*y+x)];
        p[(x/40)%3]=225;
    }
    assert(!hdmi_front::has_scene_content(playback.data()));
    for(int x=0;x<320;++x)for(int y=110;y<190;++y)playback[3*(320*y+x)]=100;
    assert(hdmi_front::has_scene_content(playback.data()));

    TrafficLightData traffic = {};
    traffic.session = 1;
    traffic.source_id = 1;
    traffic.state = traffic.raw_state = TRAFFIC_GREEN;
    traffic.flags = TRAFFIC_TRACKED | TRAFFIC_BOX | TRAFFIC_CONFIRMED | TRAFFIC_MODEL_READY;
    traffic.width = traffic.height = 20;
    traffic_set_deadline(&traffic, now + TRAFFIC_TTL_NS);
    uint8_t traffic_packet[traffic_wire::SIZE];
    assert(traffic_wire::encode(traffic_packet, traffic, now));
    traffic_wire::Receiver traffic_receiver;
    TrafficLightData traffic_copy = {};
    assert(traffic_receiver.ingest(traffic_packet, sizeof(traffic_packet), now, now, traffic_copy));
    assert(traffic_copy.state == TRAFFIC_GREEN && traffic_go_color(&traffic_copy, now) == TRAFFIC_GREEN);
}
#endif

int main()
{
    const uint8_t red[] = {81, 90, 81, 240, 81, 90, 81, 240};
    {
        using hdmi_front::SignalDetail;
        const unsigned stride = SignalDetail::WIDTH * 2 + 8;
        std::vector<uint8_t> padded(stride * SignalDetail::HEIGHT, 0);
        for (int row = 0; row < SignalDetail::HEIGHT; ++row)
            std::copy(red, red + sizeof(red), padded.data() + row * stride);
        SignalDetail detail, consumer;
        detail.capture(padded.data(), 640, 480, stride, false, false);
        detail.swap(consumer);
        std::vector<uint8_t> rgb;
        assert(!detail.rgb(rgb) && consumer.rgb(rgb));
        assert(rgb.size() == SignalDetail::WIDTH * SignalDetail::ROWS * 3);
        const auto last_row = rgb.data() + (SignalDetail::ROWS - 1) * SignalDetail::WIDTH * 3;
        assert(last_row[0] > 250 && last_row[1] < 4 && last_row[2] < 4);
        consumer.capture(padded.data(), 640, 360, stride, false, false);
        assert(!consumer.rgb(rgb));
        assert(!consumer.signal_view(3, 5, 2, rgb));
        for (int y = 0; y < 4; ++y)
            for (int x = 0; x < 4; x += 2)
            {
                auto *pair = padded.data() + (10 + y) * stride + (6 + x) * 2;
                pair[0] = 48 + y + x;
                pair[1] = 128;
                pair[2] = 148 + y + x;
                pair[3] = 128;
            }
        consumer.capture(padded.data(), 640, 480, stride, true, false);
        assert(consumer.signal_view(3, 5, 2, rgb) && rgb.size() == 4 * 4 * 3);
        for (int y = 0; y < 4; ++y)
            for (int x = 0; x < 4; ++x)
                for (int c = 0; c < 3; ++c)
                    assert(rgb[(y * 4 + x) * 3 + c] == (x % 2 ? 148 : 48) + y + (x & ~1));
        assert(!consumer.signal_view(319, 239, 2, rgb));
    }
    std::vector<uint8_t> tile(hdmi_front::WIDTH * hdmi_front::HEIGHT * 3);
    assert(!hdmi_front::convert_yuyv(red, sizeof(red) - 1, 4, 1, 8, false, false, tile.data()));
    assert(hdmi_front::convert_yuyv(red, sizeof(red), 4, 1, 8, false, false, tile.data()));
    assert(tile[0] == 0 && tile[1] == 0 && tile[2] == 0);
    const auto center = (120 * 320 + 160) * 3;
    assert(tile[center] < 4 && tile[center + 1] < 4 && tile[center + 2] > 250);

    std::vector<uint8_t> mosaic(640 * 480 * 3, 7);
    hdmi_front::apply_bgr(tile.data(), mosaic.data(), 640);
    assert(mosaic[0] == 0 && mosaic[320 * 3] == 7 && mosaic[240 * 640 * 3] == 7);
    assert(mosaic[(240 * 640 + 320) * 3] == 7);
    struct Format { unsigned w, h; int x, y, fitted_w, fitted_h; };
    for (const Format format : {Format{2560, 1600, 0, 20, 320, 200}, {1920, 1080, 0, 30, 320, 180},
                                {640, 480, 0, 0, 320, 240}, {480, 640, 70, 0, 180, 240}})
    {
        const auto rect = hdmi_front::fitted_content(format.w, format.h);
        assert(rect.x == format.x && rect.y == format.y &&
               rect.width == format.fitted_w && rect.height == format.fitted_h);
        std::vector<uint8_t> gray(format.w * format.h * 2, 128);
        assert(hdmi_front::convert_yuyv(gray.data(), gray.size(), format.w, format.h,
                                        2 * format.w, true, false, tile.data()));
        for (int y = 0; y < 240; ++y)
            for (int x = 0; x < 320; ++x)
            {
                const bool inside = x >= rect.x && x < rect.x + rect.width &&
                                    y >= rect.y && y < rect.y + rect.height;
                assert(tile[(y * 320 + x) * 3] == (inside ? 128 : 0));
            }
    }
#ifdef __linux__
    test_capture();
    test_metadata();
#endif
}
