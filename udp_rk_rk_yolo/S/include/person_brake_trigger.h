#ifndef PERSON_BRAKE_TRIGGER_H
#define PERSON_BRAKE_TRIGGER_H

#include <math.h>
#include <string.h>

/* 使用模型未取整的置信度，达到 25% 即进入后向行人刹停汇总。 */
#define PERSON_BRAKE_CONFIDENCE_THRESHOLD 0.25f

static inline int person_brake_should_trigger(const char *name, float confidence)
{
    return name != NULL && strcmp(name, "person") == 0 && isfinite(confidence) &&
           confidence >= PERSON_BRAKE_CONFIDENCE_THRESHOLD;
}

#endif
