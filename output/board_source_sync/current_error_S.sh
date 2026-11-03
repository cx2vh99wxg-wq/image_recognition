cat /tmp/s_current_build.status 2>/dev/null; tail -n 20 /tmp/s_current_build.log
ps -eo pid,ppid,stat,args | grep -E '[a]uto_home|[t]erminal_home|[v]ehicle_control|[m]ake run'
ls -l /tmp/steering-bench.active /home/linaro/S/fspi/fspi.pid 2>/dev/null
ls -ld /tmp/.X11-unix; ip -br addr show end1
