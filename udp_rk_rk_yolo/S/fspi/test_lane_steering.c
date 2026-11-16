/* Exercise the production HMI gate with a simulated FPGA and controller. */
#define clock_gettime test_clock_gettime
#define main vehicle_control_program_main
#include "csrc/main.c"
#undef main
#undef clock_gettime
#include <assert.h>

static uint64_t clock_ns = 1000000000ULL;
static uint8_t fpga_status, software_request, last_heartbeat;
static int read_failed, write_failed, ready = 1;
static unsigned approvals, revocations;

int test_clock_gettime(clockid_t id, struct timespec *stamp)
{
    assert(id == CLOCK_MONOTONIC);
    stamp->tv_sec = clock_ns / 1000000000ULL;
    stamp->tv_nsec = clock_ns % 1000000000ULL;
    return 0;
}

int fspi_read_autopilot_status(uint8_t *status)
{
    if (read_failed)
        return -1;
    *status = fpga_status;
    return 0;
}

int fspi_write_data(uint16_t address, uint8_t value)
{
    assert(address == 7);
    if (!value)
        ++revocations;
    else
    {
        assert((value & 0xc0) == 0x80);
        assert(((value >> 4) & 3u) == (fpga_status & 3u));
        last_heartbeat = value;
        ++approvals;
        if (write_failed)
            return -1;
    }
    return 0;
}

int steering_pulse_active(void) { return 0; }
int steering_runtime_available(void) { return ready; }
int steering_runtime_can_engage(void) { return ready; }
void steering_runtime_cancel(void) {}
void steering_runtime_set_hmi_request(uint8_t request) { software_request = request; }

static void poll(AutopilotSwitch *gate, uint8_t status, int control_ok, int output_allowed)
{
    clock_ns += 21000000ULL;
    fpga_status = status;
    poll_autopilot(gate, control_ok, output_allowed);
}

int main(void)
{
    AutopilotSwitch gate = {0};
    poll(&gate, 0x0c, 1, 1); /* Retained ON request at boot is not a new press. */
    poll(&gate, 0x04, 1, 1);
    assert(!approvals && !software_request);
    poll(&gate, 0x01, 1, 1);
    poll(&gate, 0x06, 1, 1);
    assert(approvals == 1 && !software_request);
    const uint8_t first = last_heartbeat;
    poll(&gate, 0x0e, 1, 1);
    assert(software_request == 6 && last_heartbeat != first);
    poll(&gate, 0x03, 1, 1);
    assert(!software_request && !gate.active);

    poll(&gate, 0x04, 1, 1);
    poll(&gate, 0x0c, 1, 1);
    assert(software_request == 4);
    read_failed = 1;
    poll(&gate, 0x0c, 1, 1);
    assert(!software_request && !gate.active);
    read_failed = 0;
    poll(&gate, 0x0c, 1, 1);
    assert(!software_request);

    poll(&gate, 0x01, 1, 1);
    poll(&gate, 0x06, 1, 0);
    poll(&gate, 0x0e, 1, 0);
    assert(gate.active && !software_request); /* Switch diagnostic cannot drive. */
    poll(&gate, 0x0e, 0, 0);
    assert(!software_request && !gate.active && revocations >= 3);

    ready = 0;
    poll(&gate, 0x03, 1, 1);
    poll(&gate, 0x04, 1, 1);
    ready = 1;
    poll(&gate, 0x0c, 1, 1);
    assert(!software_request && !gate.active);
    poll(&gate, 0x01, 1, 1);
    write_failed = 1;
    const unsigned before = revocations;
    poll(&gate, 0x06, 1, 1);
    assert(!software_request && !gate.active && revocations == before + 1);
    puts("PASS HMI gate: startup OFF, fresh press, FPGA approval, OFF, IO fault and switch diagnostic");
    return 0;
}
