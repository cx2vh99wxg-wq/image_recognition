#!/usr/bin/env bash
# Full path on RK3568 Linux. Existing stub/stereo launchers remain available.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ROLE="${1:-}"; [[ "$ROLE" = m || "$ROLE" = s ]] || { echo "Usage: $0 m|s [--drive] [adas_main options]"; exit 2; }
shift
ARGS=("$@"); DRIVE=0
for arg in "${ARGS[@]}"; do [[ "$arg" != --drive ]] || DRIVE=1; done
[[ "$ROLE" != m || "$DRIVE" = 0 ]] || { echo "--drive is S only"; exit 2; }
[[ "$(uname -m)" = aarch64 ]] || { echo "Run on RK3568 Linux (aarch64)."; exit 1; }
if pgrep -f '(^|/)(adas_main|control_main|perception_main|planning_main|udp_m_send_main|pcie_dma_read_test)([[:space:]]|$)' >/dev/null; then
    echo "Existing process owns PCIe/FSPI. First run: bash scripts/stop_all.sh"; exit 1
fi
cd "$ROOT"
export LD_LIBRARY_PATH="$ROOT/lib:${LD_LIBRARY_PATH:-}"
# Always compile the selected path; never silently run an old bin/ copy.
make -C planning adas
make -C control -B bin
[[ -r "${PCIE_DEVICE:-/dev/pango_pci_driver}" ]] || { echo "Load the kernel-matching PCIe driver first; missing PCIe device."; exit 1; }
[[ -r "${FSPI_DEVICE:-/dev/spidev4.0}" ]] || { echo "Missing FSPI device. Check device tree / FSPI_DEVICE."; exit 1; }
if [[ "$ROLE" = s ]]; then
    export DISPLAY="${DISPLAY:-:0}"
    if [[ -z "${XAUTHORITY:-}" && -n "${SUDO_USER:-}" ]]; then
        desktop_home="$(getent passwd "$SUDO_USER" | cut -d: -f6)"
        [[ -z "$desktop_home" ]] || export XAUTHORITY="$desktop_home/.Xauthority"
    fi
fi
mkdir -p logs
CONTROL=(); [[ "$DRIVE" = 0 ]] || CONTROL=(--drive)
ctrl_pid=""; vision_pid=""
cleanup() {
    trap - EXIT INT TERM
    # Revoke actuator first; its exit asserts STOP and ceases heartbeats.
    [[ -z "$ctrl_pid" ]] || kill -TERM "$ctrl_pid" 2>/dev/null || true
    [[ -z "$vision_pid" ]] || kill -TERM "$vision_pid" 2>/dev/null || true
    [[ -z "$ctrl_pid" ]] || wait "$ctrl_pid" 2>/dev/null || true
    [[ -z "$vision_pid" ]] || wait "$vision_pid" 2>/dev/null || true
}
trap cleanup EXIT
trap 'exit 130' INT TERM
./control/control_main "${CONTROL[@]}" >"logs/adas-${ROLE}-control.log" 2>&1 & ctrl_pid=$!
sleep .3
kill -0 "$ctrl_pid" 2>/dev/null || { cat "logs/adas-${ROLE}-control.log"; exit 1; }
echo "ADAS role=$ROLE drive=$DRIVE main-front=M/cmos5. Ctrl-C stops both processes."
./planning/adas_main --role "$ROLE" --ip "${S_IP:-192.168.100.20}" "${ARGS[@]}" & vision_pid=$!
# Fail the launch if either process dies, including the HMI/health observer.
set +e
wait -n "$ctrl_pid" "$vision_pid"
status=$?
set -e
exit "$status"
