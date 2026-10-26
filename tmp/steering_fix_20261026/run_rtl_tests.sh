#!/bin/bash
set -eu
cd /mnt/d/Study_Object/image_recognition/image_recognition
out=tmp/steering_fix_20261026
src=fpga/fpga_pcie_ov5640
for test in test_remote_stop_controller test_hmi_uart test_hmi_auto_brake_switch test_hmi_serial_release test_fspi_control; do
  echo Running-$test
  verilator --binary --timing -Wno-fatal --top-module "$test" --Mdir "$out/v-$test" \
    "$src/sim/$test.v" "$src/source/image_pcie_capture.v" \
    "$src/source/uart_lcd/uart_lcd.v" "$src/source/uart_lcd/uart_rx.v" \
    "$src/source/fspi/fspi_slave.v" "$src/source/fspi/spi_ram_crtl.v" \
    > "$out/$test-build.log" 2>&1
  "$out/v-$test/V$test"
done
