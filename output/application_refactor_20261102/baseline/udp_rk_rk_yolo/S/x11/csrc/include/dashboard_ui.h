#ifndef PERCEPTION_DASHBOARD_UI_H
#define PERCEPTION_DASHBOARD_UI_H

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <chrono>
#include <cstdlib>
#include "speed_warning_glyphs.h"

// Display-only compositor. It never reads or changes vehicle control state.
namespace perception_ui {

constexpr int WIDTH = 1600;
constexpr int HEIGHT = 900;

enum class MotionDirection : uint8_t {
    Idle,
    Forward,
    Left,
    Right,
    ForwardLeft,
    ForwardRight,
    Reverse
};

struct ViewStatus {
    bool pcie_live = false;
    bool udp_live = false;
    bool person = false;
    float fps = 0; // Renderer diagnostics; not shown as video FPS.
    float pcie_fps = 0; // S complete video input rate, measured from frame_id.
    float udp_fps = 0;  // M complete video arrival rate at S, measured from frame_id.
    bool recorded = false;
    MotionDirection motion = MotionDirection::Idle;
    unsigned lane_blocked = 0;
    unsigned line_left = 0, line_right = 0;
    unsigned line_left_quality=0,line_right_quality=0;
    unsigned line_left_reason=9,line_right_reason=9;
    unsigned line_left_raw=0,line_right_raw=0;
    unsigned traffic=0,traffic_raw=0,traffic_quality=0,traffic_detector_quality=0,traffic_flags=0;
    bool traffic_live=false;
    bool go_blocked=false,go_control_live=false,go_write_ok=false;
    unsigned gear=0;
    bool gear_known=false,gear_live=false,gear_shifting=false;
};

class Dashboard {
public:
    Dashboard(const uint8_t* atlas = nullptr, int atlas_width = 0, int atlas_height = 0)
        : background_(WIDTH * HEIGHT), frame_(WIDTH * HEIGHT) {
        loadFont(atlas, atlas_width, atlas_height);
        buildBackground();
        frame_ = background_;
    }

    const uint32_t* pixels() const { return frame_.data(); }

