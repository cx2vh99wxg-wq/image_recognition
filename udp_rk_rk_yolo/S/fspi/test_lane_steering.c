/* Verify the adapter from current vision metadata to the restored controller. */
#define main vehicle_control_program_main
#include "csrc/main.c"
#undef main
#include <assert.h>

static CurveType lane_type = CURVE_RIGHT;
static uint32_t lane_angle = 23;
static int lane_result;
static uint8_t autopilot_status, heartbeat;
static unsigned writes;
static int read_result, write_result;
static int center_sampling;

int steering_runtime_center_sampling(void)
{
    return center_sampling;
}

int fspi_read_autopilot_status(uint8_t *status)
{
    *status = autopilot_status;
    return read_result;
}

int fspi_write_data(uint16_t address, uint8_t value)
{
    assert(address == AUTOPILOT_HEARTBEAT_REG);
    heartbeat = value;
    ++writes;
    return write_result;
}

int curve_detection_read(uint32_t *frame, CurveType *type, uint32_t *quality, uint32_t *angle)
{
    if (lane_result)
        return lane_result;
    *frame = 41;
    *type = lane_type;
    *quality = 85;
    *angle = lane_angle;
    return 0;
}

int main(void)
{
    uint32_t frame;
    unsigned type, confidence, degrees;
    assert(read_steering_lane(NULL, &frame, &type, &confidence, &degrees) == 0);
    assert(frame == 41 && type == CURVE_RIGHT && confidence == 85 && degrees == 23);

    lane_type = CURVE_LEFT;
    lane_angle = curve_encode_image_heading(45);
    assert(read_steering_lane(NULL, &frame, &type, &confidence, &degrees) == 0);
    assert(type == CURVE_LEFT && degrees == 45);

    lane_result = -1;
    assert(read_steering_lane(NULL, &frame, &type, &confidence, &degrees) == -1);
    puts("PASS steering lane input: legacy and tagged angles; invalid input rejected");

    AutopilotGate gate = {0};
    uint64_t now = 1000000000ULL;
    const uint64_t poll_ns = 40000000ULL;
    assert(!poll_autopilot_gate(&gate, now, 1) && writes == 0);
    now += poll_ns;
    autopilot_status = AUTOPILOT_REQUEST_ON | 3;
    assert(!poll_autopilot_gate(&gate, now, 1) && heartbeat == 0xb1);
    assert(!poll_autopilot_gate(&gate, now + 1000, 1) && writes == 1);
    now += poll_ns;
    autopilot_status |= AUTOPILOT_ACTIVE;
    assert(poll_autopilot_gate(&gate, now, 1) && heartbeat == 0xb2);
    now += poll_ns;
    read_result = -1;
    assert(!poll_autopilot_gate(&gate, now, 1) && writes == 2);
    now += poll_ns;
    read_result = 0;
    write_result = -1;
    assert(!poll_autopilot_gate(&gate, now, 1) && heartbeat == 0xb3);
    now += poll_ns;
    write_result = 0;
    assert(poll_autopilot_gate(&gate, now, 1) && heartbeat == 0xb3);
    assert(!poll_autopilot_gate(&gate, now + 1000, 0) && writes == 4);
    now += poll_ns;
    autopilot_status = 3;
    assert(!poll_autopilot_gate(&gate, now, 1) && writes == 4);
    assert(gate.switch_off);
    gate.inhibit_until_off = 1;
    center_sampling = 1;
    now += poll_ns;
    assert(!poll_autopilot_gate(&gate, now, 1) && gate.inhibit_until_off);
    now += poll_ns;
    autopilot_status = AUTOPILOT_ACTIVE | AUTOPILOT_REQUEST_ON | 3;
    assert(!poll_autopilot_gate(&gate, now, 1) && writes == 4);
    center_sampling = 0;
    now += poll_ns;
    assert(!poll_autopilot_gate(&gate, now, 1) && gate.inhibit_until_off);
    autopilot_status = 3;
    now += poll_ns;
    assert(!poll_autopilot_gate(&gate, now, 1) && !gate.inhibit_until_off);
    puts("PASS Autopilot: confirmed ON required, OFF and SPI failures suppress lane targets");
    return 0;
}
