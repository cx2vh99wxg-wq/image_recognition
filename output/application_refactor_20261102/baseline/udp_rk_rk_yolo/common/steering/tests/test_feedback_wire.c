#include "steering_feedback_wire.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
{
    steering_feedback_t input = {0x01020304, 7, 7, -12345, -5000, 12, 123456789};
    uint8_t packet[STEERING_FEEDBACK_WIRE_SIZE];
    assert(steering_feedback_encode(packet, sizeof(packet), &input) == STEER_OK);
    steering_feedback_t output = {0};
    assert(steering_feedback_decode(&output, packet, sizeof(packet)) == STEER_OK);
    assert(output.session_id == input.session_id && output.sequence == input.sequence);
    assert(output.angle_mdeg == -12345 && output.rate_mdps == -5000);
    assert(output.flags == 7 && output.sample_time_ns == 123456789 && output.source_age_ms == 12);
    for (unsigned i = 0; i < sizeof(packet); ++i) {
        packet[i] ^= 1;
        output = input;
        assert(steering_feedback_decode(&output, packet, sizeof(packet)) != STEER_OK);
        assert(output.angle_mdeg == input.angle_mdeg && output.session_id == input.session_id);
        packet[i] ^= 1;
    }
    assert(steering_feedback_decode(&output, packet, sizeof(packet) - 1) != STEER_OK);
    input.session_id = 0;
    assert(steering_feedback_encode(packet, sizeof(packet), &input) != STEER_OK);
    input.session_id = 0x01020304; input.flags = 2;
    assert(steering_feedback_encode(packet, sizeof(packet), &input) != STEER_OK);
    input.flags = 8;
    assert(steering_feedback_encode(packet, sizeof(packet), &input) != STEER_OK);
    input.flags = 7;
    assert(steering_feedback_encode(packet, sizeof(packet), &input) == STEER_OK);
    if (argc == 2) {
        FILE *file = fopen(argv[1], "wb"); assert(file);
        assert(fwrite(packet, 1, sizeof(packet), file) == sizeof(packet));
        assert(fclose(file) == 0);
    }
    puts("PASS shared protocol: signed wire round-trip, corruption, framing and flags");
    return 0;
}
