#pragma once

#include "shared_memory.h"
#include "crosswalk_display.h"
#include "display_people.h"
#include "display_vehicles.h"
#include "front_person_ipc.h"
#include "line_style_ipc.h"
#include "traffic_light_ipc.h"
#include <fstream>
#include <string>
#include <unistd.h>

struct PosterFrame
{
    uint32_t id = 0, curve_id = 0, curve = 0, angle = 0, confidence = 0, points = 0;
    LineStyleData lane = {};
    TrafficLightData signal = {};
    FrontPersonData person = {};
    display_people::Snapshot people = {};
    display_vehicles::Snapshot vehicles = {};
    crosswalk_display::Snapshot crosswalk = {};
};

inline bool private_ipc()
{
    char self[64] = {}, init[64] = {};
    return readlink("/proc/self/ns/ipc", self, 63) > 0 &&
           readlink("/proc/1/ns/ipc", init, 63) > 0 && std::string(self) != init;
}

template <typename T> bool write_file(const std::string &path, const T *data, size_t count)
{
    std::ofstream stream(path, std::ios::binary);
    return bool(stream.write(reinterpret_cast<const char *>(data), count * sizeof(T)));
}
