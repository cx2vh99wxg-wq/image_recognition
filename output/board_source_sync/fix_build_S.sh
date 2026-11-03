set -e
cd /home/linaro/S
sed -i '1i#define _POSIX_C_SOURCE 200809L\r' pcie/csrc/yolo_integration.c
printf '%s  %s\n' 0ff857471e6a3d3a85a3ac65ff67a7beb5ea40dd238ba433a5cbca11dce73efe pcie/csrc/yolo_integration.c | sha256sum -c
rm -f /tmp/codex_board_build.status
nohup bash -c 'make -j2 all > /tmp/codex_board_build.log 2>&1; echo $? > /tmp/codex_board_build.status' </dev/null >/dev/null 2>&1 &
find /home/linaro/S /lib/modules/$(uname -r) -iname '*pango*.ko*' 2>/dev/null | head -12
lspci -nn
ip link set dev end1 up
ip addr replace 192.168.100.20/24 dev end1
ip -br link show end1
