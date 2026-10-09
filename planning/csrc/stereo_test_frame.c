#include "stereo_test_frame.h"
#include "driving_config.h"
#include <string.h>

void stereo_test_frame_prepare(uint8_t *frame565, int swap_eyes)
{
    if (!frame565) return;
    for (int y = 0; y < DISP_CELL_H; ++y) {
        uint8_t *top = frame565 + (size_t)y * IMG_WIDTH * 2;
        uint8_t *bottom = top + (size_t)DISP_CELL_H * IMG_WIDTH * 2;
        memset(top + DISP_CELL_W * 2, 0, DISP_CELL_W * 2);
        memset(bottom + DISP_CELL_W * 2, 0, DISP_CELL_W * 2);
        if (swap_eyes) {
            uint8_t tmp[DISP_CELL_W * 2];
            memcpy(tmp, top, sizeof(tmp));
            memcpy(top, bottom, sizeof(tmp));
            memcpy(bottom, tmp, sizeof(tmp));
        }
    }
}
