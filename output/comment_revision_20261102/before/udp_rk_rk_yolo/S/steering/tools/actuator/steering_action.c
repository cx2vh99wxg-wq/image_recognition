/* Bounded two-pulse characterization. No image/HMI/traction writes. */
#define _POSIX_C_SOURCE 200809L
#include "steering_sequence.h"
#include "steering_platform.h"
#include "steering_predictive_stop.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <inttypes.h>
#ifdef STEERING_WITH_FSPI
#include "steering_feedback.h"
#include "fspi_module.h"
#include <sched.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <unistd.h>
#endif
static volatile sig_atomic_t interrupted;
static void interrupt_action(int n)
{
    (void)n;
    interrupted = 1;
}
typedef struct
{
    uint64_t host_ns, source_ns;
    uint32_t seq, age;
    float angle, rate;
    unsigned phase;
} sample_t;
static sample_t samples[2000];
static unsigned count;
#ifdef STEERING_WITH_FSPI
static uint32_t expected_session, last_seq;
static float raw_zero, angle_limit;
static unsigned max_age;
static uint64_t last_packet;
static steering_sequence_t sequence;
static steer_result_t finish_early(void)
{
    steering_actuator_t *a = &sequence.actuator;
    steer_direction_t d = a->direction;
    steer_result_t rc = steering_actuator_stop(a);
    if (rc != STEER_OK)
        return rc;
    sequence.records[0] = (steering_pulse_record_t){d,
                                                    a->requested_us,
                                                    a->enable_begin_ns,
                                                    a->enable_end_ns,
                                                    a->release_begin_ns,
                                                    a->release_end_ns};
    sequence.completed = 1;
    sequence.status = SEQUENCE_DONE;
    return STEER_OK;
}
static steer_result_t output(void *p, steer_direction_t d)
{
    (void)p;
    if (d == STEER_RELEASE)
    {
        int a = -1, b = -1;
        for (int i = 0; i < 3 && (a || b); ++i)
        {
            if (a)
                a = fspi_write_direction_once(2, 1);
            if (b)
                b = fspi_write_direction_once(3, 1);
        }
        return a || b ? STEER_ERR_IO : STEER_OK;
    }
    if (d != STEER_LEFT && d != STEER_RIGHT)
        return STEER_ERR_ARGUMENT;
    if (fspi_write_direction_once(d == STEER_LEFT ? 3 : 2, 1))
        return STEER_ERR_IO;
    return fspi_write_direction_once(d == STEER_LEFT ? 2 : 3, 0) ? STEER_ERR_IO : STEER_OK;
}
static uint64_t clock_now(void *p)
{
    (void)p;
    return steering_now_ns();
}
static steer_result_t capture(int initial)
{
    steering_feedback_t f;
    steer_result_t rc = steering_feedback_read(&f);
    uint64_t now = steering_now_ns();
    if (rc != STEER_OK)
    {
        if (initial && !count)
            return STEER_ERR_NOT_READY;
        return rc;
    }
    if (f.session_id != expected_session || f.flags & STEER_FEEDBACK_SIMULATED)
        return STEER_ERR_STALE;
    if (count && f.sequence == last_seq)
        return STEER_OK;
    if (count >= 2000)
        return STEER_ERR_IO;
    float angle = f.angle_mdeg / 1000.f - raw_zero, rate = f.rate_mdps / 1000.f;
    if (!isfinite(angle) || !isfinite(rate) || fabsf(angle) > angle_limit)
        return STEER_ERR_DEVICE;
    samples[count++] = (sample_t){
        now, f.sample_time_ns, f.sequence, f.source_age_ms, angle, rate, (unsigned)sequence.status};
    last_seq = f.sequence;
    last_packet = now;
    if (f.source_age_ms > max_age)
        max_age = f.source_age_ms;
    return STEER_OK;
}
static int stable(float *value)
{
    if (count < 40)
        return 0;
    uint64_t latest = samples[count - 1].host_ns;
    float lo = 1e6f, hi = -1e6f;
    double sum = 0;
    unsigned n = 0;
    uint64_t first = latest;
    for (unsigned i = count; i > 0;)
    {
        --i;
        if (latest - samples[i].host_ns > 550000000)
            break;
        if (fabsf(samples[i].rate) > 2)
            return 0;
        lo = fminf(lo, samples[i].angle);
        hi = fmaxf(hi, samples[i].angle);
        sum += samples[i].angle;
        ++n;
        first = samples[i].host_ns;
    }
    if (n < 40 || latest - first < 450000000 || hi - lo > .3f)
        return 0;
    *value = (float)(sum / n);
    return 1;
}
static steer_result_t permission(int active)
{
    ControlSnapshot s;
    if (fspi_read_control_snapshot(&s))
        return STEER_ERR_IO;
    unsigned released = CONTACT_GO_N | CONTACT_BACK_N | CONTACT_STOP_N;
    if ((s.contacts & released) != released || (s.gates & (GATE_AUTO | GATE_STOP)) ||
        !(s.gates & GATE_FS_ARMED) || !(s.gates & GATE_RESET_RELEASED) || s.hmi == 0xcc ||
        s.hmi == 0xdd || s.hmi == 0xee)
        return STEER_ERR_BUSY;
    if (active)
    {
        unsigned own = sequence.actuator.direction == STEER_LEFT ? CONTACT_LEFT_N : CONTACT_RIGHT_N,
                 other = own == CONTACT_LEFT_N ? CONTACT_RIGHT_N : CONTACT_LEFT_N;
        if ((s.contacts & own) || !(s.contacts & other))
            return STEER_ERR_BUSY;
    }
    else if ((s.contacts & (CONTACT_LEFT_N | CONTACT_RIGHT_N)) !=
             (CONTACT_LEFT_N | CONTACT_RIGHT_N))
        return STEER_ERR_BUSY;
    return STEER_OK;
}
static void wait_tick(uint64_t deadline)
{
    uint64_t wake = steering_now_ns() + 500000;
    if (deadline && deadline < wake)
        wake = deadline;
    struct timespec t = {(time_t)(wake / 1000000000), (long)(wake % 1000000000)};
    (void)clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
}
#endif
static int integer(const char *s, unsigned *out)
{
    char *end;
    unsigned long v = strtoul(s, &end, 10);
    if (*end || s[0] == '-' || v > 1000000)
        return 0;
    *out = (unsigned)v;
    return 1;
}
int main(int argc, char **argv)
{
    unsigned first = 30000, brake = 0, gap = 5000, priority = 10, max_phase_ms = 60, coast_ms = 120,
             guard_ms = 200;
    int hardware = 0, fixed = 0, target_set = 0, stop_reason = 0;
    steer_direction_t dir = STEER_RELEASE;
    const char *reference = NULL, *prefix = NULL;
    float lower = -10, upper = 10, current = 0, target = 0, tolerance = 3, coast_cap = 0,
          guard_cap = 0;
    for (int i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "--fixed-chassis"))
        {
            fixed = 1;
            continue;
        }
        if (i + 1 >= argc)
            return 2;
        const char *k = argv[i], *v = argv[++i];
        char *e;
        if (!strcmp(k, "--backend"))
        {
            if (strcmp(v, "fspi") && strcmp(v, "dry-run"))
                return 2;
            hardware = !strcmp(v, "fspi");
        }
        else if (!strcmp(k, "--direction"))
        {
            dir = !strcmp(v, "left")    ? STEER_LEFT
                  : !strcmp(v, "right") ? STEER_RIGHT
                                        : STEER_RELEASE;
        }
        else if (!strcmp(k, "--first-us"))
        {
            if (!integer(v, &first))
                return 2;
        }
        else if (!strcmp(k, "--brake-us"))
        {
            if (!integer(v, &brake))
                return 2;
        }
        else if (!strcmp(k, "--gap-us"))
        {
            if (!integer(v, &gap))
                return 2;
        }
        else if (!strcmp(k, "--priority"))
        {
            if (!integer(v, &priority))
                return 2;
        }
        else if (!strcmp(k, "--max-phase-ms"))
        {
            if (!integer(v, &max_phase_ms))
                return 2;
        }
        else if (!strcmp(k, "--stop-coast-ms"))
        {
            if (!integer(v, &coast_ms))
                return 2;
        }
        else if (!strcmp(k, "--stop-guard-ms"))
        {
            if (!integer(v, &guard_ms))
                return 2;
        }
        else if (!strcmp(k, "--target-deg"))
        {
            target = strtof(v, &e);
            if (*e)
                return 2;
            target_set = 1;
        }
        else if (!strcmp(k, "--tolerance-deg"))
        {
            tolerance = strtof(v, &e);
            if (*e)
                return 2;
        }
        else if (!strcmp(k, "--stop-coast-cap-deg"))
        {
            coast_cap = strtof(v, &e);
            if (*e)
                return 2;
        }
        else if (!strcmp(k, "--stop-guard-cap-deg"))
        {
            guard_cap = strtof(v, &e);
            if (*e)
                return 2;
        }
        else if (!strcmp(k, "--reference"))
            reference = v;
        else if (!strcmp(k, "--output-prefix"))
            prefix = v;
        else if (!strcmp(k, "--guard-low"))
        {
            lower = strtof(v, &e);
            if (*e)
                return 2;
        }
        else if (!strcmp(k, "--guard-high"))
        {
            upper = strtof(v, &e);
            if (*e)
                return 2;
        }
        else if (!strcmp(k, "--current-deg"))
        {
            current = strtof(v, &e);
            if (*e)
                return 2;
        }
        else
            return 2;
    }
    if (dir == STEER_RELEASE || !max_phase_ms || max_phase_ms > 250 || !first ||
        first > max_phase_ms * 1000u || brake > 60000 || (max_phase_ms > 60 && brake) ||
        gap > 100000 || (brake && gap < 2000) || priority > 30 ||
        (target_set && (max_phase_ms <= 60 || brake || !isfinite(target) || fabsf(target) > 15)) ||
        !isfinite(tolerance) || tolerance <= 0 || tolerance > 3 || coast_ms > 200 ||
        guard_ms < coast_ms || guard_ms > 500 || !isfinite(coast_cap) || !isfinite(guard_cap) ||
        coast_cap < 0 || guard_cap < coast_cap || guard_cap > 18 ||
        ((coast_cap == 0) != (guard_cap == 0)) || !isfinite(lower) || !isfinite(upper) ||
        lower > 0 || upper < 0 || lower < -36 || upper > 36 || !isfinite(current))
        return 2;
    if (!hardware)
    {
        printf(
            "PLAN_ONLY_SIMULATED direction=%d first_us=%u brake_us=%u gap_us=%u predicted=[%.3f,%.3f] no_device_access=1\n",
            dir, first, brake, gap, current + lower, current + upper);
        return current + lower >= -18 && current + upper <= 18 ? 0 : 2;
    }
