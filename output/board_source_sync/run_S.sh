set -e
cd /home/linaro/S
tail -n 8 /tmp/codex_board_build.log
test "$(cat /tmp/codex_board_build.status)" = 0
if [ ! -c /dev/pango_pci_driver ]; then insmod ./pango_pci_driver.ko; fi
ls -l /dev/pango_pci_driver
ping -c 2 -W 1 192.168.100.10
cd pcie
nohup stdbuf -oL -eL ./build/app </dev/null >/tmp/codex_pcie.log 2>&1 &
echo $! > /tmp/codex_pcie.pid
cd ../udp
nohup stdbuf -oL -eL ./build/video_receiver </dev/null >/tmp/codex_udp.log 2>&1 &
echo $! > /tmp/codex_udp.pid
cd ../x11/csrc
nohup env DISPLAY=:0 stdbuf -oL -eL ./build/eight_camera_renderer </dev/null >/tmp/codex_x11.log 2>&1 &
echo $! > /tmp/codex_x11.pid
