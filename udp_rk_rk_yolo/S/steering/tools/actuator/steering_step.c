#include "steering_backends.h"
#include "steering_platform.h"
#include <errno.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef STEERING_WITH_FSPI
#include "fspi_module.h"
#include <time.h>
#endif

static int wait_for_tick(int hardware, uint64_t deadline_ns)
{
#ifdef STEERING_WITH_FSPI
    if (hardware) {
        struct timespec deadline = {(time_t)(deadline_ns / UINT64_C(1000000000)),
                                    (long)(deadline_ns % UINT64_C(1000000000))};
        return clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &deadline, NULL);
    }
#else
    (void)hardware; (void)deadline_ns;
#endif
    steering_sleep_ms(1);
    return 0;
}

static steer_result_t trace_active_contact(unsigned pulse, steer_direction_t direction)
{
#ifdef STEERING_WITH_FSPI
    ControlSnapshot s = {0};
    if (fspi_read_control_snapshot(&s)) {
        fprintf(stderr, "FPGA_ACTIVE pulse=%u snapshot unavailable; cancelling request.\n", pulse);
        return STEER_ERR_IO;
    }
    unsigned requested = direction == STEER_LEFT ? CONTACT_LEFT_N : CONTACT_RIGHT_N;
    unsigned opposite = direction == STEER_LEFT ? CONTACT_RIGHT_N : CONTACT_LEFT_N;
    int active = !(s.contacts & requested) && (s.contacts & opposite) &&
                 (s.contacts & CONTACT_GO_N) && (s.contacts & CONTACT_BACK_N) &&
                 (s.contacts & CONTACT_STOP_N) && !(s.gates & GATE_STOP) &&
                 (s.gates & GATE_FS_ARMED) && (s.gates & GATE_RESET_RELEASED);
    printf("FPGA_ACTIVE pulse=%u contacts=0x%02X gates=0x%02X left_n=%u right_n=%u "
           "requested_contact_low=%s (logical state, not measured voltage/angle)\n",
           pulse, s.contacts, s.gates, !!(s.contacts & CONTACT_LEFT_N),
           !!(s.contacts & CONTACT_RIGHT_N), active ? "YES" : "NO");
    return active ? STEER_OK : STEER_ERR_BUSY;
#else
    (void)pulse; (void)direction; return STEER_ERR_NOT_READY;
#endif
}

