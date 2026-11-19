#define _POSIX_C_SOURCE 200809L
#include "steering_runtime.h"
#include "steering_feedback.h"
#include "steering_center.h"
#include "steering_keepalive.h"
#include "steering_platform.h"
#include "fspi_module.h"
#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>
enum { STEERING_ANGLE_LIMIT_DEG = 18 };
static steering_response_model_t model;
static SteeringCenter center;
static const char *reference_file;
static uint64_t center_ready_until;
static steering_keepalive_t keepalive;
static ControlSnapshot snapshot;
static uint64_t snapshot_ns, lane_ns, next_lane_poll_ns;
static uint32_t reference_session, last_frame, blocked_frame;
static uint64_t reference_notice_ns;
static uint32_t reference_notice_session;
static steer_result_t reference_notice_result;
static float zero_raw, curve_gain = .25f, target_limit = 12, last_target, target_tolerance = 2,
                       center_tolerance = 3;
static int initialized, confirmed, bench, have_lane, have_frame;
static int range_recovery, skip_first_recovery_frame;
static int startup_calibration, startup_center_pending;
static uint32_t calibration_session;
static uint64_t calibration_deadline, next_calibration_request, feedback_lost_since;
static int scheduling_changed, memory_locked, previous_policy;
static struct sched_param previous_schedule;
static uint64_t now_ns(void *p)
{
    (void)p;
    return steering_now_ns();
}
int steering_runtime_requested(void)
{
    const char *s = getenv("STEERING_CONTROL_MODE");
    return s && !strcmp(s, "closed-loop");
}
static steer_result_t output(void *ctx, steer_direction_t d)
{
    (void)ctx;
    snapshot_ns = 0;
    if (d == STEER_RELEASE)
    {
        int a = -1, b = -1;
        for (int i = 0; i < 3 && (a || b); ++i)
        {
            if (a)
                a = fspi_write_direction_once(2, 1);
            if (b)
                b = fspi_write_direction_once(3, 1);
        }
        return a || b ? STEER_ERR_IO : STEER_OK;
    }
    if (d != STEER_LEFT && d != STEER_RIGHT)
        return STEER_ERR_ARGUMENT;
    if (fspi_write_direction_once(d == STEER_LEFT ? 3 : 2, 1))
        return STEER_ERR_IO;
    return fspi_write_direction_once(d == STEER_LEFT ? 2 : 3, 0) ? STEER_ERR_IO : STEER_OK;
}
static steer_result_t read_feedback(void *ctx, steering_feedback_t *f)
{
    (void)ctx;
    return steering_feedback_read(f);
}
static steer_result_t permission(void *ctx, steer_direction_t d)
{
    (void)ctx;
    uint64_t now = steering_now_ns();
    uint64_t deadline = steering_pulse_deadline_ns();
    /* SPI 的 RX/状态事务可能阻塞数毫秒，
     * 临近释放时读取会把 30ms 触点拉长。
     * 最后 6ms 复用近期快照，配合通常 5ms
     * 刷新，最大保持年龄为 11ms。物理输出仍经
     * FPGA STOP/人工仲裁，每次
     * tick 都检查反馈时效和执行器截止时间。 */
    int near_release = deadline && deadline >= now && deadline - now <= 6000000 && snapshot_ns &&
                       now >= snapshot_ns && now - snapshot_ns < 12000000;
    if (!near_release && (!snapshot_ns || now < snapshot_ns || now - snapshot_ns >= 5000000))
    {
        if (fspi_read_control_snapshot(&snapshot))
            return STEER_ERR_IO;
        snapshot_ns = now;
    }
    if (!(snapshot.gates & GATE_RESET_RELEASED) || !(snapshot.gates & GATE_FS_ARMED) ||
        (snapshot.gates & GATE_STOP) || !(snapshot.contacts & CONTACT_STOP_N) ||
        snapshot.hmi == 0xcc || snapshot.hmi == 0xdd || snapshot.hmi == 0xee)
        return STEER_ERR_BUSY;
    if (bench && ((snapshot.gates & GATE_AUTO) || !(snapshot.contacts & CONTACT_GO_N) ||
                  !(snapshot.contacts & CONTACT_BACK_N)))
        return STEER_ERR_BUSY;
    if (steering_pulse_active() && d != STEER_RELEASE)
    {
        unsigned own = d == STEER_LEFT ? CONTACT_LEFT_N : CONTACT_RIGHT_N,
                 other = d == STEER_LEFT ? CONTACT_RIGHT_N : CONTACT_LEFT_N;
        if ((snapshot.contacts & own) || !(snapshot.contacts & other))
            return STEER_ERR_BUSY;
    }
    else if ((snapshot.contacts & (CONTACT_LEFT_N | CONTACT_RIGHT_N)) !=
             (CONTACT_LEFT_N | CONTACT_RIGHT_N))
        return STEER_ERR_BUSY;
    /* FPGA 的实线权限只约束 HMI 跨线操作；
     * 弯道跟随属于车道内转向，不能借用该禁转位解释为禁止循迹。 */
    return STEER_OK;
}
static steer_result_t heartbeat(void *ctx)
{
    (void)ctx;
    ControlSnapshot s;
    if (fspi_read_control_snapshot(&s))
        return STEER_ERR_IO;
    steering_state_t st;
    steering_get_state(&st);
    printf(
        "[STEER_HEARTBEAT] seq=%llu status=%d angle=%.3f target=%.3f feedback=%d no_motion_pulse=1\n",
        (unsigned long long)keepalive.count + 1, st.status, st.angle_deg, st.target_deg,
        st.feedback_valid);
    return STEER_OK;
}
static float env_float(const char *name, float fallback)
{
    const char *s = getenv(name);
    if (!s)
        return fallback;
    char *e;
    float n = strtof(s, &e);
    return *e ? NAN : n;
}
steer_result_t steering_runtime_init(int bench_mode)
{
    const char *table = getenv("STEERING_RESPONSE_TABLE"), *ref = getenv("STEERING_REFERENCE_FILE");
    if (!table || !ref)
        return STEER_ERR_NOT_READY;
    steer_result_t rc = steering_response_load(&model, table);
    if (rc != STEER_OK)
        return rc;
    FILE *f = fopen(ref, "r");
    char magic[48], extra;
    unsigned declared;
    if (!f)
        return STEER_ERR_IO;
    int ok = fscanf(f, "%47s %u %f %u", magic, &reference_session, &zero_raw, &declared) == 4 &&
             !strcmp(magic, "STEER_PHYSICAL_REFERENCE_V1") && reference_session &&
             isfinite(zero_raw) && declared == 1;
    if (ok && fscanf(f, " %c", &extra) == 1)
        ok = 0;
    fclose(f);
    if (!ok)
        return STEER_ERR_ARGUMENT;
    if (model.burst_max_us && (model.burst_session != reference_session ||
                               fabsf(model.burst_zero_raw_deg - zero_raw) > .001f))
        return STEER_ERR_STALE;
    curve_gain = env_float("STEERING_CURVE_GAIN", .25f);
    target_limit = env_float("STEERING_LANE_TARGET_LIMIT_DEG", 12);
    target_tolerance = env_float("STEERING_TARGET_TOLERANCE_DEG", model.burst_max_us ? 3 : 2);
    center_tolerance = env_float("STEERING_CENTER_TOLERANCE_DEG", 3);
    float priority_f = env_float("STEERING_PRIORITY", 0);
    float pulse_count_f = env_float("STEERING_MAX_CORRECTION_PULSES", 20);
    float port_f = env_float("STEERING_FEEDBACK_PORT", 54321);
    if (!isfinite(curve_gain) || curve_gain <= 0 || curve_gain > 2 || !isfinite(target_limit) ||
        target_limit < 1 || target_limit > 16 || !isfinite(priority_f) || priority_f < 0 ||
        priority_f > 30 || floorf(priority_f) != priority_f || !isfinite(target_tolerance) ||
        target_tolerance < .1f || target_tolerance > 3 || !isfinite(center_tolerance) ||
        center_tolerance < .1f || center_tolerance > 3 || !isfinite(pulse_count_f) ||
        pulse_count_f < 1 || pulse_count_f > 20 || floorf(pulse_count_f) != pulse_count_f ||
        !isfinite(port_f) || port_f < 1 || port_f > 65535 || floorf(port_f) != port_f)
        return STEER_ERR_ARGUMENT;
    unsigned priority = (unsigned)priority_f;
    rc = steering_feedback_open("0.0.0.0", "192.168.100.10", (unsigned)port_f, 100);
    if (rc != STEER_OK)
        return rc;
    steering_controller_io_t io = {NULL,    {NULL, output, now_ns},  read_feedback, permission,
                                   &model,  (unsigned)pulse_count_f, 300,           3,
                                   priority};
    rc = steering_bind(io);
    if (rc == STEER_OK)
    {
        steering_config_t cfg = {-STEERING_ANGLE_LIMIT_DEG, STEERING_ANGLE_LIMIT_DEG,
                                 target_tolerance, 100,
                                 model.burst_max_us ? model.burst_max_us / 1000 : 60};
        rc = steering_init(&cfg);
    }
    if (rc != STEER_OK)
    {
        steering_feedback_close();
        return rc;
    }
    rc = output(NULL, STEER_RELEASE);
    if (rc != STEER_OK)
    {
        steering_shutdown();
        steering_feedback_close();
        return rc;
    }
    if (priority)
    {
        struct rlimit limit;
        if (!getrlimit(RLIMIT_MEMLOCK, &limit))
        {
            limit.rlim_cur = limit.rlim_max;
            (void)setrlimit(RLIMIT_MEMLOCK, &limit);
        }
        memory_locked = mlockall(MCL_CURRENT) == 0;
        previous_policy = sched_getscheduler(0);
        (void)sched_getparam(0, &previous_schedule);
        struct sched_param param = {0};
        param.sched_priority = (int)priority;
        if (!memory_locked || previous_policy < 0 || sched_setscheduler(0, SCHED_FIFO, &param))
        {
            if (memory_locked)
                (void)munlockall();
            memory_locked = 0;
            steering_shutdown();
            steering_feedback_close();
            return STEER_ERR_IO;
        }
        scheduling_changed = 1;
    }
    steering_keepalive_init(&keepalive, 1000);
    bench = bench_mode;
    initialized = 1;
    center = (SteeringCenter){0};
    center_ready_until = 0;
    reference_file = ref;
    confirmed = have_lane = have_frame = 0;
    const char *startup = getenv("STEERING_STARTUP_CALIBRATION");
    startup_calibration = !bench && !model.burst_max_us && startup && !strcmp(startup, "1");
    startup_center_pending = startup_calibration;
    feedback_lost_since = 0;
    range_recovery = skip_first_recovery_frame = 0;
    snapshot_ns = 0;
    blocked_frame = 0;
    reference_notice_ns = 0;
    reference_notice_session = 0;
    reference_notice_result = STEER_OK;
    next_lane_poll_ns = 0;
    printf(
        "[STEER_INIT] table=%s reference_session=%u zero_raw=%.3f max_pulse_us=%u priority=%u target_tolerance=%.1f center_tolerance=%.1f; awaiting fresh matching session\n",
        table, reference_session, zero_raw, model.burst_max_us ? model.burst_max_us : 60000,
        priority, target_tolerance, center_tolerance);
    return STEER_OK;
}
void steering_runtime_cancel(void)
{
    if (initialized)
        (void)steering_cancel();
    have_lane = 0;
    blocked_frame = last_frame;
}

