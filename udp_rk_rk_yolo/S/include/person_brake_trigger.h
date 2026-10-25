#ifndef PERSON_BRAKE_TRIGGER_H
#define PERSON_BRAKE_TRIGGER_H

#include <math.h>
#include <string.h>

/* Use the unrounded model score: exactly 40% does not trigger a brake. */
#define PERSON_BRAKE_CONFIDENCE_THRESHOLD 0.40f

static inline int person_brake_should_trigger(const char *name, float confidence)
{
    return name != NULL && strcmp(name, "person") == 0 &&
           isfinite(confidence) && confidence > PERSON_BRAKE_CONFIDENCE_THRESHOLD;
}

#endif
