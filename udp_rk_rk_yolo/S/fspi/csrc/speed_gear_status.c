#define _POSIX_C_SOURCE 200809L
#include "fspi_module.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static void settle(void) {
    struct timespec t={0,20000000L};
    nanosleep(&t,NULL);
}

/* Calibration changes only the FPGA estimate. It never presses the key.
 * The operator must first know that the physical remote is in gear 1. */
int main(int argc,char **argv) {
    int sync=argc==2&&!strcmp(argv[1],"--sync-gear1");
    if(argc>1&&!sync) {
        fprintf(stderr,"Usage: %s [--sync-gear1]\nOnly synchronize after the remote has actually returned to gear 1.\n",argv[0]);
        return 2;
    }
    if(fspi_init())return 1;
    SpeedSnapshot s={0};uint8_t raw=0;
    if(fspi_read_speed_snapshot(&s,&raw)) {
        perror("Read FPGA gear status (requires speed-warning bitstream)");
        fspi_cleanup();return 1;
    }
    if(sync) {
        if(!s.known||s.pressed||s.driving) {
            fprintf(stderr,"Wait until the speed key is released and FPGA is ready.\n");
            fspi_cleanup();return 1;
        }
        int failed=fspi_write_data(6,0);
        settle();if(!failed)failed=fspi_write_data(6,0xa5);
        settle();int released=fspi_write_data(6,0);
        settle();
        if(failed||released||fspi_read_speed_snapshot(&s,&raw)||s.gear!=1) {
            fprintf(stderr,"Gear estimate synchronization failed.\n");
            fspi_cleanup();return 1;
        }
    }
    printf("estimated_gear=%u known=%u key_pressed=%u hmi_pulse=%u raw=%02X\n",
           s.gear,s.known,s.pressed,s.driving,raw);
    puts("This is a button-count estimate, not a measured vehicle speed.");
    fspi_cleanup();return 0;
}
