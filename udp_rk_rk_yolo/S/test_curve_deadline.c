#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L
#include "shared_memory.h"
#include <sys/wait.h>

/* The test creates its own segment under a key derived from a temporary file. */
static key_t fixture_key;
#undef SHM_CURVE_DETECTION_KEY
#define SHM_CURVE_DETECTION_KEY fixture_key
#include "csrc/shared_memory.c"

#define CHECK(condition)                                                                      \
    do                                                                                        \
    {                                                                                         \
        if (!(condition))                                                                     \
        {                                                                                     \
            fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #condition);                     \
            exit(1);                                                                          \
        }                                                                                     \
    } while (0)

static int owned_id = -1;

static void remove_fixture(void)
{
    if (owned_id >= 0)
        shmctl(owned_id, IPC_RMID, NULL);
}

static uint64_t monotonic_ns(void)
{
    struct timespec now;
    CHECK(clock_gettime(CLOCK_MONOTONIC, &now) == 0);
    return (uint64_t)now.tv_sec * 1000000000ULL + now.tv_nsec;
}

int main(void)
{
    char path[] = "/tmp/adas-curve-deadline-test.XXXXXX";
    int fd = mkstemp(path);
    CHECK(fd >= 0);
    fixture_key = ftok(path, 67);
    close(fd);
    unlink(path);
    CHECK(fixture_key != (key_t)-1);
    CHECK(atexit(remove_fixture) == 0);

    owned_id = shmget(fixture_key, sizeof(CurveDetectionSharedMemory), IPC_CREAT | IPC_EXCL | 0666);
    CHECK(owned_id >= 0);
    CurveDetectionSharedMemory *legacy = shmat(owned_id, NULL, 0);
    CHECK(legacy != (void *)-1);
    legacy->frame_id = 33;
    legacy->curve_type = CURVE_RIGHT;
    legacy->confidence = 97;
    legacy->curve_angle = 20;
    legacy->writer_active = 1;
    legacy->timestamp = ((uint64_t)time(NULL) - 10) * 1000000ULL;
    CHECK(shmdt(legacy) == 0);
    CHECK(curve_detection_shm_init(0) == 0);
    uint32_t frame = 0, confidence = 0, angle = 0;
    CurveType kind = CURVE_NONE;
    uint64_t deadline = 0;
    CHECK(curve_detection_read_until(&frame, &kind, &confidence, &angle, &deadline) != 0);
    CHECK(shmctl(owned_id, IPC_RMID, NULL) == 0);
    owned_id = -1;

    int ready[2];
    CHECK(pipe(ready) == 0);
    pid_t child = fork();
    CHECK(child >= 0);
    if (child == 0)
    {
        close(ready[0]);
        curve_detection_shm_cleanup();
        if (curve_detection_shm_init(1) != 0)
            _exit(2);
        int id = shmget(fixture_key, sizeof(CurveDetectionSharedMemory), 0);
        if (id < 0 || write(ready[1], &id, sizeof(id)) != sizeof(id))
            _exit(3);
        uint64_t expires = monotonic_ns() + 400000000ULL;
        if (curve_detection_update_until(41, CURVE_RIGHT, 90, 20, expires) != 0 ||
            write(ready[1], &expires, sizeof(expires)) != sizeof(expires))
            _exit(4);
        _exit(0); /* 异常退出时不会清除共享内存的 writer_active。 */
    }
    close(ready[1]);
    int child_status;
    uint64_t published_deadline;
    CHECK(read(ready[0], &owned_id, sizeof(owned_id)) == sizeof(owned_id));
    CHECK(read(ready[0], &published_deadline, sizeof(published_deadline)) ==
          sizeof(published_deadline));
    close(ready[0]);
    CHECK(waitpid(child, &child_status, 0) == child && child_status == 0);

    CHECK(curve_detection_read_until(&frame, &kind, &confidence, &angle, &deadline) == 0);
    CHECK(frame == 41 && kind == CURVE_RIGHT && confidence == 90 && angle == 20 &&
          deadline == published_deadline);
    struct timespec expiry = {(time_t)(deadline / 1000000000ULL), (long)(deadline % 1000000000ULL)};
    CHECK(clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &expiry, NULL) == 0);
    CHECK(curve_detection_read_until(&frame, &kind, &confidence, &angle, &deadline) != 0);
    curve_detection_shm_cleanup();
    puts("PASS S curve deadline: legacy frame rejected, reader reattached, abandoned frame expired");
    return 0;
}
