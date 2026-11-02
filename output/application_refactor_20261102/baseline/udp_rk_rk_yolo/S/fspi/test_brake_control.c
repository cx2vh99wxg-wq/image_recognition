#include "brake_policy.h"
#include <stdio.h>

struct BrakeCase {
    const char *name;
    int enabled;
    int status_valid;
    int person_detected;
    int brake_enable;
    unsigned int last_update;
    unsigned int now;
    int expected;
};

int main(void)
{
    const struct BrakeCase cases[] = {
        {"missing detector", 1, 0, 0, 1, 0, 100, 1},
        {"first frame pending", 1, 1, 0, 1, 0, 100, 1},
        {"fresh clear frame", 1, 1, 0, 1, 100, 100, 0},
        {"person detected", 1, 1, 1, 0, 100, 100, 1},
        {"inference failure", 1, 1, 0, 0, 100, 100, 1},
        {"one second old", 1, 1, 0, 1, 99, 100, 0},
        {"two seconds old", 1, 1, 0, 1, 98, 100, 1},
        {"expired frame", 1, 1, 0, 1, 97, 100, 1},
        {"invalid future time", 1, 1, 0, 1, 101, 100, 1},
        {"explicitly disabled", 0, 0, 1, 0, 0, 100, 0},
    };

    for (unsigned int i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const struct BrakeCase *test = &cases[i];
        int actual = brake_required(test->enabled, test->status_valid,
                                    test->person_detected, test->brake_enable,
                                    test->last_update, test->now);
        if (actual != test->expected) {
            fprintf(stderr, "%s: expected %d, got %d\n",
                    test->name, test->expected, actual);
            return 1;
        }
    }

    puts("brake policy tests passed");
    return 0;
}
