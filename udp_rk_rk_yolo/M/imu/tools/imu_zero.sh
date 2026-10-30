#!/bin/sh
# Relative reference only. Never grants a physical steering-center reference.
set -eu
unit=m-steering-imu.service
pid=$(systemctl show "$unit" --property=MainPID --value)
case "$pid" in ''|0|*[!0-9]*) echo 'IMU service is not running' >&2; exit 1;; esac
if ! "/proc/$pid/exe" --help | grep -q 'SIGUSR2'; then
    echo 'Running IMU program does not support runtime relative zero' >&2
    exit 1
fi
systemctl kill --kill-who=main --signal=USR2 "$unit"
printf '%s\n' 'Relative zero requested. Look for RELATIVE ZERO set in the IMU journal.'
