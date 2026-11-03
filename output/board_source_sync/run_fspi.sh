set -e
cd /home/linaro/S
test "$(cat /tmp/codex_board_build.status)" = 0 || { tail -n 20 /tmp/codex_board_build.log; exit 1; }
python3 steering/tools/launch_vehicle_control.py --check
nohup stdbuf -oL -eL python3 steering/tools/launch_vehicle_control.py </dev/null >/tmp/codex_fspi.log 2>&1 &
echo $! > /tmp/codex_fspi.pid
