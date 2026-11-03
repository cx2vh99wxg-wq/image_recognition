set -e
test "$(readlink /proc/2697/exe)" = /home/linaro/S/x11/csrc/build/eight_camera_renderer
test "$(ps -o ppid= -p 2697 | tr -d ' ')" = 2695
test "$(ps -o ppid= -p 2695 | tr -d ' ')" = 2491
for p in 2624 2641 2697 2720 2722 2725; do
 case "$(readlink /proc/$p/exe)" in /home/linaro/S/*/build/*) ;; *) exit 1;; esac
done
kill -KILL 2695
kill -TERM 2624 2641 2697 2720 2722 2725
for i in $(seq 1 40); do
 if ! pgrep -x app && ! pgrep -x video_receiver && ! pgrep -x eight_camera_re; then break; fi
 sleep .2
done
! pgrep -x app
! pgrep -x video_receiver
! pgrep -x eight_camera_re
cd /home/linaro/S/pcie
nohup stdbuf -oL -eL ./build/app </dev/null >/tmp/codex_pcie.log 2>&1 &
echo $! > /tmp/codex_pcie.pid
cd ../udp
nohup stdbuf -oL -eL ./build/video_receiver </dev/null >/tmp/codex_udp.log 2>&1 &
echo $! > /tmp/codex_udp.pid
cd ../x11/csrc
nohup env DISPLAY=:0 stdbuf -oL -eL ./build/eight_camera_renderer </dev/null >/tmp/codex_x11.log 2>&1 &
echo $! > /tmp/codex_x11.pid
sleep 9
ps -eo pid,comm | grep -E ' app$|video_receiver|eight_camera_re|vehicle_control'
grep 'PERF S-PCIE' /tmp/codex_pcie.log | tail -1
grep 'PERF DISPLAY' /tmp/codex_x11.log | tail -1
