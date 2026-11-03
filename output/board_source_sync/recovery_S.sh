grep 'UDP_RX.*publish_fps' /tmp/codex_udp.log | tail -1
grep 'PERF DISPLAY' /tmp/codex_x11.log | tail -1
pgrep -a -x vehicle_control