static volatile sig_atomic_t cancelled;
static void cancel_handler(int number) { (void)number; cancelled = 1; }
static int number(const char *text, unsigned *value)
{
    char *end; errno = 0;
    if (!text || !*text || *text == '-') return 0;
    unsigned long parsed = strtoul(text, &end, 10);
    if (errno || *end || parsed > 1000000UL) return 0;
    *value = (unsigned)parsed; return 1;
}
static void usage(void)
{
    puts("steering_step --direction left|right|release [--duration-ms N | --duration-us N]\n"
         "  [--backend dry-run|fspi] [--repeat N] [--gap-ms N]\n"
         "  [--max-pulse-ms N] [--csv FILE]\n"
         "Defaults: dry-run, one pulse, 500ms gap, 200ms pulse limit.\n"
         "Hard pulse limit: 1000ms. Release is NOT physical centering.\n"
         "17500us = 17.5ms; us input resolution is NOT a timing accuracy guarantee.\n"
         "Hardware is bench-only: exit legacy vehicle_control first; no GO/BACK/STOP writes.\n"
         "Timing records software write windows, NOT measured pin-low duration.");
}
int main(int argc, char **argv)
{
    const char *backend_name = "dry-run", *direction_name = NULL, *csv_path = NULL;
    unsigned duration_us = 0, repeat = 1, gap = 500, limit = 200;
    int duration_seen = 0;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--help")) { usage(); return 0; }
        if (i + 1 == argc) { usage(); return 2; }
        const char *key = argv[i], *value = argv[++i];
        if (!strcmp(key, "--backend")) backend_name = value;
        else if (!strcmp(key, "--direction")) direction_name = value;
        else if (!strcmp(key, "--csv")) csv_path = value;
        else if (!strcmp(key, "--duration-ms") || !strcmp(key, "--duration-us")) {
            unsigned input;
            if (duration_seen++ || !number(value, &input)) return 2;
            duration_us = !strcmp(key, "--duration-ms") ? input * 1000u : input;
        }
        else if (!strcmp(key, "--repeat")) { if (!number(value, &repeat)) return 2; }
        else if (!strcmp(key, "--gap-ms")) { if (!number(value, &gap)) return 2; }
        else if (!strcmp(key, "--max-pulse-ms")) { if (!number(value, &limit)) return 2; }
        else { usage(); return 2; }
    }
    steer_direction_t direction = STEER_RELEASE;
    if (!direction_name) { usage(); return 2; }
    if (!strcmp(direction_name, "left")) direction = STEER_LEFT;
    else if (!strcmp(direction_name, "right")) direction = STEER_RIGHT;
    else if (strcmp(direction_name, "release")) { usage(); return 2; }
    int hardware = !strcmp(backend_name, "fspi");
    if ((!hardware && strcmp(backend_name, "dry-run")) || !repeat || repeat > 100 ||
        gap > 60000 || !limit || limit > STEERING_HARD_MAX_PULSE_MS ||
        (direction != STEER_RELEASE && (!duration_us || duration_us > limit * 1000u)) ||
        (direction == STEER_RELEASE && (duration_us || repeat != 1))) {
        fprintf(stderr, "Invalid arguments; no hardware was opened. See --help.\n"); return 2;
    }
    FILE *csv = NULL;
    if (csv_path) {
        /* Exclusive creation avoids silently overwriting a previous measurement. */
        csv = steering_create_csv(csv_path);
        if (!csv) { perror("Create CSV (choose a new filename)"); return 1; }
        fputs("backend,pulse,direction,requested_ms,enable_begin_ns,enable_end_ns,"
              "release_begin_ns,release_end_ns,software_elapsed_ms,result\n", csv);
        fflush(csv);
    }
    signal(SIGINT, cancel_handler); signal(SIGTERM, cancel_handler);
    setvbuf(stdout, NULL, _IOLBF, 0);
    steering_fspi_t fspi = {0, -1};
    steering_actuator_io_t io = steering_dry_run_io();
    if (hardware) {
        steer_result_t rc = steering_fspi_open(&fspi);
        if (rc != STEER_OK) {
            fprintf(stderr, "FSPI unavailable: %s. Build --hardware on Linux/RK; "
                    "stop other FSPI owners first.\n", steer_result_name(rc));
            if (csv) fclose(csv);
            return 1;
        }
        io = steering_fspi_io(&fspi);
    }
    steering_actuator_t actuator = {0};
    steer_result_t rc = steering_actuator_init(&actuator, io, limit);
    int exit_code = rc == STEER_OK ? 0 : 1;
    if (rc == STEER_OK && direction == STEER_RELEASE)
        exit_code = steering_actuator_stop(&actuator) == STEER_OK ? 0 : 1;
    for (unsigned pulse = 1; direction != STEER_RELEASE && !exit_code &&
         !cancelled && pulse <= repeat; ++pulse) {
        rc = steering_actuator_start_us(&actuator, direction, duration_us);
        if (hardware && rc == STEER_OK) {
            rc = trace_active_contact(pulse, direction);
            if (rc != STEER_OK) {
                steer_result_t stop_rc = steering_actuator_stop(&actuator);
                if (stop_rc != STEER_OK) rc = stop_rc;
            } else {
                /* The RX transaction consumes pulse time; keep the original deadline. */
                rc = steering_actuator_tick(&actuator);
            }
        }
        while (rc == STEER_OK && actuator.status == ACTUATOR_ACTIVE && !cancelled) {
            int wait_result = wait_for_tick(hardware, actuator.deadline_ns);
            if (wait_result && wait_result != EINTR) {
                fprintf(stderr, "Deadline wait failed (%d); releasing contacts.\n", wait_result);
                steer_result_t stop_rc = steering_actuator_stop(&actuator);
                rc = stop_rc == STEER_OK ? STEER_ERR_IO : stop_rc;
                break;
            }
            if (cancelled) break;
            rc = steering_actuator_tick(&actuator);
        }
        if (cancelled) {
            steer_result_t stop_rc = steering_actuator_stop(&actuator);
            if (stop_rc != STEER_OK) rc = stop_rc;
        }
        const char *result = rc != STEER_OK ? steer_result_name(rc) :
                             cancelled ? "CANCELLED" : "COMMAND_COMPLETED";
        double elapsed = actuator.release_end_ns >= actuator.enable_end_ns ?
            (actuator.release_end_ns - actuator.enable_end_ns) / 1e6 : 0;
        printf("%s pulse=%u direction=%s requested_ms=%.3f software_elapsed_ms=%.3f %s "
               "(no angle/pin measurement)\n", backend_name, pulse, direction_name,
               duration_us / 1000.0, elapsed, result);
        if (csv) {
            fprintf(csv, "%s,%u,%s,%.3f,%" PRIu64 ",%" PRIu64 ",%" PRIu64 ",%" PRIu64
                    ",%.3f,%s\n", backend_name, pulse, direction_name, duration_us / 1000.0,
                    actuator.enable_begin_ns, actuator.enable_end_ns,
                    actuator.release_begin_ns, actuator.release_end_ns, elapsed, result);
            if (fflush(csv) || ferror(csv)) { perror("CSV write"); exit_code = 1; }
        }
        if (rc != STEER_OK) exit_code = 1;
        if (pulse < repeat && !exit_code && !cancelled) {
            uint64_t deadline = steering_now_ns() + (uint64_t)gap * 1000000;
            while (!cancelled && steering_now_ns() < deadline) steering_sleep_ms(1);
        }
    }
    /* Retried release on ordinary exit; SIGKILL/power loss needs hardware timeout. */
    if (rc == STEER_OK || actuator.io.set_direction) {
        if (steering_actuator_stop(&actuator) != STEER_OK) {
            fprintf(stderr, "Release failed; check contacts and use physical STOP.\n"); exit_code = 1;
        }
    }
    if (hardware) steering_fspi_close(&fspi);
    if (csv && fclose(csv)) exit_code = 1;
    return exit_code ? exit_code : cancelled ? 130 : 0;
}