    // Stable overview: progress=-1. During the existing zoom animation use
    // its raw 0..1 progress; a focused view uses the central 640x480 crop.
    // Both input planes are ARGB32; the independent vehicle plane is 320x480.
    const uint32_t* compose(const uint32_t* legacy, int legacy_stride,
                            const uint32_t* vehicle, int vehicle_stride,
                            bool overview, float transition_progress,
                            const char* mode_label, const ViewStatus& status) {
        const uint64_t started = compositorClock();
        const bool fast = fastComposition();
        if (fast) {
            restoreBackground({1110,44,438,70});
            restoreBackground({712,44,392,78});
            // All changing text lies in these bands. Font metadata validates
            // the fixed 20/28px cells; restoring the whole width also covers
            // a long or clipped mode label. The remaining frame is static.
            restoreBackground({0, 180, WIDTH, 44});
            restoreBackground({0, 728, WIDTH, 80});
            restoreBackground({0, 808, WIDTH, 72});
            restoreBackground({1188, 230, 412, 500});
        } else std::copy(background_.begin(), background_.end(), frame_.begin());
        const uint64_t copied = compositorClock();
        const Rect video = {72, 224, 1092, 546};
        const bool full_overview = legacy && legacy_stride >= 1280 && overview && transition_progress < 0;
        // The six 364x273 cells exactly cover the 1092x546 video rectangle.
        if (!fast || !full_overview)
            fill(frame_, video.x, video.y, video.w, video.h, 0xFF02060D);
        const uint64_t cleared = compositorClock();

        if (legacy && legacy_stride >= 1280) {
            if (overview && transition_progress < 0) {
                for (int row = 0; row < 2; ++row) {
                    for (int col = 0; col < 3; ++col) {
                        const Rect cell = {video.x + col * 364,
                                           video.y + row * 273, 364, 273};
                        fit(frame_, legacy, legacy_stride,
                            {col * 320, row * 240, 320, 240}, cell);
                    }
                }
                // Thin separators sit on the view boundaries, not in a
                // replacement overlay that could hide detection labels.
                fill(frame_, video.x + 364, video.y, 1, video.h, 0xFF10273B);
                fill(frame_, video.x + 728, video.y, 1, video.h, 0xFF10273B);
                fill(frame_, video.x, video.y + 273, video.w, 1, 0xFF10273B);
            } else {
                float progress = transition_progress < 0 ? 1.f : transition_progress;
                if (!std::isfinite(progress)) progress = 1.f;
                progress = std::max(0.f, std::min(1.f, progress));
                const float ease = 1.f - (1.f - progress) * (1.f - progress);
                const int crop_width = 1280 - static_cast<int>(640.f * ease + .5f);
                fit(frame_, legacy, legacy_stride,
                    {(1280 - crop_width) / 2, 0, crop_width, 480}, video);
            }
        } else {
            centeredText(frame_, "WAITING FOR VIDEO", video, 2, 0xFF5B7795);
        }

        const uint64_t video_done = compositorClock();
        // Keep the source car artwork and its existing red alert unchanged.
        if (vehicle && vehicle_stride >= 320) {
            fit(frame_, vehicle, vehicle_stride, {0, 0, 320, 480},
                {1238, 230, 296, 500});
        } else {
            drawVehiclePlaceholder();
        }
        if (status.motion != MotionDirection::Idle)
            drawMotionCue(status.motion);
        if(status.lane_blocked&1)drawForbiddenCross(1282,475);
        if(status.lane_blocked&2)drawForbiddenCross(1490,475);
        if(status.go_blocked)drawTrafficGoWarning(status);
        const char* names[]={"UNKNOWN","SOLID","DASHED"};
        char line_text[80];
        unsigned lt=status.line_left?status.line_left:status.line_left_raw;
        unsigned rt=status.line_right?status.line_right:status.line_right_raw;
        std::snprintf(line_text,sizeof(line_text),"L %s%s Q%u   R %s%s Q%u",names[std::min(2u,lt)],
            !status.line_left&&status.line_left_raw?"*":"",status.line_left_quality,names[std::min(2u,rt)],
            !status.line_right&&status.line_right_raw?"*":"",status.line_right_quality);
        centeredText(frame_,line_text,{1238,699,296,18},1,0xFF93B1CB);
        const char* reasons[]={"OK","NO MASK","NO TRACK","SHORT","FIT ERR","NO PAINT","AMBIG","WAIT 3","DUPLICATE","STALE","HELD"};
        const char* lr=(!status.line_left&&status.line_left_reason==0)?"NO CLASS":reasons[std::min(10u,status.line_left_reason)];
        const char* rr=(!status.line_right&&status.line_right_reason==0)?"NO CLASS":reasons[std::min(10u,status.line_right_reason)];
        std::snprintf(line_text,sizeof(line_text),"L:%s  R:%s",lr,rr);
        centeredText(frame_,line_text,{1238,719,296,15},1,0xFFB9ACD7);

        const uint64_t car_done = compositorClock();
        drawTraffic(status);
        drawGearStatus(status);
        const char* label = safeLabel(mode_label, overview);
        text(frame_, 1162 - textWidth(label, 1), 204, label, 1, 0xFFA1C4DA);
        if (overview && transition_progress < 0) {
            text(frame_, 74, 781, "UDP / UPPER ROW", 1, 0xFF658CA8);
            text(frame_, 1002, 781, "PCIE / LOWER ROW", 1, 0xFF658CA8);
        } else {
            text(frame_, 74, 781, "FOCUSED CAMERA / 4:3", 1, 0xFF658CA8);
        }

        const uint32_t alert = status.person||status.go_blocked ? 0xFFFF748C : 0xFF6BDBE7;
        fillChamfer(frame_, {1240, 744, 292, 30}, 7,
                    status.person||status.go_blocked ? 0xFF281321 : 0xFF0A1B2A);
        const char* detection = status.person ? "PERSON DETECTED" : status.go_blocked ? "RED LIGHT / GO HOLD" : "VISION MONITOR";
        centeredText(frame_, detection, {1240, 744, 292, 30}, 2, alert);
        circle(frame_, 1232, 204, 3.f, alert, 230);

        drawLink(62, 850, "PCIE", status.pcie_live, status.recorded);
        drawLink(272, 850, "UDP", status.udp_live, status.recorded);
        char performance[64];
        if (status.recorded) {
            std::snprintf(performance, sizeof(performance), "RECORDED PREVIEW");
        } else {
            const float s_fps = std::isfinite(status.pcie_fps)
                ? std::max(0.f, std::min(999.f, status.pcie_fps)) : 0.f;
            const float m_fps = std::isfinite(status.udp_fps)
                ? std::max(0.f, std::min(999.f, status.udp_fps)) : 0.f;
            std::snprintf(performance, sizeof(performance), "VIDEO IN  S %.1f  M %.1f FPS", s_fps, m_fps);
        }
        text(frame_, 486, 844, performance, 2,
             status.recorded ? 0xFFD5B97B : 0xFF93B1CB);
        reportComposition(started, copied, cleared, video_done, car_done, compositorClock(), fast);
        return frame_.data();
    }

private:
    struct Rect { int x, y, w, h; };
    struct Point { int x, y; };
    struct FontMetrics {
        int band_y, cell_height, advance64, cap_top, cap_height;
    };
    std::vector<uint32_t> background_;
    std::vector<uint32_t> frame_;
    std::vector<uint8_t> font_atlas_;
    FontMetrics font_metrics_[3] = {};

