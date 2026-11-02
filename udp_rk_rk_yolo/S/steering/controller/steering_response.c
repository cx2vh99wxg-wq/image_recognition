#include "steering_response.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

steer_result_t steering_response_load(steering_response_model_t *m, const char *path)
{
    if (!m || !path)
        return STEER_ERR_ARGUMENT;
    FILE *f = fopen(path, "r");
    if (!f)
        return STEER_ERR_IO;
    steering_response_model_t out = {0};
    char magic[48] = {0};
    int good = 1;
    if (fscanf(f, "%47s %u", magic, &out.count) != 2 ||
        (strcmp(magic, "STEER_RESPONSE_TABLE_V1") && strcmp(magic, "STEER_RESPONSE_TABLE_V2") &&
         strcmp(magic, "STEER_RESPONSE_TABLE_V3")) ||
        !out.count || out.count > STEERING_RESPONSE_MAX_CELLS)
        good = 0;
    int burst = !strcmp(magic, "STEER_RESPONSE_TABLE_V3");
    int partitioned = strcmp(magic, "STEER_RESPONSE_TABLE_V1") != 0;
    if (good && burst)
    {
        unsigned session;
        char tag[16];
        if (fscanf(f, "%15s %u %u %f %u %u %u %u %f %f %f %f", tag, &out.burst_max_us, &session,
                   &out.burst_zero_raw_deg, &out.burst_coast_ms[0], &out.burst_guard_ms[0],
                   &out.burst_coast_ms[1], &out.burst_guard_ms[1], &out.burst_coast_cap_deg[0],
                   &out.burst_guard_cap_deg[0], &out.burst_coast_cap_deg[1],
                   &out.burst_guard_cap_deg[1]) != 12 ||
            strcmp(tag, "BURST") || out.burst_max_us <= 60000 || out.burst_max_us > 250000 ||
            out.burst_max_us % 1000 || !session || !isfinite(out.burst_zero_raw_deg) ||
            fabsf(out.burst_zero_raw_deg) > 2000)
            good = 0;
        else
            out.burst_session = session;
        for (unsigned d = 0; d < 2; ++d)
            if (out.burst_coast_ms[d] > 200 || out.burst_guard_ms[d] < out.burst_coast_ms[d] ||
                out.burst_guard_ms[d] > 500 || !isfinite(out.burst_coast_cap_deg[d]) ||
                !isfinite(out.burst_guard_cap_deg[d]) || out.burst_coast_cap_deg[d] <= 0 ||
                out.burst_guard_cap_deg[d] < out.burst_coast_cap_deg[d] ||
                out.burst_guard_cap_deg[d] > 18)
                good = 0;
    }
    for (unsigned i = 0; good && i < out.count; ++i)
    {
        steering_response_cell_t *c = &out.cells[i];
        unsigned d;
        if (fscanf(f, "%u %u %u %u %u %f %f %f %f %f %u", &d, &c->duration_us, &c->priority, &c->n,
                   &c->moved, &c->min_deg, &c->max_deg, &c->mean_deg, &c->start_min_deg,
                   &c->start_max_deg, &c->observations) != 11)
        {
            good = 0;
            break;
        }
        c->direction = (steer_direction_t)d;
        c->partitioned = (unsigned)partitioned;
        if (partitioned &&
            (fscanf(f, "%f %f %f %f", &c->zone_low_deg, &c->zone_high_deg, &c->peak_low_deg,
                    &c->peak_high_deg) != 4 ||
             !isfinite(c->zone_low_deg) || !isfinite(c->zone_high_deg) ||
             !isfinite(c->peak_low_deg) || !isfinite(c->peak_high_deg) ||
             c->zone_low_deg >= c->zone_high_deg || c->zone_low_deg > c->start_min_deg ||
             c->zone_high_deg <= c->start_max_deg || c->peak_low_deg > 0 || c->peak_high_deg < 0 ||
             c->peak_low_deg < -36 || c->peak_high_deg > 36))
        {
            good = 0;
            break;
        }
        if ((d != STEER_LEFT && d != STEER_RIGHT) || !c->duration_us || c->duration_us > 60000 ||
            c->priority > 30 || !c->n || c->moved > c->n || !c->observations ||
            c->observations > STEERING_RESPONSE_MAX_OBSERVATIONS || c->observations != c->n ||
            !isfinite(c->min_deg) || !isfinite(c->max_deg) || !isfinite(c->mean_deg) ||
            !isfinite(c->start_min_deg) || !isfinite(c->start_max_deg) || c->min_deg < -.4f ||
            c->max_deg > 40 || c->min_deg > c->mean_deg || c->mean_deg > c->max_deg ||
            c->start_min_deg > c->start_max_deg)
        {
            good = 0;
            break;
        }
        double sum = 0;
        float lo = 100, hi = -100;
        for (unsigned j = 0; j < c->observations; ++j)
        {
            float v;
            if (fscanf(f, "%f", &v) != 1 || !isfinite(v) || v < -.4f || v > 40)
            {
                good = 0;
                break;
            }
            c->responses_deg[j] = v;
            sum += v;
            lo = fminf(lo, v);
            hi = fmaxf(hi, v);
        }
        if (fabs(sum / c->observations - c->mean_deg) > .002 || fabsf(lo - c->min_deg) > .002f ||
            fabsf(hi - c->max_deg) > .002f)
            good = 0;
        if (partitioned)
        {
            float peak = d == STEER_LEFT ? c->peak_high_deg : -c->peak_low_deg;
            if (peak + .01f < c->max_deg)
                good = 0;
        }
        for (unsigned j = 0; j < i; ++j)
            if (out.cells[j].direction == c->direction &&
                out.cells[j].duration_us == c->duration_us &&
                out.cells[j].priority == c->priority &&
                (!partitioned || !(out.cells[j].zone_high_deg <= c->zone_low_deg ||
                                   out.cells[j].zone_low_deg >= c->zone_high_deg)))
                good = 0;
    }
    char extra;
    if (good && fscanf(f, " %c", &extra) == 1)
        good = 0;
    fclose(f);
    if (!good)
        return STEER_ERR_ARGUMENT;
    *m = out;
    return STEER_OK;
}
steer_result_t steering_response_choose(const steering_response_model_t *m, float current,
                                        float target, float tol, float minimum, float maximum,
                                        unsigned max_us, unsigned min_n, unsigned priority,
                                        steering_response_choice_t *out)
{
    if (!m || !out || !m->count || m->count > STEERING_RESPONSE_MAX_CELLS || !isfinite(current) ||
        !isfinite(target) || !isfinite(tol) || tol <= 0 || !isfinite(minimum) ||
        !isfinite(maximum) || minimum >= maximum || minimum < -18 || maximum > 18 ||
        current < minimum || current > maximum || target < minimum || target > maximum || !max_us ||
        max_us > 60000 || !min_n)
        return STEER_ERR_ARGUMENT;
    *out = (steering_response_choice_t){0};
    float error = target - current;
    if (fabsf(error) <= tol)
        return STEER_OK;
    int sign = error > 0 ? 1 : -1;
    float best_worst = 1e9f, best_expected = 1e9f, best_score = 1e9f;
    unsigned best_us = ~0u;
    for (unsigned i = 0; i < m->count; ++i)
    {
        const steering_response_cell_t *c = &m->cells[i];
        if (c->direction != (sign > 0 ? STEER_LEFT : STEER_RIGHT) || c->priority != priority ||
            c->n < min_n || !c->moved || !c->observations || c->duration_us > max_us ||
            c->min_deg < -.15f || current < c->start_min_deg - 2 || current > c->start_max_deg + 2)
            continue;
        if (c->partitioned &&
            (current < c->zone_low_deg || current >= c->zone_high_deg ||
             current < c->start_min_deg - .75f || current > c->start_max_deg + .75f))
            continue;
        float guard = fmaxf(1, c->max_deg * 1.2f + .3f), pred = current + sign * guard;
        if (c->partitioned)
        {
            float low = c->peak_low_deg * 1.2f - .3f, high = c->peak_high_deg * 1.2f + .3f;
            if (current + low < minimum || current + high > maximum)
                continue;
            guard = sign > 0 ? high : -low;
        }
        else if (pred < minimum || pred > maximum)
            continue;
        float worst =
            fmaxf(fabsf(error - sign * fmaxf(0, c->min_deg)), fabsf(error - sign * c->max_deg));
        double expected = 0;
        for (unsigned j = 0; j < c->observations; ++j)
            expected += fabsf(error - sign * c->responses_deg[j]);
        expected /= c->observations;
        if (worst > fabsf(error) + .15f || expected >= fabsf(error) - .15f)
            continue;
        float score = (float)expected;
        if (c->partitioned)
        {
            /* 射频与按键扫描能否接受触发，也属于脉冲族属性。
             * 局部小分区可能恰好只包含成功的 19ms
             * 记录；角度和峰值仍取局部值，
             * 但需合并实测未响应样本，避免粗调依赖不可靠的短按。 */
            unsigned family_n = 0, family_missed = 0;
            for (unsigned k = 0; k < m->count; ++k)
            {
                const steering_response_cell_t *f = &m->cells[k];
                if (f->partitioned && f->direction == c->direction &&
                    f->duration_us == c->duration_us && f->priority == priority && f->n)
                {
                    family_n += f->n;
                    family_missed += f->n - f->moved;
                }
            }
            score += (float)(family_missed + 1) / (float)(family_n + 2) * fminf(fabsf(error), 6);
        }
        int better =
            c->partitioned
                ? (score < best_score - .00001f ||
                   (fabsf(score - best_score) < .00001f &&
                    (worst < best_worst - .00001f ||
                     (fabsf(worst - best_worst) < .00001f && c->duration_us < best_us))))
                : (worst < best_worst - .00001f ||
                   (fabsf(worst - best_worst) < .00001f &&
                    (expected < best_expected - .00001 ||
                     (fabs(expected - best_expected) < .00001 && c->duration_us < best_us))));
        if (better)
        {
            best_score = score;
            best_worst = worst;
            best_expected = (float)expected;
            best_us = c->duration_us;
            *out =
                (steering_response_choice_t){c->direction, c->duration_us, guard, (float)expected};
        }
    }
    return out->duration_us ? STEER_OK : STEER_ERR_NOT_READY;
}
steer_result_t steering_response_choose_reposition(const steering_response_model_t *m,
                                                   float current, float target, float tol,
                                                   float minimum, float maximum, unsigned max_us,
                                                   unsigned min_n, unsigned priority,
                                                   steering_response_choice_t *out)
{
    steering_response_choice_t direct;
    steer_result_t rc = steering_response_choose(m, current, target, tol, minimum, maximum, max_us,
                                                 min_n, priority, &direct);
    if (rc != STEER_ERR_NOT_READY)
        return rc == STEER_OK ? STEER_ERR_ARGUMENT : rc;
    float error = target - current;
    if (fabsf(error) > 4 || fabsf(error) <= tol)
        return STEER_ERR_NOT_READY;
    *out = (steering_response_choice_t){0};
    float best = 1e9f;
    for (unsigned i = 0; i < m->count; ++i)
    {
        const steering_response_cell_t *c = &m->cells[i];
        if (!c->partitioned || c->direction != (error > 0 ? STEER_RIGHT : STEER_LEFT) ||
            c->priority != priority || c->n < min_n || !c->moved || c->duration_us > max_us ||
            c->min_deg < -.15f || c->max_deg > 5 || !c->observations || current < c->zone_low_deg ||
            current >= c->zone_high_deg || current < c->start_min_deg - .75f ||
            current > c->start_max_deg + .75f)
            continue;
        float low = c->peak_low_deg * 1.2f - .3f, high = c->peak_high_deg * 1.2f + .3f;
        if (current + low < minimum || current + high > maximum)
            continue;
        int sign = c->direction == STEER_LEFT ? 1 : -1;
        double cost = 0;
        unsigned planned = 0;
        for (unsigned j = 0; j < c->observations; ++j)
        {
            float after = current + sign * c->responses_deg[j];
            steering_response_choice_t second;
            rc = steering_response_choose(m, after, target, tol, minimum, maximum, max_us, min_n,
                                          priority, &second);
            if (rc == STEER_OK)
            {
                cost += second.duration_us ? second.expected_error_deg : 0;
                ++planned;
            }
            else
                cost += fabsf(target - after);
        }
        cost /= c->observations;
        if (planned * 2 < c->observations || cost >= fabsf(error) - .25f)
            continue;
        if (cost < best)
        {
            best = (float)cost;
            *out = (steering_response_choice_t){c->direction, c->duration_us,
                                                sign > 0 ? high : -low, (float)cost};
        }
    }
    return out->duration_us ? STEER_OK : STEER_ERR_NOT_READY;
}