static void check_range_fault(steer_result_t result)
{
    if (result != STEER_ERR_DEVICE || range_recovery || bench || model.burst_max_us)
        return;
    steering_state_t state;
    steering_get_state(&state);
    if (!state.feedback_valid || !(fabsf(state.angle_deg) > STEERING_ANGLE_LIMIT_DEG))
        return;

    range_recovery = 1;
    center_ready_until = 0;
    center = (SteeringCenter){0};
    steering_runtime_cancel();
    printf("[STEER_CENTER] angle %.3f deg outside +/- %d deg; manual zero required\n",
           state.angle_deg, STEERING_ANGLE_LIMIT_DEG);
}

void steering_runtime_poll_lane(int allowed, steering_lane_reader_t reader, void *context)
{
    if (!initialized || bench || !reader || steering_pulse_active())
        return;
    uint64_t now = steering_now_ns();
    if (next_lane_poll_ns && now < next_lane_poll_ns)
        return;
    next_lane_poll_ns = now + 10000000ULL;
    uint32_t frame = 0;
    unsigned type = 0, confidence = 0, angle = 0;
    int valid = reader(context, &frame, &type, &confidence, &angle) == 0;
    steering_runtime_lane(allowed && valid, frame, type, confidence, angle);
}

int steering_runtime_center_sampling(void)
{
    return center.status == STEERING_CENTER_SAMPLING || center.status == STEERING_CENTER_CALIBRATING_IMU;
}

