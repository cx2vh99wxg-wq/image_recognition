set -e
cd /home/linaro/S
for i in $(seq 1 90); do [ -f /tmp/s_current_build.status ] && break; sleep 1; done
test -f /tmp/s_current_build.status
id linaro
for d in build pcie/build udp/build x11/csrc/build fspi/build steering/actuator/build; do
 if [ -d "$d" ] && [ ! -L "$d" ]; then chown -Rh linaro:linaro "$d"; fi
done
for f in pcie/libshared.a udp/libshared.a pcie.pid udp.pid; do
 if [ -f "$f" ] && [ ! -L "$f" ]; then chown linaro:linaro "$f"; fi
done
stat -c '%U:%G %a %n' pcie/build pcie/build/main.d pcie/build/main.o
runuser -u linaro -- test -w pcie/build/main.d
runuser -u linaro -- test -w pcie/build
touch pcie/csrc/main.cpp
nohup bash -c 'runuser -u linaro -- make -j2 all >/tmp/s_user_build.log 2>&1; echo $? >/tmp/s_user_build.status' </dev/null >/dev/null 2>&1 &
