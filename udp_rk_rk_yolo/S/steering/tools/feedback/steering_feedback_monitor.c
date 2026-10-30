/* Linux/POSIX UDP telemetry recorder. Deliberately has no FSPI/actuator dependency. */
#include "feedback_tracker.h"
#include "steering_platform.h"
#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

static volatile sig_atomic_t cancelled;
static void stop(int signal_number) { (void)signal_number; cancelled = 1; }
static int number(const char *s, unsigned *out)
{
    char *end; errno = 0;
    if (!s || !*s || *s == '-') return 0;
    unsigned long n = strtoul(s, &end, 10);
    if (errno || *end || n > 1000000) return 0;
    *out = (unsigned)n; return 1;
}
int main(int argc, char **argv)
{
    const char *bind_ip = "0.0.0.0", *source_ip = NULL, *csv_name = NULL;
    unsigned port = 54321, duration = 10000, timeout = 100, print_every = 100;
    for (int i = 1; i < argc; ++i) {
        const char *key = argv[i];
        if (!strcmp(key, "--help")) {
            puts("steering_feedback_monitor [--bind IPv4] [--source IPv4] [--port N]\n"
                 " [--duration-ms 1..600000] [--timeout-ms 1..60000] [--print-every N] [--csv NEW_FILE]\n"
                 "Port 0 chooses a test port. Monitor only: never opens SPI or drives steering.\n"
                 "Expiry checks source age plus local holding time, excluding unknown network transit.");
            return 0;
        }
        if (++i == argc) return 2;
        const char *value = argv[i];
        if (!strcmp(key, "--bind")) bind_ip = value;
        else if (!strcmp(key, "--source")) source_ip = value;
        else if (!strcmp(key, "--csv")) csv_name = value;
        else if (!strcmp(key, "--port")) { if (!number(value, &port)) return 2; }
        else if (!strcmp(key, "--duration-ms")) { if (!number(value, &duration)) return 2; }
        else if (!strcmp(key, "--timeout-ms")) { if (!number(value, &timeout)) return 2; }
        else if (!strcmp(key, "--print-every")) { if (!number(value, &print_every)) return 2; }
        else return 2;
    }
    if (port > 65535 || !duration || duration > 600000 || !print_every) return 2;
    feedback_tracker_t tracker;
    if (feedback_tracker_init(&tracker, timeout) != STEER_OK) return 2;
    struct sockaddr_in address = {0};
    struct in_addr source = {0};
    address.sin_family = AF_INET; address.sin_port = htons((uint16_t)port);
    if (inet_pton(AF_INET, bind_ip, &address.sin_addr) != 1 ||
        (source_ip && inet_pton(AF_INET, source_ip, &source) != 1)) return 2;
    int fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (fd < 0) { perror("UDP socket"); return 1; }
    if (bind(fd, (struct sockaddr *)&address, sizeof(address))) {
        perror("UDP bind"); close(fd); return 1;
    }
    socklen_t address_size = sizeof(address);
    if (getsockname(fd, (struct sockaddr *)&address, &address_size)) { close(fd); return 1; }
    FILE *csv = NULL;
    if (csv_name) {
        csv = steering_create_csv(csv_name);
        if (!csv) { perror("CSV (choose a new file)"); close(fd); return 1; }
        fputs("rx_local_ns,source_ipv4,session,sequence,flags,source_time_ns,source_age_ms,raw_relative_deg,raw_rate_dps,accepted,cache_status\n", csv);
    }
    signal(SIGINT, stop); signal(SIGTERM, stop);
    setvbuf(stdout, NULL, _IOLBF, 0);
    printf("LISTEN bind=%s port=%u source=%s MONITOR_ONLY actuator_enabled=0\n",
           bind_ip, ntohs(address.sin_port), source_ip ? source_ip : "any");
    uint64_t start = steering_now_ns(), span = (uint64_t)duration * 1000000;
    if (!start || start > UINT64_MAX - span) { if (csv) fclose(csv); close(fd); return 1; }
    uint64_t accepted = 0, rejected = 0, foreign = 0, relative = 0, centered = 0, simulated = 0;
    uint64_t session_changes = 0;
    int last_status = -1, exit_code = 0;
    while (!cancelled && steering_now_ns() - start < span) {
        uint64_t now = steering_now_ns();
        feedback_status_t status = feedback_tracker_status(&tracker, now);
        if ((int)status != last_status) {
            printf("STATUS %s actuator_enabled=0\n", feedback_status_name(status));
            last_status = (int)status;
        }
        struct pollfd ready = {fd, POLLIN, 0};
        int result = poll(&ready, 1, 10);
        if (result < 0) { if (errno == EINTR) continue; perror("UDP poll"); exit_code = 1; break; }
        if (!result) continue;
        if (!(ready.revents & POLLIN)) { fputs("UDP socket error\n", stderr); exit_code = 1; break; }
        uint8_t packet[STEERING_FEEDBACK_WIRE_SIZE + 1];
        struct sockaddr_in peer = {0}; socklen_t peer_size = sizeof(peer);
        ssize_t n = recvfrom(fd, packet, sizeof(packet), MSG_DONTWAIT, (struct sockaddr *)&peer, &peer_size);
        if (n < 0) {
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) continue;
            perror("UDP receive"); exit_code = 1; break;
        }
        if (source_ip && source.s_addr != peer.sin_addr.s_addr) { ++foreign; continue; }
        steering_feedback_t incoming;
        if (steering_feedback_decode(&incoming, packet, (size_t)n) != STEER_OK) { ++rejected; continue; }
        now = steering_now_ns();
        uint32_t previous_session = tracker.have_sample ? tracker.sample.session_id : 0;
        steer_result_t rc = feedback_tracker_ingest(&tracker, packet, (size_t)n, now);
        status = feedback_tracker_status(&tracker, now);
        char peer_text[INET_ADDRSTRLEN];
        if (!inet_ntop(AF_INET, &peer.sin_addr, peer_text, sizeof(peer_text))) { exit_code = 1; break; }
        if (csv) {
            fprintf(csv, "%" PRIu64 ",%s,%u,%u,%u,%" PRIu64 ",%u,%.3f,%.3f,%d,%s\n",
                    now, peer_text, incoming.session_id, incoming.sequence, incoming.flags,
                    incoming.sample_time_ns, incoming.source_age_ms, incoming.angle_mdeg / 1000.0,
                    incoming.rate_mdps / 1000.0, rc == STEER_OK, feedback_status_name(status));
            if (ferror(csv)) { perror("CSV write"); exit_code = 1; break; }
        }
        if (rc != STEER_OK) { ++rejected; continue; }
        ++accepted;
        if (previous_session && previous_session != incoming.session_id) {
            ++session_changes;
            printf("SESSION_CHANGE old=%u new=%u; reference changed, actuator_enabled=0\n",
                   previous_session, incoming.session_id);
        }
        if (status == FEEDBACK_RELATIVE) ++relative;
        if (status == FEEDBACK_CENTERED) ++centered;
        if (status == FEEDBACK_SIMULATED) ++simulated;
        if (accepted == 1 || accepted % print_every == 0)
            printf("RX session=%u seq=%u flags=%u raw_relative_deg=%.3f rate_dps=%.3f source_age_ms=%u status=%s rx_local_ns=%" PRIu64 " actuator_enabled=0\n",
                   incoming.session_id, incoming.sequence, incoming.flags, incoming.angle_mdeg / 1000.0,
                   incoming.rate_mdps / 1000.0, incoming.source_age_ms, feedback_status_name(status), now);
    }
    printf("SUMMARY accepted=%" PRIu64 " rejected=%" PRIu64 " foreign=%" PRIu64
           " real_relative=%" PRIu64 " zero_flagged=%" PRIu64 " simulated=%" PRIu64
           " session_changes=%" PRIu64 " final_status=%s actuator_enabled=0\n",
           accepted, rejected, foreign, relative, centered, simulated, session_changes,
           feedback_status_name(feedback_tracker_status(&tracker, steering_now_ns())));
    if (csv && fclose(csv)) exit_code = 1;
    close(fd);
    return exit_code ? exit_code : !accepted;
}
