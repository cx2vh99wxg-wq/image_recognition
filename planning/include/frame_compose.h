#ifndef FRAME_COMPOSE_H
#define FRAME_COMPOSE_H
#include <stddef.h>
#include <stdint.h>
#include "driving_config.h"

/* Each input is one board's 640x480 RGB565 frame. Output is B,G,R,X.
 * Only the three active quadrants are copied: M on top, S on bottom.
 * A missing board is black. The unused fourth quadrant is never displayed. */
int frame_compose_6ch(const void *local565, const void *remote565,
                      DisplayMode mode, uint8_t *canvas, size_t capacity,
                      int width, int height);
#endif
