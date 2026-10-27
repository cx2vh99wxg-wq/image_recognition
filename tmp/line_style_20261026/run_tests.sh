#!/bin/bash
set -eu
cd /mnt/d/Study_Object/image_recognition/image_recognition
out=tmp/line_style_20261026
g++ -std=c++11 -Wall -Wextra -Werror -O2 -Iudp_rk_rk_yolo/M/include -Iudp_rk_rk_yolo/M/pcie/include udp_rk_rk_yolo/M/pcie/tests/test_line_style.cpp -o "$out/test_line_style"
"$out/test_line_style"
for test in test_lane_permission test_direction_arbiter test_hmi_serial_release test_hmi_auto_brake_switch test_remote_stop_controller test_hmi_uart test_fspi_control test_smdt_transport; do
  src=fpga/fpga_pcie_ov5640
  verilator --binary --timing -Wno-fatal --top-module "$test" --Mdir "$out/v-$test" \
    "$src/sim/$test.v" "$src/source/image_pcie_capture.v" "$src/source/uart_lcd/uart_lcd.v" "$src/source/uart_lcd/uart_rx.v" \
    "$src/source/fspi/fspi_slave.v" "$src/source/fspi/spi_ram_crtl.v" >"$out/$test-build.log" 2>&1
  "$out/v-$test/V$test"
done
