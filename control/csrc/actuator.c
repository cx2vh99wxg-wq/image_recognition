#include "actuator.h"
#include <string.h>
void actuator_step(actuator_t *s,ControlCommand cmd,uint64_t now,uint8_t r[5])
{
    uint64_t elapsed=s->last&&now>=s->last?now-s->last:0;
    if(elapsed>100000)elapsed=100000;
    s->position+=s->direction*(int32_t)elapsed;
    if(s->position<-500000)s->position=-500000;
    if(s->position>600000)s->position=600000;
    s->last=now;s->direction=0;memset(r,1,5);
    if(cmd==CMD_STOP||cmd==CMD_BRAKE||cmd==CMD_NONE||cmd>CMD_COAST){r[4]=0;return;}
    if(cmd==CMD_COAST)return;
    int target=cmd==CMD_LEFT?-500000:cmd==CMD_RIGHT?600000:0;
    int error=target-s->position;
    if(error>20000){s->direction=1;r[3]=0;}
    else if(error<-20000){s->direction=-1;r[2]=0;}
    else s->position=target;
    /* Turn commands explicitly include forward; centring before straight/back
     * releases throttle until complete. No inherited previous motor state. */
    if(cmd==CMD_LEFT||cmd==CMD_RIGHT||s->direction==0)r[cmd==CMD_BACK?1:0]=0;
}