#ifndef STEERING_WITH_FSPI
    (void)reference;
    (void)prefix;
    (void)fixed;
    (void)interrupt_action;
    (void)samples;
    (void)count;
    (void)stop_reason;
    return 2;
#else
    if (!fixed || !reference || !prefix || access("/tmp/steering-bench.active", F_OK))
        return 2;
    char magic[48], extra;
    unsigned declared;
    FILE *ref = fopen(reference, "r");
    if (!ref)
        return 1;
    int valid = fscanf(ref, "%47s %u %f %u", magic, &expected_session, &raw_zero, &declared) == 4 &&
                !strcmp(magic, "STEER_PHYSICAL_REFERENCE_V1") && expected_session &&
                isfinite(raw_zero) && declared == 1;
    if (valid && fscanf(ref, " %c", &extra) == 1)
        valid = 0;
    fclose(ref);
    if (!valid)
        return 2;
    char report_path[4096], csv_path[4096];
    if (snprintf(report_path, sizeof(report_path), "%s.json", prefix) >= (int)sizeof(report_path) ||
        snprintf(csv_path, sizeof(csv_path), "%s.csv", prefix) >= (int)sizeof(csv_path))
        return 2;
    FILE *report = fopen(report_path, "wx");
    if (!report)
        return 2;
    FILE *csv = fopen(csv_path, "wx");
    if (!csv)
    {
        fclose(report);
        return 2;
    }
    signal(SIGINT, interrupt_action);
    signal(SIGTERM, interrupt_action);
    angle_limit = 18;
    float before = NAN, after = NAN, low = 0, high = 0;
    int spi = 0, locked = 0, actual_priority = 0;
    steer_result_t rc = STEER_ERR_NOT_READY;
    struct sched_param prior_param;
    int prior_policy = sched_getscheduler(0);
    (void)sched_getparam(0, &prior_param);
    if (fspi_init())
        goto done;
    spi = 1;
    if (steering_feedback_open("192.168.100.20", "192.168.100.10", 54321, 100) != STEER_OK)
        goto done;
    (void)steering_sequence_init_limit(&sequence, (steering_actuator_io_t){NULL, output, clock_now},
                                       max_phase_ms);
    {
        uint64_t start = steering_now_ns();
        while (!interrupted && steering_now_ns() - start < 2500000000)
        {
            rc = capture(1);
            if (rc != STEER_OK && rc != STEER_ERR_NOT_READY)
                goto done;
            if (count && steering_now_ns() - start >= 800000000 && stable(&before))
                break;
            wait_tick(0);
        }
    }
    if (!isfinite(before) || before + lower < -18 || before + upper > 18)
    {
        rc = STEER_ERR_ARGUMENT;
        goto done;
    }
    rc = permission(0);
    if (rc != STEER_OK)
        goto done;
    if (target_set && fabsf(before - target) <= tolerance)
    {
        after = before;
        stop_reason = 5;
        rc = STEER_OK;
        goto done;
    }
    if (priority)
    {
        struct sched_param param = {0};
        param.sched_priority = (int)priority;
        if (sched_setscheduler(0, SCHED_FIFO, &param))
        {
            rc = STEER_ERR_IO;
            goto done;
        }
        actual_priority = priority;
    }
    {
        struct rlimit limit;
        if (!getrlimit(RLIMIT_MEMLOCK, &limit))
        {
            limit.rlim_cur = limit.rlim_max;
            (void)setrlimit(RLIMIT_MEMLOCK, &limit);
        }
        locked = mlockall(MCL_CURRENT) == 0;
    }
    if (!locked)
    {
        rc = STEER_ERR_IO;
        goto done;
    }
    rc = steering_sequence_start(&sequence, dir, first, brake, gap);
    {
        uint64_t last_check = 0;
        while (rc == STEER_OK && steering_sequence_active(&sequence) && !interrupted)
        {
            rc = steering_sequence_tick(&sequence);
            if (rc != STEER_OK)
                break;
            rc = capture(0);
            if (rc != STEER_OK)
                break;
            if (target_set && sequence.actuator.status == ACTUATOR_ACTIVE && count)
            {
                sample_t *last = &samples[count - 1];
                int decision = steering_predictive_release_capped(
                    last->angle, last->rate, dir, target, tolerance, coast_ms, guard_ms, 18,
                    coast_cap, guard_cap);
                if (decision)
                {
                    stop_reason = decision;
                    rc = finish_early();
                    if (rc == STEER_OK &&
                        (decision == BURST_WRONG_DIRECTION || decision == BURST_BAD_PREDICTION))
                        rc = STEER_ERR_DEVICE;
                    break;
                }
            }
            uint64_t now = steering_now_ns(), deadline = sequence.actuator.status == ACTUATOR_ACTIVE
                                                             ? sequence.actuator.deadline_ns
                                                         : sequence.status == SEQUENCE_GAP
                                                             ? sequence.gap_end_ns
                                                             : 0;
            if ((!deadline || deadline > now + 2000000) && now - last_check >= 5000000)
            {
                rc = permission(sequence.actuator.status == ACTUATOR_ACTIVE);
                last_check = now;
            }
            if (rc == STEER_OK)
                wait_tick(deadline);
        }
    }
    if (interrupted)
        rc = STEER_ERR_BUSY;
    if (rc != STEER_OK)
    {
        (void)steering_sequence_cancel(&sequence);
        goto done;
    }
    /* Only pulse deadlines need FIFO scheduling. Observe settling at normal
     * priority; a manual input invalidates the response instead of becoming
     * part of the fitted actuator model. */
    if (actual_priority)
        (void)sched_setscheduler(0, prior_policy, &prior_param);
    {
        uint64_t post = steering_now_ns(), last_check = 0;
        while (!interrupted && steering_now_ns() - post < 5000000000)
        {
            rc = capture(0);
            if (rc != STEER_OK)
                goto done;
            uint64_t now = steering_now_ns();
            if (now - last_check >= 20000000)
            {
                rc = permission(0);
                if (rc != STEER_OK)
                    goto done;
                last_check = now;
            }
            if (steering_now_ns() - post >= 1000000000 && stable(&after))
                break;
            wait_tick(0);
        }
    }
    if (!isfinite(after) || interrupted)
        rc = STEER_ERR_STALE;
