/* Run the production monitor with in-process IPC and SPI doubles. No device
 * or real SysV segment is touched, so this also runs on a live S board. */
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
static unsigned writes,last_permission,hmi_reply=0x0d;
static int write_failed;
int test_shmget(key_t key,size_t size,int flags) {
    (void)size;(void)flags;
    return key==LINE_STYLE_KEY?1:key==LINE_WARNING_KEY?2:
           key==TRAFFIC_LIGHT_KEY?3:key==TRAFFIC_GO_KEY?4:-1;
}
void* test_shmat(int id,const void* address,int flags) {
    (void)address;(void)flags;
    return id==1?(void*)&line_segment:id==2?(void*)&warning_segment:
           id==3?(void*)&signal_segment:id==4?(void*)&go_segment:(void*)-1;
}
int test_shmdt(const void* p){(void)p;return 0;}
void test_perror(const char* p){(void)p;}
int test_spi_ioctl(int fd,unsigned long request,...) {
    (void)fd;
    if(request==SPI_IOC_WR_MODE32||request==SPI_IOC_WR_BITS_PER_WORD||request==SPI_IOC_WR_MAX_SPEED_HZ)return 0;
    assert(request==SPI_IOC_MESSAGE(1));
    va_list a;va_start(a,request);struct spi_ioc_transfer* t=va_arg(a,void*);va_end(a);
    if(t->rx_buf){assert(t->len==1);*(uint8_t*)(uintptr_t)t->rx_buf=(uint8_t)hmi_reply;return 1;}
    const uint8_t* b=(const uint8_t*)(uintptr_t)t->tx_buf;
    assert(t->len==3&&b[0]==0&&b[1]==5); // no write to motion/STOP
    ++writes;last_permission=b[2];return write_failed?-1:3;
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
    puts("production monitor: atomic lane+traffic permissions, red/unknown/yellow/green, SPI retry, UI telemetry, restart PASS");
    return 0;
}
