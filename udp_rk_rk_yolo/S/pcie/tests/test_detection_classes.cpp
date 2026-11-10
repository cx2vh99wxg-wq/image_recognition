#include "postprocess.h"
#include "detection_classes.h"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unistd.h>
int main(){
 char path[]="/tmp/adas-classes-XXXXXX";int fd=mkstemp(path);assert(fd>=0);FILE*f=fdopen(fd,"w");assert(f);
 for(int i=0;i<80;++i)fprintf(f,"%s\n",i==0?"person":i==2?"car":i==9?"traffic light":i==39?"bottle":"other");fclose(f);
 assert(!initPostProcess(path));unlink(path);
 std::vector<int8_t> h[3];for(int k=0;k<3;++k){int n=(32/(8<<k))*(32/(8<<k));h[k].assign(3*85*n,-128);}
 auto anchor=[&](int cell,int cls){int n=16;for(int c=0;c<4;++c)h[0][c*n+cell]=0;h[0][4*n+cell]=127;h[0][(5+cls)*n+cell]=127;};
 anchor(0,0);anchor(1,39);anchor(2,9);anchor(3,2);
 detect_result_group_t g={};BOX_RECT pads={};assert(!post_process_simple(h[0].data(),h[1].data(),h[2].data(),32,32,.25f,.45f,pads,1,1,&g));
 assert(g.person_brake_count==1&&g.person_display_count==1&&g.count==2);
 for(int i=0;i<g.count;++i)assert(!strcmp(g.results[i].name,"person")||!strcmp(g.results[i].name,"car"));
 // An excluded higher-scoring class must not turn into a person merely
 // because only two classes would otherwise be examined.
 h[0][5*16]=90;h[0][(5+39)*16]=127;
 assert(!post_process_simple(h[0].data(),h[1].data(),h[2].data(),32,32,.25f,.45f,pads,1,1,&g));assert(g.person_brake_count==0);
 assert(s_detection_classes()[0]&&!s_detection_classes()[9]&&!s_detection_classes()[39]);
 deinitPostProcess();puts("DETECTION_CLASSES_PASS person/control competition retained; supported road obstacles only; excluded classes not reclassified");
}
