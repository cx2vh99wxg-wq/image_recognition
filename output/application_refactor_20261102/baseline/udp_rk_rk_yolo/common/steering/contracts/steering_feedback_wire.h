#ifndef STEERING_FEEDBACK_WIRE_H
#define STEERING_FEEDBACK_WIRE_H
#include <stddef.h>
#include "steering_types.h"

#define STEERING_FEEDBACK_WIRE_VERSION 1u
#define STEERING_FEEDBACK_WIRE_SIZE 48u

/* STF1, big-endian integers, IEEE CRC32 over bytes 0..43.
 * Decoding validates framing/integrity only, NOT freshness, zero, or simulation.
 * On failure, destination is unchanged. See contracts/README.md. */
steer_result_t steering_feedback_encode(uint8_t *output, size_t capacity,
                                      const steering_feedback_t *feedback);
steer_result_t steering_feedback_decode(steering_feedback_t *feedback,
                                      const uint8_t *packet, size_t size);
#endif
