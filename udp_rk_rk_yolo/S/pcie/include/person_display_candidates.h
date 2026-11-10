#ifndef S_PERSON_DISPLAY_CANDIDATES_H
#define S_PERSON_DISPLAY_CANDIDATES_H
#include "postprocess.h"
#include <algorithm>
inline void person_display_add(detect_result_group_t&g,const detect_result_t&r){
 const auto&b=r.box;if(b.right<=b.left||b.bottom<=b.top)return;
 for(int i=0;i<g.person_display_count;++i){auto&q=g.person_display_results[i];const auto&a=q.box;
  int inter=std::max(0,std::min(a.right,b.right)-std::max(a.left,b.left))*std::max(0,std::min(a.bottom,b.bottom)-std::max(a.top,b.top));
  int aa=(a.right-a.left)*(a.bottom-a.top),bb=(b.right-b.left)*(b.bottom-b.top);
  if(inter*100>=40*(aa+bb-inter)){if(r.prop>q.prop)q=r;return;}
 }
 if(g.person_display_count<16)g.person_display_results[g.person_display_count++]=r;
 else {int low=0;for(int i=1;i<16;++i)if(g.person_display_results[i].prop<g.person_display_results[low].prop)low=i;if(r.prop>g.person_display_results[low].prop)g.person_display_results[low]=r;}
}
#endif
