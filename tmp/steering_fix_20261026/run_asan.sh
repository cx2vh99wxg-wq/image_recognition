#!/bin/bash
set -eu
cd /mnt/d/Study_Object/image_recognition/image_recognition
gcc -std=c11 -O1 -g -Wall -Wextra -ffunction-sections -fdata-sections \
  -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Iudp_rk_rk_yolo/S/fspi/include -Iudp_rk_rk_yolo/S/include \
  udp_rk_rk_yolo/S/fspi/test_lane_steering.c -Wl,--gc-sections \
  -o tmp/steering_fix_20261026/test_lane_asan
tmp/steering_fix_20261026/test_lane_asan
