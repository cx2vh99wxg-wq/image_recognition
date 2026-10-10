#include "drive_policy.h"
#include "command_guard.h"
#include "actuator.h"
#include "frame_compose.h"
#include "stub_pattern.h"
#include "udp_receiver.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static VisionResult fresh(void){
 VisionResult v;memset(&v,0,sizeof(v));v.version=VISION_VERSION;v.front_camera=1;v.healthy=1;
 v.analysis_frame_id=1;v.lane.version=LANERESULT_VERSION;v.lane.confidence=90;v.lane.direction=LANE_STRAIGHT;
 memset(v.lane_x,0xff,sizeof(v.lane_x));return v;
}
static void policy_test(void){
 VisionResult m=fresh(),s=fresh();drive_policy_t p={0};uint64_t t=1000000;
 assert(drive_policy_step(&p,&m,t,&s,t,t)==CMD_GO);
 m.person_mask=1;assert(drive_policy_step(&p,&m,t,&s,t,t)==CMD_STOP);
 m.person_mask=0;
 for(int i=0;i<10;i++)assert(drive_policy_step(&p,&m,t,&s,t,t)==CMD_STOP);
 for(int i=2;i<=4;i++){m.analysis_frame_id=s.analysis_frame_id=i;
   assert(drive_policy_step(&p,&m,t,&s,t,t)==(i==4?CMD_GO:CMD_STOP));}
 s.person_mask=4;assert(drive_policy_step(&p,&m,t,&s,t,t)==CMD_STOP);
 p=(drive_policy_t){0};s.person_mask=0;m.light.detected=1;m.light.state=TL_RED;m.light.confidence=90;
 assert(drive_policy_step(&p,&m,t,&s,t,t)==CMD_STOP);
 m.light.detected=0;assert(drive_policy_step(&p,&m,t,&s,t,t)==CMD_STOP);
 m.light.detected=1;m.light.state=TL_GREEN;
 for(int i=5;i<=7;i++){m.analysis_frame_id=i;assert(drive_policy_step(&p,&m,t,&s,t,t)==(i==7?CMD_GO:CMD_STOP));}
 m.analysis_frame_id++;m.zebra.detected=1;assert(drive_policy_step(&p,&m,t,&s,t,t)==CMD_COAST);
 assert(drive_policy_step(&p,&m,t+400001,&s,t+400001,t+400001)==CMD_GO);
 m.marks.crossing=m.marks.crossing_left=1;m.marks.left_type=LANE_MARK_SOLID;
 assert(drive_policy_step(&p,&m,t,&s,t,t)==CMD_STOP);
 m.marks.left_type=LANE_MARK_DASHED;assert(drive_policy_step(&p,&m,t+400001,&s,t+400001,t+400001)==CMD_GO);
 assert(drive_policy_step(&p,&m,t,&s,t,t+500001)==CMD_STOP);
 m.age_ms=2501;assert(drive_policy_step(&p,&m,t,&s,t,t)==CMD_STOP);
 m=fresh();s.healthy=0;assert(drive_policy_step(&p,&m,t,&s,t,t)==CMD_STOP);
 m.light.state=(TrafficLightState)99;assert(vision_validate(&m)<0);
 puts("policy: pedestrians M/S, latches, distinct analyses, red/green, zebra, solid, stale PASS");
}
static void control_test(void){
 ControlCommandMsg c={0};c.enable=1;c.command=CMD_GO;command_stamp(&c,1000000);
 assert(command_valid(&c,1000001));assert(!command_valid(&c,1250001));assert(!command_valid(&c,999999));
 c.command=CMD_LEFT;assert(!command_valid(&c,1000001));command_stamp(&c,1000000);c.enable=0;assert(!command_valid(&c,1000001));
 actuator_t a={0};uint8_t r[5];actuator_step(&a,CMD_LEFT,1000000,r);assert(r[0]==0&&r[2]==0&&r[4]==1);
 actuator_step(&a,CMD_STOP,1020000,r);assert(r[0]==1&&r[2]==1&&r[3]==1&&r[4]==0);
 actuator_step(&a,CMD_COAST,1040000,r);assert(r[0]==1&&r[1]==1&&r[4]==1);
 actuator_step(&a,CMD_NONE,1060000,r);assert(r[4]==0);
 puts("control: expired/torn command, STOP preempts steering, COAST releases throttle PASS");
}
static void transport_test(void){
 VisionFrame *tx=calloc(1,sizeof(*tx)),*rx=calloc(1,sizeof(*rx));assert(tx&&rx);
 tx->result=fresh();tx->result.frame_id=55;tx->result.box_count=1;
 tx->result.boxes[0]=(VisionBox){10,20,30,40,90,0,2,0};
 for(size_t i=0;i<sizeof(tx->image);i++)tx->image[i]=(uint8_t)i;
 udp_reassembly_t r;assert(udp_reassembly_init(&r,(uint8_t *)rx,sizeof(*rx))==0);
 udp_frame_hdr_t h,out;uint8_t *frame=NULL;size_t len=0;
 udp_pack_frame_hdr(&h,NULL,640,480,UDP_BLOCK_SIZE);h.flags=UDP_FLAG_VISION;h.frame_id=55;
 h.data_size=sizeof(*tx);h.block_count=(h.data_size+h.block_size-1)/h.block_size;
 assert(udp_reassembly_feed(&r,(uint8_t *)&h,sizeof(h),&out,&frame,&len)==0);
 for(int i=(int)h.block_count-1;i>=0;i--){
  uint8_t pkt[sizeof(udp_data_hdr_t)+UDP_BLOCK_SIZE];udp_data_hdr_t dh={0};
  size_t off=(size_t)i*h.block_size,n=sizeof(*tx)-off;if(n>h.block_size)n=h.block_size;
  dh.magic=UDP_MAGIC_DATA;dh.frame_id=55;dh.block_idx=(uint32_t)i;
  memcpy(pkt+sizeof(dh),(uint8_t *)tx+off,n);dh.checksum=udp_block_checksum(pkt+sizeof(dh),n);memcpy(pkt,&dh,sizeof(dh));
  if(i==(int)h.block_count-1){
   /* Both short and oversized tails must be rejected before memcpy. */
   assert(udp_reassembly_feed(&r,pkt,n+sizeof(dh)-1,&out,&frame,&len)==0);
   assert(udp_reassembly_feed(&r,pkt,n+sizeof(dh)+1,&out,&frame,&len)==0);
   assert(r.received_blocks==0&&r.stat_drop_other==2);
  }
  int rc=udp_reassembly_feed(&r,pkt,n+sizeof(dh),&out,&frame,&len);
  assert(rc==(i==0?1:0));
 }
 assert(len==sizeof(*tx)&&memcmp(tx,rx,len)==0&&vision_validate(&rx->result)==0);
 h.block_count++;assert(udp_reassembly_feed(&r,(uint8_t *)&h,sizeof(h),&out,&frame,&len)<0);
 udp_reassembly_reset(&r);free(tx);free(rx);
 puts("UDP: full 616224-byte image+metadata, reverse order, exact counts PASS");
}
static unsigned pix(const uint8_t *p,int cam,int x,int y){p+=(((cam/2)*240+y)*640+(cam%2)*320+x)*2;return p[0]|(p[1]<<8);}
static void draw_test(void){
 uint8_t *m=calloc(1,IMG_FRAME_BYTES),*s=calloc(1,IMG_FRAME_BYTES);assert(m&&s);VisionResult v=fresh();
 v.box_count=1;v.boxes[0]=(VisionBox){20,20,60,90,90,0,0,0};
 for(int i=0;i<VISION_POINTS;i++){v.lane_x[0][i]=80;v.lane_x[1][i]=240;}
 v.marks.left_type=LANE_MARK_SOLID;v.marks.right_type=LANE_MARK_DASHED;
 v.zebra_count=1;v.zebra_rows[0]=225;vision_annotate(m,&v);vision_annotate(s,&v);
 assert(pix(m,0,20,20)==0xf800);assert(pix(m,1,80,100)==0xf800);
 assert(pix(m,1,240,90)==0xf800);assert(pix(m,1,240,102)==0);assert(pix(m,1,150,225)==0x07e0);
 stub_pattern_fill(m,640,480,0);stub_pattern_fill(s,640,480,1);vision_annotate(m,&v);
 VisionResult rear=fresh();rear.box_count=1;rear.boxes[0]=(VisionBox){100,60,140,170,90,0,2,0};vision_annotate(s,&rear);
 uint8_t *canvas=malloc(DISP_WIN_W*DISP_WIN_H*4);assert(canvas);
 assert(frame_compose_6ch(s,m,DISPLAY_MODE_SPLIT,canvas,DISP_WIN_W*DISP_WIN_H*4,DISP_WIN_W,DISP_WIN_H)==0);
 FILE *f=fopen("tmp/adas-overlay-preview.ppm","wb");if(f){fprintf(f,"P6\n960 480\n255\n");for(int i=0;i<960*480;i++){fputc(canvas[i*4+2],f);fputc(canvas[i*4+1],f);fputc(canvas[i*4],f);}fclose(f);}
 assert(frame_compose_6ch(s,m,DISPLAY_MODE_M1,canvas,DISP_WIN_W*DISP_WIN_H*4,DISP_WIN_W,DISP_WIN_H)==0);
 free(canvas);free(m);free(s);puts("display: camera crops, person/solid/dashed/zebra colors, six views and HMI zoom PASS");
}
int main(void){policy_test();control_test();transport_test();draw_test();puts("CLOSED LOOP TESTS PASS");return 0;}
