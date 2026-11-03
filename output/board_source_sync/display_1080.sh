set -e
DISPLAY=:0 xrandr --output HDMI-1 --mode 1920x1080 --rate 60
sleep 12
DISPLAY=:0 xrandr --current | head -2
grep 'PERF DISPLAY' /tmp/codex_x11.log | tail -2
grep 'PERF S-PCIE' /tmp/codex_pcie.log | tail -2
