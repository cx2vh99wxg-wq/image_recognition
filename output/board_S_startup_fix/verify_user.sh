for i in $(seq 1 30); do [ -f /tmp/s_user_build.status ] && break; sleep 1; done
cat /tmp/s_user_build.status
tail -n 18 /tmp/s_user_build.log
stat -c '%U:%G %a %n' /home/linaro/S/pcie/build/main.d
test "$(cat /tmp/s_user_build.status)" = 0
