#include "paired_replay.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

template <typename T> bool read_file(const std::string &path, T *data, size_t count)
{
    std::ifstream stream(path, std::ios::binary);
    return bool(stream.read(reinterpret_cast<char *>(data), count * sizeof(T)));
}

int main(int argc, char **argv)
{
    if (argc != 3 || !private_ipc()) return 2;
    const std::string dir = argv[1];
    std::ifstream manifest(argv[2]);
    SharedMemoryManager camera = {};
    DisplayControlSharedMemory *control = nullptr;
    LineStyleIPC lane = {-1, nullptr};
    TrafficLightIPC signal = {-1, nullptr};
    FrontPersonIPC person = {-1, nullptr};
    display_people::IPC people;
    display_vehicles::IPC vehicles;
    crosswalk_display::IPC crosswalk;
    if (!manifest || shared_memory_init_udp(&camera, 1) || display_control_shm_init(&control, 1) ||
        curve_detection_shm_init(1) || line_ipc_open(&lane, LINE_STYLE_KEY, 1) ||
        traffic_ipc_open(&signal, 1) || front_person_ipc_open(&person, 1) ||
        !people.open(display_people::FRONT_KEY, true) || !vehicles.open(display_vehicles::KEY, true) ||
        !crosswalk.open(true)) return 3;
    __atomic_store_n(&control->mode, DISPLAY_MODE_FRONT_CENTER, __ATOMIC_RELEASE);
    __atomic_store_n(&control->valid, 1, __ATOMIC_RELEASE);
    std::cout << "FEED_READY" << std::endl;
    if (std::cin.get() != '\n') return 4;
    const auto pause = []() {std::this_thread::sleep_for(std::chrono::milliseconds(32));};
    std::string name, variant;
    std::vector<uint16_t> tile(320*240), image(640*480);
    while (manifest >> name >> variant)
    {
        if (variant != "ipm" && variant != "raw") return 5;
        PosterFrame data;
        if (!read_file(dir+'/'+name+".meta", &data, 1) ||
            !read_file(dir+'/'+name+'.'+variant, tile.data(), tile.size())) return 6;
        // Allow previous actors' display-only visual hold to expire between shots.
        const LineStyleData no_lane = {};
        const TrafficLightData no_signal = {};
        const FrontPersonData no_person = {};
        line_ipc_write(&lane, &no_lane);
        traffic_ipc_write(&signal, &no_signal);
        front_person_ipc_write(&person, &no_person);
        curve_detection_update(0, CURVE_NONE, 0, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(1250));
        for (unsigned y=0;y<240;++y)
            std::copy_n(tile.data()+y*320, 320, image.data()+(y+240)*640+320);

        // Convert the recorded freshness interval to the S board's monotonic clock.
        // No fields that describe the detected scene are changed.
        const auto refresh = [&] {
            const uint64_t now=traffic_clock_ns();
            if (data.curve_id) curve_detection_update(data.curve_id, (CurveType)data.curve,
                                                    data.confidence, data.angle);
            if (data.lane.source_id) line_set_deadline(&data.lane, now+LINE_STYLE_TTL_NS);
            line_ipc_write(&lane, &data.lane);
            if (data.signal.source_id) traffic_set_deadline(&data.signal, now+TRAFFIC_TTL_NS);
            traffic_ipc_write(&signal, &data.signal);
            if (data.person.source_id) front_person_set_deadline(&data.person, now+FRONT_PERSON_TTL_NS);
            front_person_ipc_write(&person, &data.person);
            if (data.people.session) {
                display_people::expires(data.people, now+display_people::TTL);
                people.write(data.people);
            }
            if (data.vehicles.session) {
                display_vehicles::expires(data.vehicles, now+display_vehicles::TTL);
                vehicles.write(data.vehicles);
            }
            if (data.crosswalk.session) {
                crosswalk_display::expires(data.crosswalk, now+crosswalk_display::TTL);
                crosswalk.write(data.crosswalk);
            }
            return shared_memory_write_image(&camera, reinterpret_cast<const uint8_t *>(image.data()), 640, 480);
        };
        for (unsigned tick = 0; tick < 38; ++tick)
        {
            if (refresh()) return 7;
            if (tick == 12) std::cout << "CAPTURE " << name << ' ' << variant << std::endl;
            pause();
        }
    }
    shared_memory_cleanup(&camera);
    display_control_shm_cleanup(control, 1);
    curve_detection_shm_cleanup();
    line_ipc_close(&lane);
    traffic_ipc_close(&signal);
    front_person_ipc_close(&person);
}