int steering_runtime_autopilot_ready(void)
{
    return initialized && (!startup_calibration || (!startup_center_pending && confirmed && !range_recovery));
}

int steering_runtime_center_waiting_for_touch(void)
{
    return initialized && (center.status == STEERING_CENTER_RELEASE_STOP ||
                           center.status == STEERING_CENTER_TAP_OVERVIEW);
}

static unsigned center_prompt(void)
{
    if (range_recovery && center.status == STEERING_CENTER_SWITCH_OFF)
        return STEERING_CENTER_LIMIT_SWITCH_OFF;
    if (range_recovery && center.status == STEERING_CENTER_HOLD_STOP)
        return STEERING_CENTER_LIMIT_HOLD_STOP;
    return center.status;
}

static unsigned startup_center_input(uint8_t key, int parked, int switch_off, uint64_t now)
{
    if (!parked || !switch_off ||
        (center.control_ns && now - center.control_ns >= CENTER_CONTROL_TTL_NS && steering_runtime_center_sampling()))
    {
        steering_center_set(&center, !switch_off ? STEERING_CENTER_SWITCH_OFF : STEERING_CENTER_RETRY);
        return center.status;
    }
    if (center.control_ns && now - center.control_ns >= CENTER_CONTROL_TTL_NS)
        steering_center_set(&center, STEERING_CENTER_HOLD_STOP);
    center.control_ns = now;

    if (center.status == STEERING_CENTER_CALIBRATING_IMU)
    {
        steering_feedback_t feedback;
        if (now >= calibration_deadline || (key != 0 && key != DISPLAY_MODE_ALL_CAMERAS))
            steering_center_set(&center, STEERING_CENTER_RETRY);
        else if (steering_feedback_read(&feedback) == STEER_OK &&
                 feedback.session_id == calibration_session)
        {
            steering_center_set(&center, STEERING_CENTER_SAMPLING);
            center.session = calibration_session;
            center.sample_start = now;
        }
        else if (now >= next_calibration_request)
        {
            (void)steering_feedback_request_center(calibration_session);
            next_calibration_request = now + 100000000ULL;
        }
        return center.status;
    }
    const unsigned previous = center.status;
    steering_center_key(&center, key, now);
    if (previous != STEERING_CENTER_SAMPLING && center.status == STEERING_CENTER_SAMPLING)
    {
        calibration_session = steering_feedback_center_session();
        if (calibration_session && calibration_session != reference_session)
        {
            (void)steering_feedback_request_center(calibration_session);
            next_calibration_request = now + 100000000ULL;
            calibration_deadline = now + 25000000000ULL;
            steering_center_set(&center, STEERING_CENTER_CALIBRATING_IMU);
        }
        else
            steering_center_set(&center, STEERING_CENTER_RETRY);
    }
    return center.status;
}

