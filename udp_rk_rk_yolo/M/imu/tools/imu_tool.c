#include "imu_tool.h"
#include "imu_source.h"
#include "steering_angle.h"
#include "steering_platform.h"
#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static volatile sig_atomic_t cancelled;
static void cancel_handler(int number) { (void)number; cancelled = 1; }
static int integer(const char *text, unsigned *out)
{
    char *end; errno = 0;
    if (!*text || *text == '-') return 0;
    unsigned long value = strtoul(text, &end, 10);
    if (errno || *end || value > 1000000) return 0;
    *out = (unsigned)value; return 1;
}
static void usage(void)
{
    puts("imu_dump / angle_monitor [--backend mock|fpga] [--samples N]\n"
         "  [--interval-ms N] [--print-every N] [--csv FILE]\n"
         "  [--mock-rate-dps N] [--axis x|y|z] [--axis-sign 1|-1]\n"
         "Defaults: MOCK, 100 samples, 10ms interval, print every 10, Z axis.\n"
         "MOCK has constant Z gyro and gravity on Z; it never accesses hardware.\n"
         "angle_monitor integrates relative motion from start, NOT physical center.\n"
         "FPGA acquisition is an explicit NOT_READY extension point.");
}
int imu_tool_main(int argc, char **argv, int estimate)
{
    const char *backend_name = "mock", *axis_name = "z", *csv_path = NULL;
    unsigned count = 100, interval = 10, print_every = 10;
    float mock_rate = 0, sign = 1;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--help")) { usage(); return 0; }
        if (i + 1 == argc) { usage(); return 2; }
        const char *key = argv[i], *value = argv[++i];
        if (!strcmp(key, "--backend")) backend_name = value;
        else if (!strcmp(key, "--axis")) axis_name = value;
        else if (!strcmp(key, "--csv")) csv_path = value;
        else if (!strcmp(key, "--samples")) { if (!integer(value, &count)) return 2; }
        else if (!strcmp(key, "--interval-ms")) { if (!integer(value, &interval)) return 2; }
        else if (!strcmp(key, "--print-every")) { if (!integer(value, &print_every)) return 2; }
        else if (!strcmp(key, "--axis-sign")) {
            if (!strcmp(value, "-1")) sign = -1;
            else if (strcmp(value, "1")) return 2;
        } else if (!strcmp(key, "--mock-rate-dps")) {
            char *end; errno = 0; mock_rate = strtof(value, &end);
            if (errno || !*value || *end || !isfinite(mock_rate) || fabsf(mock_rate) > 2000)
                return 2;
        } else { usage(); return 2; }
    }
    int axis_index = !strcmp(axis_name, "x") ? 0 : !strcmp(axis_name, "y") ? 1 :
                     !strcmp(axis_name, "z") ? 2 : -1;
    int fpga = !strcmp(backend_name, "fpga");
    if ((!fpga && strcmp(backend_name, "mock")) || axis_index < 0 || !count ||
        !interval || interval > 1000 || !print_every || (fpga && mock_rate != 0)) {
        usage(); return 2;
    }
    imu_source_t source;
    steer_result_t rc = imu_source_open(&source, fpga ? IMU_SOURCE_FPGA : IMU_SOURCE_MOCK,
                                        mock_rate);
    if (rc != STEER_OK) {
        fprintf(stderr, "FPGA/MPU unavailable: %s. B must implement the I2C sampler, "
                "snapshot readback and motion source; no measured data was produced.\n",
                steer_result_name(rc)); return 1;
    }
    FILE *csv = NULL;
    if (csv_path) {
        csv = steering_create_csv(csv_path);
        if (!csv) { perror("Create CSV (choose a new filename)"); imu_source_close(&source); return 1; }
        fputs("backend,session,seq,sample_time_ns,flags,ax_mps2,ay_mps2,az_mps2,"
              "gx_dps,gy_dps,gz_dps,temp_c,relative_deg,axis_rate_dps,zero_valid\n", csv);
    }
    signal(SIGINT, cancel_handler); signal(SIGTERM, cancel_handler);
    cancelled = 0;
    float axis[3] = {0}, bias[3] = {0}; axis[axis_index] = sign;
    steering_angle_t integrator;
    unsigned max_gap = interval > 25 ? interval * 4 : 100;
    (void)steering_angle_init(&integrator, axis, bias, max_gap);
    puts("[SIMULATED MOCK] No sensor access. No bias/center/chassis calibration. ZERO_VALID=0.");
    int exit_code = 0;
    uint64_t next_sample = steering_now_ns();
    for (unsigned i = 0; i < count && !cancelled; ++i) {
        steering_imu_sample_t sample;
        rc = imu_source_read(&source, &sample);
        double relative = NAN; float projected = NAN;
        if (rc == STEER_OK && estimate)
            rc = steering_angle_update(&integrator, &sample, &relative, &projected);
        if (rc != STEER_OK) {
            fprintf(stderr, "Sample/integration failed: %s; explicitly restart after discontinuity.\n",
                    steer_result_name(rc)); exit_code = 1; break;
        }
        if (i % print_every == 0 || i + 1 == count)
            printf("SIMULATED seq=%u t_ns=%" PRIu64 " accel=[%.3f %.3f %.3f] m/s2 "
                   "gyro=[%.3f %.3f %.3f] dps relative_deg=%.4f ZERO_VALID=0\n",
                   sample.sequence, sample.sample_time_ns, sample.accel_mps2[0],
                   sample.accel_mps2[1], sample.accel_mps2[2], sample.gyro_dps[0],
                   sample.gyro_dps[1], sample.gyro_dps[2], relative);
        if (csv) {
            fprintf(csv, "%s,%u,%u,%" PRIu64 ",%u,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,"
                    "%.4f,%.6f,%.6f,0\n", backend_name, sample.session_id, sample.sequence,
                    sample.sample_time_ns, sample.flags, sample.accel_mps2[0],
                    sample.accel_mps2[1], sample.accel_mps2[2], sample.gyro_dps[0],
                    sample.gyro_dps[1], sample.gyro_dps[2], sample.temperature_c, relative, projected);
            if (fflush(csv) || ferror(csv)) { perror("CSV write"); exit_code = 1; break; }
        }
        next_sample += (uint64_t)interval * 1000000;
        if (i + 1 < count) {
            while (!cancelled && steering_now_ns() < next_sample) steering_sleep_ms(1);
            /* Do not emit a burst of fake catch-up samples after a long pause. */
            uint64_t now = steering_now_ns();
            if (now > next_sample + (uint64_t)interval * 1000000) next_sample = now;
        }
    }
    imu_source_close(&source);
    if (csv && fclose(csv)) exit_code = 1;
    return exit_code ? exit_code : cancelled ? 130 : 0;
}
