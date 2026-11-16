set -eu
cd /mnt/d/Study_Object/image_recognition/image_recognition
simdir=output/hmi_autopilot_20261113/final/rtl_test
src=fpga/fpga_pcie_ov5640
mkdir -p "$simdir"
verilator --binary --timing -j 4 -Wno-fatal --Mdir "$simdir" --top-module test_hmi_autopilot "$src/sim/test_hmi_autopilot.v" "$src/source/uart_lcd/uart_lcd.v" "$src/source/remote_vehicle_control.v" >"$simdir/build.log" 2>&1 || { tail -30 "$simdir/build.log"; exit 1; }
"$simdir/Vtest_hmi_autopilot"
