#ifndef TRAFFIC_OFFLINE_GUARD_H
#define TRAFFIC_OFFLINE_GUARD_H
#include <dirent.h>
#include <unistd.h>
#include <cstdio>
#include <string>
// 采集程序占用 NPU 时，检查工具不能并行启动另一份 NPU 工作负载。
inline bool traffic_capture_is_running()
{
    DIR *dir = opendir("/proc");
    if (!dir)
        return true;
    bool found = false;
    dirent *entry;
    while ((entry = readdir(dir)))
    {
        if (entry->d_name[0] < '0' || entry->d_name[0] > '9')
            continue;
        char link[512], exe[1024];
        snprintf(link, sizeof(link), "/proc/%s/exe", entry->d_name);
        ssize_t n = readlink(link, exe, sizeof(exe) - 1);
        if (n <= 0)
            continue;
        exe[n] = 0;
        std::string path(exe);
        if (path.find("/build/pcie_capture") != std::string::npos ||
            path.find("/pcie/build/app") != std::string::npos)
        {
            found = true;
            break;
        }
    }
    closedir(dir);
    return found;
}
#endif
