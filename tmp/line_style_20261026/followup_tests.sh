#!/bin/bash
set -eu
cd /mnt/d/Study_Object/image_recognition/image_recognition
out=tmp/line_style_20261026
src=fpga/fpga_pcie_ov5640
verilator --binary --timing -Wno-fatal --top-module test_fspi_control --Mdir "$out/v-test_fspi_control" "$src/sim/test_fspi_control.v" "$src/source/fspi/fspi_slave.v" "$src/source/fspi/spi_ram_crtl.v" >"$out/test_fspi_control-build.log" 2>&1
"$out/v-test_fspi_control/Vtest_fspi_control"
make -C udp_rk_rk_yolo/S/x11/csrc -j2 >"$out/s-x11-build.log" 2>&1

make -C udp_rk_rk_yolo/M/pcie test-line-style line-style-replay
cc -std=c11 -Wall -Wextra -Werror -Iudp_rk_rk_yolo/S/include udp_rk_rk_yolo/S/fspi/test_lane_rule.c -o tmp/line_style_20261026/test_lane_rule
tmp/line_style_20261026/test_lane_rule
