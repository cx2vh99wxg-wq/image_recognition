#ifndef LINE_STYLE_SNAPSHOT_H
#define LINE_STYLE_SNAPSHOT_H
#include "line_style.h"
#include "yolo_integration.h"
#include <cstdio>
#include <cstdint>
#include <vector>
#include <unistd.h>
namespace m_lane
{
// 一次性有界诊断：执行 touch /tmp/m_lane_snapshot.request 后，
// 不重启采集即可保存下一份原始 NN 输入及其对应掩码。
class SnapshotRequest
{
    uint64_t polled_ = 0;

  public:
    template<class MaskGetter>
    void poll_lazy(uint64_t now, uint64_t source, uint64_t captured, const uint8_t *rgb, int w, int h,
              MaskGetter get_mask, const YOLOLineStyleResult &raw,
              const YOLOLineStyleResult &stable)
    {
        if (polled_ && now - polled_ < 1000000000ULL)
            return;
        polled_ = now;
        if (access("/tmp/m_lane_snapshot.request", F_OK))
            return;
        const yolopv2_result_t mask=get_mask();
        FILE *f = fopen("/tmp/m_lane_snapshot.ppm.tmp", "wb");
        if (!f)
            return;
        fprintf(f, "P6\n%d %d\n255\n", w, h);
        bool ok = fwrite(rgb, 1, (size_t)w * h * 3, f) == (size_t)w * h * 3;
        if (fclose(f))
            ok = false;
        f = fopen("/tmp/m_lane_snapshot_mask.pgm.tmp", "wb");
        if (!f)
            return;
        fprintf(f, "P5\n%d %d\n255\n", mask.width, mask.height);
        std::vector<uint8_t> pixels((size_t)mask.width * mask.height);
        for (size_t i = 0; i < pixels.size(); ++i)
            pixels[i] = mask.lane_line_data[i] > .3f ? 255 : 0;
        if (fwrite(pixels.data(), 1, pixels.size(), f) != pixels.size())
            ok = false;
        if (fclose(f))
            ok = false;
        f = fopen("/tmp/m_lane_snapshot_green.pgm.tmp", "wb");
        if (!f)
            return;
        fprintf(f, "P5\n%d %d\n255\n", mask.width, mask.height);
        for (size_t i = 0; i < pixels.size(); ++i)
            pixels[i] =
                mask.drivable_area_data[pixels.size() + i] > mask.drivable_area_data[i] ? 255 : 0;
        if (fwrite(pixels.data(), 1, pixels.size(), f) != pixels.size())
            ok = false;
        if (fclose(f))
            ok = false;
        if (!ok)
            return;
        if (rename("/tmp/m_lane_snapshot.ppm.tmp", "/tmp/m_lane_snapshot.ppm") ||
            rename("/tmp/m_lane_snapshot_mask.pgm.tmp", "/tmp/m_lane_snapshot_mask.pgm") ||
            rename("/tmp/m_lane_snapshot_green.pgm.tmp", "/tmp/m_lane_snapshot_green.pgm"))
            return;
        f = fopen("/tmp/m_lane_snapshot.txt", "w");
        if (!f)
            return;
        fprintf(
            f,
            "source=%llu capture_ns=%llu\nL=%s raw=%s reason=%s Q=%u duty=%u spans=%u gaps=%u\nR=%s raw=%s reason=%s Q=%u duty=%u spans=%u gaps=%u\n",
            (unsigned long long)source, (unsigned long long)captured,
            line_style_name(stable.left.type), line_style_name(raw.left.type),
            line_reason_name(stable.left.reason), stable.left.quality, raw.left.duty,
            raw.left.spans, raw.left.gaps, line_style_name(stable.right.type),
            line_style_name(raw.right.type), line_reason_name(stable.right.reason),
            stable.right.quality, raw.right.duty, raw.right.spans, raw.right.gaps);
        fprintf(f, "ego_reference=%.2f road_area=%u objects=%u evidence_ns=%llu/%llu\n",
                raw.reference_x, raw.road_area, raw.object_count,
                (unsigned long long)stable.left.evidence_ns,
                (unsigned long long)stable.right.evidence_ns);
        if (fclose(f) == 0)
            unlink("/tmp/m_lane_snapshot.request");
    }
};
}
#endif
