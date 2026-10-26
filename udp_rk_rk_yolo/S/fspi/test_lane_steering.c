/* Production steering/brake code with a deterministic clock and SPI device. */
#define ioctl lane_test_ioctl
#define perror lane_test_perror
#define usleep lane_test_sleep
#define clock_gettime lane_test_clock
#include "csrc/fspi_module.c"
#define main lane_control_program_main
#define emergency_brake_shm_init lane_test_attach
#define emergency_brake_shm_cleanup lane_test_detach
#define emergency_brake_read_status lane_test_read
#include "csrc/main.c"
#undef main
#undef ioctl
#undef perror
#undef usleep
#undef clock_gettime
#undef emergency_brake_shm_init
#undef emergency_brake_shm_cleanup
#undef emergency_brake_read_status
#include <assert.h>
#include <stdarg.h>

static uint64_t fake_us;
static EmergencyBrakeSharedMemory detector;
static uint8_t registers[5];
static unsigned int direction_writes, traction_writes, stop_writes;
static unsigned int active_duration, expected_duration;
static int active_command, fail_start, fail_release, short_start, clear_during_pulse;

void lane_test_perror(const char *description) { (void)description; }

int lane_test_clock(clockid_t clock_id, struct timespec *ts) {
    assert(clock_id == CLOCK_MONOTONIC);
    ts->tv_sec = fake_us / 1000000;
    ts->tv_nsec = (fake_us % 1000000) * 1000;
    return 0;
}

int lane_test_sleep(useconds_t duration) {
    fake_us += duration;
    detector.last_update = fake_us / 1000000;
    if (active_command >= 0 && duration == 20000) {
        assert(registers[active_command] == 0);
        active_duration += duration;
    }
    if (clear_during_pulse && active_command >= 0) {
        clear_during_pulse = 0;
        detector.person_detected = 0;
        detector.brake_enable = 1;
    }
    return 0;
}

int lane_test_ioctl(int fd, unsigned long request, ...) {
    (void)fd;
    va_list args;
    va_start(args, request);
    void *argument = va_arg(args, void *);
    va_end(args);
    if (request == SPI_IOC_WR_MODE32 || request == SPI_IOC_WR_BITS_PER_WORD ||
        request == SPI_IOC_WR_MAX_SPEED_HZ) return 0;
    assert(request == SPI_IOC_MESSAGE(1));
    struct spi_ioc_transfer *tr = argument;
    const uint8_t *tx = (const uint8_t *)(uintptr_t)tr->tx_buf;
    assert(tr->len == 3 && tx && tx[0] == 0 && tx[1] < 5);
    const int address = tx[1], value = tx[2];
    if (address == short_start && value == 0) return 2;
    if ((address == fail_start && value == 0) ||
        (address == fail_release && value == 1 && active_command == address)) {
        errno = EIO;
        return -1;
    }
    if (address < 4) ++direction_writes; else ++stop_writes;
    if (address < 2) ++traction_writes;
    registers[address] = value;
    if (address == 2 || address == 3) {
        if (value == 0) {
            assert(registers[address == 2 ? 3 : 2] == 1);
            active_command = address;
            active_duration = 0;
        } else if (active_command == address) {
            if (expected_duration) assert(active_duration == expected_duration);
            active_command = -1;
        }
    }
    return tr->len;
}

int lane_test_attach(EmergencyBrakeSharedMemory **status, int writer) {
    (void)writer;
    *status = &detector;
    return 0;
}
void lane_test_detach(EmergencyBrakeSharedMemory *status, int writer) {
    (void)status; (void)writer;
}
int lane_test_read(EmergencyBrakeSharedMemory *status, int *person, int *brake) {
    *person = status->person_detected;
    *brake = status->brake_enable;
    return status->valid ? 0 : -1;
}

