/* Linux CLI fault injection. Never opens spidev or calls an ioctl. */
#include "fspi_module.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int left_n = 1, right_n = 1;
int fspi_init(void) { return 0; }
int fspi_write_data(uint16_t address, uint8_t value)
{
    if (value > 1 || (address != 2 && address != 3)) abort();
    if (address == 2) left_n = value; else right_n = value;
    return 0;
}
int fspi_read_control_snapshot(ControlSnapshot *s)
{
    const char *mode = getenv("STEER_TRACE_TEST");
    if (mode && !strcmp(mode, "read-fault")) { errno = EPROTO; return -1; }
    memset(s, 0, sizeof(*s));
    s->contacts = CONTACT_GO_N | CONTACT_BACK_N | CONTACT_STOP_N |
                  (left_n ? CONTACT_LEFT_N : 0) | (right_n ? CONTACT_RIGHT_N : 0);
    s->gates = GATE_FS_ARMED | GATE_RESET_RELEASED;
    if (mode && !strcmp(mode, "blocked")) {
        s->contacts |= CONTACT_LEFT_N | CONTACT_RIGHT_N;
        s->gates = GATE_STOP | GATE_RESET_RELEASED;
    }
    if (mode && !strcmp(mode, "contact-mismatch")) s->contacts |= CONTACT_LEFT_N | CONTACT_RIGHT_N;
    return 0;
}
void fspi_cleanup(void)
{
    if (!left_n || !right_n) abort();
    puts("STUB_RELEASED");
}
