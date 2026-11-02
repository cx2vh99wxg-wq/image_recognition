#ifndef STEERING_FEEDBACK_H
#define STEERING_FEEDBACK_H
#include "steering_types.h"

/* Single-process, single-owner receiver. No FSPI/control writes. */
steer_result_t steering_feedback_open(const char *bind_ip, const char *source_ip, unsigned port,
                                      unsigned timeout_ms);
steer_result_t steering_feedback_poll(void);
void steering_feedback_close(void);
/* Portable packet ingestion for deterministic tests and an existing receiver. */
steer_result_t steering_feedback_ingest(const uint8_t *packet, unsigned size, uint64_t received_ns);
steer_result_t steering_feedback_read(steering_feedback_t *feedback);
#endif
