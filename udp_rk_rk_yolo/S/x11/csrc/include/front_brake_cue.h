#ifndef FRONT_BRAKE_CUE_H
#define FRONT_BRAKE_CUE_H
#include "../../../include/front_person_guard.h"
namespace lane_cue {
// Display only: consume the controller's existing latched front request.
// Never re-run detection, advance recovery counters or write a brake/gate.
inline bool front_stop_tint(const FrontGuardData&state,uint64_t now){
 return front_guard_fresh(&state,now)&&state.auto_enabled==1&&state.raw_request==1;
}
}
#endif
