#ifndef S_PERSON_SCENE_SNAPSHOT_H
#define S_PERSON_SCENE_SNAPSHOT_H
#include "postprocess.h"
#include "../../include/person_brake_trigger.h"
#include "../../../common/perception/include/display_people.h"
namespace s_vision {
inline display_people::Snapshot person_scene(const detect_result_group_t&r,bool valid,uint64_t captured,uint32_t session,uint32_t source){
 display_people::Snapshot s={};s.session=session;s.source=source;s.ready=valid;
 display_people::expires(s,captured+display_people::TTL);
 if(!valid)return s;
 const auto add=[&](const detect_result_t&d){
  if(!person_brake_should_trigger(d.name,d.prop))return;
  const auto&b=d.box;int cx=(b.left+b.right)/2,cy=(b.top+b.bottom)/2;
  if(cx<0||cx>=640||cy<0||cy>=480||b.right<=b.left||b.bottom<=b.top)return;
  unsigned tile=(cx>=320?1:0)+(cy>=240?2:0);
  // Tiles 0 and 3 share the rear-center camera. Map both into one view;
  // box deduplication avoids a duplicate while retaining a tile-0-only hit.
  const int tx=tile%2*320,ty=tile/2*240;
  const int left=std::max(tx,b.left),top=std::max(ty,b.top);
  const int bottom=std::min(ty+240,b.bottom);
  // The existing S video compositor flips each PCIe view vertically. Keep
  // glyph foot coordinates in that same displayed image, not model rows.
  auto p=display_people::box(tile==1?display_people::RightRear:tile==2?display_people::LeftRear:display_people::Rear,
      left-tx,ty+240-bottom,std::min(tx+320,b.right)-left,bottom-top,d.prop);
  display_people::add(s,p);
 };
 if(r.person_display_count>0){for(int i=0;i<std::min(16,r.person_display_count);++i)add(r.person_display_results[i]);}
 else for(int i=0;i<std::min(r.count,OBJ_NUMB_MAX_SIZE);++i)add(r.results[i]);
 if(r.person_brake_count)add(r.person_brake_result);
 return s;
}
}
#endif
