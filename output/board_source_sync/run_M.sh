set -e
cd /home/linaro/M
tail -n 5 /tmp/codex_board_build.log
test -f /tmp/codex_board_build.status
! grep -E 'Error [1-9]|error:|undefined reference' /tmp/codex_board_build.log
ping -c 2 -W 1 192.168.100.20
cd pcie
nohup env LANE_CURVE_MODE=geometry stdbuf -oL -eL ./build/pcie_capture </dev/null >/tmp/codex_pcie.log 2>&1 &
echo $! > /tmp/codex_pcie.pid
sleep 2
cd ../udp
nohup stdbuf -oL -eL ./build/udp_sender </dev/null >/tmp/codex_udp.log 2>&1 &
echo $! > /tmp/codex_udp.pid
