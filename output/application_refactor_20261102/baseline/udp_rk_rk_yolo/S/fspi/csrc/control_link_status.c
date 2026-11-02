#define _DEFAULT_SOURCE
#include "fspi_module.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int main(int argc,char**argv){
    unsigned count=1;
    if(argc==3&&!strcmp(argv[1],"--watch"))count=(unsigned)strtoul(argv[2],0,10);
    else if(argc!=1){fprintf(stderr,"Usage: %s [--watch samples]\n",argv[0]);return 2;}
    if(!count||count>3600)return 2;
    if(fspi_init())return 1;
    int rc=0;
    for(unsigned i=0;i<count;i++){
        ControlSnapshot s={0};
        if(fspi_read_control_snapshot(&s)){
            perror("FPGA logical status read (requires speed-slider bitstream)");rc=1;break;
        }
        printf("LOGICAL sample=%u HMI=%02X GO_N=%u BACK_N=%u LEFT_N=%u RIGHT_N=%u STOP_N=%u "
            "SPEED_DRIVE=%u SPEED_PAD_N=%u SPEED_HELD=%u AUTO=%u STOP_GATE=%u LEFT_DENY=%u "
            "RIGHT_DENY=%u RED_GO_GATE=%u FS_ARMED=%u gear=%u ready=%u\n",i,s.hmi,
            !!(s.contacts&1),!!(s.contacts&2),!!(s.contacts&4),!!(s.contacts&8),!!(s.contacts&16),
            !!(s.contacts&32),!!(s.contacts&64),!!(s.contacts&128),
            !!(s.gates&1),!!(s.gates&2),!!(s.gates&4),!!(s.gates&8),!!(s.gates&16),!!(s.gates&32),
            s.speed.gear,s.speed.known);
        fflush(stdout);if(i+1<count)usleep(100000);
    }
    puts("Read-only logical status. Pin voltage, remote power and RF reception still require physical verification.");
    fspi_cleanup();return rc;
}
