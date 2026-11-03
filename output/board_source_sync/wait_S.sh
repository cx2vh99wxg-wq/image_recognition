for i in $(seq 1 40); do [ -f /tmp/codex_board_build.status ] && break; sleep 1; done
cat /tmp/codex_board_build.status
tail -n 15 /tmp/codex_board_build.log
ps -eo pid,stat,comm,args | grep -E '[c]c1plus|[m]ake -j2'
