#include "person_alert_effects.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

extern AlertEffectState reference_g_alert_state;
void reference_apply_person_alert_effects(uint8_t *, int, int, const detect_result_group_t *);
int person_alert_cache_wait(unsigned timeout_ms);

int test_gettimeofday(struct timeval *tv, void *zone)
{
    (void)zone;
    tv->tv_sec = 1000; tv->tv_usec = 0;
    return 0;
}

static double now_ms(void)
{
    struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1000000.0;
}

int main(void)
{
    const size_t bytes = 640 * 480 * 3, guard = 32;
    uint8_t *base = malloc(bytes + guard * 2), *old = malloc(bytes + guard * 2), *fast = malloc(bytes + guard * 2);
    assert(base && old && fast);
    uint32_t random = 17;
    for (size_t i = 0; i < bytes + guard * 2; ++i)
    {
        random = random * 1664525u + 1013904223u; base[i] = (uint8_t)(random >> 24);
    }
    int global_max = 0, cases = 0;
    detect_result_group_t group, before;
    for (int scene = 0; scene < 20; ++scene)
    {
        memset(&group, 0, sizeof(group)); group.count = scene % 7;
        if (scene == 17) group.count = 64;
        if (scene == 18) group.count = 10;
        if (scene == 19) group.count = 12;
        for (int i = 0; i < group.count; ++i)
        {
            detect_result_t *r = &group.results[i]; strcpy(r->name, i % 4 == 3 ? "chair" : "person");
            r->prop = 0.5f;
            r->box.left = (i * 131 + scene * 7) % 600 - 20;
            r->box.top = (i * 97 + scene * 9) % 450 - 20;
            r->box.right = r->box.left + 30 + (scene * 13 + i * 23) % 220;
            r->box.bottom = r->box.top + 50 + (scene * 19 + i * 11) % 300;
        }
        before = group;
        for (int phase = 0; phase < 2; ++phase)
        {
            memcpy(old, base, bytes + guard * 2); memcpy(fast, base, bytes + guard * 2);
            g_alert_state = (AlertEffectState){1000000, phase, 1};
            reference_g_alert_state = g_alert_state;
            reference_apply_person_alert_effects(old + guard, 640, 480, &group);
            apply_person_alert_effects(fast + guard, 640, 480, &group);
            assert(person_alert_cache_wait(5000));
            memcpy(fast, base, bytes + guard * 2);
            apply_person_alert_effects(fast + guard, 640, 480, &group);
            assert(!memcmp(&before, &group, sizeof(group)));
            assert(!memcmp(fast, base, guard) && !memcmp(fast + guard + bytes, base + guard + bytes, guard));
            int maximum = 0; double sum = 0;
            for (size_t i = guard; i < guard + bytes; ++i)
            {
                int delta = abs((int)old[i] - fast[i]);
                if (delta > maximum) maximum = delta;
                sum += delta;
            }
            // One composed overlay removes repeated byte truncation. The
            // display-only difference is bounded; detection data is exact.
            if (maximum > 8 || sum / bytes >= 1.1)
                fprintf(stderr, "CACHE DIFFERENCE scene=%d phase=%d count=%d max=%d mean=%.4f\n", scene, phase, group.count, maximum, sum / bytes);
            assert(maximum <= 8 && sum / bytes < 1.1);
            if (maximum > global_max) global_max = maximum;
            ++cases;
        }
    }
    memset(&group, 0, sizeof(group)); group.count = 4;
    for (int i = 0; i < 4; ++i)
    {
        strcpy(group.results[i].name, "person");
        group.results[i].box = (BOX_RECT){i % 2 * 320 + 40, i % 2 * 320 + 260,
                                          i / 2 * 240 + 20, i / 2 * 240 + 220};
    }
    double t = now_ms();
    for (int i = 0; i < 12; ++i)
    {
        memcpy(old, base, bytes + guard * 2);
        reference_g_alert_state = (AlertEffectState){1000000, 1, 1};
        reference_apply_person_alert_effects(old + guard, 640, 480, &group);
    }
    double old_ms = (now_ms() - t) / 12;
    apply_person_alert_effects(fast + guard, 640, 480, &group);
    assert(person_alert_cache_wait(5000));
    t = now_ms();
    for (int i = 0; i < 36; ++i)
    {
        memcpy(fast, base, bytes + guard * 2);
        g_alert_state = (AlertEffectState){1000000, 1, 1};
        apply_person_alert_effects(fast + guard, 640, 480, &group);
    }
    double warm_ms = (now_ms() - t) / 36;
    t = now_ms();
    for (int i = 0; i < 12; ++i)
    {
        group.results[0].box.left = 40 + i % 2;
        apply_person_alert_effects(fast + guard, 640, 480, &group);
        assert(person_alert_cache_wait(5000));
    }
    double rebuild_ms = (now_ms() - t) / 12;
    printf("ALERT CACHE PASS cases=%d max_channel_delta=%d old_ms=%.3f capture_blend_ms=%.3f background_rebuild_ms=%.3f\n",
           cases, global_max, old_ms, warm_ms, rebuild_ms);
    free(base); free(old); free(fast);
}
