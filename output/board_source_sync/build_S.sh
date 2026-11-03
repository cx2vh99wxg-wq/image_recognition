cd /home/linaro/S || exit 1
uname -m
command -v gcc
ps -eo pid,comm,args | grep -E '[p]cie_capture|[u]dp_sender|[v]ideo_receiver|[v]ehicle_control|[e]ight_camera_renderer|/build/[a]pp'
ip -br addr
nohup bash -c 'make -B -j2 all > /tmp/codex_board_build.log 2>&1; echo True > /tmp/codex_board_build.status' </dev/null >/dev/null 2>&1 &
