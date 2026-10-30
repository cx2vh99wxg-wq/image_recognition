/* Bench diagnostic: native RX-only snapshot, never writes a control register. */
#include "fspi_module.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>

int main(void)
{
    int lock = open("/tmp/steering-actuator.lock", O_CREAT | O_RDWR | O_NOFOLLOW, 0600);
    if (lock < 0 || flock(lock, LOCK_EX | LOCK_NB)) {
        perror("Bench tool lock"); if (lock >= 0) close(lock); return 1;
    }
    if (fspi_init()) { close(lock); return 1; }
    ControlSnapshot s = {0};
    int result = fspi_read_control_snapshot(&s);
    if (result) {
        fprintf(stderr, "Control snapshot unavailable: %s. No motion was requested.\n", strerror(errno));
    } else {
        printf("LOGICAL_SNAPSHOT hmi=0x%02X contacts=0x%02X gates=0x%02X "
               "go_n=%u back_n=%u left_n=%u right_n=%u stop_n=%u "
               "auto=%u stop=%u fs_armed=%u reset_released=%u "
               "left_denied=%u right_denied=%u red=%u gear=%u gear_known=%u\n",
               s.hmi, s.contacts, s.gates,
               !!(s.contacts & CONTACT_GO_N), !!(s.contacts & CONTACT_BACK_N),
               !!(s.contacts & CONTACT_LEFT_N), !!(s.contacts & CONTACT_RIGHT_N),
               !!(s.contacts & CONTACT_STOP_N), !!(s.gates & GATE_AUTO),
               !!(s.gates & GATE_STOP), !!(s.gates & GATE_FS_ARMED),
               !!(s.gates & GATE_RESET_RELEASED), !!(s.gates & GATE_LEFT),
               !!(s.gates & GATE_RIGHT), !!(s.gates & GATE_RED),
               s.speed.gear, s.speed.known);
        puts("Logical FPGA state only; no voltage or mechanical angle measurement.");
    }
    fspi_cleanup(); close(lock);
    return result ? 1 : 0;
}
