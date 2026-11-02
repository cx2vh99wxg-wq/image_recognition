#include "steering_feedback_wire.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    if (argc != 3 || strcmp(argv[1], "--file")) {
        puts("feedback_inspect --file packet.bin\nDecode one 48-byte STF1 file; no socket or device access.");
        return argc == 2 && !strcmp(argv[1], "--help") ? 0 : 2;
    }
    FILE *file = fopen(argv[2], "rb");
    if (!file) { perror("Open packet"); return 1; }
    uint8_t packet[STEERING_FEEDBACK_WIRE_SIZE + 1];
    size_t size = fread(packet, 1, sizeof(packet), file);
    int failed = ferror(file); fclose(file);
    steering_feedback_t feedback;
    if (failed || steering_feedback_decode(&feedback, packet, size) != STEER_OK) {
        fputs("Invalid STF1 packet.\n", stderr); return 1;
    }
    printf("session=%u seq=%u angle_deg=%.3f rate_dps=%.3f source_age_ms=%u "
           "source_time_ns=%" PRIu64 " valid=%u zero_valid=%u simulated=%u\n",
           feedback.session_id, feedback.sequence, feedback.angle_mdeg / 1000.,
           feedback.rate_mdps / 1000., feedback.source_age_ms, feedback.sample_time_ns,
           !!(feedback.flags & STEER_FEEDBACK_VALID),
           !!(feedback.flags & STEER_FEEDBACK_ZERO_VALID),
           !!(feedback.flags & STEER_FEEDBACK_SIMULATED));
    puts("Decoded only: freshness, network delay and suitability for control are not validated.");
    return 0;
}
