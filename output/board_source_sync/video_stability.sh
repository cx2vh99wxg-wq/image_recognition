ps -eo pid,stat,comm | awk '$3 ~ /^(app|video_receiver|eight_camera_re|vehicle_control)$/'
grep 'PERF S-PCIE' /tmp/codex_pcie.log | tail -3
grep 'PERF DISPLAY' /tmp/codex_x11.log | tail -3
