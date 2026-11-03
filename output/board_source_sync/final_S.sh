printf 'build='; cat /tmp/codex_board_build.status
for unit in pcie udp x11; do p=$(cat /tmp/codex_$unit.pid); ps -p "$p" -o pid,etime,stat,comm; readlink /proc/"$p"/exe; done
p=$(pgrep -xo vehicle_control)
ps -p "$p" -o pid,etime,stat,args
readlink /proc/"$p"/exe
stat -Lc '%d:%i %n' /proc/"$p"/exe /home/linaro/S/fspi/build/vehicle_control
readlink /proc/"$p"/fd/1
tr '\000' '\n' </proc/"$p"/environ | grep '^STEERING_CONTROL_MODE='
grep 'PERF DISPLAY' /tmp/codex_x11.log | tail -1
