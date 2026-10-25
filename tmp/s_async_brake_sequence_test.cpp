// Design-level test only: not linked into production. The real release policy
// is exercised behind a deterministic clock, with all bridge calls serialized.
#include "brake_release_policy.h"
#include "person_brake_trigger.h"
#include <cassert>
#include <cstdio>
#include <mutex>
#include <vector>
struct Publication { uint64_t now,source;bool person,valid,required; };
struct TestPublisher {
    BrakeReleaseState release={};std::vector<Publication> history;
    void emit(uint64_t now,uint64_t source,bool person,bool valid) {
        bool required=brake_release_required(&release,person,valid,now!=0,now)!=0;
        history.push_back({now,source,person,valid,required});
    }
    bool required() const {return history.empty()||history.back().required;}
};
class ProposedSafetyBridge {
public:
    explicit ProposedSafetyBridge(TestPublisher& p):p_(p){}
    void complete(uint64_t source,uint64_t captured,uint64_t now,bool valid,bool person) {
        std::lock_guard<std::mutex> guard(mutex_);
        bool ordered=source>source_;
        bool fresh=now&&captured&&now>=captured&&now-captured<=500;
        bool gap=completed_&&(now<completed_||now-completed_>500);
        if(gap) p_.emit(now,source_,false,false); // reset interrupted clear interval
        p_.emit(now,source,valid&&person,valid&&fresh&&ordered);
        source_=source;captured_=captured;completed_=now;
    }
    void watchdog(uint64_t now) {
        std::lock_guard<std::mutex> guard(mutex_);
        if(!now||!captured_||now<captured_||now-captured_>500)
            p_.emit(now,source_,false,false);
        // A fresh cached negative is not emitted as another valid observation.
    }
private:
    TestPublisher& p_;std::mutex mutex_;
    uint64_t source_=0,captured_=0,completed_=0;
};
int main() {
    assert(!person_brake_should_trigger("person",0.14999f));
    assert(person_brake_should_trigger("person",0.15f));
    assert(!person_brake_should_trigger("car",0.99f));
    TestPublisher p;ProposedSafetyBridge bridge(p);
    uint64_t id=0;
    for(uint64_t now=1000;now<=3000;now+=100)bridge.complete(++id,now-85,now,true,false);
    assert(!p.required());
    bridge.complete(++id,3015,3100,true,true);assert(p.required());
    bridge.complete(++id,3115,3200,true,false);assert(p.required());
    // A single cached negative cannot advance two seconds of clearance.
    for(uint64_t now=3230;now<=5300;now+=30)bridge.watchdog(now);
    assert(p.required());
    // A >500ms worker gap must reset even if main missed its watchdog check.
    bridge.complete(++id,5415,5500,true,false);assert(p.required());
    for(uint64_t now=5600;now<=7400;now+=100) {
        bridge.complete(++id,now-85,now,true,false);assert(p.required());
    }
    bridge.complete(++id,7415,7500,true,false);assert(!p.required());
    // Every completed failure publishes immediately; a later empty result
    // cannot overwrite its hold period before the display thread sees it.
    bridge.complete(++id,7515,7600,false,false);assert(p.required());
    bridge.complete(++id,7615,7700,true,false);assert(p.required());
    bridge.complete(++id,8500,9000,true,false);assert(p.required());
    // Old/out-of-order/future results never clear; a stale positive still
    // produces a conservative request through detection_valid=false.
    bridge.complete(++id,8000,9100,true,true);assert(p.required());
    bridge.complete(id,9115,9200,true,false);assert(p.required());
    bridge.complete(++id,9500,9300,true,false);assert(p.required());
    puts("real release policy: cached negatives / missing jobs / failure / threshold / stale / future / ordering PASS");
}
