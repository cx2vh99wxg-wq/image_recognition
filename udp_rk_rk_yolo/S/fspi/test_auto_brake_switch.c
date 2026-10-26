/* Exercise the production SPI transaction and brake monitor without hardware. */
#define ioctl test_spi_ioctl
#define perror test_spi_perror
#include "csrc/fspi_module.c"
#undef ioctl
#undef perror
#define main vehicle_control_program_main
#define emergency_brake_shm_init test_brake_attach
#define emergency_brake_shm_cleanup test_brake_detach
#define emergency_brake_read_status test_brake_read
#include "csrc/main.c"
#undef main
#undef emergency_brake_shm_init
#undef emergency_brake_shm_cleanup
#undef emergency_brake_read_status
#include <stdarg.h>

static EmergencyBrakeSharedMemory test_status;
static int reply = 0xa0;
static int io_failure;
static int direction_failure;
static int request_failure;
static int stopped;
static int cleared;
static int checked_transactions;
static int attach_calls;
static int detach_calls;
static int failed;

/* Retry failures are intentional fixtures; retain the high-level monitor log. */
void test_spi_perror(const char *description)
{
    (void)description;
}

static void expect(int condition, const char *description)
{
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", description);
        failed = 1;
    }
}

int test_spi_ioctl(int fd, unsigned long request, ...)
{
    (void)fd;
    va_list args;
    va_start(args, request);
    void *argument = va_arg(args, void *);
    va_end(args);
    if (request == SPI_IOC_WR_MODE32 || request == SPI_IOC_WR_BITS_PER_WORD ||
        request == SPI_IOC_WR_MAX_SPEED_HZ) {
        if (io_failure == 3 && request == SPI_IOC_WR_MAX_SPEED_HZ) {
            errno = EINVAL;
            return -1;
        }
        return 0;
    }
    if (request == SPI_IOC_MESSAGE(2)) {
        struct spi_ioc_transfer *tr = argument;
        const uint8_t *command = (const uint8_t *)(uintptr_t)tr[0].tx_buf;
        expect(tr[0].len == 3 && command[0] == 0x81 &&
               command[1] == 0 && command[2] == 5, "isolated switch opcode and address");
        expect(tr[0].tx_nbits == 4 && tr[1].rx_nbits == 4 &&
               tr[0].rx_buf == 0 && tr[1].tx_buf == 0 && tr[1].len == 1,
               "quad TX command followed by quad RX status");
        expect(tr[0].cs_change == 0 && tr[1].cs_change == 0,
               "both transfers keep CS asserted in one SPI_IOC_MESSAGE");
        expect(tr[0].speed_hz == SPI_SPEED && tr[1].speed_hz == SPI_SPEED && SPI_SPEED == 24000000u,
               "both status transfers use the board-supported 24MHz rate");
        expect((g_config.spi_mode & (SPI_TX_QUAD | SPI_RX_QUAD)) ==
               (SPI_TX_QUAD | SPI_RX_QUAD), "SPI mode enables both transfer directions");
        expect((g_config.spi_mode & (SPI_CPOL | SPI_CPHA)) ==
               (SPI_MODE_FLAGS & (SPI_CPOL | SPI_CPHA)), "existing CPOL/CPHA are unchanged");
        ++checked_transactions;
        if (io_failure) {
            errno = EINVAL;
            return io_failure == 2 ? 3 : -1;
        }
        *(uint8_t *)(uintptr_t)tr[1].rx_buf = (uint8_t)reply;
        return 4;
    }
    if (request == SPI_IOC_MESSAGE(1)) {
        struct spi_ioc_transfer *tr = argument;
        const uint8_t *command = (const uint8_t *)(uintptr_t)tr->tx_buf;
        expect(tr->len == 3 && command && command[0] == 0,
               "SMDT host write excludes the controller-generated opcode");
        expect(tr->speed_hz == SPI_SPEED, "existing control writes keep their configured speed");
        uint16_t address = ((uint16_t)command[0] << 8) | command[1];
        expect(address <= 4, "request monitor never writes display/switch configuration");
        if (direction_failure && address < 4) return -1;
        if (address == 4) {
            if (request_failure) {
                errno = EIO;
                return -1;
            }
            if (command[2] == 0) ++stopped;
            if (command[2] == 1) ++cleared;
        }
        return tr->len;
    }
    expect(0, "unexpected SPI ioctl");
    return -1;
}

int test_brake_attach(EmergencyBrakeSharedMemory **status, int is_writer)
{
    (void)is_writer;
    ++attach_calls;
    *status = &test_status;
    return 0;
}

void test_brake_detach(EmergencyBrakeSharedMemory *status, int is_writer)
{
    (void)status;
    (void)is_writer;
    ++detach_calls;
}

int test_brake_read(EmergencyBrakeSharedMemory *status, int *person, int *brake)
{
    if (!status->valid) return -1;
    *person = status->person_detected;
    *brake = status->brake_enable;
    return 0;
}

static int monitor_once(int person, int brake_enable, int valid, uint32_t age_seconds)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    test_status.last_update = (uint32_t)ts.tv_sec - age_seconds;
    test_status.frame_count = 1;
    test_status.valid = valid;
    test_status.person_detected = person;
    test_status.brake_enable = brake_enable;
    emergency_brake_shm = &test_status;
    stopped = cleared = 0;
    return monitor_emergency_brake();
}

