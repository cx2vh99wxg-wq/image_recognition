#include "steering_api.h"
#include "steering_predictive_stop.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

static steering_controller_io_t io;
static steering_config_t config;
static steering_actuator_t actuator;
static steering_state_t state = {NAN, NAN, NAN, STEERING_NOT_READY, STEER_ERR_NOT_READY, 0};
static int initialized, reference_valid, have_target, after_pulse;
static uint32_t reference_session, last_sequence;
static float zero_raw, before_angle, before_error, stable_low, stable_high, goal_tolerance;
static unsigned pulses, reversals, no_progress, stable_samples;
static unsigned reposition_used;
static int after_reposition;
static unsigned blocked_left_us, blocked_right_us;
static unsigned requested_total_us;
static int pulse_is_burst, planned_release_reason;
static steering_response_model_t choice_model;
static steer_direction_t previous_direction;
static uint64_t stable_since, settle_deadline, target_deadline, last_tick_ns, feedback_deadline;
static uint64_t stability_last_ns;
static void reset_stability(void);

static steer_result_t fail(steer_result_t rc, steering_status_t status)
{
    if (actuator.status == ACTUATOR_IDLE && actuator.direction == STEER_RELEASE &&
        state.status == status && state.last_error == rc)
        return rc;
    if (actuator.status == ACTUATOR_ACTIVE)
        reset_stability();
    steer_result_t released = steering_actuator_stop(&actuator);
    state.status = released == STEER_OK ? status : STEERING_FAULT;
    state.last_error = released == STEER_OK ? rc : released;
    return state.last_error;
}
static steer_result_t read_angle(steering_feedback_t *f)
{
    memset(f, 0, sizeof(*f));
    steer_result_t rc = io.read_feedback(io.context, f);
    state.feedback_valid = 0;
    if (rc != STEER_OK || !(f->flags & STEER_FEEDBACK_VALID) ||
        (f->flags & STEER_FEEDBACK_SIMULATED) || (f->flags & ~STEER_FEEDBACK_ALL_FLAGS) ||
        !f->session_id || !f->sample_time_ns || f->source_age_ms >= config.feedback_timeout_ms)
    {
        state.angle_deg = state.error_deg = NAN;
        return rc == STEER_OK ? STEER_ERR_STALE : rc;
    }
    if (reference_valid && f->session_id != reference_session)
    {
        reference_valid = 0;
        state.angle_deg = state.error_deg = NAN;
        return STEER_ERR_STALE;
    }
    if (!reference_valid)
        return STEER_ERR_NOT_READY;
    state.angle_deg = f->angle_mdeg / 1000.f - zero_raw;
    state.feedback_valid = 1;
    feedback_deadline = io.actuator.now_ns(io.actuator.context) +
                        (uint64_t)(config.feedback_timeout_ms - f->source_age_ms) * 1000000;
    state.error_deg = have_target ? state.target_deg - state.angle_deg : NAN;
    return state.angle_deg < config.min_angle_deg || state.angle_deg > config.max_angle_deg
               ? STEER_ERR_DEVICE
               : STEER_OK;
}
static void reset_stability(void)
{
    stable_since = 0;
    stable_samples = 0;
    stability_last_ns = 0;
    stable_low = 1e9f;
    stable_high = -1e9f;
    last_sequence = 0;
}
static void observe_stability(const steering_feedback_t *f, uint64_t now)
{
    if (f->sequence == last_sequence)
        return;
    if (stability_last_ns &&
        (now < stability_last_ns ||
         now - stability_last_ns >= (uint64_t)config.feedback_timeout_ms * 1000000))
        reset_stability();
    last_sequence = f->sequence;
    if (fabsf(f->rate_mdps / 1000.f) > 2)
    {
        reset_stability();
        return;
    }
    stability_last_ns = now;
    if (!stable_since)
        stable_since = now;
    stable_low = fminf(stable_low, state.angle_deg);
    stable_high = fmaxf(stable_high, state.angle_deg);
    ++stable_samples;
    if (stable_high - stable_low > .4f)
    {
        stable_since = now;
        stable_samples = 1;
        stable_low = stable_high = state.angle_deg;
    }
}
static void settling(uint64_t now, int reset)
{
    state.status = STEERING_SETTLING;
    if (reset)
        reset_stability();
    settle_deadline = now + UINT64_C(2500000000);
}
steer_result_t steering_bind(steering_controller_io_t binding)
{
    if (initialized || !binding.actuator.set_direction || !binding.actuator.now_ns ||
        !binding.read_feedback || !binding.permission || !binding.model || !binding.model->count ||
        !binding.max_pulses || binding.max_pulses > 20 || binding.settle_ms < 100 ||
        binding.settle_ms > 2000 || !binding.minimum_samples || binding.priority > 30)
        return STEER_ERR_ARGUMENT;
    io = binding;
    return STEER_OK;
}
steer_result_t steering_init(const steering_config_t *c)
{
    if (!c || !isfinite(c->min_angle_deg) || !isfinite(c->max_angle_deg) ||
        c->min_angle_deg >= c->max_angle_deg || c->min_angle_deg < -18 || c->max_angle_deg > 18 ||
        !isfinite(c->tolerance_deg) || c->tolerance_deg < .1f || c->tolerance_deg > 3 ||
        !c->feedback_timeout_ms || c->feedback_timeout_ms > 250 || !c->max_pulse_ms ||
        c->max_pulse_ms > 250)
        return STEER_ERR_ARGUMENT;
    if (!io.read_feedback)
        return STEER_ERR_NOT_READY;
    if ((c->max_pulse_ms > 60 && !io.model->burst_max_us) ||
        (io.model->burst_max_us &&
         (c->max_pulse_ms * 1000 != io.model->burst_max_us || io.priority != 10)))
        return STEER_ERR_ARGUMENT;
    if (initialized)
        (void)steering_actuator_stop(&actuator);
    config = *c;
    goal_tolerance = c->tolerance_deg;
    steer_result_t rc = steering_actuator_init(&actuator, io.actuator, c->max_pulse_ms);
    if (rc != STEER_OK)
        return rc;
    initialized = 1;
    reference_valid = have_target = 0;
    last_tick_ns = feedback_deadline = 0;
    reposition_used = 0;
    after_reposition = 0;
    blocked_left_us = blocked_right_us = requested_total_us = 0;
    pulse_is_burst = planned_release_reason = 0;
    reset_stability();
    state = (steering_state_t){NAN, NAN, NAN, STEERING_NOT_READY, STEER_ERR_NOT_READY, 0};
    return STEER_OK;
}
steer_result_t steering_confirm_reference(uint32_t session, float raw)
{
    if (!initialized)
        return STEER_ERR_NOT_READY;
    if (!session || !isfinite(raw) || fabsf(raw) > 2000)
        return STEER_ERR_ARGUMENT;
    if (io.model->burst_max_us &&
        (session != io.model->burst_session || fabsf(raw - io.model->burst_zero_raw_deg) > .001f))
        return STEER_ERR_STALE;
    steering_feedback_t f = {0};
    steer_result_t rc = io.read_feedback(io.context, &f);
    if (rc != STEER_OK || f.session_id != session || !(f.flags & STEER_FEEDBACK_VALID) ||
        (f.flags & STEER_FEEDBACK_SIMULATED) || f.source_age_ms >= config.feedback_timeout_ms ||
        !f.sample_time_ns)
        return STEER_ERR_STALE;
    rc = steering_actuator_stop(&actuator);
    if (rc != STEER_OK)
        return rc;
    reference_session = session;
    zero_raw = raw;
    reference_valid = 1;
    have_target = 0;
    after_pulse = 0;
    reset_stability();
    state.status = STEERING_CANCELLED;
    state.last_error = STEER_OK;
    return read_angle(&f);
}
steer_result_t steering_set_target_deg(float target)
{
    if (!isfinite(target))
        return STEER_ERR_ARGUMENT;
    if (!initialized)
        return STEER_ERR_NOT_READY;
    return steering_set_target_with_tolerance_deg(target, config.tolerance_deg);
}
steer_result_t steering_set_target_with_tolerance_deg(float target, float tolerance)
{
    if (!isfinite(target) || !isfinite(tolerance) || tolerance < .1f || tolerance > 3)
        return STEER_ERR_ARGUMENT;
    if (!initialized || !reference_valid)
        return STEER_ERR_NOT_READY;
    if (target < config.min_angle_deg || target > config.max_angle_deg)
        return STEER_ERR_ARGUMENT;
    if (state.status == STEERING_FAULT || state.status == STEERING_RESOLUTION_LIMIT)
        return state.last_error;
    if (io.model->burst_max_us && fabsf(target) > 15)
        return STEER_ERR_ARGUMENT;
    if (have_target && fabsf(target - state.target_deg) < .001f &&
        fabsf(goal_tolerance - tolerance) < .001f)
        return state.last_error;
    steering_feedback_t f;
    steer_result_t rc = read_angle(&f);
    if (rc != STEER_OK)
        return fail(rc, STEERING_FAULT);
    int interrupted_motion = actuator.status == ACTUATOR_ACTIVE;
    rc = steering_actuator_stop(&actuator);
    if (rc != STEER_OK)
        return fail(rc, STEERING_FAULT);
    uint64_t now = io.actuator.now_ns(io.actuator.context);
    if (!now)
        return fail(STEER_ERR_IO, STEERING_FAULT);
    if (last_tick_ns && now < last_tick_ns)
        return fail(STEER_ERR_STALE, STEERING_FAULT);
    last_tick_ns = now;
    int unfinished =
        have_target && (state.status == STEERING_MOVING || state.status == STEERING_SETTLING);
    have_target = 1;
    goal_tolerance = tolerance;
    state.target_deg = target;
    state.error_deg = target - state.angle_deg;
    state.last_error = STEER_OK;
    if (!unfinished)
    {
        pulses = reversals = no_progress = reposition_used = 0;
        blocked_left_us = blocked_right_us = requested_total_us = 0;
        previous_direction = STEER_RELEASE;
        target_deadline = now + (uint64_t)(io.max_pulses + 1) * UINT64_C(3000000000);
    }
    /* The IMU is sampled continuously, independently of vision arrivals.
     * A fresh target must not discard an already stable steering history.
     * A stopped active pulse still needs its entire post-motion settling. */
    after_pulse = 0;
    settling(now, interrupted_motion);
    return STEER_OK;
}
steer_result_t steering_tick(void)
{
    if (!initialized)
        return STEER_ERR_NOT_READY;
    uint64_t now = io.actuator.now_ns(io.actuator.context);
    if (!now)
        return fail(STEER_ERR_IO, STEERING_FAULT);
    if (last_tick_ns && now < last_tick_ns)
        return fail(STEER_ERR_STALE, STEERING_FAULT);
    last_tick_ns = now;
    int was_active = actuator.status == ACTUATOR_ACTIVE;
    steer_result_t rc = steering_actuator_tick(&actuator);
    if (rc != STEER_OK)
        return fail(rc, STEERING_FAULT);
    steering_feedback_t f;
    rc = read_angle(&f);
    if (rc != STEER_OK)
    {
        reset_stability();
        return fail(rc, reference_valid ? STEERING_FAULT : STEERING_NOT_READY);
    }
    if (actuator.status == ACTUATOR_ACTIVE || was_active)
        reset_stability();
    else
        observe_stability(&f, now);
    if (!have_target)
        return STEER_OK;
    if (state.status == STEERING_FAULT || state.status == STEERING_RESOLUTION_LIMIT)
        return state.last_error;
    rc = io.permission(io.context,
                       actuator.status == ACTUATOR_ACTIVE ? actuator.direction : STEER_RELEASE);
    if (rc != STEER_OK)
    {
        have_target = 0;
        return fail(rc, STEERING_CANCELLED);
    }
    if (pulse_is_burst && actuator.status == ACTUATOR_ACTIVE)
    {
        unsigned d = actuator.direction == STEER_LEFT ? 0 : 1;
        int decision = steering_predictive_release_capped(
            state.angle_deg, f.rate_mdps / 1000.f, actuator.direction, state.target_deg,
            goal_tolerance, io.model->burst_coast_ms[d], io.model->burst_guard_ms[d],
            fminf(-config.min_angle_deg, config.max_angle_deg), io.model->burst_coast_cap_deg[d],
            io.model->burst_guard_cap_deg[d]);
        if (decision == BURST_WRONG_DIRECTION || decision == BURST_BAD_PREDICTION)
            return fail(STEER_ERR_DEVICE, STEERING_FAULT);
        if (decision)
        {
            planned_release_reason = decision;
            rc = steering_actuator_stop(&actuator);
            if (rc != STEER_OK)
                return fail(rc, STEERING_FAULT);
        }
    }
    if (state.status != STEERING_SETTLED && now > target_deadline)
        return fail(STEER_ERR_STALE, STEERING_FAULT);
    if (was_active && actuator.status == ACTUATOR_IDLE)
    {
        uint64_t actual = actuator.release_end_ns - actuator.enable_end_ns;
        printf(
            "[STEER_PULSE_END] pulse=%u direction=%d requested_us=%u actual_us=%.3f early_release=%d\n",
            pulses, previous_direction, actuator.requested_us, actual / 1000.,
            planned_release_reason);
        if ((!planned_release_reason && actual + 50000 < (uint64_t)actuator.requested_us * 1000) ||
            actual > (uint64_t)actuator.requested_us * 1000 + 2000000)
            return fail(STEER_ERR_IO, STEERING_FAULT);
        after_pulse = 1;
        settling(now, 1);
    }
    if (actuator.status == ACTUATOR_ACTIVE)
        return STEER_OK;
    if (state.status == STEERING_SETTLED)
    {
        if (fabsf(state.error_deg) <= goal_tolerance)
            return STEER_OK;
        target_deadline = now + (uint64_t)(io.max_pulses + 1) * UINT64_C(3000000000);
        settling(now, 0);
    }
    if (state.status != STEERING_SETTLING)
        return STEER_OK;
    if (now > settle_deadline)
        return fail(STEER_ERR_STALE, STEERING_FAULT);
    if (!stable_since || now - stable_since < (uint64_t)io.settle_ms * 1000000 ||
        stable_samples < 3)
        return STEER_OK;
    if (after_pulse)
    {
        int sign = previous_direction == STEER_LEFT ? 1 : -1;
        printf(
            "[STEER_RESPONSE] pulse=%u before_deg=%.3f after_deg=%.3f delta_deg=%.3f error_deg=%.3f\n",
            pulses, before_angle, state.angle_deg, state.angle_deg - before_angle, state.error_deg);
        if ((state.angle_deg - before_angle) * sign < -.4f)
            return fail(STEER_ERR_DEVICE, STEERING_FAULT);
        if (fabsf(state.angle_deg - before_angle) < .15f)
        {
            if (previous_direction == STEER_LEFT)
                blocked_left_us = actuator.requested_us;
            else
                blocked_right_us = actuator.requested_us;
        }
        else
            blocked_left_us = blocked_right_us = 0;
        if (after_reposition && fabsf(state.angle_deg - before_angle) >= .15f)
            no_progress = 0;
        else
        {
            no_progress = fabsf(before_error) - fabsf(state.error_deg) < .15f ? no_progress + 1 : 0;
            if (after_reposition && reposition_used)
                --reposition_used; /* A zero response did not reposition. */
        }
        after_reposition = 0;
        after_pulse = 0;
        if (no_progress >= 2)
            return fail(STEER_ERR_NOT_READY, STEERING_RESOLUTION_LIMIT);
    }
    if (fabsf(state.error_deg) <= goal_tolerance)
    {
        state.status = STEERING_SETTLED;
        state.last_error = STEER_OK;
        return STEER_OK;
    }
    /* Fast mode is one continuous coarse action and at most two short
     * corrections. It must never silently become a long staircase. */
    if (pulses >= (io.model->burst_max_us && io.max_pulses > 3 ? 3u : io.max_pulses))
        return fail(STEER_ERR_NOT_READY, STEERING_RESOLUTION_LIMIT);
    steering_response_choice_t choice;
    int reposition = 0;
    int burst_choice = io.model->burst_max_us && !pulses && fabsf(state.error_deg) >= 6 &&
                       (fabsf(state.angle_deg) <= 4 || state.angle_deg * state.error_deg < 0);
    if (burst_choice)
    {
        choice = (steering_response_choice_t){state.error_deg > 0 ? STEER_LEFT : STEER_RIGHT,
                                              io.model->burst_max_us, 18, 0};
        rc = STEER_OK;
    }
    else
    {
        const steering_response_model_t *planning_model = io.model;
        if (blocked_left_us || blocked_right_us)
        {
            choice_model = *io.model;
            for (unsigned i = 0; i < choice_model.count; ++i)
            {
                steering_response_cell_t *c = &choice_model.cells[i];
                if (c->partitioned &&
                    c->duration_us ==
                        (c->direction == STEER_LEFT ? blocked_left_us : blocked_right_us))
                    c->n = 0;
            }
            planning_model = &choice_model;
        }
        rc = steering_response_choose(planning_model, state.angle_deg, state.target_deg,
                                      goal_tolerance, config.min_angle_deg, config.max_angle_deg,
                                      config.max_pulse_ms > 60 ? 60000 : config.max_pulse_ms * 1000,
                                      io.minimum_samples, io.priority, &choice);
        if (rc == STEER_ERR_NOT_READY && !reposition_used)
        {
            rc = steering_response_choose_reposition(
                planning_model, state.angle_deg, state.target_deg, goal_tolerance,
                config.min_angle_deg, config.max_angle_deg,
                config.max_pulse_ms > 60 ? 60000 : config.max_pulse_ms * 1000, io.minimum_samples,
                io.priority, &choice);
            reposition = rc == STEER_OK;
        }
        /* If no distinct measured alternative exists, retain the original bounded
     * second attempt. A write/timing failure never reaches this branch. */
        if (rc == STEER_ERR_NOT_READY && planning_model != io.model && no_progress < 2)
        {
            rc = steering_response_choose(
                io.model, state.angle_deg, state.target_deg, goal_tolerance, config.min_angle_deg,
                config.max_angle_deg, config.max_pulse_ms > 60 ? 60000 : config.max_pulse_ms * 1000,
                io.minimum_samples, io.priority, &choice);
            reposition = 0;
        }
    }
    if (rc != STEER_OK || !choice.duration_us)
        return fail(STEER_ERR_NOT_READY, STEERING_RESOLUTION_LIMIT);
    if (choice.duration_us > 600000u - requested_total_us)
        return fail(STEER_ERR_NOT_READY, STEERING_RESOLUTION_LIMIT);
    unsigned reversal_limit = (reposition_used || reposition) ? 3 : 1;
    if (previous_direction != STEER_RELEASE && choice.direction != previous_direction &&
        ++reversals > reversal_limit)
        return fail(STEER_ERR_NOT_READY, STEERING_RESOLUTION_LIMIT);
    rc = io.permission(io.context, choice.direction);
    if (rc != STEER_OK)
    {
        have_target = 0;
        return fail(rc, STEERING_CANCELLED);
    }
    before_angle = state.angle_deg;
    before_error = state.error_deg;
    previous_direction = choice.direction;
    pulse_is_burst = burst_choice;
    planned_release_reason = 0;
    printf(
        "[STEER_PULSE_BEGIN] pulse=%u direction=%d duration_us=%u current_deg=%.3f target_deg=%.3f guard_deg=%.3f expected_error_deg=%.3f reposition=%d\n",
        pulses + 1, choice.direction, choice.duration_us, state.angle_deg, state.target_deg,
        choice.guard_deg, choice.expected_error_deg, reposition);
    rc = steering_actuator_start_us(&actuator, choice.direction, choice.duration_us);
    if (rc != STEER_OK)
        return fail(rc, STEERING_FAULT);
    requested_total_us += choice.duration_us;
    if (reposition)
        ++reposition_used;
    after_reposition = reposition;
    ++pulses;
    state.status = STEERING_MOVING;
    return STEER_OK;
}
steer_result_t steering_get_state(steering_state_t *out)
{
    if (!out)
        return STEER_ERR_ARGUMENT;
    *out = state;
    if (initialized && out->feedback_valid &&
        io.actuator.now_ns(io.actuator.context) >= feedback_deadline)
    {
        out->feedback_valid = 0;
        out->angle_deg = out->error_deg = NAN;
        out->last_error = STEER_ERR_STALE;
    }
    return initialized ? STEER_OK : STEER_ERR_NOT_READY;
}
steer_result_t steering_cancel(void)
{
    if (!initialized)
        return STEER_ERR_NOT_READY;
    have_target = 0;
    after_pulse = 0;
    return fail(STEER_OK, STEERING_CANCELLED);
}
int steering_pulse_active(void)
{
    return initialized && actuator.status == ACTUATOR_ACTIVE;
}
uint64_t steering_pulse_deadline_ns(void)
{
    return steering_pulse_active() ? actuator.deadline_ns : 0;
}
const char *steering_status_name(steering_status_t status)
{
    switch (status)
    {
    case STEERING_NOT_READY:
        return "NOT_READY";
    case STEERING_MOVING:
        return "MOVING";
    case STEERING_SETTLED:
        return "SETTLED";
    case STEERING_CANCELLED:
        return "CANCELLED";
    case STEERING_FAULT:
        return "FAULT";
    case STEERING_SETTLING:
        return "SETTLING";
    case STEERING_RESOLUTION_LIMIT:
        return "RESOLUTION_LIMIT";
    default:
        return "UNKNOWN";
    }
}
void steering_shutdown(void)
{
    if (initialized)
        (void)steering_cancel();
    initialized = reference_valid = have_target = 0;
    memset(&io, 0, sizeof(io));
    state = (steering_state_t){NAN, NAN, NAN, STEERING_NOT_READY, STEER_ERR_NOT_READY, 0};
}