done:
    if (spi)
    {
        steer_result_t release = output(NULL, STEER_RELEASE);
        if (release != STEER_OK)
            rc = release;
    }
    if (actual_priority)
        (void)sched_setscheduler(0, prior_policy, &prior_param);
    if (locked)
        (void)munlockall();
    if (count && isfinite(before))
    {
        low = high = samples[0].angle - before;
        for (unsigned i = 0; i < count; ++i)
        {
            low = fminf(low, samples[i].angle - before);
            high = fmaxf(high, samples[i].angle - before);
        }
    }
    fprintf(
        report,
        "{\n\"schema\":\"%s\",\"max_phase_ms\":%u,\"session\":%u,\"zero_raw_deg\":%.9g,\"priority\":%d,\"memory_locked\":%s,\"result\":\"%s\",\"direction\":%d,\"first_us\":%u,\"brake_us\":%u,\"gap_us\":%u,",
        max_phase_ms > 60 ? "STEER_SINGLE_BURST_TRIAL_V1" : "STEER_ACTION_TRIAL_V2", max_phase_ms,
        expected_session, raw_zero, actual_priority, locked ? "true" : "false",
        rc == STEER_OK ? "RECORDED" : "STOPPED", dir, first, brake, gap);
    fprintf(
        report,
        "\"target_set\":%s,\"target_deg\":%.9g,\"tolerance_deg\":%.9g,\"stop_coast_ms\":%u,\"stop_guard_ms\":%u,\"stop_reason\":%d,",
        target_set ? "true" : "false", target, tolerance, coast_ms, guard_ms, stop_reason);
    fprintf(report, "\"stop_coast_cap_deg\":%.9g,\"stop_guard_cap_deg\":%.9g,", coast_cap,
            guard_cap);
    if (isfinite(before))
        fprintf(report, "\"before_deg\":%.9g,", before);
    else
        fputs("\"before_deg\":null,", report);
    if (isfinite(after))
        fprintf(report, "\"after_deg\":%.9g,\"delta_deg\":%.9g,", after, after - before);
    else
        fputs("\"after_deg\":null,\"delta_deg\":null,", report);
    fprintf(
        report,
        "\"peak_low_deg\":%.9g,\"peak_high_deg\":%.9g,\"samples\":%u,\"max_age_ms\":%u,\"error\":\"%s\",\"timings\":[",
        low, high, count, max_age, steer_result_name(rc));
    for (unsigned i = 0; i < sequence.completed; ++i)
    {
        steering_pulse_record_t *t = &sequence.records[i];
        fprintf(report,
                "%s{\"direction\":%d,\"duration_us\":%u,\"enable_begin_ns\":%" PRIu64
                ",\"enable_end_ns\":%" PRIu64 ",\"release_begin_ns\":%" PRIu64
                ",\"release_end_ns\":%" PRIu64 "}",
                i ? "," : "", t->direction, t->duration_us, t->enable_begin_ns, t->enable_end_ns,
                t->release_begin_ns, t->release_end_ns);
    }
    fputs("]}\n", report);
    fputs("host_ns,source_ns,sequence,age_ms,angle_deg,rate_dps,phase\n", csv);
    for (unsigned i = 0; i < count; ++i)
    {
        sample_t *s = &samples[i];
        fprintf(csv, "%" PRIu64 ",%" PRIu64 ",%u,%u,%.9g,%.9g,%u\n", s->host_ns, s->source_ns,
                s->seq, s->age, s->angle, s->rate, s->phase);
    }
    int report_error = ferror(report) || ferror(csv);
    int close_report = fclose(report), close_csv = fclose(csv);
    if (close_report || close_csv)
        report_error = 1;
    steering_feedback_close();
    if (spi)
        fspi_cleanup();
    printf(
        "ACTION_RESULT result=%s before=%.3f after=%.3f delta=%.3f phases=%u priority=%d mlock=%d\n",
        steer_result_name(rc), before, after, after - before, sequence.completed, actual_priority,
        locked);
    return rc == STEER_OK && !report_error ? 0 : 1;
#endif
}
