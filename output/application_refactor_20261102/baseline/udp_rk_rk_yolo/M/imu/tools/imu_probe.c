#include "mpu9250_transport.h"
#include "steering_platform.h"
#include <stdio.h>
int main(void) {
    mpu9250_bus_t bus;
    steer_result_t rc=mpu9250_fpga_open(&bus);
    if(rc!=STEER_OK){fprintf(stderr,"M FPGA IMU1 bridge: %s (old firmware is rejected before any write)\n",steer_result_name(rc));return 1;}
    int identified=0;
    for(unsigned k=0;k<12;++k){
        fpga_imu_snapshot_t s;uint64_t midpoint;
        rc=mpu9250_fpga_snapshot(&bus,&s,&midpoint);
        if(rc==STEER_OK){
            printf("IMU1 flags=%u who=0x%02X address=0x%02X i2c_error=%u errors=%u seq=%u fpga_us=%llu sample_us=%llu generation=%u\n",
                s.flags,s.identity,s.address,s.error,s.errors,s.sequence,
                (unsigned long long)s.now_us,(unsigned long long)s.sample_us,s.generation);
            if(s.flags==7&&s.identity==0x71&&!s.error)identified=1;
        }else fprintf(stderr,"Snapshot: %s\n",steer_result_name(rc));
        steering_sleep_ms(100);
    }
    mpu9250_fpga_close(&bus);
    puts(identified?"REAL SENSOR DATA PRESENT; physical axis/center validation is still required":
         "NO VALID SENSOR: check wiring/power/pull-ups/CS/AD0; no angle was fabricated");
    return identified?0:3;
}