    void drawGauge(int x,int y,int radius,uint32_t color,unsigned gear) {
        circle(frame_,x,y,(float)radius+5,color,35);
        circle(frame_,x,y,(float)radius,0xFF10141E,235);
        Point arc[13];
        for(int i=0;i<13;++i) {
            float a=3.66519143f+i*4.1887902f/12;
            arc[i]={x+(int)std::lround(std::cos(a)*radius),y+(int)std::lround(std::sin(a)*radius)};
        }
        path(frame_,arc,13,color,false);
        float a=gear>=3?-.35f:gear==2?-1.10f:-2.30f;
        Point tip={x+(int)std::lround(std::cos(a)*(radius-5)),y+(int)std::lround(std::sin(a)*(radius-5))};
        line(frame_,{x,y},tip,color,3,255);circle(frame_,x,y,3.f,color,255);
    }
    void drawGearStatus(const ViewStatus& status) {
        bool known=status.gear_known&&status.gear>=1&&status.gear<=3;
        bool high=known&&status.gear>=2;
        uint32_t color=high?0xFFFFBE55:known&&status.gear_live?0xFF6BDBE7:0xFF8A9AAD;
        fillChamfer(frame_,{720,52,380,62},8,high?0xFF2B1E14:0xFF0A1428);
        drawGauge(749,82,18,color,known?status.gear:1);
        char label[40];
        if(known)std::snprintf(label,sizeof(label),"GEAR %u / %s",status.gear,!status.gear_live?"STALE":high?"HIGH":"LOW");
        else std::snprintf(label,sizeof(label),"GEAR / UNKNOWN");
        text(frame_,782,63,label,2,color);
        const char* detail=!status.gear_live?"GEAR LINK UNAVAILABLE":status.gear_shifting?"SPEED BUTTON HELD":"REMOTE GEAR ESTIMATE";
        text(frame_,782,90,detail,1,high?0xFFE6BC82:0xFF8DA5BD);
        if (high) {
            // Right side of the gear card; refreshed with the card on every
            // frame, so returning to gear 1 removes the warning immediately.
            for (int y = 0; y < SPEED_WARNING_HEIGHT; ++y)
                for (int x = 0; x < SPEED_WARNING_WIDTH; ++x)
                    if (SPEED_WARNING_MASK[y][x / 8] & (0x80 >> (x % 8)))
                        pixel(frame_, 1008 + x, 61 + y, 0xFFFF526D, 255);
        }
    }
    void drawTrafficGoWarning(const ViewStatus& status) {
        // Place the alert ahead of the car; leave the left/right X locations
        // and the existing pedestrian alert visible at the same time.
        const Point triangle[]={{1386,237},{1418,293},{1354,293},{1386,237}};
        fillPolygon(frame_,triangle,3,0xFF6D142B,0xFF26101D,235);
        path(frame_,triangle,4,0xFFFF526D,false);
        line(frame_,{1386,254},{1386,272},0xFFFFE5EB,5,255);
        circle(frame_,1386,282,3.f,0xFFFFE5EB,255);
        fillChamfer(frame_,{1258,304,256,44},7,0xFF2A1020);
        centeredText(frame_,"RED LIGHT / GO HOLD",{1258,307,256,18},1,0xFFFF8296);
        const char* detail=!status.go_control_live?"CONTROL LINK LOST":
            !status.go_write_ok?"SPI WRITE RETRY":"WAIT CONFIRMED GREEN";
        centeredText(frame_,detail,{1258,327,256,14},1,0xFFFFC6D0);
    }

    void drawTraffic(const ViewStatus& status) {
        fillChamfer(frame_,{1120,52,420,54},8,0xFF0A1428);
        const uint32_t active[]={0xFF75869B,0xFFFF5368,0xFFFFCE63,0xFF43E6B2};
        unsigned state=status.traffic_live?std::min(3u,status.traffic):0;
        for(unsigned i=1;i<=3;++i) {
            int x=1144+(i-1)*26;
            circle(frame_,x,78,10.f,active[i],state==i?65:12);
            circle(frame_,x,78,5.f,state==i?active[i]:0xFF334355,240);
        }
        const char* names[]={"UNKNOWN","RED","YELLOW","GREEN"};
        char label[48];std::snprintf(label,sizeof(label),"TRAFFIC / %s",names[state]);
        text(frame_,1220,63,label,2,active[state]);
        std::snprintf(label,sizeof(label),"M FRONT / COLOR %u / BOX %u%s",status.traffic_quality,status.traffic_detector_quality,!state&&status.traffic_raw?" / WAIT":"");
        text(frame_,1220,88,label,1,0xFF8DA5BD);
    }

    uint64_t compositor_profile_start_ = 0, compositor_profile_calls_ = 0;
    uint64_t compositor_copy_ = 0, compositor_clear_ = 0, compositor_video_ = 0;
    uint64_t compositor_car_ = 0, compositor_ui_ = 0;

    static uint64_t compositorClock() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    static bool fastComposition() {
        static const char* value = std::getenv("VISION_DASHBOARD_FAST");
        static const bool enabled = value && !std::strcmp(value, "1");
        return enabled;
    }

    void restoreBackground(Rect area) {
        for (int y = area.y; y < area.y + area.h; ++y)
            std::copy_n(background_.data() + y * WIDTH + area.x, area.w,
                        frame_.data() + y * WIDTH + area.x);
    }

    void reportComposition(uint64_t started, uint64_t copied, uint64_t cleared,
                           uint64_t video, uint64_t car, uint64_t ended, bool fast) {
        if (!compositor_profile_start_) compositor_profile_start_ = started;
        ++compositor_profile_calls_;
        compositor_copy_ += copied - started;
        compositor_clear_ += cleared - copied;
        compositor_video_ += video - cleared;
        compositor_car_ += car - video;
        compositor_ui_ += ended - car;
        if (ended - compositor_profile_start_ < 5000000000ULL) return;
        const double divisor = compositor_profile_calls_ * 1e6;
        std::printf("[PERF DASHBOARD] copy_ms=%.2f clear_ms=%.2f video_fit_ms=%.2f car_fit_ms=%.2f ui_ms=%.2f fast=%d\n",
                    compositor_copy_ / divisor, compositor_clear_ / divisor,
                    compositor_video_ / divisor, compositor_car_ / divisor, compositor_ui_ / divisor, fast);
        compositor_profile_start_ = ended;
        compositor_profile_calls_ = compositor_copy_ = compositor_clear_ = 0;
        compositor_video_ = compositor_car_ = compositor_ui_ = 0;
    }

    // The optional PNG is a grayscale opacity atlas, loaded with stbi's
    // requested_channels=1. Copy it so the caller may release its image.
    // Each 16x6 ASCII band leaves its last cell for versioned metrics.
    void loadFont(const uint8_t* atlas, int width, int height) {
        if (!atlas || width != 512 || height != 612) return;
        const int band_y[3] = {0, 120, 288};
        const int cell_height[3] = {20, 28, 54};
        FontMetrics metrics[3];
        for (int i = 0; i < 3; ++i) {
            const uint8_t* meta = atlas + (band_y[i] + 5 * cell_height[i]) * width + 15 * 32;
            if (std::memcmp(meta, "PEUI", 4) != 0 || meta[4] != 1) return;
            const int advance = meta[5] | (int(meta[6]) << 8);
            const int top = meta[7], cap_height = meta[8];
            if (advance < 64 || advance > 32 * 64 || cap_height < 1 ||
                top + cap_height > cell_height[i]) return;
            metrics[i] = {band_y[i], cell_height[i], advance, top, cap_height};
        }
        font_atlas_.assign(atlas, atlas + width * height);
        for (int i = 0; i < 3; ++i) font_metrics_[i] = metrics[i];
    }

