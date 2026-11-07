#ifndef TRAFFIC_FRAME_INPUT_H
#define TRAFFIC_FRAME_INPUT_H
#include "front_radial_preview.h"
#include "traffic_vision.h"

namespace traffic
{
inline bool copy_input_rgb(const uint8_t *mosaic, int width, int height, const InputView &view,
                            const m_lane::FrontRadialPreview *correction,
                            std::vector<uint8_t> &pixels)
{
    if (!correction)
        return copy_camera_rgb(mosaic, width, height, width * 3, view, pixels);
    if (!mosaic || !correction->enabled() || width != 640 || height != 480 ||
        correction->mosaic_width() != width || correction->mosaic_height() != height)
        return false;
    pixels.resize(320 * 240 * 3);
    correction->render(mosaic, pixels.data());
    if (view.order == PixelOrder::BGR)
        for (size_t p = 0; p < pixels.size(); p += 3)
            std::swap(pixels[p], pixels[p + 2]);
    return true;
}
}
#endif
