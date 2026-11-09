#include "latest_detection.h"
#include <cassert>
#include <cstdio>
#include <future>

int main()
{
    assert(s_vision::configured_submit_interval(nullptr)==3);
    assert(s_vision::configured_submit_interval("1")==1);
    assert(s_vision::configured_submit_interval("2")==2);
    for(const char* bad:{"0","6","-1","2ms",""})assert(s_vision::configured_submit_interval(bad)==3);
    std::promise<void> entered, release;
    auto first_started = entered.get_future();
    auto release_first = release.get_future();
    std::vector<int> inputs;
    s_vision::LatestDetection worker(1, [&](const uint8_t *data, detect_result_group_t *results) {
        const int value = data[0];
        inputs.push_back(value);
        if (value == 1)
        {
            entered.set_value();
            release_first.wait();
            assert(data[0] == 1);
        }
        results->person_brake_count = value;
        return value == 5 ? -1 : 0;
    });
    auto await = [&](uint64_t timestamp) {
        for (int i = 0; i < 1000; ++i)
        {
            auto result = worker.latest();
            if (result.captured == timestamp)
                return result;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        assert(false);
        return s_vision::DetectionSnapshot{};
    };
    assert(!worker.latest().fresh(1));
    uint8_t pixels = 1;
    worker.submit(&pixels, 1);
    const auto started = first_started.wait_for(std::chrono::seconds(1));
    assert(started == std::future_status::ready);
    for (pixels = 2; pixels <= 4; ++pixels)
        worker.submit(&pixels, pixels);
    worker.submit(&pixels, 4); // duplicate must not replace the waiting frame
    release.set_value();
    const auto result = await(4);
    assert(result.results.person_brake_count == 4 && result.fresh(4));
    assert(!result.fresh(3) && !result.fresh(350000005));
    worker.submit(&pixels, 5);
    assert(!await(5).fresh(5));
    pixels = 6;
    worker.submit(&pixels, 6);
    assert(await(6).fresh(6));
    worker.stop();
    assert(!worker.latest().fresh(6));
    assert(inputs == std::vector<int>({1, 4, 5, 6}));
    puts("S ASYNC PASS: latest-only input, immutable inference pixels, failed/stale result, recovery and shutdown");
}
