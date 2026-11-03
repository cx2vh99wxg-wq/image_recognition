p=$(pgrep -xo vehicle_control)
log=$(readlink /proc/"$p"/fd/1)
if [ -f "$log" ]; then tail -n 15 "$log"; fi
