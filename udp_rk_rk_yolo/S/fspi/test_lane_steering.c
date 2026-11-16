/* Verify the adapter from current vision metadata to the restored controller. */
#define main vehicle_control_program_main
#include "csrc/main.c"
#undef main
#include <assert.h>

static CurveType lane_type = CURVE_RIGHT;
static uint32_t lane_angle = 23;
static int lane_result;

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
    return 0;
}