    static int fontTier(int scale) { return scale <= 1 ? 0 : scale == 2 ? 1 : 2; }

    static uint32_t blend(uint32_t dst, uint32_t color, int alpha) {
        alpha = std::max(0, std::min(255, alpha));
        const uint32_t inv = 255 - alpha;
        const uint32_t r = (((dst >> 16) & 255) * inv + ((color >> 16) & 255) * alpha + 127) / 255;
        const uint32_t g = (((dst >> 8) & 255) * inv + ((color >> 8) & 255) * alpha + 127) / 255;
        const uint32_t b = ((dst & 255) * inv + (color & 255) * alpha + 127) / 255;
        return 0xFF000000 | (r << 16) | (g << 8) | b;
    }

    static void pixel(std::vector<uint32_t>& image, int x, int y,
                      uint32_t color, int alpha = 255) {
        if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
        uint32_t& dst = image[y * WIDTH + x];
        dst = alpha >= 255 ? (color | 0xFF000000) : blend(dst, color, alpha);
    }

    static void fill(std::vector<uint32_t>& image, int x, int y, int w, int h,
                     uint32_t color, int alpha = 255) {
        const int right = std::min(WIDTH, x + w), bottom = std::min(HEIGHT, y + h);
        x = std::max(0, x); y = std::max(0, y);
        if (right <= x || bottom <= y) return;
        if (alpha >= 255) {
            const uint32_t opaque = color | 0xFF000000;
            for (int yy = y; yy < bottom; ++yy)
                std::fill_n(image.data() + yy * WIDTH + x, right - x, opaque);
            return;
        }
        for (int yy = y; yy < bottom; ++yy) {
            for (int xx = x; xx < right; ++xx) pixel(image, xx, yy, color, alpha);
        }
    }

    static void fillChamfer(std::vector<uint32_t>& image, Rect rect, int cut,
                            uint32_t color, int alpha = 255) {
        for (int y = 0; y < rect.h; ++y) {
            const int inset = std::max(0, std::max(cut - y, cut - (rect.h - 1 - y)));
            fill(image, rect.x + inset, rect.y + y, rect.w - 2 * inset, 1, color, alpha);
        }
    }

    static void line(std::vector<uint32_t>& image, Point a, Point b,
                     uint32_t color, float width = 1.f, int alpha = 255) {
        const float dx = static_cast<float>(b.x - a.x), dy = static_cast<float>(b.y - a.y);
        const float length2 = dx * dx + dy * dy;
        const float radius = width * .5f;
        const int margin = static_cast<int>(radius + 1.5f);
        const int left = std::max(0, std::min(a.x, b.x) - margin);
        const int right = std::min(WIDTH - 1, std::max(a.x, b.x) + margin);
        const int top = std::max(0, std::min(a.y, b.y) - margin);
        const int bottom = std::min(HEIGHT - 1, std::max(a.y, b.y) + margin);
        for (int y = top; y <= bottom; ++y) {
            for (int x = left; x <= right; ++x) {
                float t = length2 > 0 ? ((x - a.x) * dx + (y - a.y) * dy) / length2 : 0;
                t = std::max(0.f, std::min(1.f, t));
                const float ex = x - a.x - t * dx, ey = y - a.y - t * dy;
                const float coverage = std::max(0.f, std::min(1.f, radius + .5f - std::sqrt(ex * ex + ey * ey)));
                if (coverage > 0) pixel(image, x, y, color, static_cast<int>(coverage * alpha));
            }
        }
    }

    static void circle(std::vector<uint32_t>& image, int cx, int cy, float radius,
                       uint32_t color, int alpha) {
        const int bound = static_cast<int>(radius + 1.f);
        for (int y = -bound; y <= bound; ++y) {
            for (int x = -bound; x <= bound; ++x) {
                const float coverage = std::max(0.f, std::min(1.f, radius + .5f - std::sqrt(float(x * x + y * y))));
                if (coverage > 0) pixel(image, cx + x, cy + y, color, static_cast<int>(coverage * alpha));
            }
        }
    }

    static void node(std::vector<uint32_t>& image, int x, int y, uint32_t color) {
        circle(image, x, y, 9, color, 8);
        circle(image, x, y, 5, color, 25);
        circle(image, x, y, 2.2f, color, 210);
        pixel(image, x, y, 0xFFE2F4FF, 220);
    }

    static void path(std::vector<uint32_t>& image, const Point* points, int count,
                     uint32_t color, bool strong) {
        const float widths[] = {9.f, 5.f, 2.5f, 1.f};
        const int alphas[] = {strong ? 10 : 3, strong ? 27 : 8,
                              strong ? 98 : 22, strong ? 225 : 100};
        for (int pass = 0; pass < 4; ++pass) {
            for (int i = 1; i < count; ++i) {
                line(image, points[i - 1], points[i], color, widths[pass], alphas[pass]);
            }
        }
    }

    static bool insidePolygon(const Point* vertices, int count, float x, float y) {
        bool inside = false;
        for (int i = 0, previous = count - 1; i < count; previous = i++) {
            const Point& a = vertices[previous];
            const Point& b = vertices[i];
            if ((a.y > y) != (b.y > y) &&
                x < a.x + (y - a.y) * (b.x - a.x) / (b.y - a.y))
                inside = !inside;
        }
        return inside;
    }

