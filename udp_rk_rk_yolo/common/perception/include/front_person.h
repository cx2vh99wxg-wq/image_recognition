#ifndef FRONT_PERSON_DATA_H
#define FRONT_PERSON_DATA_H
#include <stdint.h>
/* M corrected front camera, before lane overlay/IPM. Dedicated class-0 result;
 * no traffic-light color, lane decision or actuator state shares this record. */
enum { FRONT_PERSON_VALID=1, FRONT_PERSON_READY=2, FRONT_PERSON_FOUND=4 };
#define FRONT_PERSON_CONFIDENCE_THRESHOLD .15f
typedef struct {
    uint32_t session, source_id, count, flags;
    uint32_t quality, x, y, width;
    uint32_t height, deadline_lo, deadline_hi, reserved[5];
} FrontPersonData;
#endif
