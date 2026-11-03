ls -l /tmp/steering-bench.active
if [ -f /tmp/steering-bench.active ]; then head -c 300 /tmp/steering-bench.active; echo; fi
ps -eo pid,stat,comm,args | grep -E '[s]teering_(bench|step|goal|action|status|validate|auto_calibrate)|[v]ehicle_control'
grep -E 'PERF|fps|ERROR|失败' /tmp/codex_udp.log /tmp/codex_pcie.log /tmp/codex_x11.log | tail -n 16