    static void fillPolygon(std::vector<uint32_t>& image, const Point* vertices,
                            int count, uint32_t top_color, uint32_t bottom_color,
                            int alpha) {
        int left = WIDTH - 1, right = 0, top = HEIGHT - 1, bottom = 0;
        for (int i = 0; i < count; ++i) {
            left = std::min(left, vertices[i].x);
            right = std::max(right, vertices[i].x);
            top = std::min(top, vertices[i].y);
            bottom = std::max(bottom, vertices[i].y);
        }
        left = std::max(0, left - 1); right = std::min(WIDTH - 1, right + 1);
        top = std::max(0, top - 1); bottom = std::min(HEIGHT - 1, bottom + 1);
        for (int y = top; y <= bottom; ++y) {
            const int shade = (y - top) * 255 / std::max(1, bottom - top);
            const int inverse = 255 - shade;
            const uint32_t r = (((top_color >> 16) & 255) * inverse +
                                ((bottom_color >> 16) & 255) * shade) / 255;
            const uint32_t g = (((top_color >> 8) & 255) * inverse +
                                ((bottom_color >> 8) & 255) * shade) / 255;
            const uint32_t b = ((top_color & 255) * inverse +
                                (bottom_color & 255) * shade) / 255;
            const uint32_t color = 0xFF000000 | (r << 16) | (g << 8) | b;
            for (int x = left; x <= right; ++x) {
                int coverage = 0;
                for (int sy = 0; sy < 2; ++sy)
                    for (int sx = 0; sx < 2; ++sx)
                        coverage += insidePolygon(vertices, count,
                                                  x + (sx ? .75f : .25f),
                                                  y + (sy ? .75f : .25f));
                if (coverage) pixel(image, x, y, color, alpha * coverage / 4);
            }
        }
    }

    static Point rotateArrowPoint(Point local, Point center, float cosine,
                                  float sine, float scale = 1.f) {
        const float x = local.x * scale, y = local.y * scale;
        return {center.x + static_cast<int>(std::lround(x * cosine - y * sine)),
                center.y + static_cast<int>(std::lround(x * sine + y * cosine))};
    }

    void drawMotionCue(MotionDirection motion) {
        Point center;
        float angle;
        float size;
        switch (motion) {
            case MotionDirection::Left:         center = {1285, 472}; angle = 3.14159265f; size = 1.20f; break;
            case MotionDirection::Right:        center = {1487, 472}; angle = 0.f; size = 1.20f; break;
            case MotionDirection::Forward:      center = {1386, 269}; angle = -1.57079633f; size = 1.16f; break;
            case MotionDirection::Reverse:      center = {1386, 689}; angle = 1.57079633f; size = 1.16f; break;
            case MotionDirection::ForwardLeft:  center = {1288, 321}; angle = -2.35619449f; size = 1.10f; break;
            case MotionDirection::ForwardRight: center = {1484, 321}; angle = -.78539816f; size = 1.10f; break;
            case MotionDirection::Idle: return;
        }

        // A translucent navigation marker uses the same cyan and violet light as
        // the vehicle and panel frame. Keep the marker in the image margin so
        // neither the car nor the detection banner is covered.
        static const Point shape[7] = {{-28, -7}, {-3, -7}, {-3, -17},
                                       {29, 0}, {-3, 17}, {-3, 7}, {-28, 7}};
        const float cosine = std::cos(angle), sine = std::sin(angle);
        Point aura[7], shell[8], face[7];
        for (int i = 0; i < 7; ++i) {
            aura[i] = rotateArrowPoint(shape[i], center, cosine, sine, 1.23f * size);
            shell[i] = rotateArrowPoint(shape[i], center, cosine, sine, 1.10f * size);
            face[i] = rotateArrowPoint(shape[i], center, cosine, sine, size);
        }
        shell[7] = shell[0];
        const bool reverse = motion == MotionDirection::Reverse;
        const uint32_t light = reverse ? 0xFFD178FF : 0xFF57E8FC;
        const uint32_t top = reverse ? 0xFFCF9AFE : 0xFF9AF8FF;
        const uint32_t bottom = reverse ? 0xFF7E4BC4 : 0xFF168DBB;

        fillPolygon(frame_, aura, 7, light, light, 17);
        path(frame_, shell, 8, light, false);
        fillPolygon(frame_, shell, 7, 0xFF0B243B, 0xFF061325, 190);
        fillPolygon(frame_, face, 7, top, bottom, 205);
        path(frame_, shell, 8, light, true);

        // The illuminated center rail and small blade cuts echo the circuit
        // traces already present around the vehicle panel.
        const Point rail_a = rotateArrowPoint({-21, 0}, center, cosine, sine, size);
        const Point rail_b = rotateArrowPoint({10, 0}, center, cosine, sine, size);
        line(frame_, rail_a, rail_b, 0xFFE7FCFF, 2.f, 135);
        for (int x : {-19, -13}) {
            const Point a = rotateArrowPoint({x, -4}, center, cosine, sine, size);
            const Point b = rotateArrowPoint({x, 4}, center, cosine, sine, size);
            line(frame_, a, b, 0xFFDEFBFF, 1.f, 105);
        }
    }

