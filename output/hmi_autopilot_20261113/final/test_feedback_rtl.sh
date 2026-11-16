set -eu
cd /mnt/d/Study_Object/image_recognition/image_recognition
root=output/hmi_autopilot_20261113/final/rtl_test
src=fpga/fpga_pcie_ov5640
for test in test_hmi_gear_feedback test_hmi_camera_view; do
 simdir="$root/$test"
 mkdir -p "$simdir"
 verilator --binary --timing -j 4 -Wno-fatal --Mdir "$simdir" --top-module "$test" "$src/sim/$test.v" "$src/source/uart_lcd/uart_lcd.v" "$src/source/uart_lcd/uart_tx.v" "$src/source/uart_lcd/uart_rx.v" "$src/source/uart_lcd/hmi_camera_view.v" "$src/source/fspi/fspi_slave.v" "$src/source/remote_vehicle_control.v" >"$simdir/build.log" 2>&1 || { tail -30 "$simdir/build.log"; exit 1; }
 "$simdir/V$test"
done
