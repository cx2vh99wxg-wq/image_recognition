#ifndef TRAFFIC_LIGHT_DATA_H
#define TRAFFIC_LIGHT_DATA_H
#include <stdint.h>
/* Display/telemetry only. This namespace never owns a vehicle STOP request. */
enum { TRAFFIC_UNKNOWN=0,TRAFFIC_RED=1,TRAFFIC_YELLOW=2,TRAFFIC_GREEN=3 };
enum { TRAFFIC_BOX=1,TRAFFIC_CONFIRMED=2,TRAFFIC_TRACKED=4,TRAFFIC_MODEL_READY=8,
       TRAFFIC_LANE_PRIORITY=16,TRAFFIC_BUDGET=32,TRAFFIC_DISABLED=64 };
enum { TRAFFIC_OK=0,TRAFFIC_NO_BOX=1,TRAFFIC_SMALL=2,TRAFFIC_NO_COLOR=3,
       TRAFFIC_AMBIGUOUS=4,TRAFFIC_CONFIRMING=5,TRAFFIC_STALE=6,
       TRAFFIC_TRACK_LOST=7,TRAFFIC_MODEL_ERROR=8,TRAFFIC_DEFERRED=9 };
typedef struct {
    uint32_t session,source_id,detector_id,state;
    uint32_t raw_state,color_quality,detector_quality,flags;
    uint32_t x,y,width,height;
    uint32_t deadline_lo,deadline_hi,detection_age_ms,reason;
} TrafficLightData;
static inline const char* traffic_state_name(uint32_t state){
    return state==TRAFFIC_RED?"RED":state==TRAFFIC_GREEN?"GREEN":state==TRAFFIC_YELLOW?"YELLOW":"UNKNOWN";
}
#endif
