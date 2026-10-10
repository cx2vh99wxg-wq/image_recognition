#include "fspi_driver.h"
#include "fpga_regmap.h"
#include "actuator.h"
#include "camera_status.h"
#include "command_guard.h"
#include "shm_ipc.h"
#include "time_util.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>
#if defined(__linux__)
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>
static volatile sig_atomic_t running=1;
static void finish(int sig){(void)sig;running=0;}
static void stop_all(fspi_driver_t *d)
{
    fspi_driver_emergency_stop(d);
    for(uint8_t r=0;r<4;r++)fspi_driver_write_reg(d,r,1);
}
int main(int argc,char **argv)
{
    int drive=argc==2&&!strcmp(argv[1],"--drive");
    if(argc>1&&!drive){fprintf(stderr,"Usage: %s [--drive] (default: HMI observer + held STOP)\n",argv[0]);return 2;}
    int lockfd=open("/tmp/adas-fspi.lock",O_CREAT|O_RDWR,0660);
    if(lockfd<0||flock(lockfd,LOCK_EX|LOCK_NB)<0)return 1;
    signal(SIGINT,finish);signal(SIGTERM,finish);
    fspi_driver_t *d=NULL;
    if(fspi_driver_init(&d)<0){perror("FSPI init");return 1;}
    uint8_t version=0;
    if(fspi_driver_read_reg(d,FSPI_REG_VERSION,&version)<0||version!=FSPI_VERSION){
        fprintf(stderr,"FSPI version 0x%02x != A6; load drive_6ch bitstream. Refusing motion.\n",version);
        stop_all(d);fspi_driver_deinit(d);return 1;
    }
    stop_all(d);
    void *p=NULL;
    if(shm_create("display",SHM_KEY_DISPLAY,SHM_DISPLAY_SIZE,&p)<0){fspi_driver_deinit(d);return 1;}
    if(shm_create("cmd",SHM_KEY_CMD,SHM_CMD_SIZE,&p)<0){fspi_driver_deinit(d);return 1;}
    ControlCommandMsg initial={0};initial.command=CMD_STOP;initial.enable=1;
    command_stamp(&initial,now_us_mono());
    if(shm_write_cmd(&initial)<0){fspi_driver_deinit(d);return 1;}
    actuator_t a={0};uint64_t last_view=0;uint8_t camera=0;int fault=0;
    while(running&&!fault){
        uint64_t now=now_us_mono();ControlCommandMsg c;uint8_t regs[5];
        ControlCommand action=CMD_STOP;
        if(drive&&shm_read_cmd(&c)==0&&command_valid(&c,now))action=c.command;
        if(now-last_view>=100000){
            uint8_t view;
            if(fspi_driver_read_reg(d,FSPI_REG_VIEW,&view)<0||view>6||fspi_driver_read_reg(d,FSPI_REG_CAMERA,&camera)<0){fault=1;break;}
            DisplayMode mode=view==0?DISPLAY_MODE_SPLIT:(DisplayMode)(DISPLAY_MODE_M0+view-1);
            if(shm_write_display(&mode)<0){fault=1;break;}
            if(camera_status_write(camera,now)<0){fault=1;break;}
            last_view=now;
        }
        if(camera!=7)action=CMD_STOP;
        actuator_step(&a,action,now,regs);
        /* Assert STOP before touching steering; release STOP last. */
        if(regs[4]==0&&fspi_driver_emergency_stop(d)<0){fault=1;break;}
        for(uint8_t r=0;r<4;r++)if(fspi_driver_write_reg(d,r,regs[r])<0){fault=1;break;}
        if(fault)break;
        if(fspi_driver_write_reg(d,FSPI_REG_HEARTBEAT,FSPI_HEARTBEAT)<0||
           fspi_driver_write_reg(d,FSPI_REG_STOP,regs[4])<0){fault=1;break;}
        usleep(20000);
    }
    camera_status_write(0,now_us_mono());stop_all(d);fspi_driver_deinit(d);close(lockfd);
    if(fault)fprintf(stderr,"FSPI fault: held STOP; FPGA watchdog is the independent fallback.\n");
    return fault?1:0;
}
#else
int main(void){return 1;}
#endif
