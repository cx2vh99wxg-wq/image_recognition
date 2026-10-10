/* Full six-camera path. Capture/NPU have separate workers; this thread owns
 * network, display and 20 ms decision publication. Preview never enables motion. */
#include "vision_worker.h"
#include "vision_frame.h"
#include "drive_policy.h"
#include "command_guard.h"
#include "camera_status.h"
#include "udp_sender.h"
#include "udp_receiver.h"
#include "render_lcd.h"
#include "shm_ipc.h"
#include "time_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#if defined(__linux__)
#include <unistd.h>
#include <sys/file.h>
#include <fcntl.h>
static volatile sig_atomic_t running=1;
static void signal_stop(int sig){(void)sig;running=0;}
static void publish(ControlCommand action,uint32_t sequence)
{
    ControlCommandMsg cmd;memset(&cmd,0,sizeof(cmd));
    cmd.frame_id=sequence;cmd.command=action;cmd.enable=1;
    cmd.priority=action==CMD_STOP?CMD_PRI_EMERGENCY:CMD_PRI_NORMAL;
    command_stamp(&cmd,now_us_mono());
    if(shm_write_cmd(&cmd)<0)fprintf(stderr,"[ADAS] command publication failed\n");
}
int main(int argc,char **argv)
{
    int master=0,role_set=0,drive=0,lcd_on=1,interval=1,fps=10;
    const char *person="model/yolov5s-640-640.rknn",*road="model/yolopv2_Nx3x480x640_rk3568.rknn",*ip=UDP_IP_S;
    for(int i=1;i<argc;i++) {
        if(!strcmp(argv[i],"--build-info")){puts("ADAS-CLOSED-LOOP-v1 M-front=cmos5 layout=960x480");return 0;}
        if(!strcmp(argv[i],"--role")&&i+1<argc){const char *r=argv[++i];if(strcmp(r,"m")&&strcmp(r,"s"))return 2;master=!strcmp(r,"m");role_set=1;}
        else if(!strcmp(argv[i],"--drive"))drive=1;
        else if(!strcmp(argv[i],"--no-lcd"))lcd_on=0;
        else if(!strcmp(argv[i],"--person-model")&&i+1<argc)person=argv[++i];
        else if(!strcmp(argv[i],"--lane-model")&&i+1<argc)road=argv[++i];
        else if(!strcmp(argv[i],"--ip")&&i+1<argc)ip=argv[++i];
        else if(!strcmp(argv[i],"--interval")&&i+1<argc)interval=atoi(argv[++i]);
        else if(!strcmp(argv[i],"--fps")&&i+1<argc)fps=atoi(argv[++i]);
        else {fprintf(stderr,"Usage: %s --role m|s [--drive] [--no-lcd] [--interval 1|3|5] [--fps 1..30] [--ip S_IP] [--person-model path] [--lane-model path]\n",argv[0]);return 2;}
    }
    if(!role_set||(master&&drive))return 2;
    /* Shared lock with control startup scripts prevents duplicate DMA users. */
    int lockfd=open("/tmp/adas-pcie.lock",O_CREAT|O_RDWR,0660);
    if(lockfd<0||flock(lockfd,LOCK_EX|LOCK_NB)<0){fprintf(stderr,"[ADAS] PCIe already owned\n");return 1;}
    signal(SIGINT,signal_stop);signal(SIGTERM,signal_stop);
    if(!master) {
        void *p=NULL;
        if(shm_create("cmd",SHM_KEY_CMD,SHM_CMD_SIZE,&p)<0||shm_create("display",SHM_KEY_DISPLAY,SHM_DISPLAY_SIZE,&p)<0)return 1;
        publish(CMD_STOP,0);DisplayMode mode=DISPLAY_MODE_SPLIT;shm_write_display(&mode);
    }
    vision_worker_t *worker=NULL;
    vision_worker_cfg_t cfg={person,road,master,interval,fps};
    if(vision_worker_start(&worker,&cfg)<0){fprintf(stderr,"[ADAS] camera/model startup failed; driving remains stopped\n");return 1;}
    VisionFrame *local=calloc(1,sizeof(*local)),*remote=calloc(1,sizeof(*remote)),*assembly=calloc(1,sizeof(*assembly));
    uint8_t *draw_local=malloc(IMG_FRAME_BYTES),*draw_remote=malloc(IMG_FRAME_BYTES);
    udp_sender_t *sender=NULL;udp_receiver_t *receiver=NULL;render_lcd_ctx_t *lcd=NULL;
    udp_reassembly_t re={0};int status=1;
    if(!local||!remote||!assembly||!draw_local||!draw_remote)goto done;
    if(master){if(udp_sender_init(&sender,ip,UDP_PORT)<0)goto done;}
    else {
        if(udp_receiver_init(&receiver,UDP_PORT)<0||udp_reassembly_init(&re,(uint8_t *)assembly,sizeof(*assembly))<0)goto done;
        if(lcd_on&&render_lcd_init(&lcd,DISP_WIN_W,DISP_WIN_H)<0)goto done;
    }
    drive_policy_t policy={0};uint64_t sampled=0,received=0,captured=0,assembly_started=0,lastdraw=0,lastcmd=0,lasthb=0,lastlog=0;
    uint32_t sent=0,seq=0,hbseq=0;int manual_stop=0;
    ControlCommand command=CMD_STOP,decision=CMD_STOP,last_decision=CMD_NONE;
    fprintf(stderr,"[ADAS] %s front=M/cmos5 interval=%d drive=%d (models on all three local cameras)\n",master?"M":"S",interval,drive);
    while(running) {
        uint64_t now=now_us_mono();
        int snap=vision_worker_snapshot(worker,local,&captured);
        now=now_us_mono();sampled=now;
        if(snap<0||now<captured||now-captured>500000||camera_status_read(now)!=7) {
            local->result.healthy=0;
        }
        if(master) {
            if(captured&&local->result.frame_id!=sent&&now>=captured&&now-captured<=500000) {
                if(udp_sender_send_vision(sender,local)>0)sent=local->result.frame_id;
            }
            if(now-lasthb>=200000){udp_sender_send_heartbeat(sender,++hbseq);lasthb=now;}
        } else {
            uint8_t pkt[sizeof(udp_data_hdr_t)+UDP_BLOCK_SIZE+64];
            uint64_t drain_start=now_us_mono();
            for(int k=0;k<UDP_RECV_DRAIN_MAX;k++) {
                if(k%32==0 && now_us_mono()-drain_start>10000)break;
                int n=udp_receiver_recv_nb(receiver,pkt,sizeof(pkt));if(n<=0)break;
                uint8_t *frame;size_t len;udp_frame_hdr_t hdr;
                uint64_t headers_before=re.stat_hdr_rx;
                int complete=udp_reassembly_feed(&re,pkt,(size_t)n,&hdr,&frame,&len);
                if(re.stat_hdr_rx!=headers_before)assembly_started=now_us_mono();
                if(complete==1 &&
                   (hdr.flags&UDP_FLAG_VISION)&&hdr.width==640&&hdr.height==480&&len==sizeof(*remote)&&
                   vision_validate(&assembly->result)==0&&hdr.frame_id==assembly->result.frame_id&&
                   (!received||(int32_t)(hdr.frame_id-remote->result.frame_id)>0||now-received>500000)) {
                    memcpy(remote,frame,len);received=now_us_mono();
                    uint64_t age=(uint64_t)remote->result.age_ms+(assembly_started?(received-assembly_started)/1000:0);
                    remote->result.age_ms=age>UINT32_MAX?UINT32_MAX:(uint32_t)age;
                }
            }
            now=now_us_mono();
            if(now-lastcmd>=20000) {
                decision=drive_policy_step(&policy,&remote->result,received,&local->result,sampled,now);
                command=decision;
                if(!drive||manual_stop)command=CMD_STOP;
                if(decision!=last_decision) {
                    fprintf(stderr,"[POLICY] decision=%d command=%d mode=%s\n",decision,command,drive?"DRIVE":"PREVIEW");
                    last_decision=decision;
                }
                publish(command,++seq);lastcmd=now;
            }
            if(lcd&&now-lastdraw>=100000) {
                memcpy(draw_local,local->image,IMG_FRAME_BYTES);memcpy(draw_remote,remote->image,IMG_FRAME_BYTES);
                if(vision_fresh(&local->result,sampled,now))vision_annotate(draw_local,&local->result);
                if(vision_fresh(&remote->result,received,now))vision_annotate(draw_remote,&remote->result);
                DisplayMode mode=DISPLAY_MODE_SPLIT,requested;
                if(shm_read_display(&requested)==0&&requested>=DISPLAY_MODE_SPLIT&&requested<=DISPLAY_MODE_S2)mode=requested;
                render_lcd_overlay(lcd,command,vision_fresh(&remote->result,received,now)?remote->result.light.state:TL_UNKNOWN,command==CMD_STOP,seq);
                render_lcd_draw(lcd,captured&&now>=captured&&now-captured<500000?draw_local:NULL,
                                received&&now-received<500000?draw_remote:NULL,mode,seq);
                int key=render_lcd_poll_key(lcd);
                if(key==RENDER_KEY_STOP)manual_stop=1;
                if(key==RENDER_KEY_AUTO)manual_stop=0;
                lastdraw=now;
            }
        }
        if(now-lastlog>=2000000) {
            fprintf(stderr,"[ADAS] frame local=%u remote=%u age=%u/%u ms healthy=%u/%u people=%x/%x TL=%d decision=%d command=%d %s\n",
                    local->result.frame_id,remote->result.frame_id,local->result.age_ms,remote->result.age_ms,
                    local->result.healthy,remote->result.healthy,local->result.person_mask,remote->result.person_mask,
                    remote->result.light.state,decision,command,drive?"DRIVE":"PREVIEW");lastlog=now;
        }
        usleep(2000);
    }
    status=0;
done:
    if(!master)publish(CMD_STOP,UINT32_MAX);
    vision_worker_stop(worker);render_lcd_deinit(lcd);udp_sender_close(sender);udp_receiver_close(receiver);
    udp_reassembly_reset(&re);free(local);free(remote);free(assembly);free(draw_local);free(draw_remote);close(lockfd);return status;
}
#else
int main(void){fprintf(stderr,"ADAS runtime requires RK3568 Linux; run portable tests on this host.\n");return 1;}
#endif
