cd /home/linaro/S || exit 1
grep -nE 'make run|TERMINAL_CENTER|exit|return' /home/linaro/.bashrc | tail -20
find /home/linaro/S -maxdepth 3 -name '*.log' -mmin -20 | head -20
make -n run >/tmp/s_run_preview.log 2>&1
tail -8 /tmp/s_run_preview.log
nohup bash -c 'make -j2 all >/tmp/s_current_build.log 2>&1; echo $? >/tmp/s_current_build.status' </dev/null >/dev/null 2>&1 &
