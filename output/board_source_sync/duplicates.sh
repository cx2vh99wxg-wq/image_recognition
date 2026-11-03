ps -eo pid,ppid,stat,args | grep -E '[e]ight_camera_renderer|/build/[a]pp|[m]ake run|[v]ideo_receiver|[v]ehicle_control'
for p in $(pgrep -x app; pgrep -x eight_camera_re); do echo PID=$p; readlink /proc/$p/exe; readlink /proc/$p/fd/1; cat /proc/$p/cgroup; done
