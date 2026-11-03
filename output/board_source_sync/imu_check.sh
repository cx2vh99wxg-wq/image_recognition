systemctl --no-pager show m-steering-imu -p LoadState -p ActiveState -p SubState -p ExecStart
journalctl -u m-steering-imu --no-pager -n 8
ls -l /home/linaro/M/imu/build/angle_monitor /home/linaro/M/imu/calibration/startup_bias.current.cal

