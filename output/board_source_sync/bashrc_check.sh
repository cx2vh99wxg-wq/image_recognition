for f in /home/linaro/.bashrc /home/linaro/M/.bashrc /home/linaro/S/.bashrc; do
 if [ -f "$f" ]; then sha256sum "$f"; grep -nE 'make run|TERMINAL_CENTER|setup_network|insmod|pgrep' "$f"; fi
done