    void drawForbiddenCross(int x,int y) {
        // Screen-space geometry remains crisp through every camera/zoom mode.
        circle(frame_,x,y,35.f,0xFFFF254F,30);
        circle(frame_,x,y,28.f,0xFF200D1B,235);
        for(int i=0;i<32;++i) {
            float a=i*6.2831853f/32,b=(i+1)*6.2831853f/32;
            line(frame_,{x+(int)(27*std::cos(a)),y+(int)(27*std::sin(a))},
                         {x+(int)(27*std::cos(b)),y+(int)(27*std::sin(b))},0xFFFF4368,2.f,230);
        }
        for(int slope:{-1,1}) {
            Point a={x-13,y-slope*13},b={x+13,y+slope*13};
            line(frame_,a,b,0xFFFF244F,13.f,40);
            line(frame_,a,b,0xFFFF375C,7.f,255);
            line(frame_,a,b,0xFFFFCBD4,2.f,230);
        }
        centeredText(frame_,"BLOCKED",{x-42,y+38,84,16},1,0xFFFF7C96);
    }

    static void panel(std::vector<uint32_t>& image, Rect box, uint32_t color) {
        const int c = 16;
        fillChamfer(image, box, c, 0xFF07101E, 244);
        const Point outer[] = {{box.x + c, box.y}, {box.x + box.w - c, box.y},
            {box.x + box.w, box.y + c}, {box.x + box.w, box.y + box.h - c},
            {box.x + box.w - c, box.y + box.h}, {box.x + c, box.y + box.h},
            {box.x, box.y + box.h - c}, {box.x, box.y + c}, {box.x + c, box.y}};
        path(image, outer, 9, color, true);
        line(image, {box.x + 22, box.y + 6}, {box.x + box.w - 22, box.y + 6}, color, 1, 65);
        line(image, {box.x + 22, box.y + box.h - 6}, {box.x + box.w - 22, box.y + box.h - 6}, color, 1, 55);
        line(image, {box.x + 6, box.y + 22}, {box.x + 6, box.y + box.h - 22}, color, 1, 40);
        line(image, {box.x + box.w - 6, box.y + 22}, {box.x + box.w - 6, box.y + box.h - 22}, color, 1, 40);
    }

    // Rows of a compact 5x7 display alphabet. No font or GUI package is needed
    // on the ARM board; all labels use the same image compositor as the video.
    static const uint8_t* glyph(char ch) {
        static const uint8_t letters[26][7] = {
            {14,17,17,31,17,17,17}, {30,17,17,30,17,17,30},
            {14,17,16,16,16,17,14}, {30,17,17,17,17,17,30},
            {31,16,16,30,16,16,31}, {31,16,16,30,16,16,16},
            {14,17,16,23,17,17,15}, {17,17,17,31,17,17,17},
            {14,4,4,4,4,4,14}, {7,2,2,2,18,18,12},
            {17,18,20,24,20,18,17}, {16,16,16,16,16,16,31},
            {17,27,21,21,17,17,17}, {17,25,21,19,17,17,17},
            {14,17,17,17,17,17,14}, {30,17,17,30,16,16,16},
            {14,17,17,17,21,18,13}, {30,17,17,30,20,18,17},
            {15,16,16,14,1,1,30}, {31,4,4,4,4,4,4},
            {17,17,17,17,17,17,14}, {17,17,17,17,17,10,4},
            {17,17,17,21,21,21,10}, {17,17,10,4,10,17,17},
            {17,17,10,4,4,4,4}, {31,1,2,4,8,16,31}
        };
        static const uint8_t digits[10][7] = {
            {14,17,19,21,25,17,14}, {4,12,4,4,4,4,14},
            {14,17,1,2,4,8,31}, {30,1,1,14,1,1,30},
            {2,6,10,18,31,2,2}, {31,16,16,30,1,1,30},
            {14,16,16,30,17,17,14}, {31,1,2,4,8,8,8},
            {14,17,17,14,17,17,14}, {14,17,17,15,1,1,14}
        };
        static const uint8_t blank[7] = {0,0,0,0,0,0,0};
        static const uint8_t dash[7] = {0,0,0,31,0,0,0};
        static const uint8_t dot[7] = {0,0,0,0,0,6,6};
        static const uint8_t colon[7] = {0,6,6,0,6,6,0};
        static const uint8_t slash[7] = {1,2,2,4,8,8,16};
        static const uint8_t question[7] = {14,17,1,2,4,0,4};
        if (ch >= 'a' && ch <= 'z') ch = static_cast<char>(ch - 'a' + 'A');
        if (ch >= 'A' && ch <= 'Z') return letters[ch - 'A'];
        if (ch >= '0' && ch <= '9') return digits[ch - '0'];
        if (ch == ' ') return blank;
        if (ch == '-') return dash;
        if (ch == '.') return dot;
        if (ch == ':') return colon;
        if (ch == '/') return slash;
        return question;
    }

    int textWidth(const char* label, int scale) const {
        if (!label || !*label || scale < 1) return 0;
        if (!font_atlas_.empty()) {
            return static_cast<int>((std::strlen(label) * font_metrics_[fontTier(scale)].advance64 + 32) / 64);
        }
        return static_cast<int>(std::strlen(label)) * 6 * scale - scale;
    }

