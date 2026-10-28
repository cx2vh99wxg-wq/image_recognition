#define _POSIX_C_SOURCE 200809L
#include "traffic_go_rule.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
    const uint64_t now=1000000000ULL;
    TrafficLightData d={0};TrafficGoPolicy p={0};
    d.flags=TRAFFIC_BOX|TRAFFIC_CONFIRMED|TRAFFIC_MODEL_READY|TRAFFIC_TRACKED;
    d.state=d.raw_state=TRAFFIC_RED;traffic_set_deadline(&d,now+TRAFFIC_TTL_NS);
    assert(traffic_go_prepare(&p,TRAFFIC_UNKNOWN)==0&&!p.blocked);
    unsigned color=traffic_go_color(&d,now);assert(color==TRAFFIC_RED);
    unsigned cmd=traffic_go_prepare(&p,color);assert(cmd==4&&p.blocked);
    traffic_go_commit(&p,cmd,0);assert(p.blocked);
    assert(traffic_go_prepare(&p,TRAFFIC_YELLOW)==4);
    assert(traffic_go_prepare(&p,TRAFFIC_UNKNOWN)==4);
    d.state=d.raw_state=TRAFFIC_GREEN;
    d.flags&=~TRAFFIC_CONFIRMED;assert(!traffic_go_color(&d,now));
    d.flags|=TRAFFIC_CONFIRMED;
    d.flags|=TRAFFIC_DISABLED;assert(!traffic_go_color(&d,now));d.flags&=~TRAFFIC_DISABLED;
    d.raw_state=TRAFFIC_RED;assert(!traffic_go_color(&d,now));d.raw_state=TRAFFIC_GREEN;
    assert(!traffic_go_color(&d,now+TRAFFIC_TTL_NS));
    assert(!traffic_go_color(&d,now-1));
    cmd=traffic_go_prepare(&p,traffic_go_color(&d,now));assert(cmd==8&&p.blocked);
    traffic_go_commit(&p,cmd,0);assert(p.blocked); // failed clear retries, warning stays
    traffic_go_commit(&p,cmd,1);assert(!p.blocked);
    assert(traffic_go_prepare(&p,TRAFFIC_UNKNOWN)==0);
    TrafficGoSegment seg={0};TrafficGoIPC ipc={0,&seg};TrafficGoData a={0},b={0};
    assert(traffic_go_read(&ipc,&b));
    a.blocked=1;a.write_ok=1;a.signal_state=TRAFFIC_RED;a.source_id=123;
    traffic_go_set_deadline(&a,now+400000000ULL);traffic_go_write(&ipc,&a);
    assert(!traffic_go_read(&ipc,&b)&&b.blocked&&b.source_id==123);
    assert(traffic_go_fresh(&b,now)&&!traffic_go_fresh(&b,now+400000000ULL));
    // A restarted controller can recover the previous warning even after TTL.
    TrafficGoPolicy restarted={b.blocked};assert(traffic_go_prepare(&restarted,TRAFFIC_UNKNOWN)==4);
    seg.sequence|=1;assert(traffic_go_read(&ipc,&b));
    puts("traffic GO: confirmed colors, stale/missing data, green-only release, failed SPI, restart telemetry PASS");
    return 0;
}
