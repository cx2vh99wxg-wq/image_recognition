#ifndef FRONT_PERSON_DATA_H
#define FRONT_PERSON_DATA_H
#include <stdint.h>
/* M corrected front camera, before lane overlay/IPM. Dedicated class-0 result;
 * no traffic-light color, lane decision or actuator state shares this record. */
enum { FRONT_PERSON_VALID=1, FRONT_PERSON_READY=2, FRONT_PERSON_FOUND=4,
       FRONT_PERSON_ROAD_VALID=8, FRONT_PERSON_IN_PATH=16,
       FRONT_PERSON_PATH_UNKNOWN=32, FRONT_PERSON_ROI_MODE=64 };
#define FRONT_PERSON_CONFIDENCE_THRESHOLD .15f
typedef struct {
    uint32_t session, source_id, count, flags;
    uint32_t quality, x, y, width;
    uint32_t height, deadline_lo, deadline_hi;
    uint32_t risk_count, outside_count, unknown_count, road_pixels, road_source_id;
} FrontPersonData;
/* Legacy producers remain fail-closed. An absent/ambiguous road mask never
 * turns a detected person into an off-road person. No detector result is a
 * separate fault handled by the guard's freshness/VALID checks. */
static inline unsigned front_person_hazard(const FrontPersonData *d)
{
    if (!(d->flags & FRONT_PERSON_FOUND)) return 0;
    if (!(d->flags & FRONT_PERSON_ROI_MODE)) return 1;
    return !(d->flags & FRONT_PERSON_ROAD_VALID) ||
           (d->flags & (FRONT_PERSON_IN_PATH | FRONT_PERSON_PATH_UNKNOWN));
}
#endif
