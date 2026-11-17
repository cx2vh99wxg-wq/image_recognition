#ifndef STEERING_CENTER_H
#define STEERING_CENTER_H

#include "camera_views.h"
#include "steering_center_status.h"
#include "steering_types.h"

enum { CENTER_SAMPLES = 60 };
#define CENTER_SAMPLE_GAP_NS 100000000ULL
#define CENTER_OBSERVE_NS 1800000000ULL
#define CENTER_STOP_HOLD_NS 3000000000ULL
#define CENTER_CONFIRM_NS 30000000000ULL
#define CENTER_CONTROL_TTL_NS 250000000ULL

typedef struct
{
    unsigned status;
    uint32_t session;
    uint64_t hold_start, confirm_deadline, control_ns;
    uint64_t sample_start, latest_ns, sample_time_ns;
    uint32_t sequence;
    unsigned count, slot;
    int32_t low, high, angles[CENTER_SAMPLES];
} SteeringCenter;

static inline void steering_center_set(SteeringCenter *center, unsigned status)
{
    center->status = status;
    center->hold_start = center->confirm_deadline = center->sample_start = center->latest_ns = 0;
    center->count = center->slot = 0;
}

/* A new session or a range fault requires explicit confirmation while parked with Autopilot off. */
static inline void steering_center_key(SteeringCenter *center, uint8_t key, uint64_t now)
{
    if (center->status == STEERING_CENTER_SAMPLING)
    {
        if (key != 0 && key != 0x99)
            steering_center_set(center, STEERING_CENTER_RETRY);
        return;
    }
    if (center->status == STEERING_CENTER_RELEASE_STOP)
    {
        if (key == 0xee)
            return;
        if (key == 0)
        {
            center->status = STEERING_CENTER_TAP_OVERVIEW;
            center->confirm_deadline = now + CENTER_CONFIRM_NS;
            return;
        }
    }
    else if (center->status == STEERING_CENTER_TAP_OVERVIEW && now < center->confirm_deadline)
    {
        if (key == DISPLAY_MODE_ALL_CAMERAS)
        {
            steering_center_set(center, STEERING_CENTER_SAMPLING);
            center->sample_start = now;
            return;
        }
        if (key == 0 || camera_mode_normalize(key, 0))
            return;
    }
    if (center->status == STEERING_CENTER_RETRY && key != 0xee)
        return;
    if (center->status != STEERING_CENTER_HOLD_STOP)
        steering_center_set(center, STEERING_CENTER_HOLD_STOP);
    if (key != 0xee)
        center->hold_start = 0;
    else if (!center->hold_start)
        center->hold_start = now;
    else if (now - center->hold_start >= CENTER_STOP_HOLD_NS)
        center->status = STEERING_CENTER_RELEASE_STOP;
}

/* The value is only a zero candidate after 60 distinct fresh readings within
 * a continuous 1.8 s stationary observation. Duplicate frames never count. */
static inline int steering_center_sample(SteeringCenter *center, const steering_feedback_t *f,
                                        uint64_t now)
{
    if (f->session_id != center->session ||
        !(f->flags & STEER_FEEDBACK_VALID) || (f->flags & STEER_FEEDBACK_SIMULATED) ||
        !f->sample_time_ns || f->source_age_ms >= 100 ||
        f->rate_mdps < -2000 || f->rate_mdps > 2000 ||
        f->angle_mdeg < -2000000 || f->angle_mdeg > 2000000 ||
        (center->count && now - center->latest_ns >= CENTER_SAMPLE_GAP_NS))
        return -1;
    if (center->count)
    {
        if (f->sequence == center->sequence)
            return 0;
        if ((int32_t)(f->sequence - center->sequence) <= 0 ||
            f->sample_time_ns <= center->sample_time_ns ||
            f->sample_time_ns - center->sample_time_ns >= CENTER_SAMPLE_GAP_NS)
            return -1;
        if (f->angle_mdeg < center->low)
            center->low = f->angle_mdeg;
        if (f->angle_mdeg > center->high)
            center->high = f->angle_mdeg;
        if (center->high - center->low > 400)
            return -1;
    }
    else
    {
        center->low = center->high = f->angle_mdeg;
        center->sample_start = now;
    }
    center->latest_ns = now;
    center->sequence = f->sequence;
    center->sample_time_ns = f->sample_time_ns;
    center->angles[center->slot] = f->angle_mdeg;
    center->slot = (center->slot + 1) % CENTER_SAMPLES;
    if (center->count < CENTER_SAMPLES)
        ++center->count;
    return center->count == CENTER_SAMPLES && now - center->sample_start >= CENTER_OBSERVE_NS;
}

#endif