unsigned steering_runtime_center_input(uint8_t key, int parked, int switch_off)
{
    if (!initialized || bench || model.burst_max_us)
        return STEERING_CENTER_HIDDEN;
    const uint64_t now = steering_now_ns();
    if (startup_center_pending)
        return startup_center_input(key, parked, switch_off, now);
    steering_feedback_t feedback;
    if (steering_feedback_read(&feedback) != STEER_OK)
    {
        if (startup_calibration)
        {
            if (!feedback_lost_since)
                feedback_lost_since = now;
            if (now - feedback_lost_since >= 1000000000ULL)
            {
                steering_runtime_cancel();
                confirmed = 0;
                startup_center_pending = 1;
                steering_center_set(&center, STEERING_CENTER_HOLD_STOP);
                return startup_center_input(key, parked, switch_off, now);
            }
        }
        steering_center_set(&center, center.session ? STEERING_CENTER_RETRY : STEERING_CENTER_HIDDEN);
        return range_recovery ? STEERING_CENTER_LIMIT_WAIT_IMU : center.status;
    }
    feedback_lost_since = 0;
    if (feedback.session_id == reference_session && !range_recovery)
    {
        steering_center_set(&center, now < center_ready_until ? STEERING_CENTER_READY
                                                             : STEERING_CENTER_HIDDEN);
        center.session = 0;
        return center.status;
    }
    if (center.session != feedback.session_id)
    {
        steering_center_set(&center, STEERING_CENTER_HOLD_STOP);
        center.session = feedback.session_id;
        center.control_ns = 0;
    }
    if (!switch_off || !parked)
    {
        steering_center_set(&center, switch_off ? STEERING_CENTER_HOLD_STOP
                                               : STEERING_CENTER_SWITCH_OFF);
        return center_prompt();
    }
    if (center.control_ns && now - center.control_ns >= CENTER_CONTROL_TTL_NS)
        steering_center_set(&center, steering_runtime_center_sampling() ? STEERING_CENTER_RETRY
                                                                        : STEERING_CENTER_HOLD_STOP);
    center.control_ns = now;
    steering_center_key(&center, key, now);
    return center_prompt();
}

