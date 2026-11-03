for d in /sys/devices/system/cpu/cpufreq/policy* /sys/class/devfreq/*; do
 [ -r "$d/governor" ] || [ -r "$d/scaling_governor" ] || continue
 echo "$d"; for n in governor scaling_governor cur_freq scaling_cur_freq scaling_max_freq available_governors scaling_available_governors; do [ ! -r "$d/$n" ] || cat "$d/$n"; done
done
for f in /sys/class/thermal/thermal_zone*/temp; do cat "$f"; done
ps -eo pid,psr,pcpu,comm --sort=-pcpu | head -15
python3 -c 'import numpy; print(numpy.__version__)'
DISPLAY=:0 xwininfo -id "$(sed -n 's/^PERCEPTION_WINDOW_ID=//p' /tmp/codex_x11.log | tail -1)" | head -14
