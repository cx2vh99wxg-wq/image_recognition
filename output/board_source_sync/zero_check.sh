p=$(pgrep -xo vehicle_control)
log=$(readlink /proc/"$p"/fd/1)
grep -Ei 'feedback|reference|closed.loop|rearm|failsafe' "$log" | grep -v 'SHM Read' | tail -n 15
ss -lunp | grep 54321
d=$(dirname "$log"); ls -1 "$d" | head -18
for f in "$d"/result.json "$d"/qualification.json; do if [ -f "$f" ]; then tail -c 1200 "$f"; echo; fi; done
