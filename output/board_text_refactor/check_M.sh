set -e
bash -n /home/linaro/.bashrc /home/linaro/M/.bashrc
cd /home/linaro/M
make -n all >/tmp/codex_text_make_preview.log 2>&1 || { tail -20 /tmp/codex_text_make_preview.log; exit 1; }
echo 'BASHRC_AND_MAKE_OK'
ps -eo pid,stat,comm | awk '$3 ~ /^(app|pcie_capture|udp_sender|video_receiver|eight_camera_re|vehicle_control)$/'
