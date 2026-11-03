tail -n 22 /tmp/codex_board_build.log
printf 'build_status='; cat /tmp/codex_board_build.status
ls -l /dev/pango_pci_driver /dev/spidev4.0 /tmp/.X11-unix 2>/dev/null
ip -br link show end1
