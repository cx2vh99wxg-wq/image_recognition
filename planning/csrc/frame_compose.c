#include "frame_compose.h"
#include <string.h>

int frame_compose_6ch(const void *local565, const void *remote565,
                      DisplayMode mode, uint8_t *canvas, size_t capacity,
                      int width, int height)
{
    const size_t bytes = (size_t)DISP_WIN_W * DISP_WIN_H * 4u;
    if (!canvas || capacity < bytes || width != DISP_WIN_W || height != DISP_WIN_H ||
        mode < DISPLAY_MODE_SPLIT || mode > DISPLAY_MODE_S2)
        return -1;
    memset(canvas, 0, bytes);
    const uint8_t *boards[2] = {remote565, local565};
    if (mode >= DISPLAY_MODE_M0) {
        int index=mode-DISPLAY_MODE_M0,cam=index%3;
        const uint8_t *src=boards[index/3];
        if (!src) return 0;
        /* Preserve 4:3 camera aspect ratio: 640x480 centered on the 960x480 canvas. */
        for (int y=0;y<480;y++) for (int x=0;x<640;x++) {
            const uint8_t *p=src+(((cam/2)*240+y/2)*640+(cam%2)*320+x/2)*2;
            unsigned n=p[0]|((unsigned)p[1]<<8);
            uint8_t *d=canvas+((size_t)y*width+x+160)*4;
            d[0]=(uint8_t)((n&31)*255/31);d[1]=(uint8_t)(((n>>5)&63)*255/63);d[2]=(uint8_t)(((n>>11)&31)*255/31);
        }
        return 0;
    }
    for (int row = 0; row < 2; ++row) {
        if (!boards[row] || mode == DISPLAY_MODE_BLANK ||
            (row == 0 && mode == DISPLAY_MODE_LOCAL_ONLY) ||
            (row == 1 && mode == DISPLAY_MODE_REMOTE_ONLY)) continue;
        for (int ch = 0; ch < 3; ++ch) {
            const int sx = (ch % 2) * DISP_CELL_W;
            const int sy = (ch / 2) * DISP_CELL_H;
            for (int y = 0; y < DISP_CELL_H; ++y) {
                const uint8_t *src = boards[row] + ((size_t)(sy + y) * IMG_WIDTH + sx) * 2u;
                uint8_t *dst = canvas + ((size_t)(row * DISP_CELL_H + y) * width + ch * DISP_CELL_W) * 4u;
                for (int x = 0; x < DISP_CELL_W; ++x) {
                    uint16_t p = (uint16_t)(src[0] | ((uint16_t)src[1] << 8));
                    uint8_t r = (uint8_t)((p >> 11) & 31), g = (uint8_t)((p >> 5) & 63), b = (uint8_t)(p & 31);
                    dst[0] = (uint8_t)((b << 3) | (b >> 2));
                    dst[1] = (uint8_t)((g << 2) | (g >> 4));
                    dst[2] = (uint8_t)((r << 3) | (r >> 2));
                    dst[3] = 0;
                    src += 2; dst += 4;
                }
            }
        }
    }
    return 0;
}