    void text(std::vector<uint32_t>& image, int x, int y,
              const char* label, int scale, uint32_t color, int alpha = 255) const {
        if (!label || scale < 1) return;
        if (!font_atlas_.empty()) {
            const FontMetrics& font = font_metrics_[fontTier(scale)];
            int pen64 = 0;
            for (; *label; ++label, pen64 += font.advance64) {
                unsigned char ch = static_cast<unsigned char>(*label);
                if (ch < 32 || ch > 126) ch = '?';
                const int index = ch - 32;
                const int sx = (index % 16) * 32;
                const int sy = font.band_y + (index / 16) * font.cell_height;
                const int dx = x + (pen64 + 32) / 64 - 2;
                const int dy = y - font.cap_top;
                for (int row = 0; row < font.cell_height; ++row) {
                    const uint8_t* source = font_atlas_.data() + (sy + row) * 512 + sx;
                    for (int col = 0; col < 32; ++col) {
                        if (source[col]) {
                            pixel(image, dx + col, dy + row, color,
                                  (int(source[col]) * alpha + 127) / 255);
                        }
                    }
                }
            }
            return;
        }
        for (; *label; ++label, x += 6 * scale) {
            const uint8_t* rows = glyph(*label);
            for (int row = 0; row < 7; ++row) {
                for (int col = 0; col < 5; ++col) {
                    if (rows[row] & (1 << (4 - col))) {
                        fill(image, x + col * scale, y + row * scale,
                             scale, scale, color, alpha);
                    }
                }
            }
        }
    }

    void centeredText(std::vector<uint32_t>& image, const char* label,
                      Rect box, int scale, uint32_t color) const {
        const int height = font_atlas_.empty() ? 7 * scale : font_metrics_[fontTier(scale)].cap_height;
        text(image, box.x + (box.w - textWidth(label, scale)) / 2,
             box.y + (box.h - height) / 2, label, scale, color);
    }

    static const char* safeLabel(const char* label, bool overview) {
        if (label && *label) {
            bool ascii = true;
            int length = 0;
            for (; length < 42 && label[length]; ++length) {
                if (static_cast<unsigned char>(label[length]) >= 128) ascii = false;
            }
            if (ascii && length < 42) return label;
        }
        return overview ? "SURROUND VIEW" : "CAMERA VIEW";
    }

    static void fit(std::vector<uint32_t>& image, const uint32_t* source,
                    int stride, Rect src, Rect dst) {
        if (!source || stride <= 0 || src.w <= 0 || src.h <= 0 || dst.w <= 0 || dst.h <= 0) return;
        const double ratio = std::min(double(dst.w) / src.w, double(dst.h) / src.h);
        const int width = std::max(1, static_cast<int>(src.w * ratio + .5));
        const int height = std::max(1, static_cast<int>(src.h * ratio + .5));
        const int left = dst.x + (dst.w - width) / 2;
        const int top = dst.y + (dst.h - height) / 2;
        // Six camera tiles share this mapping. Compute divisions once per
        // column instead of once for every pixel on every destination row.
        static thread_local std::vector<int> columns;
        static thread_local int mapped_source_width = 0;
        if ((int)columns.size() != width || mapped_source_width != src.w) {
            columns.resize(width);
            for (int x = 0; x < width; ++x)
                columns[x] = std::min(src.w - 1, x * src.w / width);
            mapped_source_width = src.w;
        }
        const bool unclipped = left >= 0 && top >= 0 &&
                               left + width <= WIDTH && top + height <= HEIGHT;
        for (int y = 0; y < height; ++y) {
            const int sy = src.y + std::min(src.h - 1, y * src.h / height);
            const uint32_t* row = source + sy * stride + src.x;
            if (unclipped) {
                uint32_t* destination = image.data() + (top + y) * WIDTH + left;
                for (int x = 0; x < width; ++x)
                    destination[x] = row[columns[x]] | 0xFF000000;
            } else for (int x = 0; x < width; ++x)
                pixel(image, left + x, top + y, row[columns[x]]);
        }
    }

    static void chip(std::vector<uint32_t>& image, int x, int y, int w, int h,
                     uint32_t tint) {
        fill(image, x, y, w, h, 0xFF0B1022);
        line(image, {x, y}, {x + w, y}, tint, 1, 85);
        line(image, {x, y + h}, {x + w, y + h}, tint, 1, 85);
        line(image, {x, y}, {x, y + h}, tint, 1, 85);
        line(image, {x + w, y}, {x + w, y + h}, tint, 1, 85);
        for (int i = 5; i < w - 3; i += 5) {
            line(image, {x + i, y - 4}, {x + i, y - 1}, tint, 1, 70);
            line(image, {x + i, y + h + 1}, {x + i, y + h + 4}, tint, 1, 70);
        }
        for (int i = 5; i < h - 3; i += 5) {
            line(image, {x - 4, y + i}, {x - 1, y + i}, tint, 1, 70);
            line(image, {x + w + 1, y + i}, {x + w + 4, y + i}, tint, 1, 70);
        }
        for (int yy = y + 5; yy < y + h - 3; yy += 5) {
            for (int xx = x + 5; xx < x + w - 3; xx += 5) {
                fill(image, xx, yy, 2, 2, tint, 40);
            }
        }
    }

    void buildBackground() {
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                const float purple_dx = (x - 1470.f) / 480.f;
                const float purple_dy = (y - 120.f) / 390.f;
                const float cyan_dx = (x - 80.f) / 480.f;
                const float cyan_dy = (y - 620.f) / 430.f;
                const float p = 1.f / (1.f + purple_dx * purple_dx + purple_dy * purple_dy);
                const float c = 1.f / (1.f + cyan_dx * cyan_dx + cyan_dy * cyan_dy);
                const int grid = (x % 32 == 0 || y % 32 == 0) ? 2 : 0;
                const int r = 3 + static_cast<int>(8 * p) + grid;
                const int g = 6 + static_cast<int>(8 * c) + grid;
                const int b = 17 + static_cast<int>(13 * p + 9 * c) + grid;
                background_[y * WIDTH + x] = 0xFF000000 | (r << 16) | (g << 8) | b;
            }
        }

