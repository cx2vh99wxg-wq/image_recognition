for i in $(seq 1 75); do [ -f /tmp/s_current_build.status ] && break; sleep 1; done
if [ -f /tmp/s_current_build.status ]; then
 cat /tmp/s_current_build.status
 tail -n 8 /tmp/s_current_build.log
 test "$(cat /tmp/s_current_build.status)" = 0
else
 ps -eo pid,etime,pcpu,comm | grep -E 'cc1plus|make'
 tail -n 6 /tmp/s_current_build.log
fi
