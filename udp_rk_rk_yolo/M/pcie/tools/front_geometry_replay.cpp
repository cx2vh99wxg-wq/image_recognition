#include "front_radial.h"
#include "ground_projection.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

int main(int argc, char **argv)
{
    if (argc != 4 && argc != 5)
    {
        std::fprintf(stderr, "Usage: %s front.ppm corrected.ppm lambda [ground_rect]\n", argv[0]);
        return 2;
    }
    const auto radial = m_lane::FrontRadial::from_text(argv[3]);
    const m_lane::GroundProjection ground(argc == 5 ? argv[4] : nullptr);
    std::ifstream input(argv[1], std::ios::binary);
    std::string format;
    int width = 0, height = 0, maximum = 0;
    if (!radial.valid() || !ground.valid() || !(input >> format >> width >> height >> maximum) || format != "P6" ||
        maximum != 255 || width < 2 || height < 2 || width > 4096 || height > 4096)
        return 2;
    const int delimiter = input.get();
    if (delimiter == '\r' && input.peek() == '\n')
        input.get();
    else if (delimiter != '\n' && delimiter != ' ' && delimiter != '\t')
        return 2;
    std::vector<char> pixels((size_t)width * height * 3);
    if (!input.read(pixels.data(), pixels.size()))
        return 2;
    const int output_width = width;
    const int output_height = ground.enabled() ? std::lround(width * ground.aspect()) : height;
    if (output_height < 2 || output_height > 4096)
        return 2;
    std::vector<char> corrected((size_t)output_width * output_height * 3, 0);
    for (int y = 0; y < output_height; ++y)
        for (int x = 0; x < output_width; ++x)
        {
            m_lane::ImagePoint source{(double)x, (double)y};
            if (ground.enabled())
            {
                source = {source.x / (output_width - 1), source.y / (output_height - 1)};
                if (!ground.image_at(source, width, height))
                    continue;
            }
            if (!radial.distort(source, width, height))
                continue;
            const long sx = std::lround(source.x), sy = std::lround(source.y);
            if (sx >= 0 && sx < width && sy >= 0 && sy < height)
                for (int c = 0; c < 3; ++c)
                    corrected[((size_t)y * output_width + x) * 3 + c] =
                        pixels[((size_t)sy * width + sx) * 3 + c];
        }
    std::ofstream output(argv[2], std::ios::binary);
    output << "P6\n" << output_width << ' ' << output_height << "\n255\n";
    output.write(corrected.data(), corrected.size());
    return output ? 0 : 1;
}
