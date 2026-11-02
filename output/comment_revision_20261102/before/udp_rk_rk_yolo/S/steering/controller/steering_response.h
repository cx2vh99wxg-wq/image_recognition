#ifndef STEERING_RESPONSE_H
#define STEERING_RESPONSE_H
#include "steering_types.h"
#define STEERING_RESPONSE_MAX_CELLS 128u
#define STEERING_RESPONSE_MAX_OBSERVATIONS 64u
typedef struct
{
    steer_direction_t direction;
    unsigned duration_us, priority, n, moved, observations;
    float min_deg, max_deg, mean_deg, start_min_deg, start_max_deg;
    float responses_deg[STEERING_RESPONSE_MAX_OBSERVATIONS];
    unsigned partitioned;
    float zone_low_deg, zone_high_deg, peak_low_deg, peak_high_deg;
} steering_response_cell_t;
typedef struct
{
    unsigned count;
    steering_response_cell_t cells[STEERING_RESPONSE_MAX_CELLS];
    /* V3 is an explicitly qualified continuous coarse action plus the same
     * measured <=60ms fine table. V1/V2 leave these fields zero. */
    unsigned burst_max_us, burst_coast_ms[2], burst_guard_ms[2];
    float burst_coast_cap_deg[2], burst_guard_cap_deg[2];
    uint32_t burst_session;
    float burst_zero_raw_deg;
} steering_response_model_t;
typedef struct
{
    steer_direction_t direction;
    unsigned duration_us;
    float guard_deg, expected_error_deg;
} steering_response_choice_t;
/* Validated compact export of STEER_BENCH_RESPONSE_V1, not a fitted straight line. */
steer_result_t steering_response_load(steering_response_model_t *model, const char *path);
steer_result_t steering_response_choose(const steering_response_model_t *model, float current,
                                        float target, float tolerance, float minimum, float maximum,
                                        unsigned max_us, unsigned minimum_samples,
                                        unsigned priority, steering_response_choice_t *choice);
steer_result_t steering_response_choose_reposition(const steering_response_model_t *model,
                                                   float current, float target, float tolerance,
                                                   float minimum, float maximum, unsigned max_us,
                                                   unsigned minimum_samples, unsigned priority,
                                                   steering_response_choice_t *choice);
#endif
