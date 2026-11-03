date
for u in pcie x11; do
 p=$(cat /tmp/codex_$u.pid); ps -p "$p" -o pid,pcpu,etime,comm
 tr '\000' '\n' </proc/"$p"/environ | grep -E '^(DISPLAY|VISION_|PCIE_)'
 grep -E 'PERF|DMA failed|ERROR|失败' /tmp/codex_$u.log | tail -5
done
DISPLAY=:0 xrandr --current | head -22
