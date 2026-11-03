tail -n 35 /home/linaro/.xsession-errors 2>/dev/null
nl -ba /home/linaro/S/Makefile | sed -n '176,212p'
find /home/linaro/S/steering/logs -name result.json -mmin -20 -print | tail -5
