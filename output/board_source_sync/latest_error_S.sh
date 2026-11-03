date
ps -eo pid,ppid,stat,args | grep -E '[m]ake run|[e]ight_camera_renderer|[v]ideo_receiver|[v]ehicle_control|/build/[a]pp' | tail -18
for f in /tmp/codex_board_build.log /tmp/codex_pcie.log /tmp/codex_udp.log /tmp/codex_x11.log; do
 if [ -f "$f" ]; then stat -c '%y %n' "$f"; tail -6 "$f"; fi
done
ip -br addr show end1
ls -l /dev/pango_pci_driver /tmp/steering-bench.active 2>/dev/null
journalctl -b -p err --no-pager -n 8
