#ifndef VEHICLE_CONTROL_STATUS_H
#define VEHICLE_CONTROL_STATUS_H
#include "speed_status.h"
/* Logical FPGA contact levels and gate states, NOT a voltmeter readback.
 * Old one/four-byte RX-only replies are unchanged. Eight bytes add D7,
 * contacts, gates, CRC8. No control register is written by this API. */
typedef struct {SpeedSnapshot speed;uint8_t hmi,contacts,gates;} ControlSnapshot;
enum {CONTACT_GO_N=1,CONTACT_BACK_N=2,CONTACT_LEFT_N=4,CONTACT_RIGHT_N=8,
      CONTACT_STOP_N=16,CONTACT_SPEED_DRIVE=32,CONTACT_SPEED_PAD_N=64,CONTACT_SPEED_HELD=128};
enum {GATE_AUTO=1,GATE_STOP=2,GATE_LEFT=4,GATE_RIGHT=8,GATE_RED=16,GATE_FS_ARMED=32,
      GATE_SPEED_READY=64,GATE_RESET_RELEASED=128};
static inline uint8_t control_crc_byte(uint8_t crc,uint8_t value){
    crc^=value;for(unsigned i=0;i<8;i++)crc=(uint8_t)((crc&0x80)?(crc<<1)^7:crc<<1);return crc;
}
static inline int control_decode_packet(const uint8_t b[8],ControlSnapshot* out){
    ControlSnapshot s={0};
    if(speed_decode_packet(b,&s.speed)||b[4]!=0xd7||
       b[7]!=control_crc_byte(control_crc_byte(control_crc_byte(0,b[4]),b[5]),b[6]))return -1;
    s.hmi=b[0];s.contacts=b[5];s.gates=b[6];*out=s;return 0;
}
#endif
