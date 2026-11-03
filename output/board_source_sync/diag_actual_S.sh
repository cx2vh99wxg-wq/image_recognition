cd /home/linaro/S
tail -n 12 /tmp/s_run_preview.log
tail -n 15 /home/linaro/.cache/steering/terminal-home.log
nohup bash -c 'make -j2 all >/tmp/s_current_build.log 2>&1; echo $? >/tmp/s_current_build.status' </dev/null >/dev/null 2>&1 &