static int save_center_reference(uint32_t session, float raw)
{
    struct stat original;
    if (lstat(reference_file, &original) || !S_ISREG(original.st_mode))
        return -1;
    char pending[PATH_MAX];
    int length = snprintf(pending, sizeof(pending), "%s.tmp.XXXXXX", reference_file);
    if (length < 0 || (size_t)length >= sizeof(pending))
        return -1;
    int fd = mkstemp(pending);
    if (fd < 0)
        return -1;
    FILE *file = fdopen(fd, "w");
    if (!file)
    {
        close(fd);
        unlink(pending);
        return -1;
    }
    int failed = fchmod(fd, original.st_mode & 0777) ||
        fprintf(file, "STEER_PHYSICAL_REFERENCE_V1 %u %.6f 1\n", session, raw) < 0 ||
        fflush(file) || fsync(fd);
    if (fclose(file))
        failed = 1;
    if (!failed && rename(pending, reference_file) == 0)
        return 0;
    unlink(pending);
    return -1;
}

static int angle_order(const void *lhs, const void *rhs)
{
    const int32_t a = *(const int32_t *)lhs, b = *(const int32_t *)rhs;
    return (a > b) - (a < b);
}

static void observe_center(void)
{
    const uint64_t now = steering_now_ns();
    steering_feedback_t feedback;
    if (now - center.control_ns >= CENTER_CONTROL_TTL_NS ||
        steering_feedback_read(&feedback) != STEER_OK)
    {
        steering_center_set(&center, STEERING_CENTER_RETRY);
        return;
    }
    const int complete = steering_center_sample(&center, &feedback, now);
    if (complete < 0)
        steering_center_set(&center, STEERING_CENTER_RETRY);
    if (complete <= 0)
        return;

    qsort(center.angles, CENTER_SAMPLES, sizeof(center.angles[0]), angle_order);
    const float raw = (center.angles[CENTER_SAMPLES / 2 - 1] +
                       center.angles[CENTER_SAMPLES / 2]) / 2000.f;
    steering_runtime_cancel();
    confirmed = 0;
    const steer_result_t result = steering_confirm_reference(center.session, raw);
    if (result != STEER_OK || save_center_reference(center.session, raw) != 0)
    {
        fprintf(stderr, "[STEER_CENTER] HMI confirmation failed: %s\n",
                result != STEER_OK ? steer_result_name(result) : "could not save reference");
        steering_center_set(&center, STEERING_CENTER_RETRY);
        return;
    }
    printf("[STEER_CENTER] HMI confirmed session %u at %.3f deg (previous %u at %.3f deg)\n",
           center.session, raw, reference_session, zero_raw);
    reference_session = center.session;
    zero_raw = raw;
    confirmed = 1;
    startup_center_pending = 0;
    skip_first_recovery_frame = range_recovery;
    range_recovery = 0;
    center_ready_until = now + 5000000000ULL;
    steering_center_set(&center, STEERING_CENTER_READY);
}

