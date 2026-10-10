#include "vision_frame.h"
#include <string.h>
int vision_validate(const VisionResult *v)
{
    if (!v || v->version!=VISION_VERSION || v->front_camera>2 || v->healthy>1 ||
        v->box_count>VISION_BOXES || v->zebra_count>16 || v->person_mask>7 || v->lane.direction>LANE_RIGHT || v->lane.direction<LANE_UNKNOWN ||
        v->lane.confidence>100 || v->light.state<TL_UNKNOWN || v->light.state>TL_GREEN ||
        v->light.detected>1 || v->light.confidence>100 || v->zebra.detected>1 || v->zebra.confidence>100 ||
        v->marks.left_type<LANE_MARK_NONE || v->marks.left_type>LANE_MARK_UNKNOWN ||
        v->marks.right_type<LANE_MARK_NONE || v->marks.right_type>LANE_MARK_UNKNOWN || v->marks.confidence>100) return -1;
    for (unsigned i=0;i<v->box_count;i++) {
        const VisionBox *b=&v->boxes[i];
        if (b->camera>2 || b->x1<0 || b->y1<0 || b->x2>320 || b->y2>240 ||
            b->x1>=b->x2 || b->y1>=b->y2 || b->score>100 || b->kind>1) return -1;
    }
    for (int side=0;side<2;side++) for (int i=0;i<VISION_POINTS;i++)
        if (v->lane_x[side][i]<-1 || v->lane_x[side][i]>=320) return -1;
    for (unsigned i=0;i<v->zebra_count;i++) if (v->zebra_rows[i]>=240) return -1;
    return 0;
}
int vision_fresh(const VisionResult *v,uint64_t arrived,uint64_t now)
{
    return vision_validate(v)==0 && v->healthy && arrived && now>=arrived &&
        now-arrived<=VISION_STREAM_TIMEOUT_MS*1000ull &&
        (uint64_t)v->age_ms*1000+now-arrived<=VISION_MAX_AGE_MS*1000ull;
}
void vision_crop_rgb(const uint8_t *board,int cam,uint8_t *rgb)
{
    int ox=(cam%2)*320,oy=(cam/2)*240;
    for (int y=0;y<240;y++) for (int x=0;x<320;x++) {
        const uint8_t *p=board+((y+oy)*640+x+ox)*2;
        unsigned n=p[0]|((unsigned)p[1]<<8);
        *rgb++=(uint8_t)(((n>>11)&31)*255/31);
        *rgb++=(uint8_t)(((n>>5)&63)*255/63);
        *rgb++=(uint8_t)((n&31)*255/31);
    }
}
static void pixel(uint8_t *p,int cam,int x,int y,unsigned color)
{
    if (cam<0||cam>2||x<0||x>=320||y<0||y>=240) return;
    p+=(((cam/2)*240+y)*640+(cam%2)*320+x)*2;
    p[0]=(uint8_t)color; p[1]=(uint8_t)(color>>8);
}
static void line(uint8_t *p,int cam,int x0,int y0,int x1,int y1,unsigned color)
{
    int dx=x1-x0,dy=y1-y0,n=dx<0?-dx:dx,ay=dy<0?-dy:dy;
    if (ay>n) n=ay;
    if (n>640) return;
    if (!n) { pixel(p,cam,x0,y0,color); return; }
    for (int i=0;i<=n;i++) for (int t=0;t<2;t++) pixel(p,cam,x0+dx*i/n+t,y0+dy*i/n,color);
}
void vision_annotate(uint8_t *p,const VisionResult *v)
{
    if (!p||vision_validate(v)<0 || !v->healthy || v->age_ms>VISION_MAX_AGE_MS) return;
    int cam=(int)v->front_camera;
    /* Sparse drivable-area tint, separate from green zebra markings. */
    for (int y=0;y<60;y++) for (int x=0;x<80;x++)
        if (v->drivable[(y*80+x)/8]&(1u<<((y*80+x)%8))) pixel(p,cam,x*4,y*4,0x0410);
    for (int kind=1;kind>=0;kind--) for (unsigned i=0;i<v->box_count;i++) {
        const VisionBox *b=&v->boxes[i];if(b->kind!=kind)continue; unsigned c=b->kind==0?0xf800:0x001f;
        line(p,b->camera,b->x1,b->y1,b->x2-1,b->y1,c);
        line(p,b->camera,b->x1,b->y2-1,b->x2-1,b->y2-1,c);
        line(p,b->camera,b->x1,b->y1,b->x1,b->y2-1,c);
        line(p,b->camera,b->x2-1,b->y1,b->x2-1,b->y2-1,c);
    }
    for (int s=0;s<2;s++) for (int i=1;i<VISION_POINTS;i++) {
        int type=s?v->marks.right_type:v->marks.left_type;
        if (type==LANE_MARK_NONE || type==LANE_MARK_UNKNOWN ||
            (type==LANE_MARK_DASHED && (i/3)%2)) continue;
        int a=v->lane_x[s][i-1],b=v->lane_x[s][i];
        if (a>=0&&b>=0) line(p,cam,a,80+(i-1)*5,b,80+i*5,0xf800);
    }
    for (unsigned i=0;i<v->zebra_count;i++) line(p,cam,60,v->zebra_rows[i],260,v->zebra_rows[i],0x07e0);
    if (v->light.detected && v->light.box_x>=0 && v->light.box_y>=0 &&
        v->light.box_w>0 && v->light.box_h>0 && v->light.box_x+v->light.box_w<=320 && v->light.box_y+v->light.box_h<=240) {
        int x=v->light.box_x,y=v->light.box_y,w=v->light.box_w,h=v->light.box_h;
        unsigned c=v->light.state==TL_GREEN?0x07e0:v->light.state==TL_RED?0xf800:0xffe0;
        line(p,cam,x,y,x+w-1,y,c); line(p,cam,x,y+h-1,x+w-1,y+h-1,c);
        line(p,cam,x,y,x,y+h-1,c); line(p,cam,x+w-1,y,x+w-1,y+h-1,c);
    }
}
