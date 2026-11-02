#include "brake_release_policy.h"
#include <stdio.h>

static int expect_brake(const char *name, BrakeReleaseState *state,
                        int person_detected, int detection_valid,
                        int clock_valid, uint64_t now_ms, int expected)
{
    int actual = brake_release_required(state, person_detected,
                                        detection_valid, clock_valid, now_ms);
    if (actual != expected) {
        fprintf(stderr, "%s: expected %d, got %d\n", name, expected, actual);
        return 1;
    }
    return 0;
}

int main(void)
{
    BrakeReleaseState state = {0};
    int failed = 0;

    failed |= expect_brake("person asserts immediately", &state, 1, 1, 1, 100, 1);
    failed |= expect_brake("first clear frame holds", &state, 0, 1, 1, 100, 1);
    failed |= expect_brake("brief missed detection holds", &state, 0, 1, 1, 1100, 1);
    failed |= expect_brake("before two seconds holds", &state, 0, 1, 1, 2099, 1);
    failed |= expect_brake("two seconds releases", &state, 0, 1, 1, 2100, 0);

    failed |= expect_brake("person reappears immediately", &state, 1, 1, 1, 2200, 1);
    failed |= expect_brake("new clear period starts", &state, 0, 1, 1, 2210, 1);
    failed |= expect_brake("inference failure resets hold", &state, 0, 0, 1, 3200, 1);
    failed |= expect_brake("valid clear restarts after failure", &state, 0, 1, 1, 3210, 1);
    failed |= expect_brake("old clear period cannot release", &state, 0, 1, 1, 4200, 1);
    failed |= expect_brake("full new clear period releases", &state, 0, 1, 1, 5210, 0);

    failed |= expect_brake("clock failure asserts brake", &state, 0, 1, 0, 0, 1);
    failed |= expect_brake("clock recovery starts new hold", &state, 0, 1, 1, 6000, 1);
    failed |= expect_brake("clock rollback restarts hold", &state, 0, 1, 1, 5999, 1);
    failed |= expect_brake("rollback hold remains", &state, 0, 1, 1, 7998, 1);
    failed |= expect_brake("rollback hold releases", &state, 0, 1, 1, 7999, 0);
    failed |= expect_brake("null state fails safe", NULL, 0, 1, 1, 9000, 1);

    if (failed) {
        return 1;
    }
    puts("brake release policy tests passed");
    return 0;
}