        // Deterministic circuit traces: cached once, with no runtime assets.
        for (int i = 0; i < 20; ++i) {
            const int x = (i * 193 + 29) % WIDTH;
            const int y = 15 + (i * 23) % 112;
            const int turn = 18 + (i * 11) % 54;
            const int endx = std::min(WIDTH - 5, x + 55 + (i * 37) % 110);
            const Point top[] = {{x, 0}, {x, y}, {x + turn, y + turn},
                                  {endx, y + turn}};
            const uint32_t color = i % 3 ? 0xFF7039AE : 0xFF087DA8;
            path(background_, top, 4, color, false);
            if (i % 3 == 0) node(background_, endx, y + turn, color);
            const int by = 817 + (i * 13) % 61;
            const int bx = (i * 163 + 38) % WIDTH;
            const Point bottom[] = {{bx, HEIGHT}, {bx, by + 25},
                {std::min(WIDTH - 2, bx + 25), by},
                {std::min(WIDTH - 2, bx + 90), by}};
            path(background_, bottom, 4, color, false);
            if (i % 4 == 0) node(background_, std::min(WIDTH - 2, bx + 90), by, color);
        }
        for (int i = 0; i < 10; ++i) {
            const int y = 216 + i * 57;
            const Point left[] = {{0, y}, {18, y}, {40, y + 22}, {48, y + 22}};
            const Point right[] = {{1552, y + 14}, {1570, y + 14},
                                    {1592, y - 8}, {1599, y - 8}};
            path(background_, left, 4, 0xFF12668B, false);
            path(background_, right, 4, 0xFF7540A4, false);
        }
        chip(background_, 1480, 48, 43, 33, 0xFF6B4E9D);
        chip(background_, 1360, 108, 29, 20, 0xFF3A75A8);
        chip(background_, 1080, 30, 29, 22, 0xFF695198);
        chip(background_, 830, 839, 40, 29, 0xFF456794);
        chip(background_, 1106, 856, 25, 20, 0xFF67518C);

        // A restrained title band leaves most of the canvas to live imagery.
        fill(background_, 52, 44, 4, 13, 0xFF43D1F3);
        text(background_, 68, 47, "SURROUND VISION / S-END", 1, 0xFF79B4CC);
        text(background_, 56, 77, "PERCEPTION ENGINE", 5, 0xFFDCF1FD);
        text(background_, 58, 126, "REALTIME VISUAL AWARENESS", 1, 0xFF6485A6);
        line(background_, {56, 154}, {1544, 154}, 0xFF3C5C88, 1, 60);
        line(background_, {56, 154}, {190, 154}, 0xFF1CC8EB, 2, 180);
        line(background_, {1414, 154}, {1544, 154}, 0xFFBA5BEF, 2, 140);
        text(background_, 1210, 116, "SIX VIEWS. ONE ENVIRONMENT.", 1, 0xFF8499B9);

        panel(background_, {48, 186, 1140, 612}, 0xFF24BFE9);
        panel(background_, {1220, 186, 332, 612}, 0xFFBD65F2);
        text(background_, 74, 204, "CAMERA ARRAY / 06", 1, 0xFF79BDCF);
        centeredText(background_, "VEHICLE AWARENESS", {1248, 198, 284, 24}, 2, 0xFFB9ACD7);
        text(background_, 1328, 782, "DETECTION STATUS", 1, 0xFF897DAB);

        // Small connector details match the cyan/violet frame treatment.
        const Point accent[] = {{185, 798}, {221, 798}, {229, 806}, {352, 806}};
        path(background_, accent, 4, 0xFF8C48D6, true);
        line(background_, {69, 218}, {1166, 218}, 0xFF264059, 1, 95);
        line(background_, {1240, 724}, {1532, 724}, 0xFF533E77, 1, 80);
        node(background_, 68, 186, 0xFF1BC9EC);
        node(background_, 1168, 798, 0xFF1BC9EC);
        node(background_, 1536, 186, 0xFFB46AF4);

        line(background_, {56, 824}, {1544, 824}, 0xFF314765, 1, 85);
        text(background_, 56, 877, "PERCEPTION SYSTEM / LOCAL VISION DISPLAY", 1, 0xFF4D6987);
        fillChamfer(background_, {1248, 838, 38, 25}, 4, 0xFF122137);
        centeredText(background_, "F11", {1248, 838, 38, 25}, 1, 0xFFA6C7DE);
        text(background_, 1296, 847, "FULLSCREEN", 1, 0xFF7794B0);
        fillChamfer(background_, {1450, 838, 25, 25}, 4, 0xFF122137);
        centeredText(background_, "Q", {1450, 838, 25, 25}, 1, 0xFFA6C7DE);
        text(background_, 1485, 847, "EXIT", 1, 0xFF7794B0);
    }

    void drawLink(int x, int y, const char* name, bool live, bool recorded) {
        const uint32_t color = recorded ? 0xFFC8A66D : live ? 0xFF53D8D2 : 0xFF58687C;
        circle(frame_, x, y, 8.f, color, 12);
        circle(frame_, x, y, 3.f, color, 235);
        char label[24];
        std::snprintf(label, sizeof(label), "%s / %s", name,
                      recorded ? "REC" : live ? "LIVE" : "WAIT");
        text(frame_, x + 14, y - 6, label, 2, color);
    }

    void drawVehiclePlaceholder() {
        const Point body[] = {{1352, 296}, {1370, 278}, {1402, 278},
            {1420, 296}, {1426, 642}, {1404, 670}, {1368, 670},
            {1346, 642}, {1352, 296}};
        path(frame_, body, 9, 0xFF356784, false);
        centeredText(frame_, "VEHICLE VIEW", {1238, 460, 296, 40}, 1, 0xFF6E859E);
    }
};

} // namespace perception_ui

#endif
