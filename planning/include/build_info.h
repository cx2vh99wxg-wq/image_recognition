#ifndef PLANNING_BUILD_INFO_H
#define PLANNING_BUILD_INFO_H
#include <stdio.h>
#include "driving_config.h"
#define PLANNING_BUILD_TAG "ADAS-6CH-v2"
static inline void planning_build_info(const char *program)
{
    printf("%s %s layout=%dx%d M=top S=bottom STEREO-J8-v1 built=%s %s\n",
           program, PLANNING_BUILD_TAG, DISP_WIN_W, DISP_WIN_H, __DATE__, __TIME__);
    fflush(stdout);
}
#endif