int main(void)
{
    g_spi_fd = 42;
    g_config.speed_hz = SPI_SPEED;
    g_config.spi_mode = SPI_MODE_FLAGS;
    g_config.bits_per_word = 8;
    g_config.transfer_size = SPI_TRANSFER_SIZE;
    g_config.protocol_mode = SPI_MODE_QUAD;
    int enabled = 7;
    expect(fspi_read_auto_brake_switch(&enabled) == 0 && enabled == 0, "A0 means OFF");
    expect(g_config.speed_hz == SPI_SPEED, "status read does not mutate the shared SPI speed");
    reply = 0xa1;
    expect(fspi_read_auto_brake_switch(&enabled) == 0 && enabled == 1, "A1 means ON");
    reply = 0xff;
    expect(fspi_read_auto_brake_switch(&enabled) != 0 && enabled == 1, "floating/old FPGA reply is rejected");
    reply = 0xaa;
    expect(fspi_read_auto_brake_switch(&enabled) != 0 && enabled == 1, "legacy repeated mode nibble is rejected");
    int diagnostic_errno, diagnostic_result;
    uint8_t diagnostic_status;
    fspi_get_auto_brake_diagnostic(&diagnostic_errno, &diagnostic_result, &diagnostic_status);
    expect(diagnostic_errno == EPROTO && diagnostic_result == 4 && diagnostic_status == 0xaa,
           "invalid signature diagnostic retains the raw byte");
    io_failure = 1;
    expect(fspi_read_auto_brake_switch(&enabled) != 0 && enabled == 1, "SPI failure preserves caller state");
    fspi_get_auto_brake_diagnostic(&diagnostic_errno, &diagnostic_result, &diagnostic_status);
    expect(diagnostic_errno == EINVAL && diagnostic_result == -1 && diagnostic_status == 0xff,
           "unsupported driver rate EINVAL is distinguishable from bad signature");
    io_failure = 2;
    expect(fspi_read_auto_brake_switch(&enabled) != 0 && enabled == 1, "short transfer is rejected");
    fspi_get_auto_brake_diagnostic(&diagnostic_errno, &diagnostic_result, &diagnostic_status);
    expect(diagnostic_errno == EIO && diagnostic_result == 3, "short transfer diagnostics include actual length");
    io_failure = 3;
    expect(fspi_read_auto_brake_switch(&enabled) != 0 && enabled == 1, "configuration error preserves state");
    fspi_get_auto_brake_diagnostic(&diagnostic_errno, &diagnostic_result, &diagnostic_status);
    expect(diagnostic_errno == EINVAL && diagnostic_result == -1, "configuration errno is retained without perror spam");
    io_failure = 0;
    expect(fspi_read_auto_brake_switch(NULL) != 0, "null output rejected");

    /* A real board returned 00 for every 81 read. It must never be a runtime
     * prerequisite for publishing detector requests or running steering. */
    reply = 0x00;
    int diagnostic_transactions = checked_transactions;
    last_brake_value = 1;
    expect(monitor_once(1, 0, 1, 0) == 0 && stopped > 0,
           "fresh person publishes raw reg4=0 without A1 and without inferring hardware AUTO");
    emergency_brake_shm = NULL;
    next_brake_attach = 0;
    stopped = cleared = 0;
    expect(monitor_emergency_brake() == 0 && attach_calls > 0 && stopped > 0,
           "detector attaches independently of switch readback");
    expect(monitor_once(0, 0, 1, 0) == 0 && stopped > 0,
           "post-person hold remains a raw request for the FPGA gate");
    expect(monitor_once(0, 1, 1, 0) == 0 && stopped == 0 && cleared > 0,
           "clear detector request writes only reg4=1 without REARM");
    expect(monitor_once(0, 0, 0, 0) == 0 && stopped > 0,
           "detector fault retains original raw-request policy without software AUTO gating");
    expect(monitor_once(1, 0, 1, 3) == 0 && stopped > 0 && detach_calls > 0,
           "stale data publishes the original fault request and reconnects");
    io_failure = 1;
    expect(monitor_once(1, 0, 1, 0) == 0 && stopped > 0,
           "an optional diagnostic ioctl failure cannot suppress a person request");
    io_failure = 0;
    request_failure = 1;
    expect(monitor_once(1, 0, 1, 0) == 1 && stopped == 0,
           "actual raw-request write failure blocks software steering");
    request_failure = 0;
    expect(monitor_once(1, 0, 1, 0) == 0 && stopped > 0,
           "a successful retry releases the software IO-failure gate");
    request_failure = 1;
    expect(monitor_once(0, 1, 1, 0) == 1 && cleared == 0,
           "actual request-clear write failure also blocks software steering");
    request_failure = 0;
    expect(monitor_once(0, 1, 1, 0) == 0 && cleared > 0,
           "failed clear is retried without switch readback");
    direction_failure = 1;
    direction_release_failed = 1;
    expect(monitor_once(0, 1, 1, 0) == 1 && stopped > 0,
           "direction write failure gates steering and publishes a raw fault request");
    direction_failure = 0;
    expect(monitor_once(0, 1, 1, 0) == 0 && cleared > 0 && !direction_release_failed,
           "direction-release retry recovers without relying on AUTO status");
    turn_pulse_active = 1;
    expect(monitor_once(1, 0, 1, 0) == 0 && stopped > 0,
           "steering wait can publish raw requests while FPGA alone gates physical directions");
    expect(checked_transactions == diagnostic_transactions,
           "production control monitor sends no 81 status-read transactions");
    if (failed) return 1;
    puts("optional SPI diagnostic and FPGA-gated raw brake monitor tests passed");
    return 0;
}
