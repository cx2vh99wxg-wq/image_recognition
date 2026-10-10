#ifndef COMMAND_GUARD_H
#define COMMAND_GUARD_H
#include "driving_types.h"
#include <string.h>
/* reserved[0:1] carry S-board CLOCK_MONOTONIC microseconds, [2] checksum.
 * This is a same-board contract; never compare a remote board's boot clock. */
static inline uint32_t command_checksum(const ControlCommandMsg *m)
{
    ControlCommandMsg c=*m;c.reserved[2]=0;
    const unsigned char *b=(const unsigned char *)&c;uint32_t h=2166136261u;
    for(unsigned i=0;i<sizeof(c);i++)h=(h^b[i])*16777619u;
    return h;
}
static inline void command_stamp(ControlCommandMsg *m,uint64_t now)
{
    m->version=CMDMSG_VERSION;m->reserved[0]=(uint32_t)now;m->reserved[1]=(uint32_t)(now>>32);
    m->reserved[2]=command_checksum(m);
}
static inline int command_valid(const ControlCommandMsg *m,uint64_t now)
{
    uint64_t t=((uint64_t)m->reserved[1]<<32)|m->reserved[0];
    return m->version==CMDMSG_VERSION&&m->enable==1&&m->command>=CMD_GO&&m->command<=CMD_COAST&&
           m->reserved[2]==command_checksum(m)&&t&&now>=t&&now-t<=250000;
}
#endif
