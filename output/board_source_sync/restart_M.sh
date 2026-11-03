set -e
for unit in udp pcie; do
 p=$(cat /tmp/codex_$unit.pid)
 case "$(readlink /proc/"$p"/exe)" in /home/linaro/M/*/build/*) kill -TERM "$p";; *) exit 1;; esac
 for i in $(seq 1 30); do [ ! -e /proc/"$p"/exe ] && break; sleep .1; done
 test ! -e /proc/"$p"/exe
done
cd /home/linaro/M/pcie
nohup env LANE_CURVE_MODE=ego-boundaries stdbuf -oL -eL ./build/pcie_capture </dev/null >/tmp/codex_pcie.log 2>&1 &
echo $! >/tmp/codex_pcie.pid
sleep 2
cd ../udp
nohup stdbuf -oL -eL ./build/udp_sender </dev/null >/tmp/codex_udp.log 2>&1 &
echo $! >/tmp/codex_udp.pid
sleep 6
grep -E 'ego-boundaries|PERF M' /tmp/codex_pcie.log | tail -3
grep 'UDP_TX.*fps=' /tmp/codex_udp.log | tail -1
