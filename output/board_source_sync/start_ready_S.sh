set -e
cd /home/linaro/S
if [ ! -c /dev/pango_pci_driver ]; then insmod ./pango_pci_driver.ko; fi
ls -l /dev/pango_pci_driver
if command -v nmcli >/dev/null; then nmcli device set end1 managed no || true; fi
ip link set end1 up
ip addr replace 192.168.100.20/24 dev end1
ping -c 2 -W 1 192.168.100.10 || true
(cd pcie; nohup stdbuf -oL -eL ./build/app </dev/null >/tmp/codex_pcie.log 2>&1 & echo $! >/tmp/codex_pcie.pid)
(cd udp; nohup stdbuf -oL -eL ./build/video_receiver </dev/null >/tmp/codex_udp.log 2>&1 & echo $! >/tmp/codex_udp.pid)
python3 steering/tools/launch_vehicle_control.py --check
nohup stdbuf -oL -eL python3 steering/tools/launch_vehicle_control.py </dev/null >/tmp/codex_fspi.log 2>&1 &
echo $! >/tmp/codex_fspi.pid
nohup bash -c 'while [ ! -f /tmp/codex_board_build.status ]; do sleep 2; done; test "$(cat /tmp/codex_board_build.status)" = 0 || exit 1; cd /home/linaro/S/x11/csrc; echo $$ >/tmp/codex_x11.pid; exec env DISPLAY=:0 stdbuf -oL -eL ./build/eight_camera_renderer' </dev/null >/tmp/codex_x11.log 2>&1 &

