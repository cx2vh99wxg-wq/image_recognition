for unit in pcie udp; do p=$(cat /tmp/codex_$unit.pid); ps -p "$p" -o pid,etime,stat,comm; done
grep -E 'LANE_CURVE_MODE|curve_mode|PERF M' /tmp/codex_pcie.log | head -2
grep 'PERF M' /tmp/codex_pcie.log | tail -1
grep '\[UDP_TX\] fps' /tmp/codex_udp.log | tail -1
