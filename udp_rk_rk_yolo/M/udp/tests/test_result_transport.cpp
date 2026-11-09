#include "../../../common/transport/include/curve_result_wire.h"
#include "../../../common/transport/include/result_schedule.h"
#include <cassert>
#include <cstdio>
int main(){
 uint64_t now=1000000000ULL;video_v2::Metadata a;a.session_id=7;a.curve_frame_id=100;a.curve_type=1;a.curve_confidence=85;a.curve_angle=190;a.curve_age_ms=240;
 uint8_t p[curve_wire::SIZE];assert(curve_wire::encode(p,a));curve_wire::Receiver packets;video_v2::Metadata decoded;
 assert(packets.ingest(p,sizeof(p),12,now,now,decoded));video_v2::CurveReceiverState state;video_v2::CurveEvent event;
 assert(state.observe(decoded,now,event)&&event.confidence==85&&event.type==1);
 assert(!state.observe(decoded,now+100000000ULL,event));assert(!state.expire(now+259000000ULL,event));
 assert(state.expire(now+260000000ULL,event)&&!event.confidence);assert(!state.observe(decoded,now+300000000ULL,event));
 a.session_id=8;a.curve_frame_id=1;assert(curve_wire::encode(p,a));assert(packets.ingest(p,sizeof(p),12,now+400000000ULL,now+400000000ULL,decoded));
 video_v2::Metadata old=decoded;old.session_id=7;assert(!packets.allow_video(old,now+500000000ULL));assert(packets.allow_video(decoded,now+500000000ULL));
 a.session_id=7;assert(curve_wire::encode(p,a));assert(!packets.ingest(p,sizeof(p),12,now+500000000ULL,now+500000000ULL,decoded));
 p[15]^=1;assert(!packets.ingest(p,sizeof(p),12,now,now,decoded));a.curve_confidence=101;assert(!curve_wire::encode(p,a));
 result_transport::Schedule send;a.curve_confidence=85;assert(curve_wire::encode(p,a));assert(send.due(p,sizeof(p),28,now,50000000ULL));
 a.curve_age_ms=241;assert(curve_wire::encode(p,a));assert(!send.due(p,sizeof(p),28,now+1000000ULL,50000000ULL));
 a.curve_angle=195;assert(curve_wire::encode(p,a));assert(send.due(p,sizeof(p),28,now+2000000ULL,50000000ULL));
 assert(!send.due(p,sizeof(p),28,now+51000000ULL,50000000ULL));assert(send.due(p,sizeof(p),28,now+52000000ULL,50000000ULL));
 puts("RESULT TRANSPORT PASS: direct curve before video, one receiver/expiry state, duplicate expiry, retired session protection, checksum/bounds, immediate semantic changes, bounded heartbeat");
}
