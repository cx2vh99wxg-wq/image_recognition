/* Bench diagnostic: recorded camera pixels -> unchanged RKNN -> normal
 * worker/SHM/UDP. NOT live camera capture, NOT normal startup. No SPI access.
 * Run only with capture stopped and the vehicle fixed; <=30s then exits. */
#include "latest_lane_worker.h"
#include "shared_memory.h"
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <thread>
#include <vector>
static volatile sig_atomic_t stopped;
static void stop_replay(int)
{
    stopped = 1;
}
int main(int argc, char **argv)
{
    if (argc != 5 || std::strcmp(argv[1], "--bench-replay"))
    {
        std::fprintf(
            stderr,
            "curve_frame_replay --bench-replay RAW_P6 MODEL_RKNN SECONDS(1..30)\nNot live capture; stop capture and fix vehicle first.\n");
        return 2;
    }
    char *end = nullptr;
    unsigned seconds = (unsigned)std::strtoul(argv[4], &end, 10);
    if (!seconds || seconds > 30 || !end || *end ||
        std::system("pgrep -x pcie_capture >/dev/null") != 256)
        return 2;
    FILE *file = std::fopen(argv[2], "rb");
    if (!file)
        return 2;
    char magic[3];
    int w = 0, h = 0, maximum = 0;
    bool ok = std::fscanf(file, "%2s %d %d %d", magic, &w, &h, &maximum) == 4 &&
              !std::strcmp(magic, "P6") && w == 640 && h == 480 && maximum == 255;
    if (!ok)
    {
        std::fclose(file);
        return 2;
    }
    std::fgetc(file);
    std::vector<uint8_t> raw((size_t)w * h * 3), display(raw.size());
    ok = std::fread(raw.data(), 1, raw.size(), file) == raw.size();
    std::fclose(file);
    if (!ok)
        return 2;
    setenv("LANE_CURVE_MODE", "ego-boundaries", 1);
    YOLOEngine engine = {};
    if (yolo_engine_init(&engine, argv[3], nullptr))
        return 1;
    SharedMemoryManager image = {};
    if (shared_memory_init(&image, SHM_TYPE_PCIE, 1))
    {
        yolo_engine_cleanup(&engine);
        return 1;
    }
    if (curve_detection_shm_init(1))
    {
        shared_memory_cleanup(&image);
        yolo_engine_cleanup(&engine);
        return 1;
    }
    std::signal(SIGTERM, stop_replay);
    std::signal(SIGINT, stop_replay);
    setvbuf(stdout, nullptr, _IOLBF, 0);
    std::printf(
        "RECORDED_FRAME_REPLAY_ACTIVE input=%s live_camera=0 classifier_and_NPU_real=1 seconds=%u\n",
        argv[2], seconds);
    uint32_t id = 0, last = 0;
    uint64_t last_capture = 0;
    bool fresh_before = false;
    std::vector<uint16_t> pixels((size_t)w * h);
    {
        m_lane::LatestWorker worker(&engine, w, h);
        auto start = std::chrono::steady_clock::now(), next = start;
        while (!stopped && std::chrono::steady_clock::now() - start < std::chrono::seconds(seconds))
        {
            uint64_t captured = m_lane::clock_ns();
            worker.submit(raw.data(), ++id, captured);
            auto result = worker.latest();
            bool fresh = result && result->fresh(m_lane::clock_ns()) && !worker.failed();
            display = raw;
            if (fresh)
            {
                auto mask = result->masks();
                yolo_overlay_lane_and_drivable(display.data(), w, h, &mask);
                if (!fresh_before || last != result->source_id)
                {
                    last = (uint32_t)result->source_id;
                    last_capture = result->captured_ns;
                    curve_detection_update_source(last, (CurveType)result->curve.curve_type,
                                                  result->curve.confidence, result->curve.angle,
                                                  last_capture);
                    std::printf(
                        "REPLAY_NN source=%u type=%u confidence=%u angle=%u replayed_pixels=1\n",
                        last, result->curve.curve_type, result->curve.confidence,
                        result->curve.angle);
                }
            }
            else if (fresh_before)
                curve_detection_update_source(last, CURVE_NONE, 0, 0, last_capture);
            fresh_before = fresh;
            for (size_t i = 0; i < pixels.size(); ++i)
            {
                const uint8_t *p = display.data() + 3 * i;
                pixels[i] = (uint16_t)(((p[0] >> 3) << 11) | ((p[1] >> 2) << 5) | (p[2] >> 3));
            }
            if (pcie_shared_memory_write_frame(&image, (const uint8_t *)pixels.data(), w, h))
            {
                stopped = 1;
                ok = false;
            }
            next += std::chrono::microseconds(33333);
            std::this_thread::sleep_until(next);
        }
        worker.stop();
    }
    curve_detection_shm_cleanup();
    shared_memory_cleanup(&image);
    yolo_engine_cleanup(&engine);
    std::puts("RECORDED_FRAME_REPLAY_COMPLETE live_capture_must_be_restored=1");
    return ok ? 0 : 1;
}
