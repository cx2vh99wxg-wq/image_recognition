#include "drive_policy.h"
ControlCommand drive_policy_step(drive_policy_t *p,const VisionResult *m,uint64_t mt,
                                 const VisionResult *s,uint64_t st,uint64_t now)
{
    if (!p) return CMD_STOP;
    int mf=vision_fresh(m,mt,now),sf=vision_fresh(s,st,now);
    if ((mf&&m->person_mask)||(sf&&s->person_mask)) {p->person_latched=1;p->clear_frames=0;}
    if (mf&&m->light.detected&&(m->light.state==TL_RED||m->light.state==TL_YELLOW)) {
        p->red_latched=1;p->green_frames=0;
    }
    if (!mf||!sf) { p->clear_frames=p->green_frames=0;return CMD_STOP; }
    if (m->analysis_frame_id!=p->last_m) {
        p->last_m=m->analysis_frame_id;
        if (m->light.detected&&m->light.state==TL_GREEN&&m->light.confidence>=50) {
            if (++p->green_frames>=3)p->red_latched=0;
        } else p->green_frames=0;
        if (m->zebra.detected) {
            if (!p->zebra_seen) p->coast_until=now+400000;
            p->zebra_seen=1;
        } else p->zebra_seen=0;
    }
    /* Require three distinct completed sweeps from BOTH boards. */
    if (s->analysis_frame_id!=p->clear_s && m->analysis_frame_id!=p->clear_m) {
        p->clear_s=s->analysis_frame_id;p->clear_m=m->analysis_frame_id;
        p->last_s=s->analysis_frame_id;
        if (!m->person_mask&&!s->person_mask) { if(++p->clear_frames>=3)p->person_latched=0; }
        else p->clear_frames=0;
    }
    if(p->person_latched||p->red_latched)return CMD_STOP;
    if(m->lane.version!=LANERESULT_VERSION||m->lane.confidence<DEC_GO_CONF_MIN)return CMD_STOP;
    if(m->marks.crossing && ((m->marks.crossing_left&&m->marks.left_type==LANE_MARK_SOLID)||
                             (m->marks.crossing_right&&m->marks.right_type==LANE_MARK_SOLID))) return CMD_STOP;
    if(now<p->coast_until)return CMD_COAST;
    switch(m->lane.direction) {
        case LANE_STRAIGHT:return CMD_GO;
        case LANE_LEFT:return CMD_LEFT;
        case LANE_RIGHT:return CMD_RIGHT;
        default:return CMD_STOP;
    }
}
