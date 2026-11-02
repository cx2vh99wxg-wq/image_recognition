#ifndef PERSON_BRAKE_TRIGGER_H
#define PERSON_BRAKE_TRIGGER_H

#include <math.h>
#include <string.h>

/* 使用模型未取整的置信度，达到 15% 即进入人员刹停汇总。 */
#define PERSON_BRAKE_CONFIDENCE_THRESHOLD 0.15f

static inline int person_brake_should_trigger(const char *name, float confidence)
{
    return name != NULL && strcmp(name, "person") == 0 && isfinite(confidence) &&
           confidence >= PERSON_BRAKE_CONFIDENCE_THRESHOLD;
}

#endif
