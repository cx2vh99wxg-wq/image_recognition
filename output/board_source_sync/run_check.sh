for unit in pcie udp x11 fspi; do
 p=/tmp/codex_${unit}.pid
 [ -f "$p" ] || continue
 echo "=== $unit ==="
 ps -p "$(cat "$p")" -o pid,stat,comm
 tail -n 12 /tmp/codex_${unit}.log
done
