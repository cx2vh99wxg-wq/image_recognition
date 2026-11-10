#include "person_display_candidates.h"
#include "person_scene_snapshot.h"
#include <cassert>
#include <cstdio>
#include <cstring>
int main(){
 detect_result_group_t g={};g.count=OBJ_NUMB_MAX_SIZE;
 detect_result_t r={};strcpy(r.name,"person");r.prop=.2f;r.box.left=80;r.box.right=110;r.box.top=80;r.box.bottom=170;
 person_display_add(g,r);r.prop=.8f;r.box.left++;r.box.right++;person_display_add(g,r);
 assert(g.person_display_count==1&&g.person_display_results[0].prop==.8f);
 r.box.left=400;r.box.right=430;person_display_add(g,r);assert(g.person_display_count==2);
 const auto scene=s_vision::person_scene(g,true,1000000000ULL,12,1);assert(scene.count==2);
 assert(display_people::deadline(scene)==1750000000ULL);
 assert(scene.people[0].view==display_people::Rear&&scene.people[1].view==display_people::RightRear);
 assert(!g.person_brake_count); // the display reservoir never mutates braking statistics
 puts("PERSON_DISPLAY_PASS: strong overlap replacement, two views, full legacy display buffer cannot hide people, original capture timestamp, no control mutation");
}
