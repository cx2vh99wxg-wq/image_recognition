#ifndef STEREO_TEST_FRAME_H
#define STEREO_TEST_FRAME_H
#include <stdint.h>
/* stereo_j8 bitstream: source quadrants 0=CAM1, 1=empty, 2=CAM2.
 * Known unconnected slots are black. This is not camera presence detection. */
void stereo_test_frame_prepare(uint8_t *frame565, int swap_eyes);
#endif
