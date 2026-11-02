#ifndef BRAKE_POLICY_H
#define BRAKE_POLICY_H

#include <stdint.h>

/* The detector updates last_update with CLOCK_MONOTONIC seconds every frame.
 * This policy controls the software request, not the FPGA STOP latch. */
#define BRAKE_STATUS_TIMEOUT_SECONDS 2u

static inline int brake_required(int enabled, int status_valid,
                                 int person_detected, int brake_enable,
                                 uint32_t last_update, uint32_t now)
{
    if (!enabled) {
        return 0;
    }

    /* Missing or stale detection data keeps the software request asserted.
     * Clearing a request does not unlock STOP; manual REARM owns that latch
     * and may explicitly override the currently asserted request. */
    if (!status_valid || last_update == 0 || last_update > now ||
        now - last_update >= BRAKE_STATUS_TIMEOUT_SECONDS) {
        return 1;
    }

    return person_detected != 0 || brake_enable == 0;
}

#endif