void steering_runtime_tick(void)
{
    if (!initialized)
        return;
    if (center.status == STEERING_CENTER_SAMPLING)
        observe_center();
    if (startup_center_pending)
        return;
    if (!confirmed)
    {
        if (range_recovery)
            return;
        steering_feedback_t f = {0};
        steer_result_t rc = steering_feedback_read(&f);
        if (rc == STEER_OK && f.session_id == reference_session)
        {
            rc = steering_confirm_reference(reference_session, zero_raw);
            check_range_fault(rc);
            if (rc == STEER_OK)
            {
                confirmed = 1;
                puts(
                    "[STEER_REFERENCE] explicitly confirmed current session; physical zero offset bound");
                return;
            }
        }
        uint64_t now = steering_now_ns();
        if (!reference_notice_ns || now - reference_notice_ns >= 5000000000ULL ||
            f.session_id != reference_notice_session || rc != reference_notice_result)
        {
            const char *reason = rc != STEER_OK                      ? "feedback_not_ready"
                                 : f.session_id != reference_session ? "imu_session_changed"
                                                                     : "reference_not_ready";
            printf(
                "[STEER_WAIT_REFERENCE] reason=%s expected_session=%u received_session=%u feedback=%s automatic_steering=blocked physical_center_confirmation_required=1\n",
                reason, reference_session, f.session_id, steer_result_name(rc));
            reference_notice_ns = now;
            reference_notice_session = f.session_id;
            reference_notice_result = rc;
        }
        return;
    }
    uint64_t now = steering_now_ns();
    if (!bench && have_lane && now - lane_ns >= 500000000)
    {
        steering_runtime_cancel();
        puts("[STEER_CANCEL] lane result expired");
    }
    check_range_fault(steering_tick());
    steering_state_t st;
    steering_get_state(&st);
    if (st.status == STEERING_CANCELLED && st.last_error != STEER_OK)
    {
        have_lane = 0;
        blocked_frame = last_frame;
    }
    /* S/R 仅清控制器故障，不能据此绑定新的 M 会话。 */
    if (!steering_pulse_active() && (!snapshot_ns || now - snapshot_ns >= 5000000))
    {
        if (!fspi_read_control_snapshot(&snapshot))
            snapshot_ns = now;
    }
    if (snapshot_ns && now - snapshot_ns < 10000000 && snapshot.hmi == 0xee &&
        (st.status == STEERING_FAULT || st.status == STEERING_RESOLUTION_LIMIT))
        steering_runtime_cancel();
    static int previous = -1;
    /* 终端行缓冲或 SD 写入可能超过短脉冲时长；
     * BEGIN 在使能前输出，状态日志延后至释放后输出。 */
    if (!steering_pulse_active() && previous != (int)st.status)
    {
        printf("[STEER_STATE] state=%d name=%s angle=%.3f target=%.3f error=%.3f result=%s\n",
               st.status, steering_status_name(st.status), st.angle_deg, st.target_deg,
               st.error_deg, steer_result_name(st.last_error));
        previous = st.status;
    }
    (void)steering_keepalive_tick(&keepalive, now, bench || steering_pulse_active(), heartbeat,
                                  NULL);
}
void steering_runtime_lane(int valid, uint32_t frame, unsigned type, unsigned confidence,
                           unsigned road_angle)
{
    if (!initialized || !confirmed || range_recovery)
        return;
    if (!valid || confidence < 60 || type > 2 || road_angle > 90)
    {
        if (have_lane)
        {
            steering_runtime_cancel();
            printf(
                "[STEER_CANCEL] reason=lane_invalid_or_expired frame=%u valid=%d type=%u confidence=%u road_angle=%u\n",
                frame, valid, type, confidence, road_angle);
        }
        return;
    }
    if (skip_first_recovery_frame)
    {
        skip_first_recovery_frame = 0;
        have_frame = 1;
        last_frame = frame;
        return;
    }
    if (have_frame && frame == last_frame)
        return;
    have_frame = 1;
    last_frame = frame;
    lane_ns = steering_now_ns();
    if (frame == blocked_frame)
        return;
    float target =
        type == 0 ? 0 : fminf(target_limit, road_angle * curve_gain) * (type == 1 ? 1 : -1);
    if (type && road_angle == 0)
    {
        steering_runtime_cancel();
        return;
    }
    if (have_lane && ((model.burst_max_us && type && (target * last_target > 0) &&
                       fabsf(target - last_target) < target_tolerance) ||
                      (!model.burst_max_us && fabsf(target - last_target) < 1) ||
                      fabsf(target - last_target) < .001f))
        return;
    steer_result_t rc = steering_set_target_with_tolerance_deg(
        target, type == 0 ? center_tolerance : target_tolerance);
    printf("[STEER_LANE] frame=%u type=%u confidence=%u road_angle=%u target_deg=%.3f result=%s\n",
           frame, type, confidence, road_angle, target, steer_result_name(rc));
    if (rc == STEER_OK)
    {
        have_lane = 1;
        last_target = target;
    }
}
void steering_runtime_wait(void)
{
    uint64_t wake = steering_now_ns() + 500000, deadline = steering_pulse_deadline_ns();
    if (deadline && deadline < wake)
        wake = deadline;
    struct timespec t = {(time_t)(wake / 1000000000), (long)(wake % 1000000000)};
    (void)clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
}
void steering_runtime_close(void)
{
    steering_shutdown();
    steering_feedback_close();
    initialized = confirmed = have_lane = 0;
    if (scheduling_changed)
        (void)sched_setscheduler(0, previous_policy, &previous_schedule);
    if (memory_locked)
        (void)munlockall();
    scheduling_changed = memory_locked = 0;
}
