#include "yolo_integration.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Use the deployed model and its production overlay function on recorded frames.
// This executable reads files and writes images, with no control or IPC calls.
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    YOLOEngine engine = {};
    if (yolo_engine_init(&engine, argv[1], nullptr) != 0) return 3;
    std::ifstream list(argv[2]);
    std::string input, output;
    const int width = 320, height = 240;
    std::vector<unsigned char> bgr(width * height * 3);
    while (list >> input >> output)
    {
        std::ifstream frame(input, std::ios::binary);
        if (!frame.read(reinterpret_cast<char *>(bgr.data()), bgr.size())) return 4;
        yolopv2_result_t result = {};
        if (yolo_engine_detect(&engine, bgr.data(), width, height, &result) != 0) return 5;
        int count = yolo_overlay_lane_and_drivable(bgr.data(), width, height, &result);
        if (count < 0) return 6;
        FILE *file = std::fopen(output.c_str(), "wb");
        if (!file) return 7;
        std::fprintf(file, "P6\n%d %d\n255\n", width, height);
        for (int i = 0; i < width * height; ++i)
        {
            unsigned char rgb[3] = {bgr[3 * i + 2], bgr[3 * i + 1], bgr[3 * i]};
            if (std::fwrite(rgb, 1, 3, file) != 3) return 8;
        }
        std::fclose(file);
        std::cout << output << " lane_pixels=" << count << std::endl;
    }
    yolo_engine_cleanup(&engine);
    return 0;
}
