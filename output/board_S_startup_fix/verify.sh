set -e
cd /home/linaro/S
before=$(pgrep -x eight_camera_re || true)
timeout 8 make -n run >/tmp/s_run_preview_fixed.log 2>&1
after=$(pgrep -x eight_camera_re || true)
test "$before" = "$after"
grep -F 'trap ' /tmp/s_run_preview_fixed.log | tail -1
echo 'S_DRY_RUN_CHECKED'
cat /tmp/s_current_build.status 2>/dev/null || true
tail -n 9 /tmp/s_current_build.log
ps -eo pid,etime,pcpu,comm | grep -E 'cc1plus|make'
