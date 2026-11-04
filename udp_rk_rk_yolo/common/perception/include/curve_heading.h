#ifndef CURVE_HEADING_H
#define CURVE_HEADING_H

/* The existing 0..360 transport field carries legacy angles directly.
 * Reserve 181..270 for a nonzero image heading measured from vertical. */
enum { CURVE_IMAGE_HEADING_OFFSET = 180, CURVE_IMAGE_HEADING_MAX_DEG = 90 };

static inline unsigned curve_encode_image_heading(unsigned degrees)
{
    return degrees ? CURVE_IMAGE_HEADING_OFFSET + degrees : 0;
}

static inline int curve_is_image_heading(unsigned angle)
{
    return angle > CURVE_IMAGE_HEADING_OFFSET &&
           angle <= CURVE_IMAGE_HEADING_OFFSET + CURVE_IMAGE_HEADING_MAX_DEG;
}

static inline unsigned curve_angle_degrees(unsigned angle)
{
    return curve_is_image_heading(angle) ? angle - CURVE_IMAGE_HEADING_OFFSET : angle;
}

#endif