static void reset_fixture(void) {
    fake_us = 10000000;
    memset(&detector, 0, sizeof(detector));
    detector.frame_count = detector.valid = detector.brake_enable = 1;
    detector.last_update = fake_us / 1000000;
    emergency_brake_shm = &detector;
    memset(registers, 1, sizeof(registers));
    direction_writes = traction_writes = stop_writes = 0;
    active_duration = expected_duration = 0;
    active_command = fail_start = fail_release = short_start = -1;
    clear_during_pulse = 0;
    last_brake_value = 1;
    next_brake_release_refresh = 0;
    raw_request_active = turn_pulse_active = turn_pulse_interrupted = 0;
    direction_release_failed = 0;
    wheel_position = WHEEL_NEUTRAL;
    g_running = 1;
    g_spi_fd = 42;
    g_config.speed_hz = SPI_SPEED;
    g_config.spi_mode = SPI_MODE_FLAGS;
    g_config.bits_per_word = 8;
    g_config.transfer_size = SPI_TRANSFER_SIZE;
    g_config.protocol_mode = SPI_MODE_QUAD;
}

int main(void) {
    reset_fixture();
    registers[0] = 0; // An existing forward request must survive idle heartbeats.
    assert(monitor_emergency_brake() == 0 && stop_writes == 1);
    assert(direction_writes == 0 && registers[0] == 0);
    expected_duration = TIME_NEUTRAL_TO_LEFT;
    turn_to_left();
    assert(wheel_position == WHEEL_LEFT && traction_writes == 0);
    assert(registers[2] == 1 && registers[3] == 1 && registers[0] == 0);
    unsigned int before = direction_writes;
    turn_to_left();
    assert(direction_writes == before); // Preserve one pulse per target position.
    expected_duration = TIME_LEFT_TO_RIGHT;
    turn_to_right();
    assert(wheel_position == WHEEL_RIGHT && traction_writes == 0);
    expected_duration = TIME_RIGHT_TO_NEUTRAL;
    turn_to_neutral();
    assert(wheel_position == WHEEL_NEUTRAL && traction_writes == 0);
    expected_duration = 0;
    before = direction_writes;
    next_brake_release_refresh = 0;
    assert(monitor_emergency_brake() == 0 && direction_writes == before);

    reset_fixture();
    fail_start = 2;
    turn_to_left();
    assert(wheel_position == WHEEL_NEUTRAL && direction_release_failed);
    fail_start = -1;
    expected_duration = TIME_NEUTRAL_TO_LEFT;
    turn_to_left();
    assert(wheel_position == WHEEL_LEFT && !direction_release_failed);

    reset_fixture();
    short_start = 2;
    turn_to_left();
    assert(wheel_position == WHEEL_NEUTRAL && direction_release_failed);
    short_start = -1;
    expected_duration = TIME_NEUTRAL_TO_LEFT;
    turn_to_left();
    assert(wheel_position == WHEEL_LEFT && !direction_release_failed);

    reset_fixture();
    fail_release = 3;
    turn_to_right();
    assert(wheel_position == WHEEL_NEUTRAL && direction_release_failed);
    fail_release = -1;
    expected_duration = 0;
    detector.person_detected = 1;
    detector.brake_enable = 0;
    assert(monitor_emergency_brake() == 0 && registers[3] == 1);
    assert(!direction_release_failed); // Retry releases even with a raw brake request.
    detector.person_detected = 0;
    detector.brake_enable = 1;
    assert(monitor_emergency_brake() == 0 && registers[3] == 1);
    expected_duration = TIME_NEUTRAL_TO_RIGHT;
    turn_to_right();
    assert(wheel_position == WHEEL_RIGHT && !direction_release_failed);

    reset_fixture();
    detector.person_detected = 1;
    detector.brake_enable = 0;
    last_brake_value = 0;
    raw_request_active = 1;
    clear_during_pulse = 1;
    turn_to_left();
    assert(wheel_position == WHEEL_NEUTRAL && registers[2] == 1);
    expected_duration = TIME_NEUTRAL_TO_LEFT;
    turn_to_left();
    assert(wheel_position == WHEEL_LEFT);
    puts("PASS lane steering: unchanged calibration, independent traction, heartbeat isolation, write failures and interrupted-pulse retry");
    return 0;
}
