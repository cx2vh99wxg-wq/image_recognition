/* 使用进程内 IPC 和 SPI 替身运行正式监视器，
 * 不访问设备或真实 SysV 段，因此可在运行中的 S 板作软件检查。 */
#define shmget test_shmget
#define shmat test_shmat
#define shmdt test_shmdt
#define ioctl test_spi_ioctl
#define perror test_perror
#include "csrc/fspi_module.c"
#define main vehicle_program_main
#include "csrc/main.c"
#undef main
#undef ioctl
#undef perror
#undef shmget
#undef shmat
#undef shmdt
#include <assert.h>
#include <stdarg.h>
static LineStyleSegment line_segment,warning_segment;
static TrafficLightSegment signal_segment;
static TrafficGoSegment go_segment;
static SpeedWarningSegment speed_segment;
static FrontPersonSegment front_segment;
static FrontGuardSegment front_control_segment;
static int control_reply_enabled;
static uint8_t contact_reply=31;
static int last_stop=-1;
static unsigned speed_reply=0xc1;
static int speed_failed;
static unsigned writes,last_permission,hmi_reply=0x0d;
static int write_failed;
int test_shmget(key_t key,size_t size,int flags) {
    (void)size;(void)flags;
    return key==LINE_STYLE_KEY?1:key==LINE_WARNING_KEY?2:
           key==TRAFFIC_LIGHT_KEY?3:key==TRAFFIC_GO_KEY?4:key==SPEED_WARNING_KEY?5:key==FRONT_PERSON_KEY?6:key==FRONT_GUARD_KEY?7:-1;
}
void* test_shmat(int id,const void* address,int flags) {
    (void)address;(void)flags;
    return id==1?(void*)&line_segment:id==2?(void*)&warning_segment:
           id==3?(void*)&signal_segment:id==4?(void*)&go_segment:id==5?(void*)&speed_segment:id==6?(void*)&front_segment:id==7?(void*)&front_control_segment:(void*)-1;
}
int test_shmdt(const void* p){(void)p;return 0;}
void test_perror(const char* p){(void)p;}
int test_spi_ioctl(int fd,unsigned long request,...) {
    (void)fd;
    if(request==SPI_IOC_WR_MODE32||request==SPI_IOC_WR_BITS_PER_WORD||request==SPI_IOC_WR_MAX_SPEED_HZ)return 0;
    assert(request==SPI_IOC_MESSAGE(1));
    va_list a;va_start(a,request);struct spi_ioc_transfer* t=va_arg(a,void*);va_end(a);
    if(t->rx_buf){
        if(t->len==8){
            uint8_t* b=(void*)(uintptr_t)t->rx_buf;memset(b,0,8);
            if(control_reply_enabled){b[0]=0xcc;b[1]=0xa7;b[2]=0xc1;b[3]=speed_crc8(b[2]);
                b[4]=0xd7;b[5]=contact_reply;b[6]=GATE_AUTO;
                b[7]=control_crc_byte(control_crc_byte(control_crc_byte(0,b[4]),b[5]),b[6]);}
            return 8;
        }
        if(t->len==4){
            uint8_t* b=(void*)(uintptr_t)t->rx_buf;
            b[0]=0xdd;b[1]=0xa7;b[2]=(uint8_t)speed_reply;b[3]=speed_crc8(b[2]);
            if(speed_failed){errno=ETIMEDOUT;return -1;}
            return 4;
        }
        assert(t->len==1);*(uint8_t*)(uintptr_t)t->rx_buf=(uint8_t)hmi_reply;return 1;
    }
    const uint8_t* b=(const uint8_t*)(uintptr_t)t->tx_buf;
    assert(t->len==3 && b[0]==0 && b[1]<=5);
    if(b[1]==4){last_stop=b[2];return 3;}
    if(b[1]<4){assert(b[2]==1);return 3;} // never assert a motion contact
    ++writes;last_permission=b[2];return write_failed?-1:3;
}
int emergency_brake_shm_init(EmergencyBrakeSharedMemory **p,int writer){(void)p;(void)writer;return -1;}
void emergency_brake_shm_cleanup(EmergencyBrakeSharedMemory *p,int writer){(void)p;(void)writer;}
int emergency_brake_read_status(EmergencyBrakeSharedMemory *p,int *person,int *enable){*person=p->person_detected;*enable=p->brake_enable;return 0;}
static void publish_front(int found) {
    FrontPersonData d={0};d.session=20;d.source_id=++writes;d.flags=3|(found?4:0);d.count=found?1:0;
    front_person_set_deadline(&d,front_person_clock_ns()+FRONT_PERSON_TTL_NS);
    FrontPersonIPC p={6,&front_segment};front_person_ipc_write(&p,&d);
}
static void publish(unsigned color,int fresh,int confirmed) {
    TrafficLightData d={0};d.session=1;d.source_id=++writes;
    d.state=d.raw_state=color;d.flags=TRAFFIC_BOX|TRAFFIC_MODEL_READY|(confirmed?TRAFFIC_CONFIRMED:0);
    traffic_set_deadline(&d,traffic_clock_ns()+(fresh?500000000ULL:0));
    TrafficLightIPC p={3,&signal_segment};traffic_ipc_write(&p,&d);
}
static TrafficGoData poll(void) {
    lane_last_poll=0;monitor_lane_rule();
    TrafficGoData d={0};assert(!traffic_go_read(&traffic_warning,&d));return d;
}
int main(void) {
    g_spi_fd=7;
    g_config.transfer_size=SPI_TRANSFER_SIZE;g_config.speed_hz=SPI_SPEED;
    g_config.protocol_mode=SPI_MODE_QUAD;g_config.bits_per_word=8;
    LineStyleIPC lp={1,&line_segment};LineStyleData lane={0};
    lane.left=LINE_DASHED;lane.right=LINE_SOLID;lane.right_quality=90;
    line_set_deadline(&lane,line_clock_ns()+LINE_STYLE_TTL_NS);line_ipc_write(&lp,&lane);
    publish(TRAFFIC_RED,1,0);TrafficGoData s=poll();assert(!s.blocked&&(last_permission&15)==2);
    publish(TRAFFIC_RED,1,1);s=poll();assert(s.blocked&&s.write_ok&&(last_permission&15)==6);
    LineStyleData warning={0};assert(!line_ipc_read(&lane_warning,&warning)&&warning.blocked==2);
    publish(TRAFFIC_UNKNOWN,0,0);s=poll();assert(s.blocked&&(last_permission&15)==6);
    publish(TRAFFIC_YELLOW,1,1);s=poll();assert(s.blocked&&(last_permission&15)==6);
    publish(TRAFFIC_GREEN,1,0);s=poll();assert(s.blocked&&(last_permission&15)==6);
    publish(TRAFFIC_GREEN,1,1);write_failed=1;s=poll();assert(s.blocked&&!s.write_ok&&(last_permission&15)==10);
    unsigned retry_heartbeat=last_permission&0x80;
    write_failed=0;s=poll();assert(!s.blocked&&s.write_ok&&(last_permission&15)==10);
    assert((last_permission&0x80)==retry_heartbeat);
    publish(TRAFFIC_RED,1,1);s=poll();assert(s.blocked);
    traffic_policy.blocked=0;traffic_policy_initialized=0;
    publish(TRAFFIC_UNKNOWN,0,0);s=poll();assert(s.blocked&&(last_permission&4));
    assert((last_permission&0x40)&&warning.blocked==2&&writes>8);
    // 档位告警只发布状态，不写运动或 STOP；
    // 上方 ioctl 替身会拒绝既有 reg5 以外的全部写入。
    for(unsigned gear=1;gear<=3;++gear){
        speed_reply=0xc0|gear;poll();SpeedWarningData status={0};
        assert(!speed_ipc_read(&speed_warning,&status));
        assert(status.gear==gear&&status.known&&speed_fresh(&status,speed_clock_ns()));
    }
    SpeedWarningData before={0},after={0};speed_ipc_read(&speed_warning,&before);
    speed_failed=1;poll();assert(!speed_ipc_read(&speed_warning,&after));
    assert(after.gear==3&&after.known&&!after.link_ok&&after.read_errno==ETIMEDOUT);
    assert(speed_deadline(&after)==speed_deadline(&before));
    speed_failed=0;speed_reply=0xc1;poll();speed_ipc_read(&speed_warning,&after);
    assert(after.gear==1&&after.link_ok);
    control_reply_enabled=1; contact_reply=31 & ~CONTACT_GO_N & ~CONTACT_LEFT_N;
    publish_front(1);publish(TRAFFIC_GREEN,1,1);poll();
    assert(!traffic_policy.blocked && front_guard.raw_request && front_guard.held_go && (last_permission&4));
    // Actual production STOP monitor, still with process-local IPC/SPI substitutes.
    EmergencyBrakeSharedMemory rear={0};rear.frame_count=rear.valid=1;rear.brake_enable=1;
    rear.last_update=(uint32_t)(line_clock_ns()/1000000000ULL);emergency_brake_shm=&rear;
    last_brake_value=1;monitor_emergency_brake();assert(last_stop==0);
    // Once GO is inhibited its contact is high, but the front stop request stays latched.
    contact_reply=31;poll();assert(front_guard.raw_request);
    publish_front(0);front_guard.release.clear_pending=1;
    front_guard.release.clear_since_ms=line_clock_ns()/1000000ULL-2100;
    publish(TRAFFIC_RED,1,1);poll();assert(!front_guard.raw_request && (last_permission&4));
    monitor_emergency_brake();assert(last_stop==1 && traffic_policy.blocked);
    publish(TRAFFIC_GREEN,1,1);poll();assert(!traffic_policy.blocked && (last_permission&8));
    // Reverse alone does not create a front STOP, while GO remains denied under AUTO.
    publish_front(1);contact_reply=31 & ~CONTACT_BACK_N;poll();
    assert(!front_guard.raw_request && (last_permission&4));
    // Either steering contact can coexist with forward; right also triggers STOP.
    contact_reply=31 & ~CONTACT_GO_N & ~CONTACT_RIGHT_N;poll();assert(front_guard.raw_request);
    puts("production monitor: atomic lane+traffic permissions, red/unknown/yellow/green, SPI retry, UI telemetry, restart PASS");
    return 0;
}
