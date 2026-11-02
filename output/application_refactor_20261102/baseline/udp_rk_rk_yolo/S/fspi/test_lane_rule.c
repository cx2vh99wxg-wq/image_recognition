#define _POSIX_C_SOURCE 200809L
#include "lane_rule.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
    for(unsigned left=0;left<=2;++left)for(unsigned right=0;right<=2;++right) {
        LineStyleData d={0};d.left=left;d.right=right;d.left_quality=d.right_quality=90;
        line_set_deadline(&d,2000000000ULL);
        unsigned denied=lane_denied_sides(&d,1500000000ULL);
        assert(denied==((left==1?1u:0u)|(right==1?2u:0u)));
        assert(lane_blocked_request(denied,0xcc)==(denied&1));
        assert(lane_blocked_request(denied,0xdd)==(denied&2));
        assert(!lane_blocked_request(denied,0xaa));
        assert(!lane_blocked_request(denied,0xbb));
        assert(!lane_blocked_request(denied,0xee));
        assert(!lane_denied_sides(&d,2000000000ULL));
        d.left_quality=d.right_quality=64;assert(!lane_denied_sides(&d,1500000000ULL));
    }
    puts("lane policy: 9 side combinations, CC/DD input, traction independence, expiry and quality PASS");
}
